#include "PaymentCallbackController.h"
#include "dto/BaseApiResponse.h"

Task<HttpResponsePtr> PaymentCallbackController::handleDirectReceiveMoney(HttpRequestPtr req)
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

    raffle::dto::BaseApiResponse response;
    response.success = true;

    LOG_DEBUG << "acknowledged direct_receive_money_callback from_hubtel =>" << jsonBody->toStyledString();

    response.message = "Direct Receive Money Callback acknowledged";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;
}


Task<HttpResponsePtr> PaymentCallbackController::handleCheckout(HttpRequestPtr req)
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

    raffle::dto::BaseApiResponse response;
    response.success = true;

    LOG_DEBUG << "acknowledged checkout_callback from hubtel =>" << jsonBody->toStyledString();

    response.message = "Checkout Callback acknowledged";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;
}


Task<HttpResponsePtr> PaymentCallbackController::handleDirectSendMobileMoney(HttpRequestPtr req)
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

    raffle::dto::BaseApiResponse response;
    response.success = true;

    LOG_DEBUG << "acknowledged direct_send_money_callback from hubtel =>" << jsonBody->toStyledString();

    response.message = "Direct send money Callback acknowledged";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;
}

Task<HttpResponsePtr> PaymentCallbackController::handleDirectSendToBank(HttpRequestPtr req)
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

    raffle::dto::BaseApiResponse response;
    response.success = true;

    LOG_DEBUG << "acknowledged direct_send_to_bank_callback from hubtel =>" << jsonBody->toStyledString();

    response.message = "Direct send to bank Callback acknowledged";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;
}



