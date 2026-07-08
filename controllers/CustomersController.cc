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

  auto customerId = req->attributes()->get<std::string>("customerId");

  auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &customerService = plugin->getCustomerService();

  auto result = co_await customerService.getProfileDetails(customerId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;


}


drogon::Task<HttpResponsePtr> CustomersController::getCustomerStats(HttpRequestPtr req) {

  auto customerId = req->attributes()->get<std::string>("customerId");

  auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &customerService = plugin->getCustomerService();

  auto result = co_await customerService.getStats(customerId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

drogon::Task<HttpResponsePtr> CustomersController::getTransactionHistory(HttpRequestPtr req) {

  auto customerId = req->attributes()->get<std::string>("customerId");

  auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &customerService = plugin->getCustomerService();

  auto result = co_await customerService.getTransactionHistory(customerId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}


drogon::Task<HttpResponsePtr> CustomersController::buyTicket(HttpRequestPtr req) {

  auto customerId = req->attributes()->get<std::string>("customerId");

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  raffle::dto::InitializeTicketPurchaseDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &customerService = plugin->getCustomerService();

  auto result = co_await customerService.initTicketPurchase(dto,customerId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}


drogon::Task<HttpResponsePtr> CustomersController::buyGuestTicket(HttpRequestPtr req) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  raffle::dto::InitializeTicketPurchaseDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &customerService = plugin->getCustomerService();

  auto result = co_await customerService.buyGuestTicket(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

drogon::Task<HttpResponsePtr> CustomersController::getTickets(HttpRequestPtr req) {

  auto customerId = req->attributes()->get<std::string>("customerId");

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

  auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &customerService = plugin->getCustomerService();

  auto result = co_await customerService.getTickets(customerId,pageNo, pageSize, query);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

drogon::Task<HttpResponsePtr> CustomersController::getRaffleHistory(HttpRequestPtr req) {


  auto customerId = req->attributes()->get<std::string>("customerId");

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

  auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &customerService = plugin->getCustomerService();

  auto result = co_await customerService.getRaffleHistory(customerId,pageNo, pageSize, query);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}