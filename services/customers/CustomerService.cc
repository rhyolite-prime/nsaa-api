//
// Created by Emmanuel Addo-Odame on 26/02/2026.
//

#include "CustomerService.h"
#include "Customers.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include "plugins/RaffleServicePlugin.h"
#include "services/hubtel_api/HubtelApi.h"
#include "services/redis/RedisCacheManager.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>

#include "bcrypt.h"
#include "jwt-cpp/jwt.h"
#include "utils/IdGeneratorUtils.h"
#include "utils/PasswordUtils.h"

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {
  drogon::Task<dto::BaseApiResponse> CustomerService::getAll(int pageNo, int pageSize, const std::string &query) {

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

  drogon::Task<dto::BaseApiResponse> CustomerService::verifyAccountStatus(const std::string &phoneNumber) {

    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Customers> mp(dbClient);

    Criteria searchCriteria = Criteria(Customers::Cols::_msisdn, CompareOperator::EQ, phoneNumber) && Criteria(Customers::Cols::_is_active, CompareOperator::EQ, true) && Criteria(Customers::Cols::_is_locked_out, CompareOperator::EQ, false);

    std::string requestId = utils::IdGeneratorUtils::generateGuid();

    auto plugin = drogon::app().getPlugin<plugins::RaffleServicePlugin>();
    auto &redisCacheManager = plugin->getRedisCacheManager();

    std::string authRequestRedisKey = "auth:request:" + requestId;

    co_await redisCacheManager.setValueWithTtl(authRequestRedisKey, phoneNumber, 60);

    try {
      auto customer = co_await mp.findOne(searchCriteria);

      // Customer found -> account exists
      bool hasPin = (customer.getSecretPin() != nullptr && !customer.getSecretPin()->empty());

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


  drogon::Task<dto::BaseApiResponse> CustomerService::validateCredentials(const dto::CustomerAuthDto &dto) {

    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Customers> mapper(dbClient);

    Criteria criteria =
        (Criteria(Customers::Cols::_msisdn, CompareOperator::EQ, dto.getPhoneNumber()) &&
         Criteria(Customers::Cols::_is_active, CompareOperator::EQ, true) &&
         Criteria(Customers::Cols::_is_locked_out, CompareOperator::EQ, false));

    dto::BaseApiResponse response;

    try {
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
                         .sign(jwt::algorithm::hs256{jwtSecurityKey});

        response.success = true;
        response.message = "Authentication successful";
        response.result["token"] = token;
        response.result["customerId"] = customer.getValueOfId();
        response.result["requestId"] = dto.getRequestId();
        response.result["phoneNumber"] = customer.getValueOfEmail();
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

  drogon::Task<dto::BaseApiResponse> CustomerService::sendOtp(const dto::CustomerOtpDto &dto) {

    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Customers> mapper(dbClient);

    auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
    auto &redisCacheManager = plugin->getRedisCacheManager();

    const std::string requestKey = "auth:request:" + dto.getRequestId();

    std::string storedPhone = co_await redisCacheManager.getValue(requestKey, true);

    if (storedPhone.empty()) {
      dto::BaseApiResponse response;
      response.success = false;
      response.error["code"] = constants::ERR_AUTH_TOKEN_EXPIRED;
      response.error["message"] = "Request ID is invalid or has expired. Please restart the authentication flow.";
      co_return response;
    }

    // 2. Generate a 6-digit OTP and persist it in Redis for 5 minutes (300 s).
    const std::string otp = utils::IdGeneratorUtils::generateRandomSixDigit();
    const std::string otpKey = "otp:" + dto.getRequestId();

    co_await redisCacheManager.setValueWithTtl(otpKey, otp, 300);

    // 3. Build the SMS message and deliver it via Hubtel.
    const std::string message = "Your login verification code is: " + otp + ". Valid for 5 minutes. Do not share this code with anyone.";

    LOG_INFO << " OTP => " << otp;

    auto &hubtelApi = plugin->getHubtelApi();
    //bool smsSent = co_await hubtelApi.SendSms(storedPhone, message);
    bool smsSent = true;

    if (!smsSent)
    {
      LOG_ERROR << "sendOtp: SMS delivery failed for requestId=" << dto.getRequestId() << ", phone=" << storedPhone;

      dto::BaseApiResponse response;
      response.success = false;
      response.error["code"] = constants::ERR_INTERNAL;
      response.error["message"] = "Failed to deliver OTP. Please try again.";
      co_return response;
    }

    LOG_INFO << "sendOtp: OTP sent successfully for requestId=" << dto.getRequestId()
             << ", phone=" << storedPhone;

    //create the customer in database

    Customers customer;
    customer.setMsisdn(storedPhone);
    customer.setIsActive(true);
    customer.setCreatedAt(trantor::Date::now());
    customer.setLastActive(trantor::Date::now());


    co_await mapper.insert(customer);
    LOG_INFO << "sendOtp: Customer created successfully for requestId=" << dto.getRequestId();

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "OTP sent successfully.";
    response.result["requestId"] = dto.getRequestId();
    response.result["phoneNumber"] = dto.getPhoneNumber();
    co_return response;
  }

  drogon::Task<dto::BaseApiResponse> CustomerService::verifyOtp(const dto::CustomerVerifyOtpDto &dto) {

    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Customers> mapper(dbClient);

    auto plugin = drogon::app().getPlugin<plugins::RaffleServicePlugin>();
    auto &redisCacheManager = plugin->getRedisCacheManager();

    const std::string otpKey = "otp:" + dto.getRequestId();
    const std::string storedOtp = co_await redisCacheManager.getValue(otpKey, true);

    dto::BaseApiResponse response;

    if (storedOtp.empty()) {

      response.success = false;
      response.error["code"] = constants::ERR_AUTH_TOKEN_EXPIRED;
      response.error["message"] = "Request ID is invalid or has expired. Please restart the authentication flow.";
      co_return response;
    }

    Criteria criteria =
        (Criteria(Customers::Cols::_msisdn, CompareOperator::EQ, dto.getPhoneNumber()) &&
         Criteria(Customers::Cols::_is_active, CompareOperator::EQ, true) &&
         Criteria(Customers::Cols::_is_locked_out, CompareOperator::EQ, false));

    try {

      Customers customer = co_await mapper.findOne(criteria);

      // go ahead and generate a JWT token
      auto &app = drogon::app();
      auto customConfig = app.getCustomConfig();
      std::string jwtSecurityKey = customConfig["JwtBearer"]["JwtSecurityKey"].asString();
      std::string jwtIssuer = customConfig["JwtBearer"]["JwtIssuer"].asString();

      auto token = jwt::create()
                       .set_issuer(jwtIssuer)
                       .set_type("JWT")
                       .set_issued_at(std::chrono::system_clock::now())
                       .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24 * 120))
                       .set_payload_claim("customerId", jwt::claim(customer.getValueOfId()))
                       .sign(jwt::algorithm::hs256{jwtSecurityKey});

      response.success = true;
      response.message = "Authentication successful";
      response.result["token"] = token;

    }catch (const DrogonDbException &e) {
      // User not found
      response.success = false;
      response.message = "User not found";
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      response.error["message"] = "User not found";
    }
    co_return response;

  }
} // namespace raffle::services
