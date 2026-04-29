//
// Created by Emmanuel Addo-Odame on 23/03/2026.
//

#include "HubtelApi.h"
#include <ctime>
#include <iomanip>
#include <sstream>

#include "dto/DirectReceiveMoneyResponse.h"

namespace raffle::services {

drogon::Task<dto::CheckoutResponse> HubtelApi::InitializeWebCheckout(const dto::WebCheckoutDto &dto) {

  dto::CheckoutResponse response;

  try {
    const auto &config = drogon::app().getCustomConfig();
    const auto &hubtelConfig = config["HubtelPaymentApi"];

    const std::string username = hubtelConfig["ApiIdOrUsername"].asString();
    const std::string password = hubtelConfig["ApiKeyOrPassword"].asString();
    const std::string merchantAccountNumber = hubtelConfig["MerchantAccountNumber"].asString();

    Json::Value payload;
    payload["totalAmount"] = dto.getTotalAmount();
    payload["description"] = dto.getDescription();
    payload["callbackUrl"] = dto.getCallbackUrl();
    payload["returnUrl"] = dto.getReturnUrl();
    payload["merchantAccountNumber"] = merchantAccountNumber;
    payload["cancellationUrl"] = dto.getCancellationUrl();
    payload["clientReference"] = dto.getClientReference();

    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    const std::string body = Json::writeString(writer, payload);

    const std::string auth = username + ":" + password;
    const std::string authHeader = "Basic " + drogon::utils::base64Encode(auth);

    const std::string baseUrl = "https://payproxyapi.hubtel.com";
    auto client = drogon::HttpClient::newHttpClient(baseUrl);
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Post);
    req->setPath("/items/initiate");
    req->setBody(body);
    req->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    req->addHeader("Authorization", authHeader);

    auto httpResp = co_await client->sendRequestCoro(req);

    LOG_INFO << "InitializeWebCheckout response => "
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

drogon::Task<dto::DirectReceiveMoneyResponse> HubtelApi::InitializeDirectReceiveMoney(const dto::DirectReceiveMoneyDto &dto) {

  dto::DirectReceiveMoneyResponse response;

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
    payload["CustomerMsisdn"] = dto.getCustomerMsisdn();
    payload["CustomerEmail"] = dto.getCustomerEmail();
    payload["Channel"] = dto.getChannel();
    payload["Amount"] = dto.getAmount();
    payload["PrimaryCallbackUrl"] = dto.getPrimaryCallbackUrl();
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
    req->setPath("/merchantaccount/merchants/" + merchantAccountNumber +
                 "/receive/mobilemoney");
    req->setBody(body);
    req->addHeader("Content-Type", "application/json");
    req->addHeader("Authorization", authHeader);

    auto httpResp = co_await client->sendRequestCoro(req);

    LOG_INFO << "InitializeCheckout response => "
             << (httpResp ? httpResp->getBody() : "No response")
             << ", reference: " << dto.getClientReference();

    if (!httpResp) {
      response.message = "Failure";
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
      response.message = "Failure";
      response.responseCode = "INVALID_JSON";
      co_return response;
    }

    response = dto::DirectReceiveMoneyResponse::fromJson(jsonResponse);
    co_return response;

  } catch (const std::exception &e) {
    LOG_ERROR << "Error! Failed to initiate Hubtel web_checkout. " << e.what();
    response.message = "Failure";
    response.responseCode = "EXCEPTION";
    co_return response;
  }
}

drogon::Task<dto::TransactionStatusCheckResponse> HubtelApi::TransactionStatusCheck(const std::string &clientReference) {

  dto::TransactionStatusCheckResponse response;
  try {
    const auto &config = drogon::app().getCustomConfig();
    const auto &hubtelConfig = config["HubtelPaymentApi"];

    const std::string username = hubtelConfig["ApiIdOrUsername"].asString();
    const std::string password = hubtelConfig["ApiKeyOrPassword"].asString();
    const std::string merchantAccountNumber =
        hubtelConfig["MerchantAccountNumber"].asString();

    const std::string auth = username + ":" + password;
    const std::string authHeader = "Basic " + drogon::utils::base64Encode(auth);

    const std::string baseUrl = "https://api-txnstatus.hubtel.com";
    auto client = drogon::HttpClient::newHttpClient(baseUrl);
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Get);
    req->setPath("/transactions/" + merchantAccountNumber + "/status");
    req->setParameter("clientReference", clientReference);
    req->addHeader("Authorization", authHeader);

    auto httpResp = co_await client->sendRequestCoro(req);

    LOG_INFO << "TransactionStatusCheck response => "
             << (httpResp ? httpResp->getBody() : "No response")
             << ", clientReference: " << clientReference;

    if (!httpResp) {
      response.message = "Failure";
      response.responseCode = "ERROR";
      co_return response;
    }

    const std::string content = std::string(httpResp->getBody());
    Json::CharReaderBuilder readerBuilder;
    Json::Value jsonResponse;
    std::string errs;
    std::istringstream iss(content);

    if (!Json::parseFromStream(readerBuilder, iss, &jsonResponse, &errs)) {
      response.message = "Failure";
      response.responseCode = "INVALID_JSON";
      co_return response;
    }

    response = dto::TransactionStatusCheckResponse::fromJson(jsonResponse);
    co_return response;

  } catch (const std::exception &e) {
    LOG_ERROR << "Error! Failed to check transaction status. " << e.what();
    response.message = "Failure";
    response.responseCode = "EXCEPTION";
    co_return response;
  }
}


drogon::Task<bool> HubtelApi::SendSms(const std::string &phoneNumber, const std::string &messageContent) {

  LOG_INFO << "smsMessage => " << messageContent;

  auto client = drogon::HttpClient::newHttpClient("https://sms.hubtel.com");

  // Construct the path with query parameters
  auto req = drogon::HttpRequest::newHttpRequest();
  req->setPath("/v1/messages/send");
  req->setParameter("clientsecret", "sffrctup");
  req->setParameter("clientid", "xfuqfoyu");
  req->setParameter("from", "RHYOLITE");
  req->setParameter("to", phoneNumber);
  req->setParameter("content", messageContent);
  req->setMethod(drogon::Get);

  try {
    auto resp = co_await client->sendRequestCoro(req);

    // The API returns the response body which we should log
    std::string smsNotificationResponse = std::string(resp->getBody());
    LOG_INFO << "hubtelSmsApiResponse => " << smsNotificationResponse << ",phoneNo => " << phoneNumber  ;

    if (resp->getStatusCode() != drogon::k201Created) {
      LOG_ERROR << "Hubtel SMS request failed. Status: " << resp->getStatusCode();
      co_return false;
    }
    co_return true;

  } catch (const std::exception &e) {
    LOG_ERROR << "Exception sending SMS: " << e.what();
    co_return false;
  }
}

} // namespace raffle::services
