#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class CustomersController : public drogon::HttpController<CustomersController> {
public:
  static constexpr const char *PREFIX = "/api/v1/customers/";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(CustomersController::getAll, std::string(PREFIX) + "get-all", Get, Options);
  ADD_METHOD_TO(CustomersController::getStats, std::string(PREFIX) + "get-all-stats", Get, Options);

  ADD_METHOD_TO(CustomersController::getProfileDetails, std::string(PREFIX) + "profile-details", Get, Options, "CustomerJwtAuthFilter");
  ADD_METHOD_TO(CustomersController::getCustomerStats, std::string(PREFIX) + "get-stats", Get, Options, "CustomerJwtAuthFilter");
  ADD_METHOD_TO(CustomersController::getTransactionHistory, std::string(PREFIX) + "get-transaction-history", Get, Options, "CustomerJwtAuthFilter");
  ADD_METHOD_TO(CustomersController::buyTicket, std::string(PREFIX) + "buy-ticket", Post, Options, "CustomerJwtAuthFilter");
  ADD_METHOD_TO(CustomersController::buyGuestTicket, std::string(PREFIX) + "buy-guest-ticket", Post, Options);
  ADD_METHOD_TO(CustomersController::getTickets, std::string(PREFIX) + "get-tickets", Get, Options, "CustomerJwtAuthFilter");
  ADD_METHOD_TO(CustomersController::getRaffleHistory, std::string(PREFIX) + "get-raffle-history", Get, Options, "CustomerJwtAuthFilter");
  METHOD_LIST_END

  drogon::Task<HttpResponsePtr> getAll(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> getStats(HttpRequestPtr req);

  //customer dashboard
  drogon::Task<HttpResponsePtr> getProfileDetails(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> getCustomerStats(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> getTransactionHistory(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> buyTicket(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> buyGuestTicket(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> getTickets(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> getRaffleHistory(HttpRequestPtr req);

};
