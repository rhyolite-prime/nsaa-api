#include "RaffleController.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include "plugins/RaffleServicePlugin.h"

Task<HttpResponsePtr> RaffleController::getAll(HttpRequestPtr req) {

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

  std::string startDate = req->getParameter("startDate");
  if (startDate.empty()) {
    startDate = "";
  }

  std::string endDate = req->getParameter("endDate");
  if (startDate.empty()) {
    startDate = "";
  }

  std::string status = req->getParameter("status");
  if (status.empty()) {
    status = "";
  }

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &raffleService = plugin->getRaffleService();

  auto result = co_await raffleService.getAll(pageNo, pageSize, query, startDate, endDate, status);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> RaffleController::create(HttpRequestPtr req) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  raffle::dto::CreateRaffleDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &raffleService = plugin->getRaffleService();

  auto result = co_await raffleService.createAsync(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> RaffleController::getActiveRaffle(HttpRequestPtr req) {

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &raffleService = plugin->getRaffleService();

  auto result = co_await raffleService.getActiveRaffle();
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

Task<HttpResponsePtr> RaffleController::update(HttpRequestPtr req) {

  auto jsonBody = req->getJsonObject();

  if (!jsonBody) {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Invalid JSON body";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  raffle::dto::UpdateRaffleDto dto;
  dto.fromJson(*jsonBody);

  auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &raffleService = plugin->getRaffleService();

  auto result = co_await raffleService.updateAsync(dto);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> RaffleController::getDetails(HttpRequestPtr req) {
  auto id = req->getParameter("id");

  if (id.empty()) {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Raffle ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &raffleService = plugin->getRaffleService();

  auto result = co_await raffleService.getDetails(id);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}


Task<HttpResponsePtr> RaffleController::extendRaffleDeadline(HttpRequestPtr req) {
  raffle::dto::BaseApiResponse response;
  response.success = true;

  response.message = "Raffle deadline extended successfully";

  auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
  co_return resp;
}

Task<HttpResponsePtr> RaffleController::getTopTicketHolders(HttpRequestPtr req) {

  auto id = req->getParameter("id");

  if (id.empty()) {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Raffle ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &raffleService = plugin->getRaffleService();

  auto result = co_await raffleService.getTopTicketHolders(id);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

Task<HttpResponsePtr> RaffleController::getRecentTicketHolders(HttpRequestPtr req) {

  auto id = req->getParameter("id");

  if (id.empty()) {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Raffle ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &raffleService = plugin->getRaffleService();

  auto result = co_await raffleService.getRecentTicketHolders(id);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> RaffleController::getTickets(HttpRequestPtr req) {
  auto raffleId = req->getParameter("id");
  auto pageNoStr = req->getParameter("pageNo");
  auto pageSizeStr = req->getParameter("pageSize");

  int pageNo = pageNoStr.empty() ? 1 : std::stoi(pageNoStr);
  int pageSize = pageSizeStr.empty() ? 10 : std::stoi(pageSizeStr);

  if (raffleId.empty()) {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Raffle ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &raffleService = plugin->getRaffleService();

  auto result = co_await raffleService.getTickets(raffleId, pageNo, pageSize);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}

Task<HttpResponsePtr> RaffleController::getTicketStats(HttpRequestPtr req) {
  auto raffleId = req->getParameter("id");

  if (raffleId.empty()) {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Raffle ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &raffleService = plugin->getRaffleService();

  auto result = co_await raffleService.getTicketStats(raffleId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;
}



Task<HttpResponsePtr> RaffleController::initiateFinalDraw(HttpRequestPtr req) {
  raffle::dto::BaseApiResponse response;
  response.success = true;

  response.message = "Raffle retrieved successfully";

  auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
  co_return resp;
}

Task<HttpResponsePtr> RaffleController::getRngLogs(HttpRequestPtr req) {
  raffle::dto::BaseApiResponse response;
  response.success = true;

  response.message = "Raffle retrieved successfully";

  auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
  co_return resp;
}

Task<HttpResponsePtr> RaffleController::runDraw(HttpRequestPtr req) {
  raffle::dto::BaseApiResponse response;
  response.success = true;

  response.message = "Draw run successfully";

  auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
  co_return resp;
}

Task<HttpResponsePtr> RaffleController::executeDraw(HttpRequestPtr req) {

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &raffleService = plugin->getRaffleService();

  auto result = co_await raffleService.executeDraw();
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  co_return resp;

}

Task<HttpResponsePtr> RaffleController::activate(HttpRequestPtr req) {
  auto jsonBody = req->getJsonObject();

  std::string raffleId;

  if (jsonBody->isString()) {
    raffleId = jsonBody->asString();
  } else {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Expected a string value for campaign ID";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &raffleService = plugin->getRaffleService();

  co_await raffleService.activate(raffleId);

  raffle::dto::BaseApiResponse response;
  response.success = true;
  response.error["message"] = "Scheduled campaign run successfully";
  auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
  resp->setStatusCode(k400BadRequest);
  co_return resp;
}

Task<HttpResponsePtr> RaffleController::deleteRaffle(HttpRequestPtr req)
{
  auto raffleId = req->getParameter("id");

  if (raffleId.empty()) {
    raffle::dto::BaseApiResponse response;
    response.success = false;
    response.error["message"] = "Raffle ID is required";
    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    resp->setStatusCode(k400BadRequest);
    co_return resp;
  }

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &raffleService = plugin->getRaffleService();

  auto result = co_await raffleService.deleteRaffle(raffleId);
  auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
  resp->setStatusCode(result.success ? k200OK : (result.error.isMember("code") && result.error["code"].asInt() == raffle::constants::ERR_RESOURCE_NOT_FOUND ? k404NotFound : k500InternalServerError));
  co_return resp;
}