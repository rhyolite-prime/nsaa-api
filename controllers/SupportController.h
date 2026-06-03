#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class SupportController : public drogon::HttpController<SupportController>
{
  public:
  static constexpr const char *PREFIX = "/api/v1/support/";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(SupportController::createTicket, std::string(PREFIX) + "create-ticket", Post, Options);
  METHOD_LIST_END

  Task<HttpResponsePtr> createTicket(HttpRequestPtr req);

};
