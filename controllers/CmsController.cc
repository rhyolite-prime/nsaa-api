#include "CmsController.h"
#include "plugins/RaffleServicePlugin.h"
#include "constants/ErrorCodes.h"
#include "services/cloudinary_api/CloudinaryApi.h"

#include <drogon/MultiPart.h>

#include <algorithm>
#include <cctype>
#include <optional>

//public

Task<HttpResponsePtr> CmsController::getHeroSlides(HttpRequestPtr req) {

    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    std::string query = req->getParameter("query");
    if (query.empty()) {
        query = "";
    }


    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &heroSliderService = plugin->getHeroSliderService();

    auto result = co_await heroSliderService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}

Task<HttpResponsePtr> CmsController::getPromotionalBanners(HttpRequestPtr req) {

    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    std::string query = req->getParameter("query");
    if (query.empty()) {
        query = "";
    }


    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &bannerService = plugin->getBannerService();

    auto result = co_await bannerService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}

Task<HttpResponsePtr> CmsController::getFaqs(HttpRequestPtr req) {

    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    std::string query = req->getParameter("query");
    if (query.empty()) {
        query = "";
    }


    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &faqService = plugin->getFaqService();

    auto result = co_await faqService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}



Task<HttpResponsePtr> CmsController::getPages(HttpRequestPtr req, std::string slug) {

    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }


    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &pageService = plugin->getPageService();

    auto result = co_await pageService.getAll(pageNo, pageSize, slug);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}

Task<HttpResponsePtr> CmsController::getMediaAssets(HttpRequestPtr req) {

    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    std::string query = req->getParameter("query");
    if (query.empty()) {
        query = "";
    }

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &mediaLibraryService = plugin->getMediaLibraryService();

    auto result = co_await mediaLibraryService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;


}

Task<HttpResponsePtr> CmsController::createContact(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::SupportTicketDto dto;
    dto.setCategory("contact");
    dto.setSubject("Contact");
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &supportTicketService = plugin->getSupportTicketService();

    auto result = co_await supportTicketService.create(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}


//admin

Task<HttpResponsePtr> CmsController::getAllHeroSlides(HttpRequestPtr req) {

    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    std::string query = req->getParameter("query");
    if (query.empty()) {
        query = "";
    }


    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &heroSliderService = plugin->getHeroSliderService();

    auto result = co_await heroSliderService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}


namespace {

    // Largest image accepted for a hero slide (Cloudinary's free plan allows up to 10MB).
    constexpr size_t kMaxHeroSlideImageSize = 5 * 1024 * 1024;

    // Name of the multipart form field that carries the hero slide image.
    constexpr const char *kHeroSlideImageField = "image";

    // Cloudinary folder the hero slide images are stored in.
    constexpr const char *kHeroSlideImageFolder = "hero-slides";

    HttpResponsePtr jsonError(const std::string &message,
                              HttpStatusCode statusCode = k400BadRequest,
                              int errorCode = raffle::constants::ERR_VALIDATION) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.message = message;
        response.error["code"] = errorCode;
        response.error["message"] = message;
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(statusCode);
        return resp;
    }

    HttpResponsePtr jsonResult(const raffle::dto::BaseApiResponse &result) {
        auto statusCode = k200OK;
        if (!result.success) {
            if (result.error.isMember("code")) {
                const auto code = result.error["code"].asInt();
                if (code == raffle::constants::ERR_VALIDATION ||
                    code == raffle::constants::ERR_MISSING_PARAMETER) {
                    statusCode = k400BadRequest;
                } else if (code == raffle::constants::ERR_RESOURCE_NOT_FOUND ||
                           code == raffle::constants::ERR_DB_NOT_FOUND) {
                    statusCode = k404NotFound;
                } else {
                    statusCode = k500InternalServerError;
                }
            } else {
                statusCode = k500InternalServerError;
            }
        }
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        resp->setStatusCode(statusCode);
        return resp;
    }

    /**
     * @brief Reads a string field of a multipart body.
     */
    std::string formField(const MultiPartParser &parser, const std::string &key) {
        const auto &params = parser.getParameters();
        auto it = params.find(key);
        return it == params.end() ? "" : it->second;
    }

    /**
     * @brief Tells whether a field was part of the multipart body.
     */
    bool formHasField(const MultiPartParser &parser, const std::string &key) {
        const auto &params = parser.getParameters();
        return params.find(key) != params.end();
    }

    /**
     * @brief Reads an integer field of a multipart body.
     * @return An empty optional when the field is absent or not a number.
     */
    std::optional<int> formIntField(const MultiPartParser &parser, const std::string &key) {
        auto value = formField(parser, key);
        if (value.empty()) {
            return std::nullopt;
        }
        try {
            return std::stoi(value);
        } catch (const std::exception &) {
            return std::nullopt;
        }
    }

    /**
     * @brief Reads a boolean field of a multipart body.
     * Accepts "true"/"false" as well as "1"/"0".
     * @return An empty optional when the field is absent or not a boolean.
     */
    std::optional<bool> formBoolField(const MultiPartParser &parser, const std::string &key) {
        auto value = formField(parser, key);
        if (value.empty()) {
            return std::nullopt;
        }
        std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (value == "true" || value == "1") {
            return true;
        }
        if (value == "false" || value == "0") {
            return false;
        }
        return std::nullopt;
    }

    /**
     * @brief Picks the image out of the uploaded files.
     *
     * Whatever the field name used by the client ("image", "file", "imageFile",
     * ...) the first uploaded file is used, as long as it is an image.
     */
    const HttpFile *uploadedImage(const MultiPartParser &parser) {

        const auto &files = parser.getFiles();
        if (files.empty()) {
            return nullptr;
        }

        for (const auto &file : files) {
            if (file.getItemName() == kHeroSlideImageField) {
                return &file;
            }
        }

        return &files.front();
    }

    bool isSupportedImage(const HttpFile &file) {

        if (file.fileLength() == 0 || file.fileData() == nullptr) {
            return false;
        }

        if (file.getFileType() == FT_IMAGE) {
            return true;
        }

        // The file name is not always reliable (a browser may send a generic
        // name), so fall back to the extension and then to the mime type.
        static const std::vector<std::string> extensions = {
            "png", "jpg", "jpeg", "gif", "bmp", "webp", "svg", "tif", "tiff", "avif"};

        std::string extension(file.getFileExtension());
        std::transform(extension.begin(), extension.end(), extension.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
            return true;
        }

        return !raffle::services::CloudinaryApi::mimeTypeForFile(file.getFileName()).empty();
    }

    /**
     * @brief Outcome of uploading the image that comes with a hero slide form.
     */
    struct ImageUploadResult {
        bool provided = false;   // a file was part of the form
        bool success = false;    // the file was uploaded to cloudinary
        std::string url;         // cloudinary secure url when successful
        std::string errorMessage;
        HttpStatusCode errorStatus = k400BadRequest;
        int errorCode = raffle::constants::ERR_VALIDATION;
    };

    /**
     * @brief Uploads the image of a hero slide form to cloudinary.
     *
     * The image is optional here, callers decide whether it is required.
     */
    drogon::Task<ImageUploadResult> uploadHeroSlideImage(const MultiPartParser &parser,
                                                        raffle::services::CloudinaryApi &cloudinaryApi) {

        ImageUploadResult upload;

        const auto *image = uploadedImage(parser);
        if (image == nullptr) {
            co_return upload;
        }

        upload.provided = true;

        if (!isSupportedImage(*image)) {
            upload.errorMessage = "The uploaded file is not a supported image (png, jpg, gif, bmp, webp or svg).";
            co_return upload;
        }

        if (image->fileLength() > kMaxHeroSlideImageSize) {
            upload.errorMessage = "The image is too large. The maximum allowed size is 5MB.";
            co_return upload;
        }

        std::string fileName = image->getFileName().empty() ? "hero-slide" : image->getFileName();
        std::string contentType = raffle::services::CloudinaryApi::mimeTypeForFile(fileName);
        if (contentType.empty()) {
            contentType = raffle::services::CloudinaryApi::mimeTypeForFile(std::string(image->getFileExtension()));
        }

        auto imageUrl = co_await cloudinaryApi.uploadFile(image->fileData(),
                                                         image->fileLength(),
                                                         fileName,
                                                         contentType,
                                                         kHeroSlideImageFolder);

        if (!imageUrl.has_value() || imageUrl->empty()) {
            upload.errorMessage = "Failed to upload the image, please try again.";
            upload.errorStatus = k502BadGateway;
            upload.errorCode = raffle::constants::ERR_INTERNAL;
            co_return upload;
        }

        upload.success = true;
        upload.url = *imageUrl;
        co_return upload;
    }

}

Task<HttpResponsePtr> CmsController::createHeroSlides(HttpRequestPtr req) {

    MultiPartParser parser;
    if (parser.parse(req) != 0) {
        co_return jsonError("The request must be sent as multipart/form-data with the slide fields and the image file.");
    }

    raffle::dto::HeroSliderDto dto;
    dto.setHeadline(formField(parser, "headline"));
    dto.setCopy(formField(parser, "copy"));
    dto.setCtaPrimary(formField(parser, "ctaPrimary"));
    dto.setCtaSecondary(formField(parser, "ctaSecondary"));

    // validate the text fields before spending an upload on cloudinary
    if (dto.getHeadline().empty()) {
        co_return jsonError("Headline is required to create a hero slide.");
    }

    if (auto displayOrder = formIntField(parser, "displayOrder"); displayOrder.has_value()) {
        dto.setDisplayOrder(*displayOrder);
    } else if (!formField(parser, "displayOrder").empty()) {
        co_return jsonError("displayOrder must be a number.");
    }

    if (auto isActive = formBoolField(parser, "isActive"); isActive.has_value()) {
        dto.setIsActive(*isActive);
    } else if (!formField(parser, "isActive").empty()) {
        co_return jsonError("isActive must be either true or false.");
    }

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();

    // the image goes to cloudinary, the returned url is what we persist
    auto upload = co_await uploadHeroSlideImage(parser, plugin->getCloudinaryApi());

    if (!upload.provided) {
        co_return jsonError("An image file is required to create a hero slide.");
    }

    if (!upload.success) {
        co_return jsonError(upload.errorMessage, upload.errorStatus, upload.errorCode);
    }

    dto.setImageUrl(upload.url);

    auto result = co_await plugin->getHeroSliderService().create(dto);

    if (!result.success) {
        co_return jsonResult(result);
    }

    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(k201Created);
    co_return resp;

}


Task<HttpResponsePtr> CmsController::updateHeroSlides(HttpRequestPtr req, std::string id) {

    if (id.empty()) {
        co_return jsonError("Hero slide id is required.");
    }

    MultiPartParser parser;
    const bool isMultipart = parser.parse(req) == 0;

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();

    if (!isMultipart) {
        // A plain json body is still accepted for updates that do not change
        // the image, so existing clients keep working.
        auto jsonBody = req->getJsonObject();
        if (!jsonBody) {
            co_return jsonError("Send the slide fields as multipart/form-data, "
                                "including an image file when the image has to change.");
        }

        raffle::dto::HeroSliderDto jsonDto;
        jsonDto.fromJson(*jsonBody);

        auto &sliderService = plugin->getHeroSliderService();
        co_return jsonResult(co_await sliderService.update(jsonDto, id));
    }

    raffle::dto::HeroSliderDto dto;

    // when an image is part of the form it replaces the stored one
    auto upload = co_await uploadHeroSlideImage(parser, plugin->getCloudinaryApi());

    if (upload.provided && !upload.success) {
        co_return jsonError(upload.errorMessage, upload.errorStatus, upload.errorCode);
    }

    if (upload.success) {
        dto.setImageUrl(upload.url);
    }

    // every other field is optional, only the ones that were supplied are updated
    if (formHasField(parser, "headline")) {
        if (formField(parser, "headline").empty()) {
            co_return jsonError("Headline cannot be empty.");
        }
        dto.setHeadline(formField(parser, "headline"));
    }
    if (formHasField(parser, "copy")) {
        dto.setCopy(formField(parser, "copy"));
    }
    if (formHasField(parser, "ctaPrimary")) {
        dto.setCtaPrimary(formField(parser, "ctaPrimary"));
    }
    if (formHasField(parser, "ctaSecondary")) {
        dto.setCtaSecondary(formField(parser, "ctaSecondary"));
    }
    if (formHasField(parser, "displayOrder")) {
        auto displayOrder = formIntField(parser, "displayOrder");
        if (!displayOrder.has_value()) {
            co_return jsonError("displayOrder must be a number.");
        }
        dto.setDisplayOrder(*displayOrder);
    }
    if (formHasField(parser, "isActive")) {
        auto isActive = formBoolField(parser, "isActive");
        if (!isActive.has_value()) {
            co_return jsonError("isActive must be either true or false.");
        }
        dto.setIsActive(*isActive);
    }

    auto result = co_await plugin->getHeroSliderService().update(dto, id);
    co_return jsonResult(result);

}


Task<HttpResponsePtr> CmsController::deleteHeroSlides(HttpRequestPtr req, std::string id) {

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &heroSliderService = plugin->getHeroSliderService();

    auto result = co_await heroSliderService.deleteHeroSlider(id);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : (result.error.isMember("code") && result.error["code"].asInt() == raffle::constants::ERR_RESOURCE_NOT_FOUND ? k404NotFound : k500InternalServerError));
    co_return resp;

}


Task<HttpResponsePtr> CmsController::getAllBanners(HttpRequestPtr req) {

    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    std::string query = req->getParameter("query");
    if (query.empty()) {
        query = "";
    }


    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &bannerService = plugin->getBannerService();

    auto result = co_await bannerService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}


Task<HttpResponsePtr> CmsController::createBanner(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::BannerDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &bannerService = plugin->getBannerService();

    auto result = co_await bannerService.create(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}


Task<HttpResponsePtr> CmsController::updateBanner(HttpRequestPtr req, std::string id) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::BannerDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &bannerService = plugin->getBannerService();

    auto result = co_await bannerService.update(dto,id);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;


}


Task<HttpResponsePtr> CmsController::deleteBanner(HttpRequestPtr req, std::string id) {

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &bannerService = plugin->getBannerService();

    auto result = co_await bannerService.deleteBanner(id);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : (result.error.isMember("code") && result.error["code"].asInt() == raffle::constants::ERR_RESOURCE_NOT_FOUND ? k404NotFound : k500InternalServerError));
    co_return resp;

}


Task<HttpResponsePtr> CmsController::getAllFaqs(HttpRequestPtr req) {

    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    std::string query = req->getParameter("query");
    if (query.empty()) {
        query = "";
    }


    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &faqService = plugin->getFaqService();

    auto result = co_await faqService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}


Task<HttpResponsePtr> CmsController::createFaqs(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::FaqDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &faqService = plugin->getFaqService();

    auto result = co_await faqService.create(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}


Task<HttpResponsePtr> CmsController::updateFaqs(HttpRequestPtr req, std::string id) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::FaqDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &faqService = plugin->getFaqService();

    auto result = co_await faqService.update(dto, id);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}


Task<HttpResponsePtr> CmsController::deleteFaqs(HttpRequestPtr req, std::string id) {

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &faqService = plugin->getFaqService();

    auto result = co_await faqService.deleteFaq(id);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : (result.error.isMember("code") && result.error["code"].asInt() == raffle::constants::ERR_RESOURCE_NOT_FOUND ? k404NotFound : k500InternalServerError));
    co_return resp;

}


Task<HttpResponsePtr> CmsController::getAllPages(HttpRequestPtr req) {

    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    std::string query = req->getParameter("query");
    if (query.empty()) {
        query = "";
    }

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &pageService = plugin->getPageService();

    auto result = co_await pageService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}

Task<HttpResponsePtr> CmsController::createPages(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::PageDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &pageService = plugin->getPageService();

    auto result = co_await pageService.create(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}

Task<HttpResponsePtr> CmsController::updatePages(HttpRequestPtr req, std::string id) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::PageDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &pageService = plugin->getPageService();

    auto result = co_await pageService.update(dto, id);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}

Task<HttpResponsePtr> CmsController::deletePages(HttpRequestPtr req, std::string id) {

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &pageService = plugin->getPageService();

    auto result = co_await pageService.deletePage(id);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : (result.error.isMember("code") && result.error["code"].asInt() == raffle::constants::ERR_RESOURCE_NOT_FOUND ? k404NotFound : k500InternalServerError));
    co_return resp;

}


Task<HttpResponsePtr> CmsController::getAllMediaLibraryAssets(HttpRequestPtr req) {

    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    std::string query = req->getParameter("query");
    if (query.empty()) {
        query = "";
    }

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &mediaLibraryService = plugin->getMediaLibraryService();

    auto result = co_await mediaLibraryService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}


Task<HttpResponsePtr> CmsController::addMediaLibraryAsset(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::MediaAssetDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &mediaLibraryService = plugin->getMediaLibraryService();

    auto result = co_await mediaLibraryService.create(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}


Task<HttpResponsePtr> CmsController::updateMediaLibraryAsset(HttpRequestPtr req, std::string id) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::MediaAssetDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &mediaLibraryService = plugin->getMediaLibraryService();

    auto result = co_await mediaLibraryService.update(dto, id);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}


Task<HttpResponsePtr> CmsController::deleteMediaLibraryAsset(HttpRequestPtr req, std::string id) {

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &mediaLibraryService = plugin->getMediaLibraryService();

    auto result = co_await mediaLibraryService.deleteMediaAsset(id);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : (result.error.isMember("code") && result.error["code"].asInt() == raffle::constants::ERR_RESOURCE_NOT_FOUND ? k404NotFound : k500InternalServerError));
    co_return resp;

}


Task<HttpResponsePtr> CmsController::getTicketGroups(HttpRequestPtr req) {

    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &ticketCollectionService = plugin->getTicketCollectionService();

    auto result = co_await ticketCollectionService.getAllActive(pageNo, pageSize, "");
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}


Task<HttpResponsePtr> CmsController::getPrizeCategories(HttpRequestPtr req) {

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &prizeConfigService = plugin->getPrizeConfigService();

    auto result = co_await prizeConfigService.getAllCategories(1, 100, "");
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}

Task<HttpResponsePtr> CmsController::getActiveRaffles(HttpRequestPtr req) {

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &raffleService = plugin->getRaffleService();

    auto result = co_await raffleService.getActiveRaffles();
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}


Task<HttpResponsePtr> CmsController::getActiveRaffleOptions(HttpRequestPtr req) {

    auto raffleId = req->getParameter("raffleId");

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &raffleService = plugin->getRaffleService();

    auto result = co_await raffleService.getActiveRaffleOptions(raffleId);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}

Task<HttpResponsePtr> CmsController::getDrawHistory(HttpRequestPtr req) {

    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &raffleService = plugin->getRaffleService();

    auto result = co_await raffleService.getDrawHistory(pageNo, pageSize);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}


Task<HttpResponsePtr> CmsController::getRecentDraws(HttpRequestPtr req) {

    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &drawService = plugin->getDrawService();

    auto result = co_await drawService.getRecentDraws(pageNo, pageSize);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}

Task<HttpResponsePtr> CmsController::getRecentWinners(HttpRequestPtr req) {


    int pageNo = 1;
    int pageSize = 10;

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        pageNo = std::stoi(pageNoStr);
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        pageSize = std::stoi(pageSizeStr);
    }

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &drawService = plugin->getDrawService();

    auto result = co_await drawService.getRecentWinners(pageNo, pageSize);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}