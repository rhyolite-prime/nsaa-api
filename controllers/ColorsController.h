#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class ColorsController : public drogon::HttpController<ColorsController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/colours";
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ColorsController::getAll, std::string(PREFIX) + "/get-all", Get, Options);
        ADD_METHOD_TO(ColorsController::create, std::string(PREFIX) + "/create", Post, Options);
        ADD_METHOD_TO(ColorsController::update, std::string(PREFIX) + "/update", Post, Options);
        ADD_METHOD_TO(ColorsController::deleteColor, std::string(PREFIX) + "/delete", Delete, Options);
    METHOD_LIST_END

    drogon::Task<HttpResponsePtr> getAll(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> create(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> update(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> deleteColor(HttpRequestPtr req);
};
