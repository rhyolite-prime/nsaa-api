#include "DashboardController.h"

#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include "plugins/RaffleServicePlugin.h"
#include "utils/DateUtils.h"

#include <string>

namespace {

// Extracts an optional query parameter.
std::string param(const HttpRequestPtr &req, const char *name) {
  return req->getParameter(name);
}

// Reads an optional int query parameter with a default.
int intParam(const HttpRequestPtr &req, const char *name, int defaultValue) {
  auto value = req->getParameter(name);
  if (value.empty())
    return defaultValue;
  try {
    return std::stoi(value);
  } catch (...) {
    return defaultValue;
  }
}

// Validates the optional startDate/endDate query params; on failure sends a
// 400 response and returns false.
Task<HttpResponsePtr> validateDates(const HttpRequestPtr &req,
                                    bool &ok) {
  ok = true;
  auto startDate = req->getParameter("startDate");
  auto endDate = req->getParameter("endDate");
  if (!raffle::utils::DateUtils::isValidDateParam(startDate) ||
      !raffle::utils::DateUtils::isValidDateParam(endDate)) {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["code"] = raffle::constants::ERR_VALIDATION;
    response.error["message"] =
        "Invalid date parameter. Use ISO format YYYY-MM-DD.";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    ok = false;
    co_return resp;
  }
  co_return nullptr;
}

} // namespace

Task<HttpResponsePtr> DashboardController::getOverview(HttpRequestPtr req) {
  bool ok = true;
  auto errResp = co_await validateDates(req, ok);
  if (!ok)
    co_return errResp;

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &dashboardService = plugin->getDashboardService();

  auto result = co_await dashboardService.getOverview(param(req, "startDate"),
                                                      param(req, "endDate"));
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr>
DashboardController::getColourBreakdown(HttpRequestPtr req) {
  bool ok = true;
  auto errResp = co_await validateDates(req, ok);
  if (!ok)
    co_return errResp;

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &dashboardService = plugin->getDashboardService();

  auto result = co_await dashboardService.getColourBreakdown(
      param(req, "startDate"), param(req, "endDate"), param(req, "raffleId"));
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> DashboardController::getDrawStats(HttpRequestPtr req) {
  bool ok = true;
  auto errResp = co_await validateDates(req, ok);
  if (!ok)
    co_return errResp;

  int pageNo = intParam(req, "pageNo", 1);
  int pageSize = intParam(req, "pageSize", 10);
  if (pageNo < 1)
    pageNo = 1;
  if (pageSize < 1)
    pageSize = 10;

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &dashboardService = plugin->getDashboardService();

  auto result = co_await dashboardService.getDrawStats(
      param(req, "startDate"), param(req, "endDate"), param(req, "raffleId"),
      pageNo, pageSize);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr>
DashboardController::getTopParticipants(HttpRequestPtr req) {
  bool ok = true;
  auto errResp = co_await validateDates(req, ok);
  if (!ok)
    co_return errResp;

  int limit = intParam(req, "limit", 10);

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &dashboardService = plugin->getDashboardService();

  auto result = co_await dashboardService.getTopParticipants(
      param(req, "startDate"), param(req, "endDate"), limit);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> DashboardController::getRevenueTrend(HttpRequestPtr req) {
  bool ok = true;
  auto errResp = co_await validateDates(req, ok);
  if (!ok)
    co_return errResp;

  std::string interval = param(req, "interval");
  if (interval.empty())
    interval = "day";
  if (interval != "day" && interval != "week" && interval != "month") {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["code"] = raffle::constants::ERR_VALIDATION;
    response.error["message"] =
        "Invalid interval. Allowed values: day, week, month.";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &dashboardService = plugin->getDashboardService();

  auto result = co_await dashboardService.getRevenueTrend(
      param(req, "startDate"), param(req, "endDate"), interval,
      param(req, "raffleId"));
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}
