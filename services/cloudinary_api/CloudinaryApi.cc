//
// Created by Emmanuel Addo-Odame on 05/11/2026.
//
#include "CloudinaryApi.h"

#include <drogon/HttpAppFramework.h>
#include <drogon/HttpClient.h>

#include "utils/IdGeneratorUtils.h"

namespace raffle::services {

    // may accept a file stream
    drogon::Task<std::string> CloudinaryApi::uploadFile() {

        // upload the thumbnail to cloudinary ...
        auto customConfig = drogon::app().getCustomConfig();
        std::string cloud = customConfig["CloudinarySettings"]["Cloud"].asString();
        std::string apiKey = customConfig["CloudinarySettings"]["ApiKey"].asString();
        std::string apiSecret = customConfig["CloudinarySettings"]["ApiSecret"].asString();

        auto now = std::chrono::system_clock::now();
        auto timestamp = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count());

        const std::string& publicId = utils::IdGeneratorUtils::generateGuid();

        std::string stringToSign = "folder=thumbnails&public_id=" + publicId +
                                   "&timestamp=" + timestamp + apiSecret;
        std::string signature = drogon::utils::getSha256(stringToSign);

        auto req = drogon::HttpRequest::newFileUploadRequest({drogon::UploadFile(thumbnailFilePath, "", "file")});
        req->setPath("/v1_1/" + cloud + "/image/upload");
        req->setMethod(drogon::Post);

        req->setParameter("api_key", apiKey);
        req->setParameter("folder", "thumbnails");
        req->setParameter("public_id", publicId);
        req->setParameter("timestamp", timestamp);
        req->setParameter("signature", signature);

        auto client = drogon::HttpClient::newHttpClient("https://api.cloudinary.com");

        std::string secureUrl;

        try {

            drogon::HttpResponsePtr resp = co_await client->sendRequestCoro(req);

            if (resp && resp->getStatusCode() == 200) {
                auto json = resp->getJsonObject();

                if (json && json->isMember("secure_url")) {

                    secureUrl = (*json)["secure_url"].asString();

                    LOG_DEBUG << "[uploadFile] Cloudinary upload successful, URL: " << secureUrl;
                    co_return secureUrl;
                }
            }

            // Any non-200 response or missing secure_url field falls through to here
            LOG_ERROR << "[uploadFile] Cloudinary upload failed for: "
                      << publicId;
            if (resp) {
                LOG_ERROR << "[uploadFile] Status: " << resp->getStatusCode()
                          << ", Response: " << std::string(resp->getBody());
            }
        } catch (const std::exception &e) {
            // sendRequestCoro throws on transport-level failures (DNS, timeout, TLS, etc.)
            LOG_ERROR << "[uploadFile] Cloudinary request threw an exception: "
                      << e.what();
        }

    }


}
