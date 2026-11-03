#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class ReportsController : public drogon::HttpController<ReportsController> {
public:
  static constexpr const char *PREFIX = "/api/v1/reports/";
  METHOD_LIST_BEGIN
    ADD_METHOD_TO(ReportsController::getSummary, std::string(PREFIX) + "summary", Get, Options);
    ADD_METHOD_TO(ReportsController::getTicketSalesReport, std::string(PREFIX) + "tickets", Get, Options);
    ADD_METHOD_TO(ReportsController::getTicketDistribution, std::string(PREFIX) + "ticket-distribution", Get, Options);
    ADD_METHOD_TO(ReportsController::getDrawStatistics, std::string(PREFIX) + "draw-statistics", Get, Options);
    ADD_METHOD_TO(ReportsController::getCustomerParticipation, std::string(PREFIX) + "customer-participation", Get, Options);
    ADD_METHOD_TO(ReportsController::getRevenueReport, std::string(PREFIX) + "revenue", Get, Options);
    ADD_METHOD_TO(ReportsController::getWinnersReport, std::string(PREFIX) + "winners", Get, Options);
    ADD_METHOD_TO(ReportsController::getCustomerReport, std::string(PREFIX) + "customers", Get, Options);
  METHOD_LIST_END

  Task<HttpResponsePtr> getSummary(HttpRequestPtr req);
  Task<HttpResponsePtr> getTicketSalesReport(HttpRequestPtr req);
  Task<HttpResponsePtr> getTicketDistribution(HttpRequestPtr req);
  Task<HttpResponsePtr> getDrawStatistics(HttpRequestPtr req);
  Task<HttpResponsePtr> getCustomerParticipation(HttpRequestPtr req);
  Task<HttpResponsePtr> getRevenueReport(HttpRequestPtr req);
  Task<HttpResponsePtr> getWinnersReport(HttpRequestPtr req);
  Task<HttpResponsePtr> getCustomerReport(HttpRequestPtr req);
};
