#include "CustomersController.h"

#include "dto/BaseApiResponse.h"
#include "plugins/RaffleServicePlugin.h"

Task<HttpResponsePtr> CustomersController::getAll(HttpRequestPtr req) {
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
  auto &customerService = plugin->getCustomerService();

  auto result = co_await customerService.getAll(pageNo, pageSize, query);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> CustomersController::getStats(HttpRequestPtr req) {

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &customerService = plugin->getCustomerService();

    auto result = co_await customerService.getStats();
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

//customer dashboard

drogon::Task<HttpResponsePtr> CustomersController::getProfileDetails(HttpRequestPtr req) {


}


drogon::Task<HttpResponsePtr> CustomersController::getCustomerStats(HttpRequestPtr req) {


}

drogon::Task<HttpResponsePtr> CustomersController::getTransactionHistory(HttpRequestPtr req) {


}