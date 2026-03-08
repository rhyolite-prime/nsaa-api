#include "AuthController.h"

#include "dto/BaseApiResponse.h"



Task<HttpResponsePtr> AuthController::checkAccountStatus(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Customers retrieved successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
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

    response.message = "Password set successfully";

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