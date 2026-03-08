//
// Created by Emmanuel Addo-Odame on 26/02/2026.
//

#include "PriceTierService.h"
#include "PrizeTiers.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

drogon::Task<dto::BaseApiResponse>
PriceTierService::getAll(int pageNo, int pageSize, const std::string &query) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<PrizeTiers> mapper(dbClient);

  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria = Criteria(PrizeTiers::Cols::_tier_name,
                              CompareOperator::Like, likeQuery) ||
                     Criteria(PrizeTiers::Cols::_description,
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
    auto priceTiers =
        co_await mapper.orderBy(PrizeTiers::Cols::_created_at, SortOrder::DESC)
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
    for (const auto &tier : priceTiers) {
      Json::Value j = tier.toJson();
      Json::Value item;
      item["id"] = j["id"];
      item["tierName"] = j["tier_name"];
      item["tierLevel"] = j["tier_level"];
      item["description"] = j["description"];
      item["prizeAmount"] = j["prize_amount"];
      item["prizePercentage"] = j["prize_percentage"];
      item["numberOfWinners"] = j["number_of_winners"];
      item["isCashBack"] = j["is_cashback"];
      item["createdAt"] = j["created_at"];
      data.append(item);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching prize tiers.";
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
