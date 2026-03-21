#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class UsersController : public drogon::HttpController<UsersController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/users";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(UsersController::getAll, std::string(PREFIX) + "/get-all", Get, Options);
    ADD_METHOD_TO(UsersController::getPermissions, std::string(PREFIX) + "/get-permissions", Get, Options);
    ADD_METHOD_TO(UsersController::getStats, std::string(PREFIX) + "/get-stats", Get, Options);
    ADD_METHOD_TO(UsersController::create, std::string(PREFIX) + "/create", Post, Options);
    ADD_METHOD_TO(UsersController::update, std::string(PREFIX) + "/update", Post, Options);
    ADD_METHOD_TO(UsersController::deleteUser, std::string(PREFIX) + "/delete", Delete, Options);
    METHOD_LIST_END

    Task<HttpResponsePtr> getAll(HttpRequestPtr req);
    Task<HttpResponsePtr> getPermissions(HttpRequestPtr req);
    Task<HttpResponsePtr> getStats(HttpRequestPtr req);
    Task<HttpResponsePtr> create(HttpRequestPtr req);
    Task<HttpResponsePtr> update(HttpRequestPtr req);
    Task<HttpResponsePtr> deleteUser(HttpRequestPtr req);
};
