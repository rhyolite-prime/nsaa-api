#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class HubtelCallbackProcessorController : public drogon::HttpController<HubtelCallbackProcessorController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/hubtel-callback-processor/";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(HubtelCallbackProcessorController::processWebCheckout, std::string(PREFIX) + "web-checkout", Post, Options);
    ADD_METHOD_TO(HubtelCallbackProcessorController::processDirectReceiveMoney, std::string(PREFIX) + "direct-receive-money", Post, Options);
    METHOD_LIST_END

    Task<HttpResponsePtr> processWebCheckout(HttpRequestPtr req);
    Task<HttpResponsePtr> processDirectReceiveMoney(HttpRequestPtr req);
};
