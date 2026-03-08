#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class PriceTiersController : public drogon::HttpController<PriceTiersController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/price-tiers";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(PriceTiersController::getAll, std::string(PREFIX) + "/get-all", Get, Options);
    ADD_METHOD_TO(PriceTiersController::create, std::string(PREFIX) + "/create", Post, Options);
    ADD_METHOD_TO(PriceTiersController::update, std::string(PREFIX) + "/update", Post, Options);
    ADD_METHOD_TO(PriceTiersController::deletePriceTier, std::string(PREFIX) + "/delete", Delete, Options);
    METHOD_LIST_END

    drogon::Task<HttpResponsePtr> getAll(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> create(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> update(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> deletePriceTier(HttpRequestPtr req);
};
