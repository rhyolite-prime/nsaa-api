#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class RaffleController : public drogon::HttpController<RaffleController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/raffle/";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RaffleController::getAll, std::string(PREFIX) + "get-all", Get, Options);
    ADD_METHOD_TO(RaffleController::create, std::string(PREFIX) + "create", Post, Options);
    ADD_METHOD_TO(RaffleController::update, std::string(PREFIX) + "update/{1}", Post, Options);
    ADD_METHOD_TO(RaffleController::extendRaffleDeadline, std::string(PREFIX) + "extend-deadline", Post, Options);
    ADD_METHOD_TO(RaffleController::deleteRaffle, std::string(PREFIX) + "delete/{1}", Delete, Options);
    ADD_METHOD_TO(RaffleController::getTopTicketHolders, std::string(PREFIX) + "get-top-ticket-holders", Get, Options);
    ADD_METHOD_TO(RaffleController::getTickets, std::string(PREFIX) + "get-tickets", Get, Options);
    ADD_METHOD_TO(RaffleController::getTicketStats, std::string(PREFIX) + "get-tickets-stats", Get, Options);
    ADD_METHOD_TO(RaffleController::getRecentTicketHolders, std::string(PREFIX) + "get-recent-ticket-holders", Get, Options);
    ADD_METHOD_TO(RaffleController::getDrawLogs, std::string(PREFIX) + "get-draw-logs", Get, Options);
    ADD_METHOD_TO(RaffleController::activate, std::string(PREFIX) + "activate/{1}", Post, Options);
    ADD_METHOD_TO(RaffleController::runClosure, std::string(PREFIX) + "run-closure/{1}", Post, Options);
    ADD_METHOD_TO(RaffleController::runCompletion, std::string(PREFIX) + "run-completion/{1}", Post, Options);
    ADD_METHOD_TO(RaffleController::executeDraw, std::string(PREFIX) + "run-draw/{1}", Post, Options);
    ADD_METHOD_TO(RaffleController::executeDraw, std::string(PREFIX) + "execute-draw/{1}", Get, Options);
    ADD_METHOD_TO(RaffleController::getDrawDetails, std::string(PREFIX) + "draw-details/{1}", Get, Options);
    ADD_METHOD_TO(RaffleController::getDetails, std::string(PREFIX) + "details/{1}", Get, Options);
    ADD_METHOD_TO(RaffleController::getActiveRaffles, std::string(PREFIX) + "get-active-raffles", Get, Options);
    ADD_METHOD_TO(RaffleController::getRaffleStats, std::string(PREFIX) + "get-raffle-stats", Get, Options); // for USSD
    ADD_METHOD_TO(RaffleController::getActiveRaffleOptions, std::string(PREFIX) + "get-active-raffle-options", Get, Options);
    ADD_METHOD_TO(RaffleController::getWeeklyRaffles, std::string(PREFIX) + "get-weekly", Get, Options);
    ADD_METHOD_TO(RaffleController::getWeeklyRaffleSchedule, std::string(PREFIX) + "get-weekly-schedule", Get, Options);
    //draws
    METHOD_LIST_END

    Task<HttpResponsePtr> getAll(HttpRequestPtr req);

    Task<HttpResponsePtr> getTickets(HttpRequestPtr req);
    Task<HttpResponsePtr> getTicketStats(HttpRequestPtr req);
    Task<HttpResponsePtr> create(HttpRequestPtr req);
    Task<HttpResponsePtr> update(HttpRequestPtr req, std::string raffleId);
    Task<HttpResponsePtr> deleteRaffle(HttpRequestPtr req, std::string raffleId);
    Task<HttpResponsePtr> extendRaffleDeadline(HttpRequestPtr req);

    Task<HttpResponsePtr> getTopTicketHolders(HttpRequestPtr req);
    Task<HttpResponsePtr> getRecentTicketHolders(HttpRequestPtr req);
    Task<HttpResponsePtr> getDrawLogs(HttpRequestPtr req);
    Task<HttpResponsePtr> activate(HttpRequestPtr req, std::string raffleId);
    Task<HttpResponsePtr> runClosure(HttpRequestPtr req, std::string raffleId);
    Task<HttpResponsePtr> executeDraw (HttpRequestPtr req, std::string raffleId);
    Task<HttpResponsePtr> getDrawDetails (HttpRequestPtr req, std::string raffleId);
    Task<HttpResponsePtr> getDetails (HttpRequestPtr req, std::string raffleId);
    Task<HttpResponsePtr> runCompletion (HttpRequestPtr re, std::string raffleId);
    Task<HttpResponsePtr> getActiveRaffles(HttpRequestPtr req);
    Task<HttpResponsePtr> getWeeklyRaffles(HttpRequestPtr req);
    Task<HttpResponsePtr> getActiveRaffleOptions(HttpRequestPtr req);
    Task<HttpResponsePtr> getRaffleStats(HttpRequestPtr req);
    Task<HttpResponsePtr> getWeeklyRaffleSchedule(HttpRequestPtr req);
};
