//
// Created by Emmanuel Addo-Odame on 05/11/2026.
//
#include "CloudinaryApi.h"

#include <drogon/HttpAppFramework.h>
#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>
#include <drogon/UploadFile.h>
#include <drogon/utils/Utilities.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iterator>
#include <unordered_map>

#include "utils/IdGeneratorUtils.h"

namespace raffle::services {

    namespace {

        constexpr const char *kCloudinaryHost = "https://api.cloudinary.com";

        // Cloudinary accepts both SHA-1 and SHA-256 digests; SHA-256 is used here.
        constexpr const char *kSignatureAlgorithm = "sha256";

        // API v1.1 is required for sha256 signatures, the url is
        // /v1_1/:cloud_name/:resource_type/upload
        constexpr const char *kUploadPathPrefix = "/v1_1/";

        /**
         * @brief Builds the Cloudinary upload signature.
         *
         * Every field sent with the upload (file, cloud name, resource type and
         * api key excluded) is sorted alphabetically, joined as name=value pairs
         * with '&' and the api secret is appended without any separator. The
         * resulting string is hashed with SHA-256.
         */
        std::string buildSignature(const std::unordered_map<std::string, std::string> &paramsToSign,
                                   const std::string &apiSecret) {

            std::vector<std::string> keys;
            keys.reserve(paramsToSign.size());
            for (const auto &[key, value] : paramsToSign) {
                keys.push_back(key);
            }
            std::sort(keys.begin(), keys.end());

            std::string stringToSign;
            for (const auto &key : keys) {
                if (!stringToSign.empty()) {
                    stringToSign += "&";
                }
                stringToSign += key + "=" + paramsToSign.at(key);
            }

            stringToSign += apiSecret;
            return drogon::utils::getSha256(stringToSign);
        }

        /**
         * @brief Reads the Cloudinary credentials from the custom config.
         */
        bool readCredentials(std::string &cloud, std::string &apiKey, std::string &apiSecret) {

            if (!drogon::app().getCustomConfig().isMember("CloudinarySettings")) {
                LOG_ERROR << "[CloudinaryApi] Missing CloudinarySettings in the custom config";
                return false;
            }

            const auto &settings = drogon::app().getCustomConfig()["CloudinarySettings"];
            cloud = settings.get("Cloud", "").asString();
            apiKey = settings.get("ApiKey", "").asString();
            apiSecret = settings.get("ApiSecret", "").asString();

            if (cloud.empty() || apiKey.empty() || apiSecret.empty()) {
                LOG_ERROR << "[CloudinaryApi] CloudinarySettings is not configured "
                             "(Cloud, ApiKey and ApiSecret are required)";
                return false;
            }

            return true;
        }

        /**
         * @brief Removes the characters that would break the multipart header.
         */
        std::string sanitizeFileName(const std::string &fileName) {

            std::string clean;
            clean.reserve(fileName.size());
            for (unsigned char c : fileName) {
                if (c == '"' || c == '\r' || c == '\n' || c == '\\' || c < 0x20) {
                    continue;
                }
                clean.push_back(static_cast<char>(c));
            }

            // keep the extension, it is how cloudinary detects the format
            if (clean.empty()) {
                return "image";
            }
            return clean;
        }

        /**
         * @brief Maps a mime type onto the drogon content type enum.
         */
        drogon::ContentType toContentType(const std::string &mimeType) {

            static const std::unordered_map<std::string, drogon::ContentType> mimeTypes = {
                {"image/jpeg", drogon::CT_IMAGE_JPG},
                {"image/jpg", drogon::CT_IMAGE_JPG},
                {"image/png", drogon::CT_IMAGE_PNG},
                {"image/gif", drogon::CT_IMAGE_GIF},
                {"image/webp", drogon::CT_IMAGE_WEBP},
                {"image/bmp", drogon::CT_IMAGE_BMP},
                {"image/tiff", drogon::CT_IMAGE_TIFF},
                {"image/svg+xml", drogon::CT_IMAGE_SVG_XML},
            };

            auto it = mimeTypes.find(mimeType);
            return it == mimeTypes.end() ? drogon::CT_APPLICATION_OCTET_STREAM : it->second;
        }

        drogon::Task<std::optional<std::string>> performUpload(const char *data,
                                                              size_t length,
                                                              const std::string &fileName,
                                                              std::string contentType,
                                                              const std::string &folder) {

            std::string cloud;
            std::string apiKey;
            std::string apiSecret;

            if (!readCredentials(cloud, apiKey, apiSecret)) {
                co_return std::nullopt;
            }

            if (data == nullptr || length == 0) {
                LOG_ERROR << "[CloudinaryApi] Cannot upload an empty file";
                co_return std::nullopt;
            }

            const auto timestamp = std::to_string(
                std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch())
                    .count());

            const std::string publicId = utils::IdGeneratorUtils::generateGuid();

            std::unordered_map<std::string, std::string> paramsToSign = {
                {"folder", folder},
                {"public_id", publicId},
                {"timestamp", timestamp},
                {"signature_algorithm", kSignatureAlgorithm},
            };

            // The multipart request needs to know the mime type of the part,
            // fall back to a binary stream when the client did not provide one.
            drogon::ContentType partContentType =
                contentType.empty() ? drogon::CT_APPLICATION_OCTET_STREAM : toContentType(contentType);

            auto req = drogon::HttpRequest::newFileUploadRequest(
                {drogon::UploadFile(data,
                                    length,
                                    sanitizeFileName(fileName),
                                    "file",
                                    partContentType)});

            req->setPath(kUploadPathPrefix + cloud + "/image/upload");
            req->setMethod(drogon::Post);

            req->setParameter("api_key", apiKey);
            for (const auto &[key, value] : paramsToSign) {
                req->setParameter(key, value);
            }
            req->setParameter("signature", buildSignature(paramsToSign, apiSecret));

            auto client = drogon::HttpClient::newHttpClient(kCloudinaryHost);

            try {

                drogon::HttpResponsePtr resp = co_await client->sendRequestCoro(req);

                if (resp && resp->getStatusCode() == drogon::k200OK) {
                    auto json = resp->getJsonObject();

                    if (json && json->isMember("secure_url")) {
                        auto secureUrl = (*json)["secure_url"].asString();
                        LOG_DEBUG << "[CloudinaryApi] upload successful, url: " << secureUrl;
                        co_return secureUrl;
                    }

                    LOG_ERROR << "[CloudinaryApi] Upload response did not contain a secure_url";
                    co_return std::nullopt;
                }

                // Any non-200 response or missing secure_url field falls through to here
                LOG_ERROR << "[CloudinaryApi] Cloudinary upload failed for public_id: " << publicId;
                if (resp) {
                    LOG_ERROR << "[CloudinaryApi] Status: " << resp->getStatusCode()
                              << ", Response: " << std::string(resp->body());
                }
            } catch (const std::exception &e) {
                // sendRequestCoro throws on transport-level failures (DNS, timeout, TLS, etc.)
                LOG_ERROR << "[CloudinaryApi] Cloudinary request threw an exception: " << e.what();
            }

            co_return std::nullopt;
        }

    } // namespace

    std::string CloudinaryApi::mimeTypeForFile(const std::string &fileName) {

        static const std::unordered_map<std::string, std::string> mimeTypes = {
            {"jpg", "image/jpeg"},
            {"jpeg", "image/jpeg"},
            {"png", "image/png"},
            {"gif", "image/gif"},
            {"webp", "image/webp"},
            {"bmp", "image/bmp"},
            {"tif", "image/tiff"},
            {"tiff", "image/tiff"},
            {"svg", "image/svg+xml"},
            {"avif", "image/avif"},
        };

        auto pos = fileName.find_last_of('.');
        if (pos == std::string::npos || pos + 1 >= fileName.size()) {
            return "";
        }

        std::string extension = fileName.substr(pos + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        auto it = mimeTypes.find(extension);
        return it == mimeTypes.end() ? "" : it->second;
    }

    drogon::Task<std::optional<std::string>> CloudinaryApi::uploadFile(const char *data,
                                                                      size_t length,
                                                                      const std::string &fileName,
                                                                      const std::string &contentType,
                                                                      const std::string &folder) {
        auto secureUrl = co_await performUpload(data, length, fileName, contentType, folder);
        co_return secureUrl;
    }

    drogon::Task<std::optional<std::string>> CloudinaryApi::uploadFile(const std::string &filePath,
                                                                      const std::string &folder) {

        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            LOG_ERROR << "[CloudinaryApi] Unable to open file for upload: " << filePath;
            co_return std::nullopt;
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        file.close();

        if (content.empty()) {
            LOG_ERROR << "[CloudinaryApi] File is empty, refusing to upload: " << filePath;
            co_return std::nullopt;
        }

        auto pos = filePath.find_last_of("/" + std::string(1, '\\'));
        const std::string fileName = pos == std::string::npos ? filePath : filePath.substr(pos + 1);

        auto secureUrl = co_await performUpload(content.data(), content.size(), fileName, "", folder);
        co_return secureUrl;
    }

}
