#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class AuthController : public drogon::HttpController<AuthController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/auth";
    METHOD_LIST_BEGIN
    //admin user
    ADD_METHOD_TO(AuthController::checkAccountStatus, std::string(PREFIX) + "/admin-user/check-account-status", Get, Options);
    ADD_METHOD_TO(AuthController::sendOtp, std::string(PREFIX) + "/admin-user/send-otp", Get, Options);
    ADD_METHOD_TO(AuthController::verifyOtp, std::string(PREFIX) + "/admin-user/verify-otp", Post, Options);
    ADD_METHOD_TO(AuthController::setPassword, std::string(PREFIX) + "/admin-user/set-password", Post, Options);
    ADD_METHOD_TO(AuthController::signIn, std::string(PREFIX) + "/admin-user/login", Post, Options);
    ADD_METHOD_TO(AuthController::adminSignIn, std::string(PREFIX) + "/admin-user/admin-login", Post, Options);

    //partners
    ADD_METHOD_TO(AuthController::checkPartnerAccountStatus, std::string(PREFIX) + "/partner/check-account-status", Get, Options);
    ADD_METHOD_TO(AuthController::sendPartnerOtp, std::string(PREFIX) + "/partner/send-otp", Post, Options);
    ADD_METHOD_TO(AuthController::verifyPartnerOtp, std::string(PREFIX) + "/partner/verify-otp", Post, Options);
    ADD_METHOD_TO(AuthController::generatePartnerAuth, std::string(PREFIX) + "/partner/token", Post, Options);

    //customers...
    ADD_METHOD_TO(AuthController::checkCustomerAccountStatus, std::string(PREFIX) + "/customer/check-account-status", Get, Options);
    ADD_METHOD_TO(AuthController::getCustomerLoginPin, std::string(PREFIX) + "/customer/get-login-pin", Get, Options);
    ADD_METHOD_TO(AuthController::sendCustomerOtp, std::string(PREFIX) + "/customer/send-otp", Post, Options);
    ADD_METHOD_TO(AuthController::verifyCustomerOtp, std::string(PREFIX) + "/customer/verify-otp", Post, Options);
    ADD_METHOD_TO(AuthController::generateCustomerAuth, std::string(PREFIX) + "/customer/token", Post, Options);
    ADD_METHOD_TO(AuthController::resetCustomerPin, std::string(PREFIX) + "/customer/reset-pin", Get, Options);
    ADD_METHOD_TO(AuthController::changeCustomerPin, std::string(PREFIX) + "/customer/change-pin", Post, Options);



    METHOD_LIST_END

    drogon::Task<HttpResponsePtr> checkAccountStatus(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> sendOtp(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> verifyOtp(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> setPassword(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> signIn(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> adminSignIn(HttpRequestPtr req);

    //partners
    drogon::Task<HttpResponsePtr> checkPartnerAccountStatus(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> generatePartnerAuth(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> sendPartnerOtp(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> verifyPartnerOtp(HttpRequestPtr req);


    //customers
    drogon::Task<HttpResponsePtr> checkCustomerAccountStatus(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> getCustomerLoginPin(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> generateCustomerAuth(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> sendCustomerOtp(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> verifyCustomerOtp(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> resetCustomerPin(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> changeCustomerPin(HttpRequestPtr req);


};
