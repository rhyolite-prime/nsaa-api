//
// Created by Emmanuel Addo-Odame on 26/02/2026.
//

#include "CustomerService.h"
#include "Customers.h"
#include "Tickets.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include "plugins/RaffleServicePlugin.h"
#include "services/hubtel_api/HubtelApi.h"
#include "services/redis/RedisCacheManager.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>

#include "Colours.h"
#include "Draws.h"
#include "Raffles.h"
#include "TicketCollections.h"
#include "bcrypt.h"
#include "dto/CheckoutResponse.h"
#include "dto/InitializeTicketPurchaseDto.h"
#include "dto/WebCheckoutDto.h"
#include <jwt-cpp/jwt.h>
#include "utils/IdGeneratorUtils.h"
#include "utils/PasswordUtils.h"
#include <iomanip>
#include <sstream>

#include "constants/RewardTypes.h"

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

drogon::Task<dto::BaseApiResponse>
CustomerService::getAll(int pageNo, int pageSize, const std::string &query) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Customers> mapper(dbClient);

  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria =
        Criteria(Customers::Cols::_first_name, CompareOperator::Like,
                 likeQuery) ||
        Criteria(Customers::Cols::_last_name, CompareOperator::Like,
                 likeQuery) ||
        Criteria(Customers::Cols::_ghana_card_number, CompareOperator::Like,
                 likeQuery) ||
        Criteria(Customers::Cols::_msisdn, CompareOperator::Like, likeQuery) ||
        Criteria(Customers::Cols::_email, CompareOperator::Like, likeQuery);
  }

  try {
    auto totalCount = co_await mapper.count(searchCriteria);

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    int offset = (pageNo - 1) * pageSize;
    auto colors =
        co_await mapper.orderBy(Customers::Cols::_created_at, SortOrder::DESC)
            .limit(pageSize)
            .offset(offset)
            .findBy(searchCriteria);

    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    dto::BaseApiResponse response;
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] = (int)totalPages;
    response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] = (int)totalPages == pageNo
                                        ? (Json::UInt64)totalCount
                                        : (Json::UInt64)(pageNo * pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &color : colors) {
      Json::Value j = color.toJson();
      Json::Value item;
      item["id"] = j["id"];
      item["msisdn"] = j["msisdn"];
      item["firstName"] = j["first_name"];
      item["lastName"] = j["last_name"];
      item["mobileMoneyProvider"] = j["mobile_money_provider"];
      item["mobileMoneyAccount"] = j["mobile_money_account"];
      item["isActive"] = j["is_active"];
      item["totalSpent"] = j["total_spent"];
      item["totalWon"] = j["total_won"];
      item["numberOfWins"] = j["number_of_wins"];
      item["numberOfEntries"] = j["number_of_entries"];
      item["createdAt"] = j["created_at"];
      data.append(item);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching colors.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}

drogon::Task<dto::BaseApiResponse> CustomerService::getStats() {
  auto dbClient = drogon::app().getDbClient();
  try {
    auto result = co_await dbClient->execSqlCoro(
        "SELECT "
        "COUNT(*) as total_customers, "
        "SUM(CASE WHEN is_active = true THEN 1 ELSE 0 END) as "
        "active_customers, "
        "COALESCE(SUM(total_spent), 0) as total_spent, "
        "COALESCE(SUM(total_won), 0) as total_won, "
        "COALESCE(SUM(number_of_wins), 0) as total_wins "
        "FROM customers");

    dto::BaseApiResponse response;
    response.success = true;

    if (!result.empty()) {
      auto row = result[0];
      response.result["totalCustomers"] =
          (Json::UInt64)row["total_customers"].as<uint64_t>();
      response.result["activeCustomers"] =
          (Json::UInt64)row["active_customers"].as<uint64_t>();
      response.result["totalSpent"] = row["total_spent"].as<std::string>();
      response.result["totalWon"] = row["total_won"].as<std::string>();
      response.result["totalWins"] =
          (Json::UInt64)row["total_wins"].as<uint64_t>();
    } else {
      response.result["totalCustomers"] = 0;
      response.result["activeCustomers"] = 0;
      response.result["totalSpent"] = "0";
      response.result["totalWon"] = "0";
      response.result["totalWins"] = 0;
    }

    co_return response;
  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching customer stats.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}

drogon::Task<dto::BaseApiResponse>
CustomerService::verifyAccountStatus(const std::string &phoneNumber) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Customers> mp(dbClient);

  std::string normalizedPhone = phoneNumber;
  if (!normalizedPhone.empty() && normalizedPhone[0] == '0') {
    normalizedPhone = "233" + normalizedPhone.substr(1);
  }

  Criteria searchCriteria =
      Criteria(Customers::Cols::_msisdn, CompareOperator::EQ,
               normalizedPhone) &&
      Criteria(Customers::Cols::_is_active, CompareOperator::EQ, true) &&
      Criteria(Customers::Cols::_is_locked_out, CompareOperator::EQ, false);

  std::string requestId = utils::IdGeneratorUtils::generateGuid();

  auto plugin = drogon::app().getPlugin<plugins::RaffleServicePlugin>();
  auto &redisCacheManager = plugin->getRedisCacheManager();

  std::string authRequestRedisKey = "auth:request:" + requestId;

  co_await redisCacheManager.setValueWithTtl(authRequestRedisKey,
                                             normalizedPhone, 180);

  try {
    auto customer = co_await mp.findOne(searchCriteria);

    // Customer found -> account exists
    bool hasPin = (customer.getSecretPin() != nullptr &&
                   !customer.getSecretPin()->empty());

    dto::BaseApiResponse response;
    response.success = true;
    response.result["hasAccount"] = true;
    response.result["hasPin"] = hasPin;
    response.result["requestId"] = requestId;
    co_return response;

  } catch (const drogon::orm::UnexpectedRows &) {

    dto::BaseApiResponse response;
    response.success = true;
    response.result["hasAccount"] = false;
    response.result["hasPin"] = false;
    response.result["requestId"] = requestId;
    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "An Error while verifying account status.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;

  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}

drogon::Task<dto::BaseApiResponse>
CustomerService::validateCredentials(const dto::CustomerAuthDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Customers> mapper(dbClient);

  std::string normalizedPhone = dto.getPhoneNumber();
  if (!normalizedPhone.empty() && normalizedPhone[0] == '0') {
    normalizedPhone = "233" + normalizedPhone.substr(1);
  }

  Criteria criteria =
      (Criteria(Customers::Cols::_msisdn, CompareOperator::EQ,
                normalizedPhone) &&
       Criteria(Customers::Cols::_is_active, CompareOperator::EQ, true) &&
       Criteria(Customers::Cols::_is_locked_out, CompareOperator::EQ, false));

  dto::BaseApiResponse response;

  try {

    auto plugin = drogon::app().getPlugin<plugins::RaffleServicePlugin>();
    auto &redisCacheManager = plugin->getRedisCacheManager();

    const std::string requestKey = "auth:request:" + dto.getRequestId();

    std::string storedPhone =
        co_await redisCacheManager.getValue(requestKey, true);

    if (storedPhone.empty() || storedPhone != normalizedPhone) {

      response.success = false;
      response.error["code"] = constants::ERR_AUTH_TOKEN_EXPIRED;
      response.error["message"] = "Request ID is invalid or has expired. "
                                  "Please restart the authentication flow.";
      response.message = "Request ID is invalid or has expired. Please restart "
                         "the authentication flow.";
      co_return response;
    }

    Customers customer = co_await mapper.findOne(criteria);

    std::string storedHash = utils::PasswordUtils::normalizeBcryptHash(
        customer.getValueOfSecretPin());

    bool passwordMatches = bcrypt::validatePassword(dto.getPin(), storedHash);

    if (passwordMatches) {

      Customers userToUpdate = customer;
      userToUpdate.setLastActive(trantor::Date::now());
      co_await mapper.update(userToUpdate);

      // Password is correct, generate JWT token
      auto &app = drogon::app();
      auto customConfig = app.getCustomConfig();
      std::string jwtSecurityKey =
          customConfig["JwtBearer"]["JwtSecurityKey"].asString();
      std::string jwtIssuer = customConfig["JwtBearer"]["JwtIssuer"].asString();

      auto token = jwt::create()
                       .set_issuer(jwtIssuer)
                       .set_type("JWT")
                       .set_issued_at(std::chrono::system_clock::now())
                       .set_expires_at(std::chrono::system_clock::now() +
                                       std::chrono::hours(24 * 120))
                       .set_payload_claim("customerId",
                                          jwt::claim(customer.getValueOfId()))
                       .sign(jwt::algorithm::hs256(jwtSecurityKey));

      response.success = true;
      response.message = "Authentication successful";
      response.result["token"] = token;
      response.result["customerId"] = customer.getValueOfId();
      response.result["requestId"] = dto.getRequestId();

    } else {
      // Password is incorrect
      response.success = false;
      response.message = "Invalid credentials";
      response.error["code"] = constants::ERR_AUTH_INVALID_CREDENTIALS;
    }
  } catch (const DrogonDbException &e) {
    // User not found
    response.success = false;
    response.message = "User not found";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    response.error["message"] = "User not found";
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse>
CustomerService::sendOtp(const dto::CustomerOtpDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Customers> mapper(dbClient);

  auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
  auto &redisCacheManager = plugin->getRedisCacheManager();

  std::string normalizedPhone = dto.getPhoneNumber();
  if (!normalizedPhone.empty() && normalizedPhone[0] == '0') {
    normalizedPhone = "233" + normalizedPhone.substr(1);
  }

  const std::string requestKey = "auth:request:" + dto.getRequestId();

  std::string storedPhone = co_await redisCacheManager.getValue(requestKey);

  if (storedPhone.empty() || storedPhone != normalizedPhone) {
    dto::BaseApiResponse response;
    response.success = false;
    response.error["code"] = constants::ERR_AUTH_TOKEN_EXPIRED;
    response.error["message"] = "Request ID is invalid or has expired. Please "
                                "restart the authentication flow.";
    co_return response;
  }

  const std::string countKey = "auth:request:count:" + dto.getRequestId();
  std::string countStr = co_await redisCacheManager.getValue(countKey);

  if (!countStr.empty()) {
    // Second call (retry) - remove the keys to prevent a third call
    co_await redisCacheManager.removeValue(requestKey);
    co_await redisCacheManager.removeValue(countKey);
  } else {
    // First call - record that it has been called once
    co_await redisCacheManager.setValueWithTtl(countKey, "1", 100);
  }

  const std::string defaultPin =
      utils::IdGeneratorUtils::generateRandomDigit(4);

  // 3. Build the SMS message and deliver it via Hubtel.
  const std::string message = "Your ChopMoney account has been created "
                              "successfully. Your default PIN is: " +
                              defaultPin +
                              ". Do not share this PIN with anyone.";

  LOG_INFO << " Default PIN => " << defaultPin;

  auto &hubtelApi = plugin->getHubtelApi();
  bool smsSent = co_await hubtelApi.SendSms(storedPhone, message);

  if (!smsSent) {
    LOG_ERROR << "sendOtp: SMS delivery failed for requestId="
              << dto.getRequestId() << ", phone=" << storedPhone;

    dto::BaseApiResponse response;
    response.success = false;
    response.error["code"] = constants::ERR_INTERNAL;
    response.error["message"] = "Failed to deliver OTP. Please try again.";
    co_return response;
  }

  LOG_INFO << "sendOtp: OTP sent successfully for requestId="
           << dto.getRequestId() << ", phone=" << storedPhone;

  const std::string defaultPinKey = "default:pin:" + normalizedPhone;

  co_await redisCacheManager.setValueWithTtl(defaultPinKey, defaultPin, 300);

  // create the customer in database

  auto existingCount = co_await mapper.count(
      Criteria(Customers::Cols::_msisdn, CompareOperator::EQ, storedPhone));
  if (existingCount == 0) {

    const std::string hashedDefaultPin = bcrypt::generateHash(defaultPin);

    Customers customer;

    // if the phone number starts from zero, replace zero with 233

    customer.setMsisdn(normalizedPhone);
    customer.setSecretPin(hashedDefaultPin);
    customer.setIsActive(true);
    customer.setCreatedAt(trantor::Date::now());
    customer.setLastActive(trantor::Date::now());

    co_await mapper.insert(customer);
    LOG_INFO << "sendOtp: Customer created successfully for requestId="
             << dto.getRequestId();
  }

  dto::BaseApiResponse response;
  response.success = true;
  response.message = "OTP sent successfully.";
  response.result["requestId"] = dto.getRequestId();
  response.result["phoneNumber"] = dto.getPhoneNumber();

  co_return response;
}

drogon::Task<dto::BaseApiResponse>
CustomerService::getCustomerLoginPin(const std::string &phoneNumber) {

  auto plugin = drogon::app().getPlugin<plugins::RaffleServicePlugin>();
  auto &redisCacheManager = plugin->getRedisCacheManager();

  std::string normalizedPhone = phoneNumber;
  if (!normalizedPhone.empty() && normalizedPhone[0] == '0') {
    normalizedPhone = "233" + normalizedPhone.substr(1);
  }

  const std::string defaultPinKey = "default:pin:" + normalizedPhone;
  std::string storedPin =
      co_await redisCacheManager.getValue(defaultPinKey, true);

  dto::BaseApiResponse response;
  if (storedPin.empty()) {
    response.success = false;
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    response.error["message"] =
        "There is no Login PIN available for " + phoneNumber;
    response.message = "There is no Login PIN available for " + phoneNumber;
  } else {
    response.success = true;
    response.message = "Keep your account safe. Do not share this PIN with "
                       "anyone. Your Login PIN is: " +
                       storedPin;
  }

  co_return response;
}

drogon::Task<dto::BaseApiResponse>
CustomerService::verifyOtp(const dto::CustomerVerifyOtpDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Customers> mapper(dbClient);

  auto plugin = drogon::app().getPlugin<plugins::RaffleServicePlugin>();
  auto &redisCacheManager = plugin->getRedisCacheManager();

  const std::string otpKey = "otp:" + dto.getRequestId();
  const std::string storedOtp =
      co_await redisCacheManager.getValue(otpKey, true);

  dto::BaseApiResponse response;

  if (storedOtp.empty()) {

    response.success = false;
    response.error["code"] = constants::ERR_AUTH_TOKEN_EXPIRED;
    response.error["message"] = "Request ID is invalid or has expired. Please "
                                "restart the authentication flow.";
    co_return response;
  }

  Criteria criteria =
      (Criteria(Customers::Cols::_msisdn, CompareOperator::EQ,
                dto.getPhoneNumber()) &&
       Criteria(Customers::Cols::_is_active, CompareOperator::EQ, true) &&
       Criteria(Customers::Cols::_is_locked_out, CompareOperator::EQ, false));

  try {

    Customers customer = co_await mapper.findOne(criteria);

    // go ahead and generate a JWT token
    auto &app = drogon::app();
    auto customConfig = app.getCustomConfig();
    std::string jwtSecurityKey =
        customConfig["JwtBearer"]["JwtSecurityKey"].asString();
    std::string jwtIssuer = customConfig["JwtBearer"]["JwtIssuer"].asString();

    auto token = jwt::create()
                     .set_issuer(jwtIssuer)
                     .set_type("JWT")
                     .set_issued_at(std::chrono::system_clock::now())
                     .set_expires_at(std::chrono::system_clock::now() +
                                     std::chrono::hours(24 * 120))
                     .set_payload_claim("customerId",
                                        jwt::claim(customer.getValueOfId()))
                     .sign(jwt::algorithm::hs256(jwtSecurityKey));

    response.success = true;
    response.message = "Authentication successful";
    response.result["token"] = token;

  } catch (const DrogonDbException &e) {
    // User not found
    response.success = false;
    response.message = "User not found";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    response.error["message"] = "User not found";
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse>
CustomerService::resetPin(const std::string &phoneNumber) {
  dto::BaseApiResponse response;
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Customers> mapper(dbClient);

  std::string normalizedPhone = phoneNumber;
  if (!normalizedPhone.empty() && normalizedPhone[0] == '0') {
    normalizedPhone = "233" + normalizedPhone.substr(1);
  }

  try {
    Criteria criteria =
        Criteria(Customers::Cols::_msisdn, CompareOperator::EQ,
                 normalizedPhone) &&
        Criteria(Customers::Cols::_is_active, CompareOperator::EQ, true);

    auto customers = co_await mapper.findBy(criteria);

    if (customers.empty()) {
      response.success = false;
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      response.error["message"] = "Customer not found.";
      response.message = "Customer account not found.";
      co_return response;
    }

    auto customer = customers[0];

    const std::string newPin = utils::IdGeneratorUtils::generateRandomDigit(4);
    const std::string hashedNewPin = bcrypt::generateHash(newPin);

    customer.setSecretPin(hashedNewPin);
    customer.setUpdatedAt(trantor::Date::now());

    co_await mapper.update(customer);

    const std::string message =
        "Your ChopMoney PIN has been reset. Your new PIN is: " + newPin +
        ". Do not share this PIN with anyone.";
    LOG_INFO << " New PIN => " << newPin;

    auto plugin = drogon::app().getPlugin<plugins::RaffleServicePlugin>();
    if (plugin) {
      auto &hubtelApi = plugin->getHubtelApi();
      bool smsSent = co_await hubtelApi.SendSms(normalizedPhone, message);

      if (!smsSent) {
        LOG_ERROR << "resetPin: SMS delivery failed for phone="
                  << normalizedPhone;
      } else {
        LOG_INFO << "resetPin: SMS sent successfully for phone="
                 << normalizedPhone;
      }

      auto &redisCacheManager = plugin->getRedisCacheManager();
      const std::string defaultPinKey = "default:pin:" + normalizedPhone;
      co_await redisCacheManager.setValueWithTtl(defaultPinKey, newPin, 300);
    }

    response.success = true;
    response.message =
        "Keep your account safe. Your PIN has been reset. Your new PIN is: " +
        newPin;

  } catch (const drogon::orm::DrogonDbException &e) {
    LOG_ERROR << "Database error resetting pin: " << e.base().what();
    response.success = false;
    response.error["code"] = constants::ERR_DB_QUERY;
    response.error["message"] = "Database error while resetting PIN.";
    response.error["detail"] = e.base().what();
  } catch (const std::exception &e) {
    LOG_ERROR << "Internal error resetting pin: " << e.what();
    response.success = false;
    response.error["code"] = constants::ERR_INTERNAL;
    response.error["message"] = "Internal error.";
    response.error["detail"] = e.what();
  }

  co_return response;
}

drogon::Task<dto::BaseApiResponse>
CustomerService::changePin(const dto::ChangeCustomerPinDto &dto) {
  dto::BaseApiResponse response;

  if (dto.getCustomerPhoneNumber().empty()) {
    response.success = false;
    response.error["code"] = constants::ERR_UNAUTHORIZED;
    response.error["message"] = "Customer Phone is required.";
    response.message = "Customer Phone is required.";
    co_return response;
  }

  if (dto.getOldPin().empty() || dto.getNewPin().empty()) {
    response.success = false;
    response.error["code"] = constants::ERR_BAD_REQUEST;
    response.error["message"] = "Old PIN and New PIN are required.";
    response.message = "Old PIN and New PIN are required.";
    co_return response;
  }

  std::string normalizedPhone = dto.getCustomerPhoneNumber();
  if (!normalizedPhone.empty() && normalizedPhone[0] == '0') {
    normalizedPhone = "233" + normalizedPhone.substr(1);
  }

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Customers> mapper(dbClient);

  Criteria criteria =
      Criteria(Customers::Cols::_msisdn, CompareOperator::EQ,
               normalizedPhone) &&
      Criteria(Customers::Cols::_is_active, CompareOperator::EQ, true) &&
      Criteria(Customers::Cols::_is_locked_out, CompareOperator::EQ, false);

  try {
    auto customer = co_await mapper.findOne(criteria);

    std::string storedHash = utils::PasswordUtils::normalizeBcryptHash(
        customer.getValueOfSecretPin());
    bool passwordMatches =
        bcrypt::validatePassword(dto.getOldPin(), storedHash);

    if (!passwordMatches) {
      response.success = false;
      response.error["code"] = constants::ERR_AUTH_INVALID_CREDENTIALS;
      response.error["message"] = "Invalid old PIN.";
      response.message = "Invalid old PIN.";
      co_return response;
    }

    const std::string hashedNewPin = bcrypt::generateHash(dto.getNewPin());

    Customers userToUpdate = customer;
    userToUpdate.setSecretPin(hashedNewPin);
    userToUpdate.setUpdatedAt(trantor::Date::now());

    co_await mapper.update(userToUpdate);

    response.success = true;
    response.message = "PIN changed successfully.";

  } catch (const drogon::orm::UnexpectedRows &) {
    response.success = false;
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    response.error["message"] = "Customer not found or account is inactive.";
  } catch (const drogon::orm::DrogonDbException &e) {
    LOG_ERROR << "Database error changing pin: " << e.base().what();
    response.success = false;
    response.error["code"] = constants::ERR_DB_QUERY;
    response.error["message"] = "Database error while changing PIN.";
    response.error["detail"] = e.base().what();
  } catch (const std::exception &e) {
    LOG_ERROR << "Internal error changing pin: " << e.what();
    response.success = false;
    response.error["code"] = constants::ERR_INTERNAL;
    response.error["message"] = "Internal error.";
    response.error["detail"] = e.what();
  }

  co_return response;
}

  static std::string paymentStatusToString(int status) {
  switch (status) {
    case constants::PaymentStatus::PAYMENT_INITIATED:
      return "Payment Initiated";
    case constants::PaymentStatus::SUCCESS:
      return "Success";
    case constants::PaymentStatus::PAYMENT_FAILED:
      return "Payment Failed";
    default:
      return "Unknown";
  }
}

drogon::Task<dto::BaseApiResponse> CustomerService::getTransactionHistory(const std::string &customerId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Tickets> mapper(dbClient);

  try {
    auto tickets =
        co_await mapper.orderBy(Tickets::Cols::_created_at, SortOrder::DESC)
            .findBy(Criteria(Tickets::Cols::_customer_id, CompareOperator::EQ,
                             customerId));

    dto::BaseApiResponse response;
    response.success = true;
    Json::Value data = Json::arrayValue;


    for (const auto &ticket : tickets) {
      Json::Value item;
      item["id"] = ticket.getValueOfId();
      item["ticketNumber"] = ticket.getValueOfTicketNumber();
      item["drawId"] = ticket.getValueOfDrawId();
      item["raffleId"] = ticket.getValueOfRaffleId();
      item["selectedOptionValue"] = ticket.getValueOfSelectedOptionValue();
      item["selectedOptionName"] = ticket.getValueOfSelectedOptionName();
      item["entryFee"] = ticket.getValueOfEntryFee();
      item["quantity"] = ticket.getValueOfQuantity();
      item["totalAmount"] = ticket.getValueOfTotalAmount();
      item["paymentStatus"] = paymentStatusToString(ticket.getValueOfPaymentStatus());
      item["createdAt"] = ticket.getValueOfCreatedAt().toDbStringLocal();
      item["ticketAlias"] = ticket.getValueOfTicketAlias();
      data.append(item);
    }
    response.result["data"] = data;
    co_return response;
  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching transaction history.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}

drogon::Task<dto::BaseApiResponse>
CustomerService::getStats(const std::string &customerId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Customers> mapper(dbClient);

  try {
    auto customer = co_await mapper.findByPrimaryKey(customerId);
    dto::BaseApiResponse response;
    response.success = true;
    response.result["totalSpent"] = customer.getValueOfTotalSpent();
    response.result["totalWon"] = customer.getValueOfTotalWon();
    response.result["numberOfWins"] = customer.getValueOfNumberOfWins();
    response.result["numberOfEntries"] = customer.getValueOfNumberOfEntries();
    co_return response;
  } catch (const drogon::orm::UnexpectedRows &) {
    dto::BaseApiResponse response;
    response.success = false;
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    response.error["message"] = "Customer not found.";
    co_return response;
  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching customer stats.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}

drogon::Task<dto::BaseApiResponse>
CustomerService::getProfileDetails(const std::string &customerId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Customers> mapper(dbClient);

  try {
    auto customer = co_await mapper.findByPrimaryKey(customerId);
    dto::BaseApiResponse response;
    response.success = true;
    Json::Value profile;
    profile["id"] = customer.getValueOfId();
    profile["firstName"] = customer.getValueOfFirstName();
    profile["lastName"] = customer.getValueOfLastName();
    profile["msisdn"] = customer.getValueOfMsisdn();
    profile["email"] = customer.getValueOfEmail();
    profile["ghanaCardNumber"] = customer.getValueOfGhanaCardNumber();
    profile["accountType"] = customer.getValueOfAccountType();
    profile["accountProvider"] = customer.getValueOfAccountProvider();
    profile["accountName"] = customer.getValueOfAccountName();
    profile["accountNumber"] = customer.getValueOfAccountNumber();
    profile["isActive"] = customer.getValueOfIsActive();
    profile["createdAt"] = customer.getValueOfCreatedAt().toDbStringLocal();
    response.result = profile;
    co_return response;
  } catch (const drogon::orm::UnexpectedRows &) {
    dto::BaseApiResponse response;
    response.success = false;
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    response.error["message"] = "Customer not found.";
    co_return response;
  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching profile details.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}

drogon::Task<dto::BaseApiResponse>
CustomerService::initTicketPurchase(const dto::InitializeTicketPurchaseDto &dto,
                                    const std::string &customerId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Tickets> ticketMapper(dbClient);
  CoroMapper<Raffles> raffleMapper(dbClient);
  CoroMapper<Draws> drawMapper(dbClient);
  CoroMapper<Customers> customerMapper(dbClient);

  CoroMapper<TicketCollections> ticketGroupMapper(dbClient);

  LOG_INFO << "InitializeTicketPurchase request:"
           << " customerId=" << customerId
           << " selectedOption=" << dto.getSelectedOption()
           << " ticketGroupId=" << dto.getTicketGroupId();

  try {
    // 1. Fetch customer
    auto customer = co_await customerMapper.findByPrimaryKey(customerId);

    // 2. Find active raffle
    auto activeRaffles = co_await raffleMapper.findBy(
        Criteria(Raffles::Cols::_id, CompareOperator::EQ, dto.getRaffleId()));

    if (activeRaffles.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "No active raffle found.";
      co_return errorResponse;
    }

    auto activeRaffle = activeRaffles[0];

    // 3. Find draw
    auto draws = co_await drawMapper.findBy(Criteria(
        Draws::Cols::_raffle_id, CompareOperator::EQ, dto.getRaffleId()));

    if (draws.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] =
          "No corresponding draw found for the active raffle.";
      co_return errorResponse;
    }

    auto activeDraw = draws[0];

    // 4. Option Management
    std::string optionName;
    bool optionFound = false;

    std::string raffleOptionsStr = activeRaffle.getValueOfOptions();

    if (!raffleOptionsStr.empty()) {
      Json::Value raffleOptions;
      Json::Reader reader;
      if (reader.parse(raffleOptionsStr, raffleOptions) &&
          raffleOptions.isArray()) {
        for (const auto &option : raffleOptions) {
          if (option.isMember("id") &&
              option["id"].asString() == dto.getSelectedOption()) {
            optionName = option["name"].asString();
            optionFound = true;
            break;
          }
        }
      }
    }

    if (!optionFound) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Invalid selected option ID.";
      co_return errorResponse;
    }

    int quantity = 0;
    double entryFee = 0.0;
    std::string ticketFee;
    try {
      auto ticketGroup =
          co_await ticketGroupMapper.findByPrimaryKey(dto.getTicketGroupId());
      quantity = ticketGroup.getValueOfQuantity();
      ticketFee = ticketGroup.getValueOfUnitPrice();
      entryFee = std::stod(ticketFee);
    } catch (const drogon::orm::DrogonDbException &e) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Invalid ticket group ID.";
      co_return errorResponse;
    }

    // 5. Create the ticket
    Tickets newTicket;
    newTicket.setDrawId(activeDraw.getValueOfId());
    newTicket.setSelectedOptionValue(dto.getSelectedOption());
    newTicket.setSelectedOptionName(optionName);
    newTicket.setQuantity(quantity);
    newTicket.setCustomerPhone(customer.getValueOfMsisdn());
    newTicket.setRaffleId(activeRaffle.getValueOfId());

    std::string clientReference = utils::IdGeneratorUtils::generateGuid();
    newTicket.setPaymentReference(clientReference);
    newTicket.setCustomerId(customerId);

    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
    localtime_r(&nowTime, &localTime);

    std::ostringstream ticketNumberStream;
    ticketNumberStream << std::put_time(&localTime, "%Y%m%d%H%M%S");
    newTicket.setTicketNumber(ticketNumberStream.str());

    std::string raffleName = activeRaffle.getValueOfName();
    std::string initials = "";
    bool newWord = true;
    for (char c : raffleName) {
      if (std::isspace(c)) {
        newWord = true;
      } else if (newWord && std::isalpha(c)) {
        initials += std::toupper(c);
        newWord = false;
      }
    }

    auto ticketCount = co_await ticketMapper.count(
        Criteria(Tickets::Cols::_raffle_id, CompareOperator::EQ,
                 activeRaffle.getValueOfId()));
    std::ostringstream aliasStream;
    aliasStream << initials << "-" << std::setfill('0') << std::setw(3)
                << (ticketCount + 1);
    newTicket.setTicketAlias(aliasStream.str());

    auto &app = drogon::app();
    auto customConfig = app.getCustomConfig();

    newTicket.setEntryFee(ticketFee);
    double totalAmount = entryFee * quantity;
    newTicket.setTotalAmount(std::to_string(totalAmount));

    newTicket.setPaymentStatus(constants::PaymentStatus::PAYMENT_INITIATED);
    newTicket.setPurchaseChannel("WEB");
    newTicket.setPaymentMethod("HUBTEL CHECKOUT");
    newTicket.setIsActive(false);

    auto result = co_await ticketMapper.insert(newTicket);

    // 6. Initialize WebCheckout payment
    const auto &hubtelConfig = customConfig["HubtelPaymentApi"];
    raffle::services::HubtelApi hubtelApi;

    raffle::dto::WebCheckoutDto webCheckoutDto;
    webCheckoutDto.setCallbackUrl(
        hubtelConfig.isMember("WebCheckoutCallBackUrl")
            ? hubtelConfig["WebCheckoutCallBackUrl"].asString()
            : "https://nsaa-api.rhyoliteprime.com/api/v1/"
              "hubtel-callback-processor/web-checkout");
    webCheckoutDto.setCancellationUrl(hubtelConfig["ReturnUrl"].asString());
    webCheckoutDto.setReturnUrl(hubtelConfig["ReturnUrl"].asString());
    webCheckoutDto.setTotalAmount(totalAmount);
    webCheckoutDto.setDescription("Ticket Purchase - " +
                                  activeRaffle.getValueOfSlug());
    webCheckoutDto.setClientReference(clientReference);

    auto hubtelResult =
        co_await hubtelApi.InitializeWebCheckout(webCheckoutDto);

    LOG_INFO << "Hubtel WebCheckout result for clientReference="
             << clientReference << " responseCode=" << hubtelResult.responseCode
             << " status=" << hubtelResult.status
             << " checkoutUrl=" << hubtelResult.data.checkoutUrl;

    dto::BaseApiResponse response;
    if (hubtelResult.responseCode == "0000") {
      response.success = true;
      response.message =
          "Ticket created successfully. Please complete payment.";
      response.result["ticketId"] = result.getValueOfId();
      response.result["ticketNumber"] = result.getValueOfTicketNumber();
      response.result["ticketAlias"] = result.getValueOfTicketAlias();
      response.result["paymentReference"] = clientReference;
      response.result["checkoutUrl"] = hubtelResult.data.checkoutUrl;
      response.result["selectedColour"] = optionName;
    } else {
      response.success = false;
      response.message = "Ticket created but failed to initialize payment. " +
                         hubtelResult.status;
      response.error["code"] = constants::ERR_INTERNAL;
      response.error["message"] = "Payment initialization failed.";
      response.result["ticketId"] = result.getValueOfId();
    }

    co_return response;

  } catch (const drogon::orm::UnexpectedRows &) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    errorResponse.error["message"] = "Customer not found.";
    co_return errorResponse;
  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while initializing ticket purchase.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}

drogon::Task<dto::BaseApiResponse>
CustomerService::buyGuestTicket(const dto::InitializeTicketPurchaseDto &dto) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Tickets> ticketMapper(dbClient);
  CoroMapper<Raffles> raffleMapper(dbClient);
  CoroMapper<Draws> drawMapper(dbClient);
  CoroMapper<Customers> customerMapper(dbClient);

  CoroMapper<TicketCollections> ticketGroupMapper(dbClient);

  LOG_INFO << "buyGuestTicket request:"
           << " raffleId=" << dto.getRaffleId()
           << " phoneNumber=" << dto.getPhoneNumber()
           << " selectedOption=" << dto.getSelectedOption()
           << " ticketGroupId=" << dto.getTicketGroupId();

  std::string normalizedPhone = dto.getPhoneNumber();

  if (normalizedPhone.empty()) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_VALIDATION;
    errorResponse.error["message"] = "Phone number is required.";
    co_return errorResponse;
  }

  if (normalizedPhone[0] == '0') {
    normalizedPhone = "233" + normalizedPhone.substr(1);
  }

  try {
    Customers customer;
    auto existingCustomers = co_await customerMapper.findBy(Criteria(
        Customers::Cols::_msisdn, CompareOperator::EQ, normalizedPhone));
    if (existingCustomers.empty()) {
      const std::string defaultPin =
          utils::IdGeneratorUtils::generateRandomDigit(4);
      const std::string hashedDefaultPin = bcrypt::generateHash(defaultPin);

      customer.setMsisdn(normalizedPhone);
      customer.setSecretPin(hashedDefaultPin);
      customer.setIsActive(true);
      customer.setCreatedAt(trantor::Date::now());
      customer.setLastActive(trantor::Date::now());

      auto createdCustomer = co_await customerMapper.insert(customer);
      customer = createdCustomer;

      // Optionally send the OTP via SMS using Hubtel here as well
      auto plugin =
          drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
      if (plugin) {
        auto &redisCacheManager = plugin->getRedisCacheManager();
        const std::string defaultPinKey = "default:pin:" + normalizedPhone;
        co_await redisCacheManager.setValueWithTtl(defaultPinKey, defaultPin,
                                                   300);

        const std::string message =
            "Your ChopMoney account has been created. Your default PIN is: " +
            defaultPin + ". Do not share this PIN.";
        LOG_INFO << "Guest Ticket Purchase - Created User Default PIN => "
                 << defaultPin;
      }

    } else {
      customer = existingCustomers[0];
    }

    std::string customerId = customer.getValueOfId();

    // Generate JWT Token
    auto &app = drogon::app();
    auto customConfig = app.getCustomConfig();
    std::string jwtSecurityKey =
        customConfig["JwtBearer"]["JwtSecurityKey"].asString();
    std::string jwtIssuer = customConfig["JwtBearer"]["JwtIssuer"].asString();

    auto token = jwt::create()
                     .set_issuer(jwtIssuer)
                     .set_type("JWT")
                     .set_issued_at(std::chrono::system_clock::now())
                     .set_expires_at(std::chrono::system_clock::now() +
                                     std::chrono::hours(24 * 120))
                     .set_payload_claim("customerId", jwt::claim(customerId))
                     .sign(jwt::algorithm::hs256(jwtSecurityKey));

    // 2. Find active raffle
    auto activeRaffles = co_await raffleMapper.findBy(
        Criteria(Raffles::Cols::_id, CompareOperator::EQ, dto.getRaffleId()));

    if (activeRaffles.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "No active raffle found.";
      co_return errorResponse;
    }

    auto activeRaffle = activeRaffles[0];

    // 3. Find draw
    auto draws = co_await drawMapper.findBy(Criteria(
        Draws::Cols::_raffle_id, CompareOperator::EQ, dto.getRaffleId()));

    if (draws.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] =
          "No corresponding draw found for the active raffle.";
      co_return errorResponse;
    }

    auto activeDraw = draws[0];

    // 4. Option Management
    std::string optionName;
    bool optionFound = false;

    std::string raffleOptionsStr = activeRaffle.getValueOfOptions();
    if (!raffleOptionsStr.empty()) {
      Json::Value raffleOptions;
      Json::Reader reader;
      if (reader.parse(raffleOptionsStr, raffleOptions) &&
          raffleOptions.isArray()) {
        for (const auto &option : raffleOptions) {
          if (option.isMember("id") &&
              option["id"].asString() == dto.getSelectedOption()) {
            optionName = option["name"].asString();
            optionFound = true;
            break;
          }
        }
      }
    }

    if (!optionFound) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Invalid selected option ID.";
      co_return errorResponse;
    }

    int quantity = 0;
    double entryFee = 0.0;
    std::string ticketFee;
    try {
      auto ticketGroup =
          co_await ticketGroupMapper.findByPrimaryKey(dto.getTicketGroupId());
      quantity = ticketGroup.getValueOfQuantity();
      ticketFee = ticketGroup.getValueOfUnitPrice();
      entryFee = std::stod(ticketFee);
    } catch (const drogon::orm::DrogonDbException &e) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Invalid ticket group ID.";
      co_return errorResponse;
    }

    // 5. Create the ticket
    Tickets newTicket;
    newTicket.setDrawId(activeDraw.getValueOfId());
    newTicket.setSelectedOptionValue(dto.getSelectedOption());
    newTicket.setSelectedOptionName(optionName);
    newTicket.setQuantity(quantity);
    newTicket.setCustomerPhone(customer.getValueOfMsisdn());
    newTicket.setRaffleId(activeRaffle.getValueOfId());

    std::string clientReference = utils::IdGeneratorUtils::generateGuid();
    newTicket.setPaymentReference(clientReference);
    newTicket.setCustomerId(customerId);

    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
    localtime_r(&nowTime, &localTime);

    std::ostringstream ticketNumberStream;
    ticketNumberStream << std::put_time(&localTime, "%Y%m%d%H%M%S");
    newTicket.setTicketNumber(ticketNumberStream.str());

    std::string raffleName = activeRaffle.getValueOfName();
    std::string initials = "";
    bool newWord = true;
    for (char c : raffleName) {
      if (std::isspace(c)) {
        newWord = true;
      } else if (newWord && std::isalpha(c)) {
        initials += std::toupper(c);
        newWord = false;
      }
    }

    auto ticketCount = co_await ticketMapper.count(
        Criteria(Tickets::Cols::_raffle_id, CompareOperator::EQ,
                 activeRaffle.getValueOfId()));
    std::ostringstream aliasStream;
    aliasStream << initials << "-" << std::setfill('0') << std::setw(3)
                << (ticketCount + 1);
    newTicket.setTicketAlias(aliasStream.str());

    newTicket.setEntryFee(ticketFee);
    double totalAmount = entryFee * quantity;
    newTicket.setTotalAmount(std::to_string(totalAmount));

    newTicket.setPaymentStatus(constants::PaymentStatus::PAYMENT_INITIATED);
    newTicket.setPurchaseChannel("WEB");
    newTicket.setPaymentMethod("HUBTEL CHECKOUT");
    newTicket.setIsActive(false);

    auto result = co_await ticketMapper.insert(newTicket);

    // 6. Initialize WebCheckout payment
    const auto &hubtelConfig = customConfig["HubtelPaymentApi"];
    raffle::services::HubtelApi hubtelApi;

    raffle::dto::WebCheckoutDto webCheckoutDto;
    webCheckoutDto.setCallbackUrl(
        hubtelConfig.isMember("WebCheckoutCallBackUrl")
            ? hubtelConfig["WebCheckoutCallBackUrl"].asString()
            : "https://nsaa-api.rhyoliteprime.com/api/v1/"
              "hubtel-callback-processor/web-checkout");
    webCheckoutDto.setCancellationUrl(hubtelConfig["ReturnUrl"].asString());
    webCheckoutDto.setReturnUrl(hubtelConfig["ReturnUrl"].asString());
    webCheckoutDto.setTotalAmount(totalAmount);
    webCheckoutDto.setDescription("Ticket Purchase - " +
                                  activeRaffle.getValueOfSlug());
    webCheckoutDto.setClientReference(clientReference);

    auto hubtelResult =
        co_await hubtelApi.InitializeWebCheckout(webCheckoutDto);

    LOG_INFO << "Hubtel WebCheckout result for clientReference="
             << clientReference << " responseCode=" << hubtelResult.responseCode
             << " status=" << hubtelResult.status
             << " checkoutUrl=" << hubtelResult.data.checkoutUrl;

    dto::BaseApiResponse response;
    if (hubtelResult.responseCode == "0000") {
      response.success = true;
      response.message =
          "Ticket created successfully. Please complete payment.";
      response.result["token"] = token;
      response.result["customerId"] = customerId;
      response.result["ticketId"] = result.getValueOfId();
      response.result["ticketNumber"] = result.getValueOfTicketNumber();
      response.result["ticketAlias"] = result.getValueOfTicketAlias();
      response.result["paymentReference"] = clientReference;
      response.result["checkoutUrl"] = hubtelResult.data.checkoutUrl;
      response.result["selectedColour"] = optionName;
    } else {
      response.success = false;
      response.message = "Ticket created but failed to initialize payment. " +
                         hubtelResult.status;
      response.error["code"] = constants::ERR_INTERNAL;
      response.error["message"] = "Payment initialization failed.";
      response.result["token"] = token;
      response.result["customerId"] = customerId;
      response.result["ticketId"] = result.getValueOfId();
    }

    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while initializing ticket purchase.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}

drogon::Task<dto::BaseApiResponse>
CustomerService::getTickets(const std::string &customerId, int pageNo,
                            int pageSize, const std::string &query) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Tickets> mapper(dbClient);

  Criteria searchCriteria =
      Criteria(Tickets::Cols::_customer_id, CompareOperator::EQ, customerId);

  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria =
        searchCriteria && Criteria(Tickets::Cols::_ticket_number,
                                   CompareOperator::Like, likeQuery);
  }

  try {
    auto totalCount = co_await mapper.count(searchCriteria);

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    int offset = (pageNo - 1) * pageSize;
    auto result =
        co_await mapper.orderBy(Tickets::Cols::_created_at, SortOrder::DESC)
            .limit(pageSize)
            .offset(offset)
            .findBy(searchCriteria);

    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    dto::BaseApiResponse response;
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] = (int)totalPages;
    response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] = (int)totalPages == pageNo
                                        ? (Json::UInt64)totalCount
                                        : (Json::UInt64)(pageNo * pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &ticket : result) {
      Json::Value ticketJson;
      ticketJson["id"] = ticket.getValueOfId();
      ticketJson["ticketNumber"] = ticket.getValueOfTicketNumber();
      ticketJson["ticketAlias"] = ticket.getValueOfTicketAlias();
      ticketJson["drawId"] = ticket.getValueOfDrawId();
      ticketJson["customerId"] = ticket.getValueOfCustomerId();
      ticketJson["selectedOptionName"] = ticket.getValueOfSelectedOptionName();
      ticketJson["entryFee"] = ticket.getValueOfEntryFee();
      ticketJson["quantity"] = ticket.getValueOfQuantity();
      ticketJson["totalAmount"] = ticket.getValueOfTotalAmount();
      ticketJson["paymentReference"] = ticket.getValueOfPaymentReference();
      ticketJson["paymentStatus"] = ticket.getValueOfPaymentStatus();
      ticketJson["paymentMethod"] = ticket.getValueOfPaymentMethod();
      ticketJson["purchaseChannel"] = ticket.getValueOfPurchaseChannel();
      ticketJson["isActive"] = ticket.getValueOfIsActive();
      ticketJson["createdAt"] = ticket.getValueOfCreatedAt().toDbStringLocal();
      ticketJson["updatedAt"] = ticket.getValueOfUpdatedAt().toDbStringLocal();
      ticketJson["customerPhone"] = ticket.getValueOfCustomerPhone();
      ticketJson["raffleId"] = ticket.getValueOfRaffleId();

      data.append(ticketJson);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching tickets.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}

drogon::Task<dto::BaseApiResponse>
CustomerService::getRaffleHistory(const std::string &customerId, int pageNo,
                                  int pageSize, const std::string &query) {
  auto dbClient = drogon::app().getDbClient();

  try {
    std::string likeQuery = "%" + query + "%";

    auto countResult = co_await dbClient->execSqlCoro(
        "SELECT COUNT(DISTINCT r.id) as total_count "
        "FROM raffles r "
        "JOIN tickets t ON r.id = t.raffle_id "
        "WHERE t.customer_id = $1 AND r.name ILIKE $2",
        customerId, likeQuery);

    uint64_t totalCount = 0;
    if (!countResult.empty()) {
      totalCount = countResult[0]["total_count"].as<uint64_t>();
    }

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    int offset = (pageNo - 1) * pageSize;

    auto result = co_await dbClient->execSqlCoro(
        "SELECT r.id, r.name, r.status, r.draw_time, "
        "COUNT(t.id) as tickets_bought, "
        "COALESCE(SUM(CAST(t.total_amount AS NUMERIC)), 0) as total_spent "
        "FROM raffles r "
        "JOIN tickets t ON r.id = t.raffle_id "
        "WHERE t.customer_id = $1 AND r.name ILIKE $2 "
        "GROUP BY r.id, r.name, r.status, r.draw_time "
        "ORDER BY r.draw_time DESC "
        "LIMIT $3 OFFSET $4",
        customerId, likeQuery, (long long)pageSize, (long long)offset);

    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    dto::BaseApiResponse response;
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] = (int)totalPages;
    response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] = (int)totalPages == pageNo
                                        ? (Json::UInt64)totalCount
                                        : (Json::UInt64)(pageNo * pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &row : result) {
      Json::Value item;
      item["raffleId"] = row["id"].as<std::string>();
      item["raffleName"] = row["name"].as<std::string>();
      item["status"] = row["status"].as<std::string>();
      item["drawTime"] = row["draw_time"].as<std::string>();
      item["ticketsBought"] = row["tickets_bought"].as<int>();
      item["totalSpent"] = row["total_spent"].as<std::string>();
      data.append(item);
    }

    response.result["data"] = data;
    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching raffle history.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}

drogon::Task<dto::BaseApiResponse> CustomerService::getTicketSummary(const dto::TicketSummaryRequestDto &dto) {

  dto::BaseApiResponse response;

  // --- Input validation ---
  if (dto.getPhoneNo().empty()) {
    response.success = false;
    response.error["code"] = constants::ERR_MISSING_PARAMETER;
    response.error["message"] = "Phone number is required.";
    response.message = "Phone number is required.";
    co_return response;
  }

  if (dto.getPin().empty()) {
    response.success = false;
    response.error["code"] = constants::ERR_MISSING_PARAMETER;
    response.error["message"] = "PIN is required.";
    response.message = "PIN is required.";
    co_return response;
  }

  // --- Normalize phone number (0XXXXXXXXX -> 233XXXXXXXXX) ---
  std::string normalizedPhone = dto.getPhoneNo();
  if (!normalizedPhone.empty() && normalizedPhone[0] == '0') {
    normalizedPhone = "233" + normalizedPhone.substr(1);
  }

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Customers> customerMapper(dbClient);

  Criteria criteria =
      Criteria(Customers::Cols::_msisdn, CompareOperator::EQ,
               normalizedPhone) &&
      Criteria(Customers::Cols::_is_active, CompareOperator::EQ, true) &&
      Criteria(Customers::Cols::_is_locked_out, CompareOperator::EQ, false);

  try {
    // --- Authenticate customer ---
    Customers customer = co_await customerMapper.findOne(criteria);

    std::string storedHash = utils::PasswordUtils::normalizeBcryptHash(customer.getValueOfSecretPin());

    bool pinMatches = bcrypt::validatePassword(dto.getPin(), storedHash);

    if (!pinMatches) {
      response.success = false;
      response.error["code"] = constants::ERR_AUTH_INVALID_CREDENTIALS;
      response.error["message"] = "Invalid PIN.";
      response.message = "Invalid PIN.";
      co_return response;
    }

    // --- Fetch ticket summary for USSD display ---
    // Returns tickets joined with raffle name, grouped per ticket entry.
    auto result = co_await dbClient->execSqlCoro(
        "SELECT t.ticket_alias, t.quantity, t.raffle_id, "
        "       r.name AS raffle_name, t.created_at "
        "FROM tickets t "
        "JOIN raffles r ON r.id = t.raffle_id "
        "WHERE t.customer_id = $1 "
        "  AND t.is_active = true "
        "  AND t.created_at >= NOW() - INTERVAL '7 days' "
        "ORDER BY t.created_at DESC",
        customer.getValueOfId());

    Json::Value data = Json::arrayValue;

    if (result.empty()) {
      Json::Value item;
      item["ticketAlias"] = "";
      item["raffleName"]  = "No Tickets found";
      item["quantity"]    = 0;
      item["raffleId"]    = "";
      data.append(item);
    } else {
      for (const auto &row : result) {
        Json::Value item;
        item["ticketAlias"] = row["ticket_alias"].as<std::string>();
        item["raffleName"]  = row["raffle_name"].as<std::string>();
        item["quantity"]    = row["quantity"].as<int>();
        item["raffleId"]    = row["raffle_id"].as<std::string>();
        data.append(item);
      }
    }



    response.success = true;
    response.result["data"] = data;
    response.result["totalCount"] = (Json::UInt64)data.size();
    co_return response;

  } catch (const drogon::orm::UnexpectedRows &) {
    response.success = false;
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    response.error["message"] =
        "Customer not found or account is inactive/locked.";
    response.message = "Customer not found or account is inactive/locked.";
    co_return response;
  } catch (const drogon::orm::DrogonDbException &e) {
    response.success = false;
    response.error["code"] = constants::ERR_DB_QUERY;
    response.error["message"] = "Database error while fetching ticket summary.";
    response.error["detail"] = e.base().what();
    co_return response;
  } catch (const std::exception &e) {
    response.success = false;
    response.error["code"] = constants::ERR_INTERNAL;
    response.error["message"] = "Internal error.";
    response.error["detail"] = e.what();
    co_return response;
  }
}

} // namespace raffle::services
// namespace raffle::services
