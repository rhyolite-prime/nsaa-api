#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class PaymentCallbackController : public drogon::HttpController<PaymentCallbackController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/payment-callback/";
    METHOD_LIST_BEGIN

    ADD_METHOD_TO(PaymentCallbackController::handleDirectReceiveMoney, std::string(PREFIX) + "direct-receive-money", Post);
    ADD_METHOD_TO(PaymentCallbackController::handleCheckout, std::string(PREFIX) + "checkout", Post);
    ADD_METHOD_TO(PaymentCallbackController::handleDirectSendMobileMoney, std::string(PREFIX) + "direct-send-mobile-money", Post);
    ADD_METHOD_TO(PaymentCallbackController::handleDirectSendToBank, std::string(PREFIX) + "direct-send-to-bank", Post);

    METHOD_LIST_END

    Task<HttpResponsePtr> handleDirectReceiveMoney(HttpRequestPtr req);
    Task<HttpResponsePtr> handleCheckout(HttpRequestPtr req);
    Task<HttpResponsePtr> handleDirectSendMobileMoney(HttpRequestPtr req);
    Task<HttpResponsePtr> handleDirectSendToBank(HttpRequestPtr req);

};
