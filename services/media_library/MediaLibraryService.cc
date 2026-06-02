//
// Created by Emmanuel Addo-Odame on 02/06/2026.
//
#include "MediaLibraryService.h"
#include "constants/ErrorCodes.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>
#include "MediaLibrary.h"

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

drogon::Task<dto::BaseApiResponse> MediaLibraryService::getAll(int pageNo, int pageSize, const std::string &query) {
    auto dbClient = drogon::app().getDbClient();
    CoroMapper<MediaLibrary> mapper(dbClient);

    Criteria searchCriteria;
    if (!query.empty()) {
        std::string likeQuery = "%" + query + "%";
        searchCriteria =
            Criteria(MediaLibrary::Cols::_title, CompareOperator::Like, likeQuery) ||
            Criteria(MediaLibrary::Cols::_file_type, CompareOperator::Like, likeQuery) ||
            Criteria(MediaLibrary::Cols::_category, CompareOperator::Like, likeQuery);
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
        auto items = co_await mapper.orderBy(MediaLibrary::Cols::_created_at, SortOrder::DESC)
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
            Json::Value j = item.toJson();
            Json::Value entry;
            entry["id"] = j["id"];
            entry["title"] = j["title"];
            entry["fileUrl"] = j["file_url"];
            entry["fileType"] = j["file_type"];
            entry["fileSize"] = j["file_size"];
            entry["category"] = j["category"];
            entry["downloadCount"] = j["download_count"];
            entry["isActive"] = j["is_active"];
            entry["createdAt"] = j["created_at"];
            data.append(entry);
        }
        response.result["data"] = data;
        co_return response;

    } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while fetching media assets.";
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

drogon::Task<dto::BaseApiResponse> MediaLibraryService::create(const dto::MediaAssetDto &dto) {
    auto dbClient = drogon::app().getDbClient();
    CoroMapper<MediaLibrary> mapper(dbClient);

    try {
        MediaLibrary asset;
        asset.setTitle(dto.getTitle());
        asset.setFileUrl(dto.getFileUrl());
        asset.setFileType(dto.getFileType());
        if (dto.getFileSize() > 0) {
            asset.setFileSize(dto.getFileSize());
        }
        if (!dto.getCategory().empty()) {
            asset.setCategory(dto.getCategory());
        }
        // download_count defaults to 0 via DB
        asset.setIsActive(dto.isActive());

        auto result = co_await mapper.insert(asset);

        dto::BaseApiResponse response;
        response.success = true;
        response.message = "Media asset created successfully";
        response.result["id"] = result.getValueOfId();
        co_return response;

    } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while creating media asset.";
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

drogon::Task<dto::BaseApiResponse> MediaLibraryService::update(const dto::MediaAssetDto &dto, const std::string &id ) {

    auto dbClient = drogon::app().getDbClient();
    CoroMapper<MediaLibrary> mapper(dbClient);

    try {
        auto asset = co_await mapper.findByPrimaryKey(id);

        if (!dto.getTitle().empty()) {
            asset.setTitle(dto.getTitle());
        }
        if (!dto.getFileUrl().empty()) {
            asset.setFileUrl(dto.getFileUrl());
        }
        if (!dto.getFileType().empty()) {
            asset.setFileType(dto.getFileType());
        }
        if (dto.getFileSize() > 0) {
            asset.setFileSize(dto.getFileSize());
        }
        if (!dto.getCategory().empty()) {
            asset.setCategory(dto.getCategory());
        }
        if (dto.getDownloadCount() >= 0) {
            asset.setDownloadCount(dto.getDownloadCount());
        }

        asset.setIsActive(dto.isActive());

        co_await mapper.update(asset);

        dto::BaseApiResponse response;
        response.success = true;
        response.message = "Media asset updated successfully";
        co_return response;

    } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while updating media asset.";
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

drogon::Task<dto::BaseApiResponse> MediaLibraryService::deleteMediaAsset(const std::string &id) {
    auto dbClient = drogon::app().getDbClient();
    CoroMapper<MediaLibrary> mapper(dbClient);

    try {
        auto deletedCount = co_await mapper.deleteByPrimaryKey(id);

        if (deletedCount == 0) {
            dto::BaseApiResponse errorResponse;
            errorResponse.success = false;
            errorResponse.error["code"] = constants::ERR_DB_NOT_FOUND;
            errorResponse.error["message"] = "Media asset not found.";
            co_return errorResponse;
        }

        dto::BaseApiResponse response;
        response.success = true;
        response.message = "Media asset deleted successfully";
        co_return response;

    } catch (const DrogonDbException &e) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_QUERY;
        errorResponse.error["message"] = "Database error while deleting media asset.";
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