#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class CustomersController : public drogon::HttpController<CustomersController> {
public:
  static constexpr const char *PREFIX = "/api/v1/customers";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(CustomersController::getAll, std::string(PREFIX) + "/get-all",
                Get, Options);
  ADD_METHOD_TO(CustomersController::getStats,
                std::string(PREFIX) + "/get-stats", Get, Options);
  METHOD_LIST_END

  drogon::Task<HttpResponsePtr> getAll(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> getStats(HttpRequestPtr req);
};
