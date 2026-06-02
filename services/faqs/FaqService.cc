//
// Created by Emmanuel Addo-Odame on 02/06/2026.
//
#include "FaqService.h"
#include "constants/ErrorCodes.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>
#include "Faqs.h"

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

drogon::Task<dto::BaseApiResponse> FaqService::getAll(int pageNo, int pageSize, const std::string &query) {
    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Faqs> mapper(dbClient);

    Criteria searchCriteria;
    if (!query.empty()) {
        std::string likeQuery = "%" + query + "%";
        searchCriteria =
            Criteria(Faqs::Cols::_question, CompareOperator::Like, likeQuery) ||
            Criteria(Faqs::Cols::_answer, CompareOperator::Like, likeQuery);
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
        auto items = co_await mapper.orderBy(Faqs::Cols::_display_order, SortOrder::ASC)
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

drogon::Task<dto::BaseApiResponse> FaqService::create(const dto::FaqDto &dto) {
    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Faqs> mapper(dbClient);

    try {
        Faqs faq;
        faq.setQuestion(dto.getQuestion());
        faq.setAnswer(dto.getAnswer());
        if (!dto.getCategory().empty()) {
            faq.setCategory(dto.getCategory());
        }
        faq.setDisplayOrder(dto.getDisplayOrder());
        faq.setIsActive(dto.isActive());

        auto result = co_await mapper.insert(faq);

        dto::BaseApiResponse response;
        response.success = true;
        response.message = "FAQ created successfully";
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

drogon::Task<dto::BaseApiResponse> FaqService::update(const dto::FaqDto &dto,const std::string &id) {
    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Faqs> mapper(dbClient);

    try {
        auto faq = co_await mapper.findByPrimaryKey(id);

        if (!dto.getQuestion().empty()) {
            faq.setQuestion(dto.getQuestion());
        }
        if (!dto.getAnswer().empty()) {
            faq.setAnswer(dto.getAnswer());
        }
        if (!dto.getCategory().empty()) {
            faq.setCategory(dto.getCategory());
        }
        faq.setDisplayOrder(dto.getDisplayOrder());
        faq.setIsActive(dto.isActive());

        co_await mapper.update(faq);

        dto::BaseApiResponse response;
        response.success = true;
        response.message = "FAQ updated successfully";
        co_return response;

    } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while updating FAQ.";
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

drogon::Task<dto::BaseApiResponse> FaqService::deleteFaq(const std::string &id) {
    auto dbClient = drogon::app().getDbClient();
    CoroMapper<Faqs> mapper(dbClient);

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
        response.message = "FAQ deleted successfully";
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