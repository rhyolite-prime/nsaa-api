#include "SupportController.h"

#include "dto/BaseApiResponse.h"
#include "dto/SupportTicketDto.h"
#include "plugins/RaffleServicePlugin.h"


Task<HttpResponsePtr> SupportController::createTicket(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::SupportTicketDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &supportTicketService = plugin->getSupportTicketService();

    auto result = co_await supportTicketService.create(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}
