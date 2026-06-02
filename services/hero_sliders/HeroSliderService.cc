//
// Created by Emmanuel Addo-Odame on 02/06/2026.
//

#include "HeroSliderService.h"

#include <drogon/orm/CoroMapper.h>
#include "HeroSlides.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include "dto/HeroSliderDto.h"

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

drogon::Task<dto::BaseApiResponse> HeroSliderService::getAll(int pageNo, int pageSize, const std::string &query) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<HeroSlides> mapper(dbClient);

  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria =
        Criteria(HeroSlides::Cols::_headline, CompareOperator::Like, likeQuery) ||
        Criteria(HeroSlides::Cols::_copy, CompareOperator::Like, likeQuery) ||
        Criteria(HeroSlides::Cols::_cta_primary, CompareOperator::Like, likeQuery) ||
        Criteria(HeroSlides::Cols::_cta_secondary, CompareOperator::Like, likeQuery);
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
        co_await mapper.orderBy(HeroSlides::Cols::_created_at, SortOrder::DESC)
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
    for (const auto &color : colors) {
      Json::Value j = color.toJson();
      Json::Value item;

      item["id"] = j["id"];
      item["headline"] = j["headline"];
      item["copy"] = j["copy"];
      item["imageUrl"] = j["image_url"];
      item["ctaPrimary"] = j["cta_primary"];
      item["ctaSecondary"] = j["cta_secondary"];
      item["displayOrder"] = j["display_order"];
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


drogon::Task<dto::BaseApiResponse> HeroSliderService::create(const dto::HeroSliderDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<HeroSlides> mapper(dbClient);

  try {
    HeroSlides newSlide;
    newSlide.setHeadline(dto.getHeadline());
    newSlide.setCopy(dto.getCopy());
    newSlide.setImageUrl(dto.getImageUrl());
    newSlide.setCtaPrimary(dto.getCtaPrimary());
    newSlide.setCtaSecondary(dto.getCtaSecondary());
    newSlide.setDisplayOrder(dto.getDisplayOrder());
    newSlide.setIsActive(dto.isActive());


    auto result = co_await mapper.insert(newSlide);

    dto::BaseApiResponse response;
    response.success = true;
    response.result["message"] = "Heroslide created successfully";

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


drogon::Task<dto::BaseApiResponse> HeroSliderService::update(const dto::HeroSliderDto &dto, const std::string &id) {

    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<HeroSlides>(dbClient);

    dto::BaseApiResponse response;
    try {
        auto heroSlides = co_await mp.findOne(Criteria(HeroSlides::Cols::_id, CompareOperator::EQ, id));

        heroSlides.setHeadline(dto.getHeadline());
        heroSlides.setCopy(dto.getCopy());
        heroSlides.setImageUrl(dto.getImageUrl());
        heroSlides.setCtaPrimary(dto.getCtaPrimary());
        heroSlides.setCtaSecondary(dto.getCtaSecondary());
        heroSlides.setDisplayOrder(dto.getDisplayOrder());
        heroSlides.setIsActive(dto.isActive());

        co_await mp.update(heroSlides);

        response.success = true;
        response.message = "Category updated successfully";

    } catch (const UnexpectedRows &e) {
        response.success = false;
        response.message = "Category not found";
        response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Failed to update category";
        response.error["detail"] = e.base().what();
    }
    co_return response;

}


drogon::Task<dto::BaseApiResponse> HeroSliderService::deleteHeroSlider(const std::string &id) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<HeroSlides> mapper(dbClient);

  try {
    if (id.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "HeroSlider ID cannot be empty.";
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


}
