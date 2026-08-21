#include "PrizeConfigController.h"

#include "plugins/RaffleServicePlugin.h"

drogon::Task<HttpResponsePtr> PrizeConfigController::getAllCategories(HttpRequestPtr req) {

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
    auto &prizeConfigService = plugin->getPrizeConfigService();

    auto result = co_await prizeConfigService.getAllCategories(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;


}

drogon::Task<HttpResponsePtr> PrizeConfigController::createCategory(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::PrizeCategoryDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &prizeConfigService = plugin->getPrizeConfigService();

    auto result = co_await prizeConfigService.createCategory(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}

drogon::Task<HttpResponsePtr> PrizeConfigController::updateCategory(HttpRequestPtr req, const std::string &categoryId) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::PrizeCategoryDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &prizeConfigService = plugin->getPrizeConfigService();

    auto result = co_await prizeConfigService.updateCategory(dto, categoryId);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}

drogon::Task<HttpResponsePtr> PrizeConfigController::deleteCategory(HttpRequestPtr req, const std::string &categoryId) {

    if (categoryId.empty()) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Category ID is required";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &prizeConfigService = plugin->getPrizeConfigService();

    auto result = co_await prizeConfigService.deleteCategory(categoryId);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}


drogon::Task<HttpResponsePtr> PrizeConfigController::getAllPrizes(HttpRequestPtr req) {

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
    auto &prizeConfigService = plugin->getPrizeConfigService();

    auto result = co_await prizeConfigService.getAllPrizes(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}

drogon::Task<HttpResponsePtr> PrizeConfigController::getCategoryPrizes(HttpRequestPtr req, const std::string &categoryId) {

    if (categoryId.empty()) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Category ID is required";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &prizeConfigService = plugin->getPrizeConfigService();

    auto result = co_await prizeConfigService.getCategoryPrizes(categoryId);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}

drogon::Task<HttpResponsePtr> PrizeConfigController::createPrize(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::PrizeDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &prizeConfigService = plugin->getPrizeConfigService();

    auto result = co_await prizeConfigService.createPrize(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}


drogon::Task<HttpResponsePtr> PrizeConfigController::updatePrize(HttpRequestPtr req, const std::string &prizeId) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::PrizeDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &prizeConfigService = plugin->getPrizeConfigService();

    auto result = co_await prizeConfigService.updatePrize(dto, prizeId);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}


drogon::Task<HttpResponsePtr> PrizeConfigController::deletePrize(HttpRequestPtr req, const std::string &prizeId) {

    if (prizeId.empty()) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Prize ID is required";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &prizeConfigService = plugin->getPrizeConfigService();

    auto result = co_await prizeConfigService.deletePrize(prizeId);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}
