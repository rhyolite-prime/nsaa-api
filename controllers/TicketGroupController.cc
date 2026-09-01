#include "TicketGroupController.h"
#include "constants/ErrorCodes.h"
#include "plugins/RaffleServicePlugin.h"


Task<HttpResponsePtr> TicketGroupController::getAll(HttpRequestPtr req)
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
    auto &ticketCollectionService = plugin->getTicketCollectionService();

    auto result = co_await ticketCollectionService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}

Task<HttpResponsePtr> TicketGroupController::create(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::TicketCollectionDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &ticketCollectionService = plugin->getTicketCollectionService();

    auto result = co_await ticketCollectionService.create(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}

Task<HttpResponsePtr> TicketGroupController::update(HttpRequestPtr req, const std::string &id) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::TicketCollectionDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &ticketCollectionService = plugin->getTicketCollectionService();

    auto result = co_await ticketCollectionService.update(dto, id);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}

Task<HttpResponsePtr> TicketGroupController::deleteItem(HttpRequestPtr req, const std::string &id) {

    if (id.empty()) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Color ID is required";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &ticketCollectionService = plugin->getTicketCollectionService();

    auto result = co_await ticketCollectionService.deleteItem(id);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : (result.error.isMember("code") && result.error["code"].asInt() == raffle::constants::ERR_RESOURCE_NOT_FOUND ? k404NotFound : k500InternalServerError));
    co_return resp;
}
