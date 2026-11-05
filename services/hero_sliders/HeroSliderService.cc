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

  dto::BaseApiResponse response;

  if (dto.getHeadline().empty()) {
    response.success = false;
    response.message = "Headline is required.";
    response.error["code"] = constants::ERR_VALIDATION;
    response.error["message"] = "Headline is required.";
    co_return response;
  }

  // The image is uploaded by the controller, the url is what gets persisted.
  if (dto.getImageUrl().empty()) {
    response.success = false;
    response.message = "Image is required.";
    response.error["code"] = constants::ERR_VALIDATION;
    response.error["message"] = "Image is required.";
    co_return response;
  }

  try {
    HeroSlides newSlide;
    newSlide.setHeadline(dto.getHeadline());
    newSlide.setImageUrl(dto.getImageUrl());
    newSlide.setDisplayOrder(dto.getDisplayOrder());
    newSlide.setIsActive(dto.isActive());

    // Optional columns are left out so that the database defaults apply.
    if (!dto.getCopy().empty()) {
      newSlide.setCopy(dto.getCopy());
    }
    if (!dto.getCtaPrimary().empty()) {
      newSlide.setCtaPrimary(dto.getCtaPrimary());
    }
    if (!dto.getCtaSecondary().empty()) {
      newSlide.setCtaSecondary(dto.getCtaSecondary());
    }

    auto result = co_await mapper.insert(newSlide);

    response.success = true;
    response.message = "Hero slide created successfully";
    response.result["id"] = result.getValueOfId();
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while creating hero slide.";
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

    if (id.empty()) {
        response.success = false;
        response.message = "Hero slide id is required.";
        response.error["code"] = constants::ERR_VALIDATION;
        response.error["message"] = "Hero slide id is required.";
        co_return response;
    }

    try {
        auto heroSlide = co_await mp.findOne(Criteria(HeroSlides::Cols::_id, CompareOperator::EQ, id));

        // Only the fields supplied by the client are changed, so a partial
        // update can never wipe the image url or any other column.
        if (dto.hasHeadline()) {
            if (dto.getHeadline().empty()) {
                response.success = false;
                response.message = "Headline cannot be empty.";
                response.error["code"] = constants::ERR_VALIDATION;
                response.error["message"] = "Headline cannot be empty.";
                co_return response;
            }
            heroSlide.setHeadline(dto.getHeadline());
        }
        if (dto.hasCopy()) {
            heroSlide.setCopy(dto.getCopy());
        }
        if (dto.hasImageUrl()) {
            heroSlide.setImageUrl(dto.getImageUrl());
        }
        if (dto.hasCtaPrimary()) {
            heroSlide.setCtaPrimary(dto.getCtaPrimary());
        }
        if (dto.hasCtaSecondary()) {
            heroSlide.setCtaSecondary(dto.getCtaSecondary());
        }
        if (dto.hasDisplayOrder()) {
            heroSlide.setDisplayOrder(dto.getDisplayOrder());
        }
        if (dto.hasIsActive()) {
            heroSlide.setIsActive(dto.isActive());
        }

        heroSlide.setUpdatedAt(trantor::Date::now());

        co_await mp.update(heroSlide);

        response.success = true;
        response.message = "Hero slide updated successfully";
        response.result["id"] = id;

    } catch (const UnexpectedRows &e) {
        response.success = false;
        response.message = "Hero slide not found";
        response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        response.error["message"] = "Hero slide not found";
    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Failed to update hero slide";
        response.error["code"] = constants::ERR_DB_QUERY;
        response.error["message"] = "Failed to update hero slide";
        response.error["detail"] = e.base().what();
    } catch (const std::exception &e) {
        response.success = false;
        response.message = "Internal error.";
        response.error["code"] = constants::ERR_INTERNAL;
        response.error["detail"] = e.what();
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
