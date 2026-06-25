#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class TicketsController : public drogon::HttpController<TicketsController> {
public:
  static constexpr const char *PREFIX = "/api/v1/ticket/";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(TicketsController::getAll, std::string(PREFIX) + "get-all", Get, Options);
  ADD_METHOD_TO(TicketsController::getStats, std::string(PREFIX) + "get-stats", Get, Options);
  ADD_METHOD_TO(TicketsController::create, std::string(PREFIX) + "buy", Post, Options);
  //ADD_METHOD_TO(TicketsController::buyTicket, std::string(PREFIX) + "buy", Post, Options);
  METHOD_LIST_END

  Task<HttpResponsePtr> getAll(HttpRequestPtr req);
  Task<HttpResponsePtr> getStats(HttpRequestPtr req);
  Task<HttpResponsePtr> create(HttpRequestPtr req);


};
