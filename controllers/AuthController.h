#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class AuthController : public drogon::HttpController<AuthController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/auth";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthController::checkAccountStatus, std::string(PREFIX) + "/check-account-status", Get, Options);
    ADD_METHOD_TO(AuthController::sendOtp, std::string(PREFIX) + "/send-otp", Get, Options);
    ADD_METHOD_TO(AuthController::verifyOtp, std::string(PREFIX) + "/verify-otp", Post, Options);
    ADD_METHOD_TO(AuthController::setPassword, std::string(PREFIX) + "/set-password", Post, Options);
    ADD_METHOD_TO(AuthController::signIn, std::string(PREFIX) + "/login", Post, Options);
    ADD_METHOD_TO(AuthController::adminSignIn, std::string(PREFIX) + "/admin-login", Post, Options);

    //customers
    ADD_METHOD_TO(AuthController::checkCustomerAccountStatus, std::string(PREFIX) + "/customer/check-account-status", Get, Options);
    ADD_METHOD_TO(AuthController::sendCustomerOtp, std::string(PREFIX) + "/customer/send-otp", Post, Options);
    ADD_METHOD_TO(AuthController::verifyCustomerOtp, std::string(PREFIX) + "/customer/verify-otp", Post, Options);
    ADD_METHOD_TO(AuthController::generateCustomerAuth, std::string(PREFIX) + "/customer/token", Post, Options);



    METHOD_LIST_END

    drogon::Task<HttpResponsePtr> checkAccountStatus(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> sendOtp(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> verifyOtp(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> setPassword(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> signIn(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> adminSignIn(HttpRequestPtr req);

    //customers
    drogon::Task<HttpResponsePtr> checkCustomerAccountStatus(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> generateCustomerAuth(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> sendCustomerOtp(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> verifyCustomerOtp(HttpRequestPtr req);
    //drogon::Task<HttpResponsePtr> registerCustomer(HttpRequestPtr req);


};
