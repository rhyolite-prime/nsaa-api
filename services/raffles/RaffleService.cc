//
// Created by Emmanuel Addo-Odame on 05/03/2026.
//
#include "RaffleService.h"
#include "Draws.h"
#include "Raffles.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

static std::string normalizeDateString(std::string dateStr) {
  if (dateStr.empty())
    return dateStr;
  std::replace(dateStr.begin(), dateStr.end(), 'T', ' ');
  if (dateStr.length() == 16) { // YYYY-MM-DD HH:MM
    dateStr += ":00";
  }
  return dateStr;
}

drogon::Task<dto::BaseApiResponse> RaffleService::getAll(
    int pageNo, int pageSize, const std::string &query,
    const std::string &startDate, const std::string &endDate,
    const std::string &status) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> mapper(dbClient);

  Criteria searchCriteria;
  bool criteriaSet = false;

  auto addCriteria = [&](const Criteria &c) {
    if (!criteriaSet) {
      searchCriteria = c;
      criteriaSet = true;
    } else {
      searchCriteria = searchCriteria && c;
    }
  };

  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    addCriteria(Criteria(Raffles::Cols::_name, CompareOperator::Like,
                         likeQuery) ||
                Criteria(Raffles::Cols::_slug, CompareOperator::Like,
                         likeQuery));
  }

  if (!status.empty()) {
    addCriteria(
        Criteria(Raffles::Cols::_status, CompareOperator::EQ, status));
  }

  if (!startDate.empty()) {
    addCriteria(Criteria(
        Raffles::Cols::_start_time, CompareOperator::GE,
        trantor::Date::fromDbStringLocal(normalizeDateString(startDate))));
  }

  if (!endDate.empty()) {
    addCriteria(Criteria(
        Raffles::Cols::_start_time, CompareOperator::LE,
        trantor::Date::fromDbStringLocal(normalizeDateString(endDate))));
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
    auto raffles =
        co_await mapper.orderBy(Raffles::Cols::_start_time, SortOrder::DESC)
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
    for (const auto &raffle : raffles) {
      Json::Value j = raffle.toJson();
      Json::Value item;

      item["id"] = j["id"];
      item["name"] = j["name"];
      item["slug"] = j["slug"];
      item["totalEntries"] = j["total_entries"];
      item["prizePool"] = j["prize_pool"];
      item["winningColour"] = j["winning_colour"];
      item["startsAt"] = j["start_time"];
      item["endsAt"] = j["end_time"];
      item["drawTime"] = j["draw_time"];
      item["status"] = j["status"];
      item["createdAt"] = j["created_at"];

      data.append(item);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching draws.";
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

drogon::Task<dto::BaseApiResponse> RaffleService::createAsync(const dto::CreateRaffleDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> mapper(dbClient);

  try {
    if (dto.getName().empty() || dto.getStartTime().empty() ||
        dto.getEndTime().empty() || dto.getDrawTime().empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] =
          "Name, Start Time, End Time, and Draw Time are required.";
      co_return errorResponse;
    }

    Raffles newRaffle;
    newRaffle.setName(dto.getName());

    if (!dto.getSlug().empty()) {
      newRaffle.setSlug(dto.getSlug());
    } else {
      // Logic for slug if missing - maybe use name?
      // For now, let's assume slug is provided as per NOT NULL UNIQUE
      newRaffle.setSlug(dto.getName());
    }

    if (!dto.getStatus().empty()) {
      newRaffle.setStatus(dto.getStatus());
    } else {
      newRaffle.setStatus("SCHEDULED"); // Default status as per schema
    }

    newRaffle.setPrizePool(dto.getPrizePool());
    newRaffle.setTotalEntries(0);

    // Parse dates
    auto startTime = trantor::Date::fromDbStringLocal(normalizeDateString(dto.getStartTime()));
    auto endTime = trantor::Date::fromDbStringLocal(normalizeDateString(dto.getEndTime()));
    auto drawTime = trantor::Date::fromDbStringLocal(normalizeDateString(dto.getDrawTime()));

    newRaffle.setStartTime(startTime);
    newRaffle.setEndTime(endTime);
    newRaffle.setDrawTime(drawTime);

    // Ensure total_entries has a default if not set automatically
    newRaffle.setTotalEntries(0);

    auto result = co_await mapper.insert(newRaffle);

    // Create a corresponding Draw in PENDING state
    try {

      CoroMapper<Draws> drawMapper(dbClient);
      Draws newDraw;

      auto drawTimeDate = result.getValueOfDrawTime();
      time_t seconds =
          static_cast<time_t>(drawTimeDate.microSecondsSinceEpoch() / 1000000);
      struct tm tm_time;
      localtime_r(&seconds, &tm_time);
      char buf[32];
      snprintf(buf, sizeof(buf), "%04d%02d%02d-%02d%02d",
               tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
               tm_time.tm_hour, tm_time.tm_min);
      std::string drawCode(buf);
      newDraw.setDrawCode(drawCode);
      newDraw.setAlias(dto.getSlug());
      newDraw.setRaffleId(result.getValueOfId());
      newDraw.setStatus("SCHEDULED");

      // Set draw date and time from raffle's drawTime
      newDraw.setDrawDate(result.getValueOfDrawTime());

      // For draw_time (string in Draws model, but it's a timestamp without time
      // zone in DB) Wait, Draws.h says drawTime_ is shared_ptr<string>? Let's
      // check Draws.h again.

      newDraw.setTotalEntries(0);
      newDraw.setTotalPool("0.00");
      newDraw.setTotalPrizes("0.00");

      co_await drawMapper.insert(newDraw);
    } catch (const std::exception &e) {
      LOG_ERROR << "Failed to create initial draw for raffle: " << e.what();
      // We might want to decide if we should fail the raffle creation or just
      // log it. For now, let's keep it robust and continue since the raffle is
      // already created.
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Raffle created successfully";
    response.targetUrl = "";
    response.result = result.getValueOfId();

    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while creating raffle.";
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
RaffleService::updateAsync(const dto::UpdateRaffleDto &dto) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> mapper(dbClient);

  try {
    if (dto.getId().empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Raffle ID is required for update.";
      co_return errorResponse;
    }

    auto existingRaffle = co_await mapper.findByPrimaryKey(dto.getId());

    bool drawTimeChanged = false;
    bool slugChanged = false;

    if (!dto.getName().empty()) {
      existingRaffle.setName(dto.getName());
    }

    if (!dto.getSlug().empty() && existingRaffle.getValueOfSlug() != dto.getSlug()) {
      existingRaffle.setSlug(dto.getSlug());
      slugChanged = true;
    }

    if (!dto.getStatus().empty()) {
      existingRaffle.setStatus(dto.getStatus());
    }

    if (!dto.getPrizePool().empty()) {
      existingRaffle.setPrizePool(dto.getPrizePool());
    }

    if (!dto.getStartTime().empty()) {
      existingRaffle.setStartTime(
          trantor::Date::fromDbStringLocal(normalizeDateString(dto.getStartTime())));
    }

    if (!dto.getEndTime().empty()) {
      existingRaffle.setEndTime(
          trantor::Date::fromDbStringLocal(normalizeDateString(dto.getEndTime())));
    }

    if (!dto.getDrawTime().empty()) {
      auto newDrawTime = trantor::Date::fromDbStringLocal(normalizeDateString(dto.getDrawTime()));
      if (existingRaffle.getValueOfDrawTime() != newDrawTime) {
        existingRaffle.setDrawTime(newDrawTime);
        drawTimeChanged = true;
      }
    }

    auto result = co_await mapper.update(existingRaffle);

    if (drawTimeChanged || slugChanged) {
      try {
        CoroMapper<Draws> drawMapper(dbClient);
        auto draw = co_await drawMapper.findOne(
            Criteria(Draws::Cols::_raffle_id, CompareOperator::EQ, dto.getId()));

        if (drawTimeChanged) {
          auto drawTimeDate = existingRaffle.getValueOfDrawTime();
          time_t seconds = static_cast<time_t>(
              drawTimeDate.microSecondsSinceEpoch() / 1000000);
          struct tm tm_time;
          localtime_r(&seconds, &tm_time);
          char buf[32];
          snprintf(buf, sizeof(buf), "%04d%02d%02d-%02d%02d",
                   tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                   tm_time.tm_hour, tm_time.tm_min);
          draw.setDrawCode(std::string(buf));
          draw.setDrawDate(drawTimeDate);
        }

        if (slugChanged) {
          draw.setAlias(existingRaffle.getValueOfSlug());
        }

        co_await drawMapper.update(draw);
      } catch (const std::exception &e) {
        LOG_ERROR << "Failed to update associated draw for raffle "
                  << dto.getId() << ": " << e.what();
      }
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Raffle updated successfully";
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while updating raffle.";
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

drogon::Task<dto::BaseApiResponse> RaffleService::deleteRaffle(const std::string &raffleId) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> mapper(dbClient);

  try {
    if (raffleId.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Raffle ID cannot be empty.";
      co_return errorResponse;
    }

    // 1. Fetch the raffle to check its status
    auto existingRaffle = co_await mapper.findByPrimaryKey(raffleId);

    // 2. Guard: Prevent deletion of ACTIVE raffles
    if (existingRaffle.getValueOfStatus() == "ACTIVE") {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Cannot delete an active raffle. Deactivate it first.";
      co_return errorResponse;
    }

    // 3. Delete associated draw record if it exists
    try {
      CoroMapper<Draws> drawMapper(dbClient);
      co_await drawMapper.deleteBy(
          Criteria(Draws::Cols::_raffle_id, CompareOperator::EQ, raffleId));
    } catch (const std::exception &e) {
      LOG_ERROR << "Failed to delete associated draw for raffle " << raffleId
                << ": " << e.what();
      // Continue with raffle deletion even if draw deletion fails (or should we fail?)
      // Given the requirement, let's proceed but log the error.
    }

    // 4. Delete the raffle record
    auto deletedRowCount = co_await mapper.deleteByPrimaryKey(raffleId);

    if (deletedRowCount == 0) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_DB_NOT_FOUND;
      errorResponse.error["message"] = "Raffle not found or already deleted.";
      co_return errorResponse;
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Raffle deleted successfully";

    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while deleting raffle.";
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
RaffleService::incrementEntries(const std::string &raffleId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> mapper(dbClient);

  try {
    if (raffleId.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Raffle ID cannot be empty.";
      co_return errorResponse;
    }

    auto existingRaffle = co_await mapper.findByPrimaryKey(raffleId);

    int currentEntries = existingRaffle.getValueOfTotalEntries();
    existingRaffle.setTotalEntries(currentEntries + 1);

    auto updatedRowCount = co_await mapper.update(existingRaffle);

    if (updatedRowCount == 0) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_INTERNAL;
      errorResponse.error["message"] = "Failed to increment entries.";
      co_return errorResponse;
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Raffle entries incremented successfully";

    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while incrementing entries.";
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
RaffleService::incrementPricePool(const std::string &raffleId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> mapper(dbClient);

  try {
    if (raffleId.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Raffle ID cannot be empty.";
      co_return errorResponse;
    }

    auto existingRaffle = co_await mapper.findByPrimaryKey(raffleId);

    std::string currentPoolStr = existingRaffle.getValueOfPrizePool();
    long double currentPool = 0;
    if (!currentPoolStr.empty()) {
      try {
        currentPool = std::stold(currentPoolStr);
      } catch (...) {
        currentPool = 0;
      }
    }

    // Default increment by 1.0 since no amount is provided in signature
    currentPool += 1.0;

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.2Lf", currentPool);
    existingRaffle.setPrizePool(std::string(buffer));

    auto updatedRowCount = co_await mapper.update(existingRaffle);

    if (updatedRowCount == 0) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_INTERNAL;
      errorResponse.error["message"] = "Failed to increment price pool.";
      co_return errorResponse;
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Raffle price pool incremented successfully";
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while incrementing price pool.";
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
RaffleService::updateWinningColour(const std::string &raffleId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> mapper(dbClient);

  try {
    if (raffleId.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Raffle ID cannot be empty.";
      co_return errorResponse;
    }

    // Pick a random color
    auto result = co_await dbClient->execSqlCoro(
        "SELECT id, name FROM colours ORDER BY RANDOM() LIMIT 1");

    if (result.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_DB_NOT_FOUND;
      errorResponse.error["message"] = "No colours found in the database.";
      co_return errorResponse;
    }

    std::string colorId = result[0]["id"].as<std::string>();
    std::string colorName = result[0]["name"].as<std::string>();

    auto existingRaffle = co_await mapper.findByPrimaryKey(raffleId);

    existingRaffle.setWinningColourId(colorId);
    existingRaffle.setWinningColour(colorName);

    auto updatedRowCount = co_await mapper.update(existingRaffle);

    if (updatedRowCount == 0) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_INTERNAL;
      errorResponse.error["message"] = "Failed to update winning colour.";
      co_return errorResponse;
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Winning colour updated successfully";
    response.result["colourId"] = colorId;
    response.result["colourName"] = colorName;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while updating winning colour.";
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

drogon::Task<dto::BaseApiResponse>RaffleService::activate(const std::string &raffleId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> mapper(dbClient);

  try {
    if (raffleId.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Raffle ID cannot be empty.";
      co_return errorResponse;
    }

    // 1. Check if there's already an active raffle
    auto activeRaffles = co_await mapper.findBy(
        Criteria(Raffles::Cols::_status, CompareOperator::EQ, "ACTIVE"));

    if (!activeRaffles.empty()) {
      bool isSameRaffle = false;
      for (const auto &r : activeRaffles) {
        if (r.getValueOfId() == raffleId) {
          isSameRaffle = true;
          break;
        }
      }

      if (!isSameRaffle) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_VALIDATION;
        errorResponse.error["message"] =
            "Another raffle is already active. Deactivate it first.";
        co_return errorResponse;
      }
    }

    auto raffle = co_await mapper.findByPrimaryKey(raffleId);
    raffle.setStatus("ACTIVE");
    co_await mapper.update(raffle);

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Raffle activated successfully";
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while activating raffle.";
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
