#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class PrizeConfigController : public drogon::HttpController<PrizeConfigController>
{
  public:
  static constexpr const char *PREFIX = "/api/v1/prize-config/";
  METHOD_LIST_BEGIN

  ADD_METHOD_TO(PrizeConfigController::getAllCategories, std::string(PREFIX) + "get-all-categories", Get, Options);
  ADD_METHOD_TO(PrizeConfigController::createCategory, std::string(PREFIX) + "create-category", Post, Options);
  ADD_METHOD_TO(PrizeConfigController::updateCategory, std::string(PREFIX) + "update-category/{1}", Put, Options);
  ADD_METHOD_TO(PrizeConfigController::deleteCategory, std::string(PREFIX) + "delete-category/{1}", Delete, Options);

  ADD_METHOD_TO(PrizeConfigController::getAllPrizes, std::string(PREFIX) + "get-all-prizes", Get, Options);
  ADD_METHOD_TO(PrizeConfigController::getCategoryPrizes, std::string(PREFIX) + "get-category-prizes/{1}", Get, Options);
  ADD_METHOD_TO(PrizeConfigController::createPrize, std::string(PREFIX) + "create-prize", Post, Options);
  ADD_METHOD_TO(PrizeConfigController::updatePrize, std::string(PREFIX) + "update-prize/{1}", Put, Options);
  ADD_METHOD_TO(PrizeConfigController::deletePrize, std::string(PREFIX) + "delete-prize/{1}", Delete, Options);
  METHOD_LIST_END

  //
  drogon::Task<HttpResponsePtr> getAllCategories(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> createCategory(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> updateCategory(HttpRequestPtr req, const std::string &categoryId);
  drogon::Task<HttpResponsePtr> deleteCategory(HttpRequestPtr req, const std::string &categoryId);

  //prizes

  drogon::Task<HttpResponsePtr> getAllPrizes(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> getCategoryPrizes(HttpRequestPtr req, const std::string &categoryId);
  drogon::Task<HttpResponsePtr> createPrize(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> updatePrize(HttpRequestPtr req, const std::string &prizeId);
  drogon::Task<HttpResponsePtr> deletePrize(HttpRequestPtr req, const std::string &prizeId);
};
