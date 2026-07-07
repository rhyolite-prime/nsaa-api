#include "HubtelCallbackProcessorController.h"

#include "dto/BaseApiResponse.h"
#include "plugins/RaffleServicePlugin.h"

Task<HttpResponsePtr> HubtelCallbackProcessorController::processWebCheckout(HttpRequestPtr req)
{
    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    //log the incoming request
    LOG_DEBUG << "Received checkout callback from hubtel @processWebCheckout =>" << jsonBody->toStyledString();

    // Extract client reference from the payload
    std::string responseCode = (*jsonBody)["ResponseCode"].asString();
    std::string clientReference = (*jsonBody)["Data"]["ClientReference"].asString();

    if (responseCode == "0000") {
        auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
        auto &ticketService = plugin->getTicketService();

        auto result = co_await ticketService.activate(clientReference);
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        co_return resp;
    } else {

        //send an email to admin

        LOG_WARN << "Payment failed for client reference: " << clientReference << " with response code: " << responseCode;
        raffle::dto::BaseApiResponse response;
        response.success = true; // Still return success to Hubtel to acknowledge the callback
        response.message = "Callback received, but payment was not successful.";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        co_return resp;
    }
}


Task<HttpResponsePtr> HubtelCallbackProcessorController::processDirectReceiveMoney(HttpRequestPtr req)
{
    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    //log the incoming request
    LOG_DEBUG << "Received checkout callback from hubtel @processDirectReceiveMoney =>" << jsonBody->toStyledString();

    // Extract client reference from the payload
    std::string responseCode = (*jsonBody)["ResponseCode"].asString();
    std::string clientReference = (*jsonBody)["Data"]["ClientReference"].asString();

    if (responseCode == "0000") {
        auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
        auto &ticketService = plugin->getTicketService();

        auto result = co_await ticketService.activate(clientReference);
        auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
        co_return resp;
    } else {
        LOG_WARN << "Payment failed for client reference: " << clientReference << " with response code: " << responseCode;
        raffle::dto::BaseApiResponse response;
        response.success = true;
        response.message = "Callback received, but payment was not successful.";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        co_return resp;
    }
}
