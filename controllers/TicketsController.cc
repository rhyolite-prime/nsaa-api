#include "TicketsController.h"

#include "dto/BaseApiResponse.h"
#include "dto/CreateTicketDto.h"
#include "plugins/RaffleServicePlugin.h"

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

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &ticketService = plugin->getTicketService();

  auto result = co_await ticketService.getAll(pageNo, pageSize, query);
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
