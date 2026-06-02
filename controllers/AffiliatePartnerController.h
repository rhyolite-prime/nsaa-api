#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class AffiliatePartnerController : public drogon::HttpController<AffiliatePartnerController>
{
  public:
  static constexpr const char *PREFIX = "/api/v1/affiliate-partners/";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(AffiliatePartnerController::getAll, std::string(PREFIX) + "get-all", Get, Options);
  ADD_METHOD_TO(AffiliatePartnerController::getStats, std::string(PREFIX) + "get-stats", Get, Options);
  ADD_METHOD_TO(AffiliatePartnerController::create, std::string(PREFIX) + "create", Post, Options);
  ADD_METHOD_TO(AffiliatePartnerController::createDraft, std::string(PREFIX) + "register", Post, Options);
  ADD_METHOD_TO(AffiliatePartnerController::update, std::string(PREFIX) + "update/{1}", Put, Options);
  ADD_METHOD_TO(AffiliatePartnerController::deletePartner, std::string(PREFIX) + "delete", Delete, Options);
  METHOD_LIST_END

  drogon::Task<HttpResponsePtr> getAll(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> getStats(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> create(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> createDraft(HttpRequestPtr req);
  drogon::Task<HttpResponsePtr> update(HttpRequestPtr req, const std::string &partnerId);
  drogon::Task<HttpResponsePtr> deletePartner(HttpRequestPtr req);

};
