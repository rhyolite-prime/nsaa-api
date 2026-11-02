//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CUSTOMERSERVICE_H
#define CHOPMONEYRAFFLEAPI_CUSTOMERSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

#include "dto/ChangeCustomerPinDto.h"
#include "dto/CustomerAuthDto.h"
#include "dto/CustomerOtpDto.h"
#include "dto/CustomerVerifyOtpDto.h"
#include "dto/InitializeTicketPurchaseDto.h"
#include "dto/TicketSummaryRequestDto.h"

namespace raffle::services {

class CustomerService {
public:
  drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);
  drogon::Task<dto::BaseApiResponse> getStats();
  drogon::Task<dto::BaseApiResponse> verifyAccountStatus(const std::string &phoneNumber);
  drogon::Task<dto::BaseApiResponse> getCustomerLoginPin(const std::string &phoneNumber);
  drogon::Task<dto::BaseApiResponse> validateCredentials(const dto::CustomerAuthDto &dto);
  drogon::Task<dto::BaseApiResponse> sendOtp(const dto::CustomerOtpDto &dto);
  drogon::Task<dto::BaseApiResponse> verifyOtp(const dto::CustomerVerifyOtpDto &dto);
  drogon::Task<dto::BaseApiResponse> resetPin(const std::string &phoneNumber);
  drogon::Task<dto::BaseApiResponse> changePin(const dto::ChangeCustomerPinDto &dto);

  // customer dashboard related endpoints
  drogon::Task<dto::BaseApiResponse> getTransactionHistory(const std::string &customerId);

  drogon::Task<dto::BaseApiResponse> getStats(const std::string &customerId);

  drogon::Task<dto::BaseApiResponse> getProfileDetails(const std::string &customerId);

  drogon::Task<dto::BaseApiResponse> initTicketPurchase(const dto::InitializeTicketPurchaseDto &dto, const std::string &customerId);

  drogon::Task<dto::BaseApiResponse> buyGuestTicket(const dto::InitializeTicketPurchaseDto &dto);

  drogon::Task<dto::BaseApiResponse> getTickets(const std::string &customerId, int pageNo, int pageSize, const std::string &query);

  drogon::Task<dto::BaseApiResponse> getRaffleHistory(const std::string &customerId, int pageNo, int pageSize,  const std::string &query);

  drogon::Task<dto::BaseApiResponse> getTicketSummary(const dto::TicketSummaryRequestDto &dto);
};

} // namespace raffle::services
#endif // CHOPMONEYRAFFLEAPI_CUSTOMERSERVICE_H