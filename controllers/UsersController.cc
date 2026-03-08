#include "UsersController.h"

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
    auto &colorService = plugin->getColorService();

    auto result = co_await colorService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}


Task<HttpResponsePtr> UsersController::create(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Password set successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;
}


Task<HttpResponsePtr> UsersController::update(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Password set successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;
}


Task<HttpResponsePtr> UsersController::deleteUser(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Password set successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;
}