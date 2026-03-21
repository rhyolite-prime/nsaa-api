#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class RolesController : public drogon::HttpController<RolesController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/roles";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RolesController::getAll, std::string(PREFIX) + "/get-all", Get, Options);
    ADD_METHOD_TO(RolesController::create, std::string(PREFIX) + "/create", Post, Options);
    ADD_METHOD_TO(RolesController::update, std::string(PREFIX) + "/update", Post, Options);
    ADD_METHOD_TO(RolesController::deleteRole, std::string(PREFIX) + "/delete", Delete, Options);
    METHOD_LIST_END

    drogon::Task<HttpResponsePtr> getAll(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> create(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> update(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> deleteRole(HttpRequestPtr req);
};
