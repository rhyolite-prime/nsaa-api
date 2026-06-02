//
// Created by Emmanuel Addo-Odame on 02/06/2026.
//
#include "BannerService.h"
#include "constants/ErrorCodes.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>
#include "Banners.h"
#include "utils/DateUtils.h"

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

drogon::Task<dto::BaseApiResponse> BannerService::getAll(int pageNo, int pageSize, const std::string &query) {

    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Banners> mapper(dbClient);

    Criteria searchCriteria;
    if (!query.empty()) {
        std::string likeQuery = "%" + query + "%";
        searchCriteria = Criteria(Banners::Cols::_name, CompareOperator::Like, likeQuery) || Criteria(Banners::Cols::_description, CompareOperator::Like, likeQuery);
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
        auto items = co_await mapper.orderBy(Banners::Cols::_created_at, SortOrder::DESC)
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
        for (const auto &item : items) {
            data.append(item.toJson());
        }
        response.result["data"] = data;
        co_return response;

    } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while fetching banners.";
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

drogon::Task<dto::BaseApiResponse> BannerService::create(const dto::BannerDto &dto) {
    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Banners> mapper(dbClient);

    try {
        Banners banner;
        banner.setName(dto.getName());
        if (!dto.getDescription().empty()) {
            banner.setDescription(dto.getDescription());
        }
        if (!dto.getBannerImageUrl().empty()) {
            banner.setBannerImageUrl(dto.getBannerImageUrl());
        }
        if (!dto.getCtaText().empty()) {
            banner.setCtaText(dto.getCtaText());
        }
        if (!dto.getCtaLink().empty()) {
            banner.setCtaLink(dto.getCtaLink());
        }
        if (!dto.getStartDate().empty()) {
            banner.setStartDate(trantor::Date::fromDbStringLocal(utils::DateUtils::normalizeDateString(dto.getStartDate())));
        }
        if (!dto.getEndDate().empty()) {
            banner.setEndDate(trantor::Date::fromDbStringLocal(utils::DateUtils::normalizeDateString(dto.getEndDate())));
        }
        banner.setIsActive(dto.isActive());
        
        banner.setCreatedAt(trantor::Date::now());

        auto result = co_await mapper.insert(banner);

        dto::BaseApiResponse response;
        response.success = true;
        response.message = "Banner created successfully";
        response.result["id"] = result.getValueOfId();
        co_return response;

    } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while creating banner.";
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

drogon::Task<dto::BaseApiResponse> BannerService::update(const dto::BannerDto &dto, const std::string &id) {

    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Banners> mapper(dbClient);

    try {
        auto banner = co_await mapper.findByPrimaryKey(id);

        if (!dto.getName().empty()) {
            banner.setName(dto.getName());
        }
        if (!dto.getDescription().empty()) {
            banner.setDescription(dto.getDescription());
        }
        if (!dto.getBannerImageUrl().empty()) {
            banner.setBannerImageUrl(dto.getBannerImageUrl());
        }
        if (!dto.getCtaText().empty()) {
            banner.setCtaText(dto.getCtaText());
        }
        if (!dto.getCtaLink().empty()) {
            banner.setCtaLink(dto.getCtaLink());
        }
        if (!dto.getStartDate().empty()) {
            banner.setStartDate(trantor::Date::fromDbStringLocal(utils::DateUtils::normalizeDateString(dto.getStartDate())));
        }
        if (!dto.getEndDate().empty()) {
            banner.setEndDate(trantor::Date::fromDbStringLocal(utils::DateUtils::normalizeDateString(dto.getEndDate())));
        }
        banner.setIsActive(dto.isActive());
        
        banner.setUpdatedAt(trantor::Date::now());

        co_await mapper.update(banner);

        dto::BaseApiResponse response;
        response.success = true;
        response.message = "Banner updated successfully";
        co_return response;

    } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while updating banner.";
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

drogon::Task<dto::BaseApiResponse> BannerService::deleteBanner(const std::string &id) {
    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Banners> mapper(dbClient);

    try {
        auto deletedCount = co_await mapper.deleteByPrimaryKey(id);

        if (deletedCount == 0) {
            dto::BaseApiResponse errorResponse;
            errorResponse.success = false;
            errorResponse.error["code"] = constants::ERR_DB_NOT_FOUND;
            errorResponse.error["message"] = "Banner not found.";
            co_return errorResponse;
        }

        dto::BaseApiResponse response;
        response.success = true;
        response.message = "Banner deleted successfully";
        co_return response;

    } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while deleting banner.";
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