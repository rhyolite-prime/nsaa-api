//
// Created by Emmanuel Addo-Odame on 26/02/2026.
//

#include "DrawService.h"
#include "Draws.h"
#include "Winners.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

drogon::Task<dto::BaseApiResponse> DrawService::getAll(int pageNo, int pageSize, const std::string &query) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Draws> mapper(dbClient);

  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria = Criteria(Draws::Cols::_alias, CompareOperator::Like, likeQuery) ||
        Criteria(Draws::Cols::_draw_code, CompareOperator::Like, likeQuery);
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

    auto colors =
        co_await mapper.orderBy(Draws::Cols::_created_at, SortOrder::DESC).paginate(pageNo, pageSize)
            .findBy(searchCriteria);

    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    dto::BaseApiResponse response;
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] = (int)totalPages;
    response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] = (int)totalPages == pageNo ? (Json::UInt64)totalCount : (Json::UInt64)(pageNo * pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &color : colors) {
      Json::Value j = color.toJson();
      Json::Value item;
      item["id"] = j["id"];
      item["drawCode"] = j["draw_code"];
      item["drawDate"] = j["draw_date"];
      item["drawTime"] = j["draw_time"];
      item["status"] = j["status"];
      item["winningColourId"] = j["winning_colour_id"];
      item["winningColour"] = j["winning_colour"];
      item["totalEntries"] = j["total_entries"];
      item["totalPool"] = j["total_pool"];
      item["totalPrices"] = j["total_prizes"];
      item["alias"] = j["alias"];
      data.append(item);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching draws.";
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

drogon::Task<dto::BaseApiResponse> DrawService::getRecentDraws(int pageNo, int pageSize) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Draws> mapper(dbClient);

  Criteria searchCriteria(Draws::Cols::_status, CompareOperator::EQ, "COMPLETED");

  try {

    auto totalCount = co_await mapper.count(searchCriteria);

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    auto draws = co_await mapper.orderBy(Draws::Cols::_created_at, SortOrder::DESC)
            .paginate(pageNo, pageSize)
            .findBy(searchCriteria);

    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    dto::BaseApiResponse response;
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] = (int)totalPages;
    response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] = (int)totalPages == pageNo ? (Json::UInt64)totalCount : (Json::UInt64)(pageNo * pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &draw : draws) {
      Json::Value j = draw.toJson();
      Json::Value item;
      item["id"] = j["id"];
      item["drawCode"] = j["draw_code"];
      item["drawDate"] = j["draw_date"];
      item["status"] = j["status"];
      item["winningColourId"] = j["winning_colour_id"];
      item["winningColour"] = j["winning_colour"];
      item["totalEntries"] = j["total_entries"];
      item["totalPool"] = j["total_pool"];
      item["totalPrices"] = j["total_prizes"];
      item["alias"] = j["alias"];
      data.append(item);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching recent draws.";
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

drogon::Task<dto::BaseApiResponse> DrawService::getRecentWinners(int pageNo, int pageSize) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Winners> mapper(dbClient);

  Criteria searchCriteria(Winners::Cols::_created_at, CompareOperator::GE, trantor::Date::now().after(-30.0 * 24.0 * 60.0 * 60.0));
  try {

    auto totalCount = co_await mapper.count(searchCriteria);

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    auto winners = co_await mapper.orderBy(Winners::Cols::_created_at, SortOrder::DESC)
            .paginate(pageNo, pageSize)
            .findBy(searchCriteria);

    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    dto::BaseApiResponse response;
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] = (int)totalPages;
    response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] = (int)totalPages == pageNo ? (Json::UInt64)totalCount : (Json::UInt64)(pageNo * pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &winner : winners) {
      Json::Value j = winner.toJson();
      Json::Value item;
      item["id"] = j["id"];
      item["drawId"] = j["draw_id"];
      item["ticketId"] = j["ticket_id"];
      item["customerId"] = j["customer_id"];
      item["prizeAmount"] = j["prize_amount"];
      item["actualPayout"] = j["actual_payout"];
      item["ticketAlias"] = j["ticket_alias"];
      std::string phone = j["phone_number"].asString();
      if (phone.length() > 6) {
        phone = phone.substr(0, 3) + std::string(phone.length() - 6, '*') + phone.substr(phone.length() - 3);
      }
      item["phoneNumber"] = phone;
      data.append(item);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =  "Database error while fetching recent winners.";
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