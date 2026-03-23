//
// Created by Emmanuel Addo-Odame on 23/03/2026.
//

#include "HubtelApi.h"
#include <ctime>
#include <iomanip>
#include <sstream>


namespace raffle::services {


 drogon::Task<dto::CheckoutResponse> HubtelApi::InitializeWebCheckout(const dto::WebCheckoutDto &dto) {

    dto::CheckoutResponse response;


       try {
        const auto &config = drogon::app().getCustomConfig();
        const auto &hubtelConfig = config["HubtelPaymentApi"];

        const std::string username = hubtelConfig["ApiIdOrUsername"].asString();
        const std::string password = hubtelConfig["ApiKeyOrPassword"].asString();
        const std::string merchantAccountNumber = hubtelConfig["MerchantAccountNumber"].asString();

        auto now = std::time(nullptr);
        std::tm localTime{};
        localtime_r(&now, &localTime);

        Json::Value payload;
        payload["CustomerName"] = dto.getCustomerName();
        payload["CustomerMsisdn"] = dto.getPhoneNo();
        payload["CustomerEmail"] = dto.getCustomerEmail();
        payload["Channel"] = dto.getChannel();
        payload["Amount"] = dto.getTotalAmount();
        payload["PrimaryCallbackUrl"] = dto.getCallbackUrl();
        payload["Description"] = dto.getDescription();
        payload["ClientReference"] = dto.getClientReference();

        Json::StreamWriterBuilder writer;
        writer["indentation"] = "";
        const std::string body = Json::writeString(writer, payload);

        const std::string auth = username + ":" + password;
        const std::string authHeader = "Basic " + drogon::utils::base64Encode(auth);

        const std::string baseUrl = "https://rmp.hubtel.com";
        auto client = drogon::HttpClient::newHttpClient(baseUrl);
        auto req = drogon::HttpRequest::newHttpRequest();
        req->setMethod(drogon::Post);
        req->setPath("/merchantaccount/merchants/" + merchantAccountNumber + "/receive/mobilemoney");
        req->setBody(body);
        req->addHeader("Content-Type", "application/json");
        req->addHeader("Authorization", authHeader);

        auto httpResp = co_await client->sendRequestCoro(req);

        LOG_INFO << "InitializeCheckout response => "
                 << (httpResp ? httpResp->getBody() : "No response")
                 << ", reference: " << dto.getClientReference();

        if (!httpResp) {
          response.status = "Failure";
          response.responseCode = "ERROR";
          co_return response;
        }

        const auto statusCode = httpResp->statusCode();
        const std::string content = std::string(httpResp->getBody());

        Json::CharReaderBuilder readerBuilder;
        Json::Value jsonResponse;
        std::string errs;
        std::istringstream iss(content);

        if (!Json::parseFromStream(readerBuilder, iss, &jsonResponse, &errs)) {
          response.status = "Failure";
          response.responseCode = "INVALID_JSON";
          co_return response;
        }

        response = dto::CheckoutResponse::fromJson(jsonResponse);
        co_return response;

      } catch (const std::exception &e) {
        LOG_ERROR << "Error! Failed to initiate Hubtel web_checkout. " << e.what();
        response.status = "Failure";
        response.responseCode = "EXCEPTION";
        co_return response;
      }
    }






}
