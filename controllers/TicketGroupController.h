#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class TicketGroupController : public drogon::HttpController<TicketGroupController>
{
  public:
  static constexpr const char *PREFIX = "/api/v1/ticket-group/";
  METHOD_LIST_BEGIN
      ADD_METHOD_TO(TicketGroupController::getAll, std::string(PREFIX) + "get-all", Get, Options);
      ADD_METHOD_TO(TicketGroupController::getAll, std::string(PREFIX) + "list-all", Get, Options); // no auth
      ADD_METHOD_TO(TicketGroupController::create, std::string(PREFIX) + "create", Post, Options);
      ADD_METHOD_TO(TicketGroupController::update, std::string(PREFIX) + "update/{1}", Post, Options);
      ADD_METHOD_TO(TicketGroupController::deleteItem, std::string(PREFIX) + "delete/{1}", Delete, Options);
  METHOD_LIST_END

  Task<HttpResponsePtr> getAll(HttpRequestPtr req);
  Task<HttpResponsePtr> create(HttpRequestPtr req);
  Task<HttpResponsePtr> update(HttpRequestPtr req, const std::string &id);
  Task<HttpResponsePtr> deleteItem(HttpRequestPtr req, const std::string &id);
};
