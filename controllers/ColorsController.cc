#include "ColorsController.h"
#include "constants/ErrorCodes.h"
#include "plugins/RaffleServicePlugin.h"

using namespace raffle;

Task<HttpResponsePtr> ColorsController::getAll(HttpRequestPtr req)
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
    auto &colorService = plugin->getColorService();

    auto result = co_await colorService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}


Task<HttpResponsePtr> ColorsController::create(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    dto::CreateColorDto colorDto;
    colorDto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<plugins::RaffleServicePlugin>();
    auto &colorService = plugin->getColorService();

    auto result = co_await colorService.createAsync(colorDto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}

Task<HttpResponsePtr> ColorsController::update(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    dto::UpdateColorDto colorDto;
    colorDto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<plugins::RaffleServicePlugin>();
    auto &colorService = plugin->getColorService();

    auto result = co_await colorService.updateAsync(colorDto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}

Task<HttpResponsePtr> ColorsController::deleteColor(HttpRequestPtr req) {

    auto colourId = req->getParameter("id");

    if (colourId.empty()) {
        dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Color ID is required";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = drogon::app().getPlugin<plugins::RaffleServicePlugin>();
    auto &colourService = plugin->getColorService();

    auto result = co_await colourService.deleteColor(colourId);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : (result.error.isMember("code") && result.error["code"].asInt() == constants::ERR_RESOURCE_NOT_FOUND ? k404NotFound : k500InternalServerError));
    co_return resp;
}