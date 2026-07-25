//
// Created by Emmanuel Addo-Odame on 24/07/2026.
//
#include "TicketCollectionService.h"
#include "TicketCollections.h"
#include "constants/ErrorCodes.h"
#include <drogon/orm/CoroMapper.h>

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

    drogon::Task<dto::BaseApiResponse> TicketCollectionService::getAll(int pageNo, int pageSize, const std::string &query) {
        auto dbClient = drogon::app().getDbClient();
        CoroMapper<TicketCollections> mapper(dbClient);

        Criteria searchCriteria;
        if (!query.empty()) {
            std::string likeQuery = "%" + query + "%";
            searchCriteria = Criteria(TicketCollections::Cols::_name, CompareOperator::Like, likeQuery);
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

            auto collections = co_await mapper.orderBy(TicketCollections::Cols::_created_at, SortOrder::DESC)
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
            for (const auto &col : collections) {
                Json::Value j = col.toJson();
                Json::Value item;
                item["id"] = j["id"];
                item["name"] = j["name"];
                item["quantity"] = j["quantity"];
                item["unitPrice"] = j["unit_price"];
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
            errorResponse.error["message"] = "Database error while fetching ticket collections.";
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

    drogon::Task<dto::BaseApiResponse> TicketCollectionService::getAllActive(int pageNo, int pageSize, const std::string &query) {
        auto dbClient = drogon::app().getDbClient();
        CoroMapper<TicketCollections> mapper(dbClient);

        Criteria searchCriteria;
        if (!query.empty()) {
            std::string likeQuery = "%" + query + "%";
            searchCriteria = Criteria(TicketCollections::Cols::_name, CompareOperator::Like, likeQuery);
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

            auto collections = co_await mapper.orderBy(TicketCollections::Cols::_created_at, SortOrder::DESC)
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
            for (const auto &col : collections) {
                Json::Value j = col.toJson();
                Json::Value item;
                item["id"] = j["id"];
                item["name"] = j["name"];
                item["quantity"] = j["quantity"];
                item["unitPrice"] = j["unit_price"];
                data.append(item);
            }
            response.result["data"] = data;
            co_return response;

        } catch (const DrogonDbException &e) {
            dto::BaseApiResponse errorResponse;
            errorResponse.success = false;
            errorResponse.error["code"] = constants::ERR_DB_QUERY;
            errorResponse.error["message"] = "Database error while fetching ticket collections.";
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

    drogon::Task<dto::BaseApiResponse> TicketCollectionService::create(const dto::TicketCollectionDto &dto) {
        auto dbClient = drogon::app().getDbClient();
        CoroMapper<TicketCollections> mapper(dbClient);

        try {
            TicketCollections collection;
            collection.setName(dto.getName());
            collection.setQuantity(dto.getQuantity());
            collection.setIsActive(dto.getIsActive());
            collection.setUnitPrice(std::to_string(dto.getUnitPrice()));
            collection.setCreatedAt(trantor::Date::now());
            auto saved = co_await mapper.insert(collection);

            dto::BaseApiResponse response;
            response.success = true;
            response.result["message"] = "Ticket collection created successfully.";
            
            Json::Value j = saved.toJson();
            Json::Value item;
            item["id"] = j["id"];
            
            response.result["data"] = item;
            co_return response;
        } catch (const DrogonDbException &e) {
            dto::BaseApiResponse errorResponse;
            errorResponse.success = false;
            errorResponse.error["code"] = constants::ERR_DB_QUERY;
            errorResponse.error["message"] = "Database error while creating ticket collection.";
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

    drogon::Task<dto::BaseApiResponse> TicketCollectionService::update(const dto::TicketCollectionDto &dto, const std::string &id) {
        auto dbClient = drogon::app().getDbClient();
        CoroMapper<TicketCollections> mapper(dbClient);

        try {
            auto collection = co_await mapper.findByPrimaryKey(id);
            collection.setName(dto.getName());
            collection.setQuantity(dto.getQuantity());
            collection.setIsActive(dto.getIsActive());
            collection.setUnitPrice(std::to_string(dto.getUnitPrice()));

            auto updatedCount = co_await mapper.update(collection);

            if (updatedCount == 0) {
                dto::BaseApiResponse response;
                response.success = false;
                response.error["code"] = constants::ERR_DB_NOT_FOUND;
                response.error["message"] = "Ticket collection not found for update.";
                co_return response;
            }

            dto::BaseApiResponse response;
            response.success = true;
            response.result["message"] = "Ticket collection updated successfully.";
            
            Json::Value j = collection.toJson();
            Json::Value item;
            item["id"] = j["id"];
            item["name"] = j["name"];
            item["quantity"] = j["quantity"];
            item["unitPrize"] = j["unit_prize"];
            item["isActive"] = j["is_active"];
            item["createdAt"] = j["created_at"];
            item["updatedAt"] = j["updated_at"];
            
            response.result["data"] = item;
            co_return response;
        } catch (const UnexpectedRows &) {
            dto::BaseApiResponse errorResponse;
            errorResponse.success = false;
            errorResponse.error["code"] = constants::ERR_DB_NOT_FOUND;
            errorResponse.error["message"] = "Ticket collection not found.";
            co_return errorResponse;
        } catch (const DrogonDbException &e) {
            dto::BaseApiResponse errorResponse;
            errorResponse.success = false;
            errorResponse.error["code"] = constants::ERR_DB_QUERY;
            errorResponse.error["message"] = "Database error while updating ticket collection.";
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

    drogon::Task<dto::BaseApiResponse> TicketCollectionService::deleteItem(const std::string &itemId) {
        auto dbClient = drogon::app().getDbClient();
        CoroMapper<TicketCollections> mapper(dbClient);

        try {
            auto deletedCount = co_await mapper.deleteByPrimaryKey(itemId);

            if (deletedCount == 0) {
                dto::BaseApiResponse response;
                response.success = false;
                response.error["code"] = constants::ERR_DB_NOT_FOUND;
                response.error["message"] = "Ticket collection not found for deletion.";
                co_return response;
            }

            dto::BaseApiResponse response;
            response.success = true;
            response.result["message"] = "Ticket collection deleted successfully.";
            co_return response;
        } catch (const DrogonDbException &e) {
            dto::BaseApiResponse errorResponse;
            errorResponse.success = false;
            errorResponse.error["code"] = constants::ERR_DB_QUERY;
            errorResponse.error["message"] = "Database error while deleting ticket collection.";
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