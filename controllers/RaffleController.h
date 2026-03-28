#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class RaffleController : public drogon::HttpController<RaffleController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/raffle";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RaffleController::getAll, std::string(PREFIX) + "/get-all", Get, Options);
    ADD_METHOD_TO(RaffleController::getActiveRaffle, std::string(PREFIX) + "/get-active", Get, Options);
    ADD_METHOD_TO(RaffleController::create, std::string(PREFIX) + "/create", Post, Options);
    ADD_METHOD_TO(RaffleController::update, std::string(PREFIX) + "/update", Post, Options);
    ADD_METHOD_TO(RaffleController::extendRaffleDeadline, std::string(PREFIX) + "/extend-deadline", Post, Options);
    ADD_METHOD_TO(RaffleController::getDetails, std::string(PREFIX) + "/get-details", Get, Options);
    ADD_METHOD_TO(RaffleController::deleteRaffle, std::string(PREFIX) + "/delete", Delete, Options);
    ADD_METHOD_TO(RaffleController::getTopTicketHolders, std::string(PREFIX) + "/get-top-ticket-holders", Get, Options);
    ADD_METHOD_TO(RaffleController::getRecentTicketHolders, std::string(PREFIX) + "/get-recent-ticket-holders", Get, Options);
    ADD_METHOD_TO(RaffleController::initiateFinalDraw, std::string(PREFIX) + "/initiate-final-draw", Get, Options);
    ADD_METHOD_TO(RaffleController::getRngLogs, std::string(PREFIX) + "/get-rng-logs", Get, Options);
    ADD_METHOD_TO(RaffleController::runDraw, std::string(PREFIX) + "/run-draw", Post, Options);
    ADD_METHOD_TO(RaffleController::executeDraw, std::string(PREFIX) + "/execute-draw", Get, Options);
    ADD_METHOD_TO(RaffleController::activate, std::string(PREFIX) + "/activate", Post, Options);
    //draws
    METHOD_LIST_END

    Task<HttpResponsePtr> getAll(HttpRequestPtr req);
    Task<HttpResponsePtr> getActiveRaffle(HttpRequestPtr req);
    Task<HttpResponsePtr> create(HttpRequestPtr req);
    Task<HttpResponsePtr> update(HttpRequestPtr req);
    Task<HttpResponsePtr> deleteRaffle(HttpRequestPtr req);
    Task<HttpResponsePtr> extendRaffleDeadline(HttpRequestPtr req);
    Task<HttpResponsePtr> getDetails(HttpRequestPtr req);
    Task<HttpResponsePtr> getTopTicketHolders(HttpRequestPtr req);
    Task<HttpResponsePtr> getRecentTicketHolders(HttpRequestPtr req);
    Task<HttpResponsePtr> initiateFinalDraw(HttpRequestPtr req);
    Task<HttpResponsePtr> getRngLogs(HttpRequestPtr req);
    Task<HttpResponsePtr> runDraw(HttpRequestPtr req);
    Task<HttpResponsePtr> executeDraw (HttpRequestPtr req);
    Task<HttpResponsePtr> activate(HttpRequestPtr req);
};
