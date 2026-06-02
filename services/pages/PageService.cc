//
// Created by Emmanuel Addo-Odame on 02/06/2026.
//

#include "PageService.h"
#include <drogon/orm/CoroMapper.h>
#include "Pages.h"
#include "constants/ErrorCodes.h"

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {


drogon::Task<dto::BaseApiResponse> PageService::getAll(int pageNo, int pageSize, const std::string &query) {

    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Pages> mapper(dbClient);

    Criteria searchCriteria;
    if (!query.empty()) {
        std::string likeQuery = "%" + query + "%";

        searchCriteria = Criteria(Pages::Cols::_slug, CompareOperator::Like, likeQuery) ||
            Criteria(Pages::Cols::_title, CompareOperator::Like, likeQuery) ||
            Criteria(Pages::Cols::_meta_description, CompareOperator::Like, likeQuery) ||
            Criteria(Pages::Cols::_meta_keywords, CompareOperator::Like, likeQuery);
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
        auto items = co_await mapper.orderBy(Pages::Cols::_created_at, SortOrder::ASC)
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
        errorResponse.error["message"] = "Database error while fetching FAQs.";
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


drogon::Task<dto::BaseApiResponse> PageService::create(const dto::PageDto &dto) {


    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Pages> mapper(dbClient);

    try {
        Pages newPage;

        newPage.setSlug(dto.getSlug());
        newPage.setTitle(dto.getTitle());
        newPage.setContent(dto.getContent());
        newPage.setMetaDescription(dto.getMetaDescription());
        newPage.setMetaKeywords(dto.getMetaKeywords());
        newPage.setVersion(dto.getVersion());
        newPage.setPublishedAtToNull();
        newPage.setIsActive(dto.isActive());

        auto result = co_await mapper.insert(newPage);

        dto::BaseApiResponse response;
        response.success = true;
        response.message = "Page created successfully";
        response.result["id"] = result.getValueOfId();
        co_return response;

    } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while creating FAQ.";
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


drogon::Task<dto::BaseApiResponse> PageService::update(const dto::PageDto &dto, const std::string &id) {

    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Pages> mapper(dbClient);

    try {
        auto page = co_await mapper.findByPrimaryKey(id);

        page.setSlug(dto.getSlug());
        page.setTitle(dto.getTitle());
        page.setContent(dto.getContent());
        page.setMetaDescription(dto.getMetaDescription());
        page.setMetaKeywords(dto.getMetaKeywords());
        page.setVersion(dto.getVersion());
        page.setIsActive(dto.isActive());

        co_await mapper.update(page);

        dto::BaseApiResponse response;
        response.success = true;
        response.message = "Page updated successfully";
        co_return response;

    } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while updating Page.";
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


drogon::Task<dto::BaseApiResponse> PageService::deletePage(const std::string &id) {

    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Pages> mapper(dbClient);

    try {

        auto deletedCount = co_await mapper.deleteByPrimaryKey(id);

        if (deletedCount == 0) {
            dto::BaseApiResponse errorResponse;
            errorResponse.success = false;
            errorResponse.error["code"] = constants::ERR_DB_NOT_FOUND;
            errorResponse.error["message"] = "FAQ not found.";
            co_return errorResponse;
        }

        dto::BaseApiResponse response;
        response.success = true;
        response.message = "Page deleted successfully";
        co_return response;

    } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while deleting FAQ.";
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