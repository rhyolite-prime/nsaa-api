//
// Created by Emmanuel Addo-Odame on 26/02/2026.
//

#include "PriceTierService.h"
#include "PrizeTiers.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include "dto/CreatePriceTierDto.h"
#include "dto/UpdatePriceTierDto.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

drogon::Task<dto::BaseApiResponse> PriceTierService::getAll(int pageNo, int pageSize, const std::string &query) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<PrizeTiers> mapper(dbClient);

  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria = Criteria(PrizeTiers::Cols::_tier_name, CompareOperator::Like, likeQuery) ||
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
    response.result["upperBound"] = (int)totalPages == pageNo ? (Json::UInt64)totalCount : (Json::UInt64)(pageNo * pageSize);

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

drogon::Task<dto::BaseApiResponse> PriceTierService::createAsync(const dto::CreatePriceTierDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<PrizeTiers> mapper(dbClient);

  try {
    PrizeTiers newTier;
    newTier.setTierName(dto.getTierName());
    newTier.setTierLevel(dto.getTierLevel());
    newTier.setDescription(dto.getDescription());
    newTier.setPrizeAmount(dto.getPrizeAmount());
    newTier.setPrizePercentage(dto.getPrizePercentage());
    newTier.setNumberOfWinners(dto.getNumberOfWinners());
    newTier.setIsCashback(dto.getIsCashback());

    auto result = co_await mapper.insert(newTier);

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Price tier created successfully.";
    response.result["id"] = result.getValueOfId();
    co_return response;
  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while creating prize tier.";
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

drogon::Task<dto::BaseApiResponse> PriceTierService::updateAsync(const dto::UpdatePriceTierDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<PrizeTiers> mapper(dbClient);

  try {
    auto existingTier = co_await mapper.findByPrimaryKey(dto.getId());

    existingTier.setTierName(dto.getTierName());
    existingTier.setTierLevel(dto.getTierLevel());
    existingTier.setDescription(dto.getDescription());
    existingTier.setPrizeAmount(dto.getPrizeAmount());
    existingTier.setPrizePercentage(dto.getPrizePercentage());
    existingTier.setNumberOfWinners(dto.getNumberOfWinners());
    existingTier.setIsCashback(dto.getIsCashback());

    co_await mapper.update(existingTier);

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Price tier updated successfully.";
    co_return response;

  } catch (const drogon::orm::UnexpectedRows &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    errorResponse.error["message"] = "Price tier not found.";
    co_return errorResponse;
  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while updating prize tier.";
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

drogon::Task<dto::BaseApiResponse> PriceTierService::deletePriceTier(const std::string &tierId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<PrizeTiers> mapper(dbClient);

  try {
    co_await mapper.deleteByPrimaryKey(tierId);

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Price tier deleted successfully.";
    co_return response;
  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while deleting prize tier.";
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

}
