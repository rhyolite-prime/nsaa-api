#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class CmsController : public drogon::HttpController<CmsController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/cms/";
    METHOD_LIST_BEGIN
    //public
    ADD_METHOD_TO(CmsController::getHeroSlides, std::string(PREFIX) + "hero-slides", Get, Options);
    ADD_METHOD_TO(CmsController::getPromotionalBanners, std::string(PREFIX) + "campaigns", Get, Options);
    ADD_METHOD_TO(CmsController::getPrizes, std::string(PREFIX) + "prizes", Get, Options);
    ADD_METHOD_TO(CmsController::getFaqs, std::string(PREFIX) + "faq", Get, Options);
    ADD_METHOD_TO(CmsController::getPages, std::string(PREFIX) + "pages/{1}", Get, Options);
    ADD_METHOD_TO(CmsController::getMediaAssets, std::string(PREFIX) + "media-assets", Get, Options);
    //admin
    //hero-slides
    ADD_METHOD_TO(CmsController::getAllHeroSlides, std::string(PREFIX) + "hero-slides/get-all", Get, Options);
    ADD_METHOD_TO(CmsController::createHeroSlides, std::string(PREFIX) + "hero-slides/create", Post, Options);
    ADD_METHOD_TO(CmsController::updateHeroSlides, std::string(PREFIX) + "hero-slides/update/{1}", Put, Options);
    ADD_METHOD_TO(CmsController::deleteHeroSlides, std::string(PREFIX) + "hero-slides/delete/{1}", Delete, Options);
    //banners
    ADD_METHOD_TO(CmsController::getAllBanners, std::string(PREFIX) + "banners/get-all", Get, Options);
    ADD_METHOD_TO(CmsController::createBanner, std::string(PREFIX) + "banners/create", Post, Options);
    ADD_METHOD_TO(CmsController::updateBanner, std::string(PREFIX) + "banners/update/{1}", Put, Options);
    ADD_METHOD_TO(CmsController::deleteBanner, std::string(PREFIX) + "banners/delete/{1}", Delete, Options);
    //faqs
    ADD_METHOD_TO(CmsController::getAllFaqs, std::string(PREFIX) + "faqs/get-all", Get, Options);
    ADD_METHOD_TO(CmsController::createFaqs, std::string(PREFIX) + "faqs/create", Post, Options);
    ADD_METHOD_TO(CmsController::updateFaqs, std::string(PREFIX) + "faqs/update/{1}", Put, Options);
    ADD_METHOD_TO(CmsController::deleteFaqs, std::string(PREFIX) + "faqs/delete/{1}", Delete, Options);

    //pages
    ADD_METHOD_TO(CmsController::getAllPages, std::string(PREFIX) + "pages/get-all", Get, Options);
    ADD_METHOD_TO(CmsController::createPages, std::string(PREFIX) + "pages/create", Post, Options);
    ADD_METHOD_TO(CmsController::updatePages, std::string(PREFIX) + "pages/update/{1}", Put, Options);
    ADD_METHOD_TO(CmsController::deletePages, std::string(PREFIX) + "pages/delete/{1}", Delete, Options);

    //media assets
    ADD_METHOD_TO(CmsController::getAllMediaLibraryAssets, std::string(PREFIX) + "media-library/get-all", Get, Options);
    ADD_METHOD_TO(CmsController::addMediaLibraryAsset, std::string(PREFIX) + "media-library/add-asset", Post, Options);
    ADD_METHOD_TO(CmsController::updateMediaLibraryAsset, std::string(PREFIX) + "media-library/update-asset/{1}", Put, Options);
    ADD_METHOD_TO(CmsController::deleteMediaLibraryAsset, std::string(PREFIX) + "media-library/delete-asset/{1}", Delete, Options);

    METHOD_LIST_END

    //public
    Task<HttpResponsePtr> getHeroSlides(HttpRequestPtr req);
    Task<HttpResponsePtr> getPromotionalBanners(HttpRequestPtr req);
    Task<HttpResponsePtr> getPrizes(HttpRequestPtr req);
    Task<HttpResponsePtr> getFaqs(HttpRequestPtr req);
    Task<HttpResponsePtr> getPages(HttpRequestPtr req, std::string slug);
    Task<HttpResponsePtr> getMediaAssets(HttpRequestPtr req);

    //admin
    //hero-slides
    Task<HttpResponsePtr> getAllHeroSlides(HttpRequestPtr req);
    Task<HttpResponsePtr> createHeroSlides(HttpRequestPtr req);
    Task<HttpResponsePtr> updateHeroSlides(HttpRequestPtr req, std::string id);
    Task<HttpResponsePtr> deleteHeroSlides(HttpRequestPtr req, std::string id);

    //banners
    Task<HttpResponsePtr> getAllBanners(HttpRequestPtr req);
    Task<HttpResponsePtr> createBanner(HttpRequestPtr req);
    Task<HttpResponsePtr> updateBanner(HttpRequestPtr req, std::string id);
    Task<HttpResponsePtr> deleteBanner(HttpRequestPtr req, std::string id);

    //faqs
    Task<HttpResponsePtr> getAllFaqs(HttpRequestPtr req);
    Task<HttpResponsePtr> createFaqs(HttpRequestPtr req);
    Task<HttpResponsePtr> updateFaqs(HttpRequestPtr req, std::string id);
    Task<HttpResponsePtr> deleteFaqs(HttpRequestPtr req, std::string id);

    //pages
    Task<HttpResponsePtr> getAllPages(HttpRequestPtr req);
    Task<HttpResponsePtr> createPages(HttpRequestPtr req);
    Task<HttpResponsePtr> updatePages(HttpRequestPtr req, std::string id);
    Task<HttpResponsePtr> deletePages(HttpRequestPtr req, std::string id);

    //media-assets
    Task<HttpResponsePtr> getAllMediaLibraryAssets(HttpRequestPtr req);
    Task<HttpResponsePtr> addMediaLibraryAsset(HttpRequestPtr req);
    Task<HttpResponsePtr> updateMediaLibraryAsset(HttpRequestPtr req, std::string id);
    Task<HttpResponsePtr> deleteMediaLibraryAsset(HttpRequestPtr req, std::string id);

};
