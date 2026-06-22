//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CUSTOMERSERVICE_H
#define CHOPMONEYRAFFLEAPI_CUSTOMERSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

#include "dto/CustomerAuthDto.h"
#include "dto/CustomerOtpDto.h"
#include "dto/CustomerVerifyOtpDto.h"


namespace raffle::services {

    class CustomerService {
    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);
        drogon::Task<dto::BaseApiResponse> getStats();
        drogon::Task<dto::BaseApiResponse> verifyAccountStatus(const std::string &phoneNumber);
        drogon::Task<dto::BaseApiResponse> validateCredentials(const dto::CustomerAuthDto &dto);
        drogon::Task<dto::BaseApiResponse> sendOtp(const dto::CustomerOtpDto &dto);
        drogon::Task<dto::BaseApiResponse> verifyOtp(const dto::CustomerVerifyOtpDto &dto);

        //customer dashboard related endpoints
        drogon::Task<dto::BaseApiResponse> getTransactionHistory(const std::string &customerId);
        drogon::Task<dto::BaseApiResponse> getStats(const std::string &customerId);
        drogon::Task<dto::BaseApiResponse> getProfileDetails(const std::string &customerId);

    };

}
#endif //CHOPMONEYRAFFLEAPI_CUSTOMERSERVICE_H