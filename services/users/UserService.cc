//
// Created by Emmanuel Addo-Odame on 07/03/2026.
//

#include "UserService.h"
#include "Users.h"
#include "bcrypt.h"
#include <jwt-cpp/jwt.h>
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

drogon::Task<dto::BaseApiResponse> UserService::getAll(int pageNo, int pageSize, const std::string &query) {

  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<Users>(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria =
        Criteria(Users::Cols::_first_name, CompareOperator::Like, likeQuery) ||
        Criteria(Users::Cols::_email, CompareOperator::Like, likeQuery) ||
        Criteria(Users::Cols::_last_name, CompareOperator::Like, likeQuery);
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
    response.result["totalPages"] =
        (int)((totalCount + pageSize - 1) / pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &role : users) {
      Json::Value roleJson = role.toJson();
      Json::Value camelCaseRole;
      camelCaseRole["id"] = roleJson["id"];
      camelCaseRole["firstName"] = roleJson["first_name"];
      camelCaseRole["lastName"] = roleJson["last_name"];
      camelCaseRole["email"] = roleJson["email"];
      camelCaseRole["phoneNumber"] = roleJson["phone_number"];
      camelCaseRole["username"] = roleJson["username"];
      camelCaseRole["profileImageUrl"] = roleJson["profile_image_url"];
      camelCaseRole["isLockedOut"] = roleJson["is_locked_out"];
      camelCaseRole["isActive"] = roleJson["is_active"];
      camelCaseRole["isAdmin"] = roleJson["is_admin_user"];
      camelCaseRole["createdAt"] = roleJson["created_at"];

      if (roleJson.isMember("roles") && roleJson["roles"].isString() && !roleJson["roles"].asString().empty()) {
        Json::Value parsedRoles;
        Json::CharReaderBuilder readerBuilder;
        std::string errs;
        std::istringstream s(roleJson["roles"].asString());
        if (Json::parseFromStream(readerBuilder, s, &parsedRoles, &errs)) {
          camelCaseRole["roles"] = parsedRoles;
        } else {
          camelCaseRole["roles"] = Json::arrayValue;
        }
      } else {
        camelCaseRole["roles"] = Json::arrayValue;
      }

      data.append(camelCaseRole);
    }
    response.result["data"] = data;
  } catch (const DrogonDbException &e) {
    response.success = false;
    response.error["message"] = "Database error while fetching users.";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse> UserService::getStats() {
  auto dbClient = drogon::app().getDbClient();

  try {
    auto result = co_await dbClient->execSqlCoro(
        "SELECT "
        "COUNT(*) as total_users, "
        "SUM(CASE WHEN is_active = true THEN 1 ELSE 0 END) as active_users, "
        "SUM(CASE WHEN is_locked_out = true THEN 1 ELSE 0 END) as "
        "locked_out_users "
        "FROM users");

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "User stats retrieved successfully";

    if (!result.empty()) {
      auto row = result[0];
      response.result["totalUsers"] =
          (Json::UInt64)row["total_users"].as<uint64_t>();
      response.result["activeUsers"] =
          (Json::UInt64)row["active_users"].as<uint64_t>();
      response.result["lockedOutUsers"] =
          (Json::UInt64)row["locked_out_users"].as<uint64_t>();
    } else {
      response.result["totalUsers"] = 0;
      response.result["activeUsers"] = 0;
      response.result["lockedOutUsers"] = 0;
    }

    co_return response;
  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching user stats.";
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

drogon::Task<dto::BaseApiResponse> UserService::getPermissions() {

    dto::BaseApiResponse response;
    try {
      const auto &customConfig = drogon::app().getCustomConfig();
      if (customConfig.isMember("Permissions") && customConfig["Permissions"].isArray()) {
        const auto &permissions = customConfig["Permissions"];
        response.result["data"] = permissions;
        response.result["totalCount"] = (Json::UInt64)permissions.size();
      } else {
        response.result["data"] = Json::arrayValue;
        response.result["totalCount"] = 0;
      }
      response.success = true;
      response.message = "Permissions retrieved successfully";
    } catch (const std::exception &e) {
      response.success = false;
      response.error["code"] = constants::ERR_INTERNAL;
      response.error["message"] = "Internal error.";
      response.error["detail"] = e.what();
    }
    co_return response;
  }

drogon::Task<dto::BaseApiResponse> UserService::createAsync(const dto::CreateUserDto &userDto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mp(dbClient);

  Users newUser;
  newUser.setFirstName(userDto.getFirstName());
  newUser.setLastName(userDto.getLastName());
  newUser.setEmail(userDto.getEmail());
  newUser.setUsername(userDto.getUsername());
  newUser.setPhoneNumber(userDto.getPhoneNumber());
  newUser.setPasswordHash(bcrypt::generateHash(userDto.getPassword()));
  newUser.setIsActive(true);
  newUser.setIsAdminUser(userDto.isAdmin());
  newUser.setIsLockedOut(false);
  newUser.setCreatedAt(trantor::Date::now());

  Json::Value rolesJson(Json::arrayValue);
  for (const auto& role : userDto.getRoles()) {
    rolesJson.append(role);
  }
  Json::StreamWriterBuilder writer;
  writer["indentation"] = "";
  newUser.setRoles(Json::writeString(writer, rolesJson));

  dto::BaseApiResponse response;
  try {
    Users publication = co_await mp.insert(newUser);
    response.success = true;
    response.message = "User created successfully";
    response.result["id"] = publication.getValueOfId();
  } catch (const drogon::orm::DrogonDbException &e) {
    response.success = false;
    response.message = "Database error while creating user";
    response.error["code"] = constants::ERR_DB_QUERY;
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse> UserService::updateAsync(const dto::UpdateUserDto &userDto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mp(dbClient);

  dto::BaseApiResponse response;
  try {
    if (userDto.getId().empty()) {
      response.success = false;
      response.message = "User ID is required for updating";
      response.error["code"] = constants::ERR_VALIDATION;
      co_return response;
    }

    auto existingUser = co_await mp.findByPrimaryKey(userDto.getId());

    if (!userDto.getFirstName().empty()) {
      existingUser.setFirstName(userDto.getFirstName());
    }
    if (!userDto.getLastName().empty()) {
      existingUser.setLastName(userDto.getLastName());
    }
    if (!userDto.getEmail().empty()) {
      existingUser.setEmail(userDto.getEmail());
    }
    if (!userDto.getUsername().empty()) {
      existingUser.setUsername(userDto.getUsername());
    }
    if (!userDto.getPhoneNumber().empty()) {
      existingUser.setPhoneNumber(userDto.getPhoneNumber());
    }

    if (!userDto.getRoles().empty()) {
      Json::Value rolesJson(Json::arrayValue);
      for (const auto& role : userDto.getRoles()) {
        rolesJson.append(role);
      }
      Json::StreamWriterBuilder writer;
      writer["indentation"] = "";
      existingUser.setRoles(Json::writeString(writer, rolesJson));
    }

    existingUser.setUpdatedAt(trantor::Date::now());
    existingUser.setIsAdminUser(userDto.isAdmin());
    existingUser.setIsActive(userDto.isActive());

    co_await mp.update(existingUser);

    response.success = true;
    response.message = "User updated successfully";
  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "User not found or database error while updating";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    response.error["detail"] = e.base().what();
  } catch (const std::exception &e) {
    response.success = false;
    response.message = "Internal error during user update";
    response.error["code"] = constants::ERR_INTERNAL;
    response.error["detail"] = e.what();
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse> UserService::deleteUser(const std::string &userId) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mp(dbClient);

  dto::BaseApiResponse response;
  try {
    // Check if user exists first
    co_await mp.findOne(
        Criteria(Users::Cols::_id, CompareOperator::EQ, userId));

    // Delete the user
    size_t count = co_await mp.deleteBy(
        Criteria(Users::Cols::_id, CompareOperator::EQ, userId));

    if (count > 0) {
      response.success = true;
      response.message = "User deleted successfully";
    } else {
      // This part should technically not be reached if findOne succeeded,
      // but added for completeness.
      response.success = false;
      response.message = "Failed to delete user";
      response.error["code"] = constants::ERR_DB_QUERY;
    }

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "User not found or database error";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
  }
  co_return response;
}

}
