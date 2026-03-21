#include "RolesController.h"
#include "dto/BaseApiResponse.h"
#include "constants/ErrorCodes.h"
#include "plugins/RaffleServicePlugin.h"

Task<HttpResponsePtr> RolesController::getAll(HttpRequestPtr req)
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
    auto &roleService = plugin->getRoleService();

    auto result = co_await roleService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}

Task<HttpResponsePtr> RolesController::create(HttpRequestPtr req)
{
    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::CreateRoleDto roleDto;
    roleDto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &roleService = plugin->getRoleService();

    auto result = co_await roleService.createAsync(roleDto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}

Task<HttpResponsePtr> RolesController::update(HttpRequestPtr req)
{
    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::UpdateRoleDto roleDto;
    roleDto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &roleService = plugin->getRoleService();

    auto result = co_await roleService.updateAsync(roleDto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}


Task<HttpResponsePtr> RolesController::deleteRole(HttpRequestPtr req)
{
    auto colourId = req->getParameter("id");

    if (colourId.empty()) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Role ID is required";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &roleService = plugin->getRoleService();

    auto result = co_await roleService.deleteRole(colourId);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : (result.error.isMember("code") && result.error["code"].asInt() == raffle::constants::ERR_RESOURCE_NOT_FOUND ? k404NotFound : k500InternalServerError));
    co_return resp;
}