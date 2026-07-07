#include "AuthController.h"

#include "dto/BaseApiResponse.h"
#include "dto/ChangeCustomerPinDto.h"
#include "dto/CustomerAuthDto.h"
#include "plugins/RaffleServicePlugin.h"


Task<HttpResponsePtr> AuthController::checkAccountStatus(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Customers retrieved successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}

Task<HttpResponsePtr> AuthController::checkPartnerAccountStatus(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Customers retrieved successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}

Task<HttpResponsePtr> AuthController::generatePartnerAuth(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Customers retrieved successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}

Task<HttpResponsePtr> AuthController::sendPartnerOtp(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Customers retrieved successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}

Task<HttpResponsePtr> AuthController::verifyPartnerOtp(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Customers retrieved successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}

Task<HttpResponsePtr> AuthController::checkCustomerAccountStatus(HttpRequestPtr req)
{
    std::string phoneNumber = req->getParameter("phoneNumber");

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &customerService = plugin->getCustomerService();

    auto result = co_await customerService.verifyAccountStatus(phoneNumber);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}

drogon::Task<HttpResponsePtr> AuthController::getCustomerLoginPin(HttpRequestPtr req)
{
    std::string phoneNumber = req->getParameter("phoneNumber");

    auto plugin = app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &customerService = plugin->getCustomerService();

    auto result = co_await customerService.getCustomerLoginPin(phoneNumber);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}


Task<HttpResponsePtr> AuthController::generateCustomerAuth(HttpRequestPtr req)
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

    raffle::dto::CustomerAuthDto customerAuthDto;
    customerAuthDto.fromJson(*jsonBody);

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &customerService = plugin->getCustomerService();

    auto result = co_await customerService.validateCredentials(customerAuthDto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;

}



drogon::Task<HttpResponsePtr> AuthController::sendCustomerOtp(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::CustomerOtpDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &customerService = plugin->getCustomerService();

    auto result = co_await customerService.sendOtp(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}



drogon::Task<HttpResponsePtr> AuthController::verifyCustomerOtp(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::CustomerVerifyOtpDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &customerService = plugin->getCustomerService();

    auto result = co_await customerService.verifyOtp(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}


drogon::Task<HttpResponsePtr> AuthController::resetCustomerPin(HttpRequestPtr req) {

    std::string phoneNumber = req->getParameter("phoneNumber");

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &customerService = plugin->getCustomerService();

    auto result = co_await customerService.resetPin(phoneNumber);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;
}


drogon::Task<HttpResponsePtr> AuthController::changeCustomerPin(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        raffle::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    raffle::dto::ChangeCustomerPinDto dto;
    dto.fromJson(*jsonBody);


    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &customerService = plugin->getCustomerService();

    auto result = co_await customerService.changePin(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    co_return resp;


}


Task<HttpResponsePtr> AuthController::sendOtp(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Customers retrieved successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}

Task<HttpResponsePtr> AuthController::verifyOtp(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Verify OTP successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}

Task<HttpResponsePtr> AuthController::setPassword(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Password set successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}

Task<HttpResponsePtr> AuthController::signIn(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Login successful";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}

Task<HttpResponsePtr> AuthController::adminSignIn(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Password set successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}