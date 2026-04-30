//
// Created by Emmanuel Addo-Odame on 29/04/2026.
//

#include "EmailService.h"
#include <json/json.h>

namespace raffle::services {

drogon::Task<dto::BaseApiResponse> EmailService::sendEmail(const dto::SendEmailDto &dto) {

  dto::BaseApiResponse apiResponse;

  try {
    // call an api to push the email.
    auto client = drogon::HttpClient::newHttpClient("https://dse-api.rhyoliteprime.com");

    auto &app = drogon::app();
    auto customConfig = app.getCustomConfig();
    std::string host = customConfig["GeneralMailConfiguration"]["Host"].asString();
    std::string userName = customConfig["GeneralMailConfiguration"]["UserName"].asString();
    std::string password = customConfig["GeneralMailConfiguration"]["Password"].asString();
    std::string senderName = customConfig["GeneralMailConfiguration"]["SenderName"].asString();

    Json::Value jsonBody;
    jsonBody["To"] = dto.getTo();
    jsonBody["Subject"] = dto.getSubject();
    jsonBody["Body"] = dto.getBody();
    jsonBody["Host"] = host;
    jsonBody["Attachments"] = Json::arrayValue;
    jsonBody["Port"] = 465;
    jsonBody["EnableSsl"] = true;
    jsonBody["UserName"] = userName;
    jsonBody["Password"] = password;
    jsonBody["SenderName"] = senderName;
    jsonBody["IsBodyHtml"] = true;

    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Post);
    req->setPath("/api/services/app/Auxillary/SendEmail");
    req->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    req->setBody(jsonBody.toStyledString());

    auto response = co_await client->sendRequestCoro(req);

    if (response->statusCode() == drogon::k200OK) {
      apiResponse.success = true;
      apiResponse.message = "Email sent successfully";
    } else {
      apiResponse.success = false;
      apiResponse.message = "Failed to send email";
      if (response) {
        apiResponse.error["statusCode"] = response->statusCode();
        apiResponse.error["body"] = std::string(response->body());
      } else {
        apiResponse.error["message"] = "Unknown error";
      }
    }
  } catch (const std::exception &ex) {
    apiResponse.success = false;
    apiResponse.message = "Failed to send email";
    apiResponse.error["message"] = ex.what();
  }

  co_return apiResponse;
}


drogon::Task<dto::BaseApiResponse> EmailService::sendOtpEmail(const dto::SendEmailDto &dto) {

  dto::BaseApiResponse apiResponse;

  try {
    // call an api to push the email.
    auto client = drogon::HttpClient::newHttpClient("https://dse-api.rhyoliteprime.com");

    auto &app = drogon::app();
    auto customConfig = app.getCustomConfig();
    std::string host = customConfig["AuthMailConfiguration"]["Host"].asString();
    std::string userName = customConfig["AuthMailConfiguration"]["UserName"].asString();
    std::string password =  customConfig["AuthMailConfiguration"]["Password"].asString();
    std::string senderName =  customConfig["AuthMailConfiguration"]["SenderName"].asString();

    Json::Value jsonBody;
    jsonBody["To"] = dto.getTo();
    jsonBody["Subject"] = dto.getSubject();
    jsonBody["Body"] = dto.getBody();
    jsonBody["Host"] = host;
    jsonBody["Attachments"] = Json::arrayValue;
    jsonBody["Port"] = 465;
    jsonBody["EnableSsl"] = true;
    jsonBody["UserName"] = userName;
    jsonBody["Password"] = password;
    jsonBody["SenderName"] = senderName;
    jsonBody["IsBodyHtml"] = true;

    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Post);
    req->setPath("/api/services/app/Auxillary/SendEmail");
    req->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    req->setBody(jsonBody.toStyledString());

    auto response = co_await client->sendRequestCoro(req);

    if (response->statusCode() == drogon::k200OK) {
      apiResponse.success = true;
      apiResponse.message = "Email sent successfully";
    } else {
      apiResponse.success = false;
      apiResponse.message = "Failed to send email";
      if (response) {
        apiResponse.error["statusCode"] = response->statusCode();
        apiResponse.error["body"] = std::string(response->body());
      } else {
        apiResponse.error["message"] = "Unknown error";
      }
    }
  } catch (const std::exception &ex) {
    apiResponse.success = false;
    apiResponse.message = "Failed to send email";
    apiResponse.error["message"] = ex.what();
  }

  co_return apiResponse;
}


}