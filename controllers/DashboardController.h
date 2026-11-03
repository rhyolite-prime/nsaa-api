#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class DashboardController : public drogon::HttpController<DashboardController> {
public:
  static constexpr const char *PREFIX = "/api/v1/dashboard/";
  METHOD_LIST_BEGIN
    ADD_METHOD_TO(DashboardController::getOverview, std::string(PREFIX) + "overview", Get, Options);
    ADD_METHOD_TO(DashboardController::getColourBreakdown, std::string(PREFIX) + "colour-breakdown", Get, Options);
    ADD_METHOD_TO(DashboardController::getDrawStats, std::string(PREFIX) + "draw-stats", Get, Options);
    ADD_METHOD_TO(DashboardController::getTopParticipants, std::string(PREFIX) + "top-participants", Get, Options);
    ADD_METHOD_TO(DashboardController::getRevenueTrend, std::string(PREFIX) + "revenue-trend", Get, Options);
  METHOD_LIST_END

  Task<HttpResponsePtr> getOverview(HttpRequestPtr req);
  Task<HttpResponsePtr> getColourBreakdown(HttpRequestPtr req);
  Task<HttpResponsePtr> getDrawStats(HttpRequestPtr req);
  Task<HttpResponsePtr> getTopParticipants(HttpRequestPtr req);
  Task<HttpResponsePtr> getRevenueTrend(HttpRequestPtr req);
};
