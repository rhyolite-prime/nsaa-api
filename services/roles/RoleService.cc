//
// Created by Emmanuel Addo-Odame on 09/03/2026.
//

#include "RoleService.h"

#include <drogon/orm/CoroMapper.h>

#include "bcrypt.h"
#include "Roles.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {


drogon::Task<dto::BaseApiResponse> RoleService::getAll(int pageNo, int pageSize, const std::string &query) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<Roles>(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria =
        Criteria(Roles::Cols::_name, CompareOperator::Like, likeQuery) ||
        Criteria(Roles::Cols::_display_name, CompareOperator::Like, likeQuery) ||
        Criteria(Roles::Cols::_description, CompareOperator::Like, likeQuery);
  }

  dto::BaseApiResponse response;
  try {
    // 2. Get the total count matching the criteria
    size_t totalCount = co_await mp.count(searchCriteria);
    if (totalCount == 0) {
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    // 3. Find the paginated data
    int offset = (pageNo - 1) * pageSize;
    auto users =
        co_await mp.limit(pageSize).offset(offset).findBy(searchCriteria);

    // 4. Build the final response
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &role : users) {
      Json::Value roleJson = role.toJson();
      Json::Value camelCaseRole;
      camelCaseRole["id"] = roleJson["id"];
      camelCaseRole["name"] = roleJson["name"];
      camelCaseRole["displayName"] = roleJson["display_name"];
      camelCaseRole["description"] = roleJson["description"];
      Json::Value permissionsArray(Json::arrayValue);
      const auto permissionsText = roleJson["permissions"].asString();
      if (!permissionsText.empty()) {
        Json::Value permissionsParsed;
        Json::CharReaderBuilder readerBuilder;
        std::string errs;
        std::istringstream inputStream(permissionsText);
        if (Json::parseFromStream(readerBuilder, inputStream, &permissionsParsed, &errs) &&
            permissionsParsed.isArray()) {
          permissionsArray = permissionsParsed;
            }
      }
      camelCaseRole["permissions"] = permissionsArray;


      camelCaseRole["createdAt"] = roleJson["created_at"];
      data.append(camelCaseRole);
    }
    response.result["data"] = data;
  } catch (const DrogonDbException &e) {
    response.success = false;
    response.error["message"] = "Database error while fetching roles.";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}


  drogon::Task<dto::BaseApiResponse> RoleService::createAsync(const dto::CreateRoleDto &roleDto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Roles> mp(dbClient);

  Roles newRole;
  newRole.setName(roleDto.getName());
  newRole.setDisplayName(roleDto.getDisplayName());
  newRole.setDescription(roleDto.getDescription());

  Json::Value permissionsJson(Json::arrayValue);

  for (const auto &permission : roleDto.getPermissions()) {
    Json::Value permissionJson;
    permissionJson["systemName"] = permission.system_name;
    permissionJson["displayName"] = permission.display_name;
    permissionsJson.append(permissionJson);
  }

  Json::StreamWriterBuilder writer;
  writer["indentation"] = "";
  newRole.setPermissions(Json::writeString(writer, permissionsJson));

  newRole.setIsActive(true);
  newRole.setCreatedAt(trantor::Date::now());

  dto::BaseApiResponse response;
  try {
    Roles role = co_await mp.insert(newRole);
    response.success = true;
    response.message = "Role created successfully";
    response.result["id"] = role.getValueOfId();
  } catch (const drogon::orm::DrogonDbException &e) {
    response.success = false;
    response.message = "Database error while creating role";
    response.error["code"] = constants::ERR_DB_QUERY;
  }
  co_return response;
}


  drogon::Task<dto::BaseApiResponse> RoleService::updateAsync(const dto::UpdateRoleDto &roleDto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Roles> mp(dbClient);

  dto::BaseApiResponse response;
  try {
    if (roleDto.getId().empty()) {
      response.success = false;
      response.message = "Role ID is required for updating";
      response.error["code"] = constants::ERR_VALIDATION;
      co_return response;
    }

    auto existingRole = co_await mp.findByPrimaryKey(roleDto.getId());

    existingRole.setName(roleDto.getName());
    existingRole.setDisplayName(roleDto.getDisplayName());
    existingRole.setDescription(roleDto.getDescription());

    Json::Value permissionsJson(Json::arrayValue);
    for (const auto &permission : roleDto.getPermissions()) {
      Json::Value permissionJson;
      permissionJson["systemName"] = permission.system_name;
      permissionJson["displayName"] = permission.display_name;
      permissionsJson.append(permissionJson);
    }

    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    existingRole.setPermissions(Json::writeString(writer, permissionsJson));

    existingRole.setUpdatedAt(trantor::Date::now());

    co_await mp.update(existingRole);

    response.success = true;
    response.message = "Role updated successfully";
  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Role not found or database error while updating";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    response.error["detail"] = e.base().what();
  } catch (const std::exception &e) {
    response.success = false;
    response.message = "Internal error during role update";
    response.error["code"] = constants::ERR_INTERNAL;
    response.error["detail"] = e.what();
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse> RoleService::deleteRole(const std::string &roleId) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Roles> mp(dbClient);

  dto::BaseApiResponse response;
  try {
    // Check if role exists first
    co_await mp.findOne(Criteria(Roles::Cols::_id, CompareOperator::EQ, roleId));

    // Delete the user
    size_t count = co_await mp.deleteBy(Criteria(Roles::Cols::_id, CompareOperator::EQ, roleId));

    if (count > 0) {
      response.success = true;
      response.message = "Role deleted successfully";
    } else {
      // This part should technically not be reached if findOne succeeded,
      // but added for completeness.
      response.success = false;
      response.message = "Failed to delete role";
      response.error["code"] = constants::ERR_DB_QUERY;
    }

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Role not found or database error";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
  }
  co_return response;
}

}