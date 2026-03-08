//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#include "ColorService.h"
#include "constants/ErrorCodes.h"
#include "models/Colours.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

drogon::Task<dto::BaseApiResponse> ColorService::getAll(int pageNo, int pageSize, const std::string &query) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Colours> mapper(dbClient);

  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria =
        Criteria(Colours::Cols::_name, CompareOperator::Like, likeQuery) ||
        Criteria(Colours::Cols::_code, CompareOperator::Like, likeQuery) ||
        Criteria(Colours::Cols::_hex_color, CompareOperator::Like, likeQuery);
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
        co_await mapper.orderBy(Colours::Cols::_created_at, SortOrder::DESC)
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
      item["name"] = j["name"];
      item["code"] = j["code"];
      item["hexColor"] = j["hex_color"];
      item["emoji"] = j["emoji"];
      item["sortOrder"] = j["sort_order"];
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

drogon::Task<dto::BaseApiResponse>
ColorService::createAsync(const dto::CreateColorDto &dto) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Colours> mapper(dbClient);

  try {
    Colours newColor;
    newColor.setName(dto.getName());
    newColor.setCode(dto.getCode());
    newColor.setHexColor(dto.getHexColor());
    newColor.setSortOrder(dto.getSortOrder());

    auto result = co_await mapper.insert(newColor);

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Color created successfully";

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

drogon::Task<dto::BaseApiResponse>
ColorService::updateAsync(const dto::UpdateColorDto &dto) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Colours> mapper(dbClient);

  try {
    auto colorId = dto.getId();
    if (colorId.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Color ID cannot be empty.";
      co_return errorResponse;
    }

    auto existingColor = co_await mapper.findByPrimaryKey(colorId);

    // Update fields if they are provided
    if (!dto.getName().empty())
      existingColor.setName(dto.getName());
    if (!dto.getCode().empty())
      existingColor.setCode(dto.getCode());
    if (!dto.getHexColor().empty())
      existingColor.setHexColor(dto.getHexColor());
    if (dto.getSortOrder() != 0)
      existingColor.setSortOrder(
          dto.getSortOrder()); // Or whatever validation fits

    auto updatedRowCount = co_await mapper.update(existingColor);

    if (updatedRowCount == 0) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_INTERNAL;
      errorResponse.error["message"] = "Failed to update color.";
      co_return errorResponse;
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Color updated successfully";

    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while updating color.";
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
ColorService::deleteColor(const std::string &colorId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Colours> mapper(dbClient);

  try {
    if (colorId.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Color ID cannot be empty.";
      co_return errorResponse;
    }

    auto deletedRowCount = co_await mapper.deleteByPrimaryKey(colorId);

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

} // namespace raffle::services
