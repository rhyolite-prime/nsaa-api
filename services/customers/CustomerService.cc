//
// Created by Emmanuel Addo-Odame on 26/02/2026.
//

#include "CustomerService.h"
#include "Customers.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>

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
        Criteria(Customers::Cols::_mobile_money_account, CompareOperator::Like,
                 likeQuery) ||
        Criteria(Customers::Cols::_mobile_money_provider, CompareOperator::Like,
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

} // namespace raffle::services
