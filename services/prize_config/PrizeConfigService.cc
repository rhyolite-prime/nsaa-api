//
// Created by Emmanuel Addo-Odame on 18/08/2026.
//
#include "PrizeConfigService.h"
#include "PrizeCategories.h"
#include "Prizes.h"
#include "constants/ErrorCodes.h"

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

    drogon::Task<dto::BaseApiResponse> PrizeConfigService::getAllCategories(int pageNo, int pageSize, const std::string &query) {

      auto dbClient = drogon::app().getDbClient();
      CoroMapper<PrizeCategories> mapper(dbClient);

      Criteria searchCriteria;
      if (!query.empty()) {
        std::string likeQuery = "%" + query + "%";
        searchCriteria =
            Criteria(PrizeCategories::Cols::_name, CompareOperator::Like, likeQuery) ||
            Criteria(PrizeCategories::Cols::_code, CompareOperator::Like, likeQuery) ||
            Criteria(PrizeCategories::Cols::_description, CompareOperator::Like, likeQuery);
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
        auto prizeCategories = co_await mapper.orderBy(PrizeCategories::Cols::_created_at, SortOrder::DESC)
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
        for (const auto &prizeCategory : prizeCategories) {
          Json::Value j = prizeCategory.toJson();
          Json::Value item;

          item["id"] = j["id"];
          item["name"] = j["name"];
          item["code"] = j["code"];
          item["description"] = j["description"];
          item["priority"] = j["priority"];
          item["priority"] = j["priority"];
          item["maxWinners"] = j["max_winners"];
          item["rewardType"] = j["reward_type"];
          item["isRepeatable"] = j["is_repeatable"];
          item["isActive"] = j["is_active"];
          item["createdAt"] = j["created_at"];
          data.append(item);
        }
        response.result["data"] = data;
        co_return response;

      } catch (const DrogonDbException &e) {
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

    drogon::Task<dto::BaseApiResponse> PrizeConfigService::createCategory(const dto::PrizeCategoryDto &dto) {

      auto dbClient = drogon::app().getDbClient();
      CoroMapper<PrizeCategories> mapper(dbClient);

      try {
        PrizeCategories newPrizeCategory;
        newPrizeCategory.setName(dto.getName());
        newPrizeCategory.setCode(dto.getCode());
        newPrizeCategory.setDescription(dto.getDescription());
        newPrizeCategory.setPriority(dto.getPriority());
        newPrizeCategory.setMaxWinners(dto.getMaxWinners());
        newPrizeCategory.setRewardType(dto.getRewardType());
        newPrizeCategory.setIsActive(dto.isActive());
        newPrizeCategory.setIsRepeatable(dto.getIsRepeatable());

        auto result = co_await mapper.insert(newPrizeCategory);

        dto::BaseApiResponse response;
        response.success = true;
        response.result["message"] = "Prize Category created successfully";

        response.result = result.getValueOfId();
        co_return response;

      } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while creating color.";
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

    drogon::Task<dto::BaseApiResponse> PrizeConfigService::updateCategory(const dto::PrizeCategoryDto &dto, const std::string &id) {

      auto dbClient = drogon::app().getDbClient();
      CoroMapper<PrizeCategories> mapper(dbClient);

      try {

        if (id.empty()) {
          dto::BaseApiResponse errorResponse;
          errorResponse.success = false;
          errorResponse.error["code"] = constants::ERR_VALIDATION;
          errorResponse.error["message"] = "Prize Category ID cannot be empty.";
          co_return errorResponse;
        }

        auto existingPrizeCategory = co_await mapper.findByPrimaryKey(id);

        existingPrizeCategory.setName(dto.getName());
        existingPrizeCategory.setCode(dto.getCode());
        existingPrizeCategory.setDescription(dto.getDescription());
        existingPrizeCategory.setPriority(dto.getPriority());
        existingPrizeCategory.setMaxWinners(dto.getMaxWinners());
        existingPrizeCategory.setRewardType(dto.getRewardType());
        existingPrizeCategory.setIsActive(dto.isActive());
        existingPrizeCategory.setIsRepeatable(dto.getIsRepeatable());

        auto updatedRowCount = co_await mapper.update(existingPrizeCategory);

        if (updatedRowCount == 0) {
          dto::BaseApiResponse errorResponse;
          errorResponse.success = false;
          errorResponse.error["code"] = constants::ERR_INTERNAL;
          errorResponse.error["message"] = "Failed to update Prize Category.";
          co_return errorResponse;
        }

        dto::BaseApiResponse response;
        response.success = true;
        response.result["message"] = "Prize Category updated successfully";

        co_return response;

      } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while updating Prize Category.";
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

    drogon::Task<dto::BaseApiResponse> PrizeConfigService::deleteCategory(const std::string &id) {

      auto dbClient = drogon::app().getDbClient();
      CoroMapper<PrizeCategories> mapper(dbClient);

      try {
        if (id.empty()) {
          dto::BaseApiResponse errorResponse;
          errorResponse.success = false;
          errorResponse.error["code"] = constants::ERR_VALIDATION;
          errorResponse.error["message"] = "Prize Category ID cannot be empty.";
          co_return errorResponse;
        }

        auto deletedRowCount = co_await mapper.deleteByPrimaryKey(id);

        if (deletedRowCount == 0) {
          dto::BaseApiResponse errorResponse;
          errorResponse.success = false;
          errorResponse.error["code"] = constants::ERR_DB_NOT_FOUND;
          errorResponse.error["message"] = "Color not found or already deleted.";
          co_return errorResponse;
        }

        dto::BaseApiResponse response;
        response.success = true;
        response.result["message"] = "Color deleted successfully";

        co_return response;

      } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while deleting color.";
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

    drogon::Task<dto::BaseApiResponse> PrizeConfigService::getAllPrizes(int pageNo, int pageSize, const std::string &query) {

      auto dbClient = drogon::app().getDbClient();
      CoroMapper<Prizes> mapper(dbClient);

      Criteria searchCriteria;
      if (!query.empty()) {
        std::string likeQuery = "%" + query + "%";
        searchCriteria =
            Criteria(Prizes::Cols::_name, CompareOperator::Like, likeQuery) ||
            Criteria(Prizes::Cols::_description, CompareOperator::Like, likeQuery);
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
        auto prizes = co_await mapper.orderBy(Prizes::Cols::_created_at, SortOrder::DESC)
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
        for (const auto &prize : prizes) {
          Json::Value j = prize.toJson();
          Json::Value item;

          item["id"] = j["id"];
          item["name"] = j["name"];
          item["value"] = j["value"];
          item["categoryId"] = j["category_id"];
          item["description"] = j["description"];
          item["currency"] = j["currency"];
          item["quantity"] = j["quantity"];
          item["rank"] = j["rank"];
          item["remainingQuantity"] = j["remaining_quantity"];
          item["probability"] = j["probability"];

          std::string metaDataStr = prize.getValueOfMetadata();
          Json::Value metaDataJson;
          Json::Reader reader;

          if (!metaDataStr.empty() && reader.parse(metaDataStr, metaDataJson)) {
            item["metaData"] = metaDataJson;
          } else {
            item["metaData"] = Json::objectValue;
          }


          data.append(item);
        }
        response.result["data"] = data;
        co_return response;

      } catch (const DrogonDbException &e) {
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

    drogon::Task<dto::BaseApiResponse> PrizeConfigService::getCategoryPrizes(const std::string &id) {

      auto dbClient = drogon::app().getDbClient();
      CoroMapper<Prizes> mapper(dbClient);

      try {

        Criteria searchCriteria =  Criteria(Prizes::Cols::_category_id, CompareOperator::EQ, id);

        auto prizes = co_await mapper.findBy(searchCriteria);

        dto::BaseApiResponse response;
        response.success = true;

        Json::Value data = Json::arrayValue;
        for (const auto &prize : prizes) {
          Json::Value j = prize.toJson();
          Json::Value item;

          item["id"] = j["id"];
          item["name"] = j["name"];
          item["value"] = j["value"];
          item["categoryId"] = j["category_id"];
          item["description"] = j["description"];
          item["currency"] = j["currency"];
          item["quantity"] = j["quantity"];
          item["rank"] = j["rank"];
          item["remainingQuantity"] = j["remaining_quantity"];
          item["probability"] = j["probability"];

          std::string metaDataStr = prize.getValueOfMetadata();
          Json::Value metaDataJson;
          Json::Reader reader;

          if (!metaDataStr.empty() && reader.parse(metaDataStr, metaDataJson)) {
            item["metaData"] = metaDataJson;
          } else {
            item["metaData"] = Json::objectValue;
          }

          data.append(item);
        }
        response.result["data"] = data;
        co_return response;

      } catch (const DrogonDbException &e) {
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

    drogon::Task<dto::BaseApiResponse> PrizeConfigService::createPrize(const dto::PrizeDto &dto) {

      auto dbClient = drogon::app().getDbClient();
      CoroMapper<Prizes> mapper(dbClient);

      try {

        Prizes newPrize;
        newPrize.setCategoryId(dto.getCategoryId());
        newPrize.setCurrency(dto.getCurrency());
        newPrize.setName(dto.getName());
        newPrize.setDescription(dto.getDescription());
        newPrize.setValue(std::to_string(dto.getValue()));
        newPrize.setProbability(std::to_string(dto.getWinProbability()));
        newPrize.setQuantity(dto.getQuantity());
        newPrize.setRemainingQuantity(dto.getRemainingQuantity());

        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";

        newPrize.setMetadata(Json::writeString(builder, dto.getMetaData()));

        auto result = co_await mapper.insert(newPrize);

        dto::BaseApiResponse response;
        response.success = true;
        response.result["message"] = "Prize created successfully";

        response.result = result.getValueOfId();
        co_return response;

      } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while creating prize.";
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

    drogon::Task<dto::BaseApiResponse> PrizeConfigService::updatePrize(const dto::PrizeDto &dto, const std::string &id) {

      auto dbClient = drogon::app().getDbClient();
      CoroMapper<Prizes> mapper(dbClient);

      try {

        if (id.empty()) {
          dto::BaseApiResponse errorResponse;
          errorResponse.success = false;
          errorResponse.error["code"] = constants::ERR_VALIDATION;
          errorResponse.error["message"] = "Prize ID cannot be empty.";
          co_return errorResponse;
        }

        auto existingPrize = co_await mapper.findByPrimaryKey(id);

        existingPrize.setName(dto.getName());
        existingPrize.setCategoryId(dto.getCategoryId());
        existingPrize.setCurrency(dto.getCurrency());
        existingPrize.setName(dto.getName());
        existingPrize.setDescription(dto.getDescription());
        existingPrize.setValue(std::to_string(dto.getValue()));
        existingPrize.setProbability(std::to_string(dto.getWinProbability()));
        existingPrize.setQuantity(dto.getQuantity());
        existingPrize.setRemainingQuantity(dto.getRemainingQuantity());

        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";

        existingPrize.setMetadata(Json::writeString(builder, dto.getMetaData()));

        auto updatedRowCount = co_await mapper.update(existingPrize);

        if (updatedRowCount == 0) {
          dto::BaseApiResponse errorResponse;
          errorResponse.success = false;
          errorResponse.error["code"] = constants::ERR_INTERNAL;
          errorResponse.error["message"] = "Failed to update prize.";
          co_return errorResponse;
        }

        dto::BaseApiResponse response;
        response.success = true;
        response.result["message"] = "Prize updated successfully";

        co_return response;

      } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while updating Prize.";
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

    drogon::Task<dto::BaseApiResponse> PrizeConfigService::deletePrize(const std::string &id) {

      auto dbClient = drogon::app().getDbClient();
      CoroMapper<Prizes> mapper(dbClient);

      try {
        if (id.empty()) {
          dto::BaseApiResponse errorResponse;
          errorResponse.success = false;
          errorResponse.error["code"] = constants::ERR_VALIDATION;
          errorResponse.error["message"] = "Prize ID cannot be empty.";
          co_return errorResponse;
        }

        auto deletedRowCount = co_await mapper.deleteByPrimaryKey(id);

        if (deletedRowCount == 0) {
          dto::BaseApiResponse errorResponse;
          errorResponse.success = false;
          errorResponse.error["code"] = constants::ERR_DB_NOT_FOUND;
          errorResponse.error["message"] = "Prize not found or already deleted.";
          co_return errorResponse;
        }

        dto::BaseApiResponse response;
        response.success = true;
        response.result["message"] = "Prize deleted successfully";

        co_return response;

      } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while deleting prize.";
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
