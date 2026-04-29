//
// Created by Emmanuel Addo-Odame on 05/03/2026.
//
#include "RaffleService.h"
#include "Draws.h"
#include "Raffles.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <ctime>
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>
#include <string>

#include "RngLogs.h"
#include "Tickets.h"
#include "PrizeTiers.h"
#include "plugins/RaffleServicePlugin.h"
#include "utils/CronUtils.h"

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

static std::string toCamelCase(const std::string &snake) {
  std::string camel;
  bool nextUpper = false;
  for (char c : snake) {
    if (c == '_') {
      nextUpper = true;
    } else {
      if (nextUpper) {
        camel += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        nextUpper = false;
      } else {
        camel += c;
      }
    }
  }
  return camel;
}

static Json::Value snakeCaseToCamelCase(const Json::Value &val) {
  if (val.isObject()) {
    Json::Value camelObj(Json::objectValue);
    for (auto it = val.begin(); it != val.end(); ++it) {
      camelObj[toCamelCase(it.name())] = snakeCaseToCamelCase(*it);
    }
    return camelObj;
  } else if (val.isArray()) {
    Json::Value camelArray(Json::arrayValue);
    for (const auto &item : val) {
      camelArray.append(snakeCaseToCamelCase(item));
    }
    return camelArray;
  }
  return val;
}

drogon::Task<dto::BaseApiResponse> RaffleService::getAll(int pageNo, int pageSize, const std::string &query,
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
    addCriteria(
        Criteria(Raffles::Cols::_name, CompareOperator::Like, likeQuery) ||
        Criteria(Raffles::Cols::_slug, CompareOperator::Like, likeQuery));
  }

  if (!status.empty()) {
    addCriteria(Criteria(Raffles::Cols::_status, CompareOperator::EQ, status));
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

drogon::Task<dto::BaseApiResponse> RaffleService::getActiveRaffle() {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> mapper(dbClient);

  try {

    auto raffle = co_await mapper.findOne(Criteria(Raffles::Cols::_status, CompareOperator::EQ, "ACTIVE"));

    if (raffle.getValueOfStatus() != "ACTIVE") {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "The specified raffle is not active.";
      co_return errorResponse;
    }

    dto::BaseApiResponse response;
    response.success = true;
    Json::Value raffleJson;
    raffleJson["id"] = raffle.getValueOfId();
    raffleJson["name"] = raffle.getValueOfName();
    raffleJson["slug"] = raffle.getValueOfSlug();
    raffleJson["status"] = raffle.getValueOfStatus();
    raffleJson["totalEntries"] = raffle.getValueOfTotalEntries();
    raffleJson["createdAt"] = raffle.getValueOfCreatedAt().toDbStringLocal();
    raffleJson["prizePool"] = raffle.getValueOfPrizePool();
    raffleJson["winningColour"] = raffle.getValueOfWinningColour();
    raffleJson["startsAt"] = raffle.getValueOfStartTime().toDbStringLocal();
    raffleJson["endsAt"] = raffle.getValueOfEndTime().toDbStringLocal();
    raffleJson["drawTime"] = raffle.getValueOfDrawTime().toDbStringLocal();
    response.result = raffleJson;
    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Raffle not found or database error.";
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
      snprintf(buf, sizeof(buf), "%04d%02d%02d-%02d%02d",  tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min);
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

      auto newDrawResult =  co_await drawMapper.insert(newDraw);

      //create a schedule on Quartz for raffle start, raffle end and raffle draw time

      auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
      auto &quartzApi = plugin->getQuartzApi();

      auto &app = drogon::app();
      auto customConfig = app.getCustomConfig();
      std::string raffleActivationCallBackUrl = customConfig["QuartzSchedulerApi"]["RaffleActivationCallBackUrl"].asString();

      dto::QuartzJobDto raffleActivationQuartzJobDto;
      raffleActivationQuartzJobDto.name = "raffle-activation-" + result.getValueOfId();
      raffleActivationQuartzJobDto.description = "Activates raffle at scheduled start time";
      raffleActivationQuartzJobDto.customData.uniqueId = newDrawResult.getValueOfId();
      // Default or empty callbackUrl as not specified by user context
      raffleActivationQuartzJobDto.customData.callbackUrl = raffleActivationCallBackUrl;

      const auto& activationStartTime = result.getValueOfStartTime();
      auto activationEndTime = activationStartTime.after(5 * 60);  // 5 mins apart

      raffleActivationQuartzJobDto.schedule = utils::CronUtils::buildCronExpression(activationStartTime);
      raffleActivationQuartzJobDto.startDate = activationStartTime.toDbStringLocal();
      raffleActivationQuartzJobDto.endDate = activationEndTime.toDbStringLocal();

      co_await quartzApi.scheduleJob(raffleActivationQuartzJobDto);

      // closure
      std::string raffleClosureCallBackUrl = customConfig["QuartzSchedulerApi"]["RaffleClosureCallBackUrl"].asString();

      dto::QuartzJobDto raffleClosureQuartzJobDto;
      raffleClosureQuartzJobDto.name = "raffle-closure-" + result.getValueOfId();
      raffleClosureQuartzJobDto.description = "Closes raffle at scheduled start time";
      raffleClosureQuartzJobDto.customData.uniqueId = newDrawResult.getValueOfId();
      // Default or empty callbackUrl as not specified by user context
      raffleClosureQuartzJobDto.customData.callbackUrl = raffleClosureCallBackUrl;


      const auto& closureStartTime = result.getValueOfEndTime();
      auto closureEndTime = closureStartTime.after(5 * 60);

      raffleClosureQuartzJobDto.schedule = raffle::utils::CronUtils::buildCronExpression(closureStartTime);
      raffleClosureQuartzJobDto.startDate = closureStartTime.toDbStringLocal();
      raffleClosureQuartzJobDto.endDate = closureEndTime.toDbStringLocal();

      co_await quartzApi.scheduleJob(raffleClosureQuartzJobDto);

      // draw
      std::string raffleDrawCallBackUrl = customConfig["QuartzSchedulerApi"]["RaffleDrawCallBackUrl"].asString();

      dto::QuartzJobDto raffleDrawQuartzJobDto;
      raffleDrawQuartzJobDto.name = "raffle-draw-" + result.getValueOfId();
      raffleDrawQuartzJobDto.description = "Initiates raffle draw at scheduled start time";
      raffleDrawQuartzJobDto.customData.uniqueId = newDrawResult.getValueOfId();
      // Default or empty callbackUrl as not specified by user context
      raffleDrawQuartzJobDto.customData.callbackUrl = raffleDrawCallBackUrl;

      auto drawStartTime = result.getValueOfDrawTime();
      auto drawEndTime = drawStartTime.after(5 * 60);

      raffleDrawQuartzJobDto.schedule = utils::CronUtils::buildCronExpression(drawStartTime);
      raffleDrawQuartzJobDto.startDate = drawStartTime.toDbStringLocal();
      raffleDrawQuartzJobDto.endDate = drawEndTime.toDbStringLocal();

      co_await quartzApi.scheduleJob(raffleDrawQuartzJobDto);

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

drogon::Task<dto::BaseApiResponse> RaffleService::updateAsync(const dto::UpdateRaffleDto &dto) {
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

    if (!dto.getSlug().empty() &&
        existingRaffle.getValueOfSlug() != dto.getSlug()) {
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
      existingRaffle.setStartTime(trantor::Date::fromDbStringLocal(
          normalizeDateString(dto.getStartTime())));
    }

    if (!dto.getEndTime().empty()) {
      existingRaffle.setEndTime(trantor::Date::fromDbStringLocal(
          normalizeDateString(dto.getEndTime())));
    }

    if (!dto.getDrawTime().empty()) {
      auto newDrawTime = trantor::Date::fromDbStringLocal(
          normalizeDateString(dto.getDrawTime()));
      if (existingRaffle.getValueOfDrawTime() != newDrawTime) {
        existingRaffle.setDrawTime(newDrawTime);
        drawTimeChanged = true;
      }
    }

    auto result = co_await mapper.update(existingRaffle);

    if (drawTimeChanged || slugChanged) {
      try {
        CoroMapper<Draws> drawMapper(dbClient);
        auto draw = co_await drawMapper.findOne(Criteria(
            Draws::Cols::_raffle_id, CompareOperator::EQ, dto.getId()));

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
      errorResponse.error["message"] =
          "Cannot delete an active raffle. Deactivate it first.";
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
      // Continue with raffle deletion even if draw deletion fails (or should we
      // fail?) Given the requirement, let's proceed but log the error.
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

drogon::Task<dto::BaseApiResponse> RaffleService::incrementEntries(const std::string &raffleId) {
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

drogon::Task<dto::BaseApiResponse> RaffleService::incrementPricePool(const std::string &raffleId) {
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

drogon::Task<dto::BaseApiResponse> RaffleService::updateWinningColour(const std::string &raffleId) {
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

drogon::Task<dto::BaseApiResponse> RaffleService::activate(const std::string &raffleId) {
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


  drogon::Task<dto::BaseApiResponse> RaffleService::closeRaffle(const std::string &raffleId) {
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

    auto raffle = co_await mapper.findByPrimaryKey(raffleId);
    raffle.setStatus("CLOSED");
    co_await mapper.update(raffle);

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Raffle closed successfully";
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

drogon::Task<dto::BaseApiResponse> RaffleService::getDetails(const std::string &raffleId) {
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

    auto raffle = co_await mapper.findByPrimaryKey(raffleId);

    dto::BaseApiResponse response;
    response.success = true;
    Json::Value raffleJson;
    raffleJson["id"] = raffle.getValueOfId();
    raffleJson["name"] = raffle.getValueOfName();
    raffleJson["slug"] = raffle.getValueOfSlug();
    raffleJson["status"] = raffle.getValueOfStatus();
    raffleJson["totalEntries"] = raffle.getValueOfTotalEntries();
    raffleJson["createdAt"] = raffle.getValueOfCreatedAt().toDbStringLocal();
    raffleJson["prizePool"] = raffle.getValueOfPrizePool();
    raffleJson["winningColour"] = raffle.getValueOfWinningColour();
    raffleJson["winningColourId"] = raffle.getValueOfWinningColourId();
    raffleJson["startsAt"] = raffle.getValueOfStartTime().toDbStringLocal();
    raffleJson["endsAt"] = raffle.getValueOfEndTime().toDbStringLocal();
    raffleJson["drawTime"] = raffle.getValueOfDrawTime().toDbStringLocal();

    // Fetch associated draw and prize tiers
    raffleJson["prizeTiers"] = Json::arrayValue;
    try {
      CoroMapper<Draws> drawMapper(dbClient);
      auto draw = co_await drawMapper.findOne(Criteria(Draws::Cols::_raffle_id, CompareOperator::EQ, raffleId));
      raffleJson["draw"] = snakeCaseToCamelCase(draw.toJson());

      // Fetch RNG logs for prize tiers
      auto drawSummary = co_await dbClient->execSqlCoro(
          "select * from rng_logs where rng_type = 'DRAW_COMPLETED' and generated_value = 'SUCCESS' and "
          "draw_id = $1 ",
          draw.getValueOfId());

      Json::Value drawSummaryJson = Json::objectValue;
      if (!drawSummary.empty()) {
        const auto &row = drawSummary[0];
        drawSummaryJson["id"] = row["id"].as<std::string>();
        drawSummaryJson["drawId"] = row["draw_id"].as<std::string>();
        drawSummaryJson["rngType"] = row["rng_type"].as<std::string>();
        drawSummaryJson["generatedValue"] = row["generated_value"].as<std::string>();

        if (row["metadata"].isNull()) {
          drawSummaryJson["metadata"] = Json::nullValue;
        } else {
          drawSummaryJson["metadata"] = row["metadata"].as<Json::Value>();
        }
      }

      raffleJson["drawSummary"] = snakeCaseToCamelCase(drawSummaryJson);

      // Fetch RNG logs for prize tiers
      auto rngLogs = co_await dbClient->execSqlCoro(
          "SELECT rng_type, count(*) as winner_count "
          "FROM rng_logs "
          "WHERE draw_id = $1 "
          "GROUP BY rng_type",
          draw.getValueOfId());

      Json::Value tiers = Json::arrayValue;

      // Fetch prize tiers from the database
      CoroMapper<PrizeTiers> prizeTierMapper(dbClient);
      auto prizeTiers = co_await prizeTierMapper.orderBy(PrizeTiers::Cols::_tier_level, SortOrder::ASC).findAll();

      for (auto const &tierData : prizeTiers) {
        Json::Value tier;
        tier["name"] = tierData.getValueOfTierName();
        
        // Convert prize_amount (string/numeric) to double/int
        std::string amountStr = tierData.getValueOfPrizeAmount();
        double amountValue = 0.0;
        if (!amountStr.empty()) {
          try {
            amountValue = std::stod(amountStr);
          } catch (...) {
            amountValue = 0.0;
          }
        }
        tier["amount"] = amountValue;
        tier["winners"] = 0;

        // Map system_name to rng_type
        std::string systemName = tierData.getValueOfSystemName();
        std::string rngType;
        if (systemName == "ultimate") {
          rngType = "ULTIMATE_WINNER";
        } else {
          std::string upperSysName = systemName;
          std::transform(upperSysName.begin(), upperSysName.end(), upperSysName.begin(), ::toupper);
          rngType = "TIER_WINNER_" + upperSysName;
        }

        for (auto const &row : rngLogs) {
          if (row["rng_type"].as<std::string>() == rngType) {
            tier["winners"] = row["winner_count"].as<int>();
            break;
          }
        }
        tiers.append(tier);
      }
      raffleJson["prizeWinnerTiers"] = tiers;

    } catch (...) {
      raffleJson["draw"] = Json::nullValue;
    }

    response.result = raffleJson;
    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Raffle not found or database error.";
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

drogon::Task<dto::BaseApiResponse> RaffleService::getRecentTicketHolders(const std::string &raffleId) {
  auto dbClient = drogon::app().getDbClient();
  try {
    if (raffleId.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Raffle ID cannot be empty.";
      co_return errorResponse;
    }

    auto result = co_await dbClient->execSqlCoro(
        "SELECT t.id, t.ticket_number, t.created_at, t.total_amount, "
        "c.first_name, c.last_name, c.msisdn "
        "FROM tickets t "
        "JOIN customers c ON t.customer_id = c.id "
        "WHERE t.raffle_id = $1 AND t.payment_status = 'PAID' "
        "ORDER BY t.created_at DESC "
        "LIMIT 10",
        raffleId);

    dto::BaseApiResponse response;
    response.success = true;
    Json::Value data = Json::arrayValue;
    for (auto const &row : result) {
      Json::Value item;
      item["id"] = row["id"].as<std::string>();
      item["ticketNumber"] = row["ticket_number"].as<std::string>();
      item["amount"] = row["total_amount"].as<std::string>();
      item["createdAt"] = row["created_at"].as<std::string>();
      item["holder"] = row["first_name"].as<std::string>() + " " +
                       row["last_name"].as<std::string>();
      item["msisdn"] = row["msisdn"].as<std::string>();
      data.append(item);
    }
    response.result["data"] = data;
    co_return response;
  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching ticket holders.";
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

drogon::Task<dto::BaseApiResponse> RaffleService::getTopTicketHolders(const std::string &raffleId) {
  auto dbClient = drogon::app().getDbClient();
  try {
    if (raffleId.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Raffle ID cannot be empty.";
      co_return errorResponse;
    }

    auto result = co_await dbClient->execSqlCoro(
        "SELECT c.first_name, c.last_name, c.msisdn, SUM(t.quantity) as "
        "total_tickets, MAX(t.ticket_number) as ticket_number, "
        "SUM(SUM(t.quantity)) OVER () as total_valid_entries "
        "FROM tickets t "
        "JOIN customers c ON t.customer_id = c.id "
        "WHERE t.raffle_id = $1 AND t.payment_status = 'PAID' "
        "GROUP BY c.id, c.first_name, c.last_name, c.msisdn "
        "ORDER BY total_tickets DESC "
        "LIMIT 6",
        raffleId);

    dto::BaseApiResponse response;
    response.success = true;
    Json::Value data = Json::arrayValue;
    for (auto const &row : result) {
      Json::Value item;
      item["firstName"] = row["first_name"].as<std::string>();
      item["lastName"] = row["last_name"].as<std::string>();
      item["holder"] = row["first_name"].as<std::string>() + " " +
                       row["last_name"].as<std::string>();
      item["msisdn"] = row["msisdn"].as<std::string>();
      item["ticketNumber"] = row["ticket_number"].as<std::string>();
      item["totalTickets"] = row["total_tickets"].as<int>();
      data.append(item);
    }
    response.result["data"] = data;
    response.result["totalValidEntries"] = result.empty() ? 0 : result[0]["total_valid_entries"].as<int>();
    co_return response;
  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching top ticket holders.";
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

drogon::Task<dto::BaseApiResponse> RaffleService::getAllRaffleTickets(const std::string &raffleId, int pageNo, int pageSize) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Tickets> mapper(dbClient);

  try {
    if (raffleId.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Raffle ID cannot be empty.";
      co_return errorResponse;
    }

    auto totalCount = co_await mapper.count(
        Criteria(Tickets::Cols::_raffle_id, CompareOperator::EQ, raffleId));

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    int offset = (pageNo - 1) * pageSize;
    auto tickets =
        co_await mapper.orderBy(Tickets::Cols::_created_at, SortOrder::DESC)
            .limit(pageSize)
            .offset(offset)
            .findBy(Criteria(Tickets::Cols::_raffle_id, CompareOperator::EQ,
                             raffleId));

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
    for (const auto &ticket : tickets) {
      data.append(snakeCaseToCamelCase(ticket.toJson()));
    }
    response.result["data"] = data;
    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching raffle tickets.";
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

drogon::Task<dto::BaseApiResponse> RaffleService::getTickets(const std::string &raffleId, int pageNo, int pageSize) {
  return getAllRaffleTickets(raffleId, pageNo, pageSize);
}

drogon::Task<dto::BaseApiResponse> RaffleService::getTicketStats(const std::string &raffleId) {
  auto dbClient = drogon::app().getDbClient();

  try {
    if (raffleId.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Raffle ID cannot be empty.";
      co_return errorResponse;
    }

    auto result = co_await dbClient->execSqlCoro(
        "SELECT count(*) as total_tickets, "
        "count(CASE WHEN is_active = true THEN 1 END) as active_tickets, "
        "count(CASE WHEN payment_status = 'PAID' THEN 1 END) as paid_tickets, "
        "coalesce(sum(CASE WHEN payment_status = 'PAID' THEN "
        "CAST(total_amount AS NUMERIC) ELSE 0 END), 0) as total_revenue, "
        "coalesce(sum(CASE WHEN payment_status = 'PAID' THEN quantity ELSE 0 "
        "END), 0) as total_entries "
        "FROM tickets "
        "WHERE raffle_id = $1",
        raffleId);

    dto::BaseApiResponse response;
    response.success = true;
    if (!result.empty()) {
      response.result["totalTickets"] = result[0]["total_tickets"].as<int>();
      response.result["activeTickets"] = result[0]["active_tickets"].as<int>();
      response.result["paidTickets"] = result[0]["paid_tickets"].as<int>();
      response.result["totalRevenue"] = result[0]["total_revenue"].as<double>();
      response.result["totalEntries"] = result[0]["total_entries"].as<int>();
    } else {
      response.result["totalTickets"] = 0;
      response.result["activeTickets"] = 0;
      response.result["paidTickets"] = 0;
      response.result["totalRevenue"] = 0.0;
      response.result["totalEntries"] = 0;
    }
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while fetching ticket stats.";
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


drogon::Task<dto::BaseApiResponse> RaffleService::getRngLogs(const std::string &raffleId) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> raffleMapper(dbClient);
  CoroMapper<Draws> drawMapper(dbClient);
  CoroMapper<RngLogs> rngLogsMapper(dbClient);

  try {

    auto raffle = co_await raffleMapper.findByPrimaryKey(raffleId);

    auto draw = co_await drawMapper.findOne(Criteria(Draws::Cols::_raffle_id, CompareOperator::EQ,  raffle.getValueOfId()));

    auto rngLogs = co_await rngLogsMapper.findBy(Criteria(RngLogs::Cols::_draw_id, CompareOperator::EQ, draw.getValueOfId()));

    dto::BaseApiResponse response;
    response.success = true;

    Json::Value data = Json::arrayValue;

    for (const auto &rngLog : rngLogs) {
      Json::Value rngLogJson;
      rngLogJson["id"] = rngLog.getValueOfId();
      rngLogJson["drawId"] = rngLog.getValueOfDrawId();
      rngLogJson["rngType"] = rngLog.getValueOfRngType();
      rngLogJson["seedValue"] = rngLog.getValueOfSeedValue();
      rngLogJson["generatedValue"] = rngLog.getValueOfGeneratedValue();
      rngLogJson["generatedAt"] = rngLog.getValueOfGeneratedAt().toDbStringLocal();

      // Handle metadata parsing
      std::string metadataStr = rngLog.getValueOfMetadata();
      if (!metadataStr.empty()) {
        Json::Value metadataJson;
        Json::Reader reader;
        if (reader.parse(metadataStr, metadataJson)) {
          rngLogJson["metadata"] = metadataJson;
        } else {
          rngLogJson["metadata"] = metadataStr;
        }
      } else {
        rngLogJson["metadata"] = Json::nullValue;
      }
      data.append(rngLogJson);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching RNG logs.";
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


drogon::Task<dto::BaseApiResponse> RaffleService::executeDraw() {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> raffleMapper(dbClient);
  CoroMapper<Draws> drawMapper(dbClient);

  try {
    // 1. Get the active raffle
    auto activeRaffle = co_await raffleMapper.findOne(
        Criteria(Raffles::Cols::_status, CompareOperator::EQ, "ACTIVE"));

    // 2. Get the associated draw ID
    auto draw = co_await drawMapper.findOne(
        Criteria(Draws::Cols::_raffle_id, CompareOperator::EQ,
                 activeRaffle.getValueOfId()));

    // 3. Run execute draw postgres function
    auto sqlResult = co_await dbClient->execSqlCoro(
        "SELECT public.execute_draw($1)", draw.getValueOfId());

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Draw executed successfully";

    if (!sqlResult.empty()) {
      // Extract the JSONB result from the first column of the first row
      response.result = snakeCaseToCamelCase(sqlResult[0][0].as<Json::Value>());

      // at this point results are guaranteed
      // update raffle status to "COMPLETED".
      activeRaffle.setStatus("COMPLETED");
      co_await raffleMapper.update(activeRaffle);

    } else {
      response.result["drawId"] = draw.getValueOfId();
    }

    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while executing draw.";
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


drogon::Task<dto::BaseApiResponse> RaffleService::runDraw(const std::string &raffleId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> raffleMapper(dbClient);
  CoroMapper<Draws> drawMapper(dbClient);

  try {

    auto activeRaffle = co_await raffleMapper.findByPrimaryKey(raffleId);

    // 2. Get the associated draw ID
    auto draw = co_await drawMapper.findOne(Criteria(Draws::Cols::_raffle_id, CompareOperator::EQ,  raffleId));

    // 3. Run execute draw postgres function
    auto sqlResult = co_await dbClient->execSqlCoro("SELECT public.execute_draw($1)", draw.getValueOfId());

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Draw executed successfully";

    if (!sqlResult.empty()) {
      // Extract the JSONB result from the first column of the first row
      response.result = snakeCaseToCamelCase(sqlResult[0][0].as<Json::Value>());

      // at this point results are guaranteed
      // update raffle status to "COMPLETED".
      activeRaffle.setStatus("COMPLETED");
      co_await raffleMapper.update(activeRaffle);

    } else {
      response.result["drawId"] = draw.getValueOfId();
    }

    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while executing draw.";
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
