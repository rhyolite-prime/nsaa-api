#include "AffiliatePartnerController.h"
#include "plugins/RaffleServicePlugin.h"
#include "dto/BaseApiResponse.h"
#include "constants/ErrorCodes.h"


Task<HttpResponsePtr> AffiliatePartnerController::getAll(HttpRequestPtr req)
{
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
    auto &affiliatePartnerService = plugin->getAffiliatePartnerService();

    auto result = co_await affiliatePartnerService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}


Task<HttpResponsePtr> AffiliatePartnerController::getStats(HttpRequestPtr req) {

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &affiliatePartnerService = plugin->getAffiliatePartnerService();

    auto result = co_await affiliatePartnerService.getPartnerStats();
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}


Task<HttpResponsePtr> AffiliatePartnerController::create(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::AffiliatePartnerDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &affiliatePartnerService = plugin->getAffiliatePartnerService();

    auto result = co_await affiliatePartnerService.create(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}

Task<HttpResponsePtr> AffiliatePartnerController::update(HttpRequestPtr req, const std::string &partnerId) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::AffiliatePartnerDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &affiliatePartnerService = plugin->getAffiliatePartnerService();

    auto result = co_await affiliatePartnerService.update(dto,partnerId);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}

Task<HttpResponsePtr> AffiliatePartnerController::deletePartner(HttpRequestPtr req) {

    auto partnerId = req->getParameter("id");

    if (partnerId.empty()) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Partner ID is required";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &affiliatePartnerService = plugin->getAffiliatePartnerService();

    auto result = co_await affiliatePartnerService.deletePartner(partnerId);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : (result.error.isMember("code") && (result.error["code"].asInt() == raffle::constants::ERR_RESOURCE_NOT_FOUND || result.error["code"].asInt() == raffle::constants::ERR_DB_NOT_FOUND) ? k404NotFound : k500InternalServerError));
    co_return resp;
}
