#include "ReportsController.h"

#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include "plugins/RaffleServicePlugin.h"
#include "utils/DateUtils.h"

#include <string>

namespace {

std::string param(const HttpRequestPtr &req, const char *name) {
  return req->getParameter(name);
}

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

Task<HttpResponsePtr> validateDates(const HttpRequestPtr &req, bool &ok) {
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

void sanitizePaging(int &pageNo, int &pageSize) {
  if (pageNo < 1)
    pageNo = 1;
  if (pageSize < 1)
    pageSize = 10;
}

} // namespace

Task<HttpResponsePtr> ReportsController::getSummary(HttpRequestPtr req) {
  bool ok = true;
  auto errResp = co_await validateDates(req, ok);
  if (!ok)
    co_return errResp;

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &reportService = plugin->getReportService();

  auto result = co_await reportService.getSummary(param(req, "startDate"),
                                                  param(req, "endDate"));
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> ReportsController::getTicketSalesReport(HttpRequestPtr req) {
  bool ok = true;
  auto errResp = co_await validateDates(req, ok);
  if (!ok)
    co_return errResp;

  int pageNo = intParam(req, "pageNo", 1);
  int pageSize = intParam(req, "pageSize", 10);
  sanitizePaging(pageNo, pageSize);

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &reportService = plugin->getReportService();

  auto result = co_await reportService.getTicketSalesReport(
      param(req, "startDate"), param(req, "endDate"), param(req, "raffleId"),
      param(req, "paymentStatus"), param(req, "query"), pageNo, pageSize);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr>
ReportsController::getTicketDistribution(HttpRequestPtr req) {
  bool ok = true;
  auto errResp = co_await validateDates(req, ok);
  if (!ok)
    co_return errResp;

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &reportService = plugin->getReportService();

  auto result = co_await reportService.getTicketDistribution(
      param(req, "startDate"), param(req, "endDate"), param(req, "raffleId"));
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> ReportsController::getRevenueReport(HttpRequestPtr req) {
  bool ok = true;
  auto errResp = co_await validateDates(req, ok);
  if (!ok)
    co_return errResp;

  std::string groupBy = param(req, "groupBy");
  if (groupBy.empty())
    groupBy = "day";
  if (groupBy != "day" && groupBy != "week" && groupBy != "month") {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["code"] = raffle::constants::ERR_VALIDATION;
    response.error["message"] =
        "Invalid groupBy. Allowed values: day, week, month.";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &reportService = plugin->getReportService();

  auto result = co_await reportService.getRevenueReport(
      param(req, "startDate"), param(req, "endDate"), groupBy,
      param(req, "raffleId"));
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> ReportsController::getWinnersReport(HttpRequestPtr req) {
  bool ok = true;
  auto errResp = co_await validateDates(req, ok);
  if (!ok)
    co_return errResp;

  int pageNo = intParam(req, "pageNo", 1);
  int pageSize = intParam(req, "pageSize", 10);
  sanitizePaging(pageNo, pageSize);

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &reportService = plugin->getReportService();

  auto result = co_await reportService.getWinnersReport(
      param(req, "startDate"), param(req, "endDate"), param(req, "drawId"),
      param(req, "status"), param(req, "query"), pageNo, pageSize);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> ReportsController::getCustomerReport(HttpRequestPtr req) {
  bool ok = true;
  auto errResp = co_await validateDates(req, ok);
  if (!ok)
    co_return errResp;

  int pageNo = intParam(req, "pageNo", 1);
  int pageSize = intParam(req, "pageSize", 10);
  sanitizePaging(pageNo, pageSize);

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &reportService = plugin->getReportService();

  auto result = co_await reportService.getCustomerReport(
      param(req, "startDate"), param(req, "endDate"), param(req, "query"),
      param(req, "isActive"), pageNo, pageSize);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}
