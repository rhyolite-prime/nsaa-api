#include "TicketsController.h"

#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include "dto/CreateTicketDto.h"
#include "plugins/RaffleServicePlugin.h"
#include "utils/TicketDateFilter.h"

Task<HttpResponsePtr> TicketsController::getAll(HttpRequestPtr req) {
  int pageNo = 1;
  int pageSize = 10;

  auto pageNoStr = req->getParameter("pageNo");
  if (!pageNoStr.empty()) {
    pageNo = std::stoi(pageNoStr);
  }

  auto pageSizeStr = req->getParameter("pageSize");
  if (!pageSizeStr.empty()) {
    pageSize = std::stoi(pageSizeStr);
  }

  std::string query = req->getParameter("query");
  if (query.empty()) {
    query = "";
  }

  std::string status = req->getParameter("status");
  if (status.empty()) {
    status = "";
  }

  std::string raffleId = req->getParameter("raffleId");
  if (raffleId.empty()) {
    raffleId = "";
  }

  const std::string startDate = req->getParameter("startDate");
  const std::string endDate = req->getParameter("endDate");
  if (!raffle::utils::isValidTicketDate(startDate) ||
      !raffle::utils::isValidTicketDate(endDate)) {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["code"] = raffle::constants::ERR_VALIDATION;
    response.error["message"] = "Invalid date parameter. Use ISO format YYYY-MM-DD.";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &ticketService = plugin->getTicketService();

  auto result = co_await ticketService.getAll(pageNo, pageSize, query, raffleId, status,
                                              startDate, endDate);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> TicketsController::revivePendingTicket(HttpRequestPtr req) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  raffle::dto::ReviveTicketDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &ticketService = plugin->getTicketService();

  auto result = co_await ticketService.revivePendingTicket(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}


Task<HttpResponsePtr> TicketsController::getRaffleTicketStats(HttpRequestPtr req, std::string raffleId) {

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &ticketService = plugin->getTicketService();

  auto result = co_await ticketService.getRaffleTicketStats(raffleId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

Task<HttpResponsePtr> TicketsController::create(const HttpRequestPtr req) {
  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  raffle::dto::CreateTicketDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &ticketService = plugin->getTicketService();

  auto result = co_await ticketService.create(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}
