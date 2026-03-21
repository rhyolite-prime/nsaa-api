#include "UsersController.h"
#include "constants/ErrorCodes.h"
#include "plugins/RaffleServicePlugin.h"

Task<HttpResponsePtr> UsersController::getAll(HttpRequestPtr req)
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
    auto &userService = plugin->getUserService();

    auto result = co_await userService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}


Task<HttpResponsePtr> UsersController::getStats(HttpRequestPtr req) {

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &userService = plugin->getUserService();

    auto result = co_await userService.getStats();
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}

Task<HttpResponsePtr> UsersController::getPermissions(HttpRequestPtr req) {

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &userService = plugin->getUserService();

    auto result = co_await userService.getPermissions();
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}

Task<HttpResponsePtr> UsersController::create(HttpRequestPtr req)
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

    raffle::dto::CreateUserDto roleDto;
    roleDto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &userService = plugin->getUserService();

    auto result = co_await userService.createAsync(roleDto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}


Task<HttpResponsePtr> UsersController::update(HttpRequestPtr req)
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

    raffle::dto::UpdateUserDto roleDto;
    roleDto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &userService = plugin->getUserService();

    auto result = co_await userService.updateAsync(roleDto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}


Task<HttpResponsePtr> UsersController::deleteUser(HttpRequestPtr req)
{
    auto userId = req->getParameter("id");

    if (userId.empty()) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "User ID is required";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &roleService = plugin->getRoleService();

    auto result = co_await roleService.deleteRole(userId);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : (result.error.isMember("code") && result.error["code"].asInt() == raffle::constants::ERR_RESOURCE_NOT_FOUND ? k404NotFound : k500InternalServerError));
    co_return resp;
}