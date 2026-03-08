#include "PriceTiersController.h"
#include "dto/BaseApiResponse.h"
#include "plugins/RaffleServicePlugin.h"

Task<HttpResponsePtr> PriceTiersController::getAll(HttpRequestPtr req)
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
    auto &priceTierService = plugin->getPriceTierService();

    auto result = co_await priceTierService.getAll(pageNo, pageSize, query);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}

Task<HttpResponsePtr> PriceTiersController::create(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Price Tiers retrieved successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}

Task<HttpResponsePtr> PriceTiersController::update(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Price Tiers retrieved successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}

Task<HttpResponsePtr> PriceTiersController::deletePriceTier(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Price Tiers deleted successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}
