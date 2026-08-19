//
// Created by Emmanuel Addo-Odame on 05/03/2026.
//
#include "RaffleService.h"
#include "Draws.h"
#include "Raffles.h"
#include "Winners.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include <string>
#include <cstdio>
#include <ctime>
#include <random>

#include <algorithm>
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>
#include "utils/DateUtils.h"
#include "utils/StringUtils.h"
#include <trantor/utils/Logger.h>
#include "RngLogs.h"
#include "Tickets.h"
#include "constants/RewardTypes.h"
#include "plugins/RaffleServicePlugin.h"
#include "utils/CronUtils.h"

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {


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
        trantor::Date::fromDbStringLocal(utils::DateUtils::normalizeDateString(startDate))));
  }

  if (!endDate.empty()) {
    addCriteria(Criteria(
        Raffles::Cols::_start_time, CompareOperator::LE,
        trantor::Date::fromDbStringLocal(utils::DateUtils::normalizeDateString(endDate))));
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
      item["options"] = j["options"];
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




  drogon::Task<dto::BaseApiResponse> RaffleService::getActiveRaffles() {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> mapper(dbClient);

  dto::BaseApiResponse response;
  response.success = true;

  try {
    // Query active raffles using the enum value directly
    auto raffles = co_await mapper.findBy(
        Criteria(Raffles::Cols::_status, CompareOperator::EQ, std::to_string(constants::RaffleStatus::ACTIVE)));

    Json::Value data = Json::arrayValue;

    for (const auto &raffle : raffles) {
      Json::Value raffleJson;
      raffleJson["id"] = raffle.getValueOfId();
      raffleJson["name"] = raffle.getValueOfName();
      raffleJson["slug"] = raffle.getValueOfSlug();
      raffleJson["status"] = raffle.getValueOfStatus();

      // Parse JSON options string back into a Json::Value array
      Json::Value optionsArray(Json::arrayValue);
      const std::string &optionsText = raffle.getValueOfOptions();

      if (!optionsText.empty()) {
        Json::Value parsedOptions;
        Json::CharReaderBuilder readerBuilder;
        std::string errs;
        std::istringstream inputStream(optionsText);

        if (Json::parseFromStream(readerBuilder, inputStream, &parsedOptions, &errs) && parsedOptions.isArray()) {
          optionsArray = parsedOptions;
        }
      }

      raffleJson["options"] = optionsArray;
      raffleJson["totalEntries"] = raffle.getValueOfTotalEntries();
      raffleJson["createdAt"] = raffle.getValueOfCreatedAt().toDbStringLocal();
      raffleJson["liability"] = raffle.getValueOfLiablity();
      raffleJson["expectedRevenue"] = raffle.getValueOfExpectedRevenue();
      raffleJson["startsAt"] = raffle.getValueOfStartTime().toDbStringLocal();
      raffleJson["endsAt"] = raffle.getValueOfEndTime().toDbStringLocal();
      raffleJson["drawTime"] = raffle.getValueOfDrawTime().toDbStringLocal();

      data.append(raffleJson);
    }

    response.result["data"] = data;
    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    LOG_ERROR << "Database error in getActiveRaffles: " << e.base().what();
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching active raffles.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    LOG_ERROR << "Error in getActiveRaffles: " << e.what();
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}


 drogon::Task<dto::BaseApiResponse> RaffleService::createAsync(const dto::RaffleDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> mapper(dbClient);

  try {
    if (dto.getName().empty() || dto.getStartTime().empty() ||
        dto.getEndTime().empty() || dto.getDrawTime().empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Name, Start Time, End Time, and Draw Time are required.";
      co_return errorResponse;
    }

    Raffles newRaffle;
    newRaffle.setName(dto.getName());
    newRaffle.setMaximumWinners(dto.getMaximumWinners());

    auto now = trantor::Date::now();
    time_t seconds = static_cast<time_t>(now.microSecondsSinceEpoch() / 1000000);
    struct tm tm_now;
    localtime_r(&seconds, &tm_now);

    char dateBuf[32];
    snprintf(dateBuf,
             sizeof(dateBuf),
             "%04d%02d%02d",
             tm_now.tm_year + 1900,
             tm_now.tm_mon + 1,
             tm_now.tm_mday);

    if (!dto.getSlug().empty()) {
      newRaffle.setSlug(dto.getSlug() + "-" + std::string(dateBuf));
    } else {
      newRaffle.setSlug(dto.getName() + "-" + std::string(dateBuf));
    }

    newRaffle.setStatus(constants::RaffleStatus::SCHEDULED);
    newRaffle.setLiablity(std::to_string(dto.getLiability()));
    newRaffle.setExpectedRevenue(std::to_string(dto.getExpectedRevenue()));
    newRaffle.setTotalEntries(0);

    // Parse dates
    auto startTime = trantor::Date::fromDbStringLocal(utils::DateUtils::normalizeDateString(dto.getStartTime()));
    auto endTime = trantor::Date::fromDbStringLocal(utils::DateUtils::normalizeDateString(dto.getEndTime()));
    auto drawTime = trantor::Date::fromDbStringLocal(utils::DateUtils::normalizeDateString(dto.getDrawTime()));

    newRaffle.setStartTime(startTime);
    newRaffle.setEndTime(endTime);
    newRaffle.setDrawTime(drawTime);

    // Convert Json::Value to string for options & exempted partners
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["indentation"] = "";

    if (!dto.getOptions().isNull() && dto.getOptions().isArray()) {
      newRaffle.setOptions(Json::writeString(writerBuilder, dto.getOptions()));
    } else {
      newRaffle.setOptions("[]");
    }

    if (!dto.getPrizeConfiguration().isNull() && dto.getPrizeConfiguration().isArray()) {
      newRaffle.setPrizeConfig(Json::writeString(writerBuilder, dto.getPrizeConfiguration()));
    } else {
      newRaffle.setPrizeConfig("[]");
    }

    Json::Value exemptedPartnersJson(Json::arrayValue);
    for (const auto& partner : dto.getExemptedPartners()) {
      exemptedPartnersJson.append(partner);
    }
    newRaffle.setExemptedPartners(Json::writeString(writerBuilder, exemptedPartnersJson));

    auto newRaffleResult = co_await mapper.insert(newRaffle);

    // Create corresponding Draw
    try {
      CoroMapper<Draws> drawMapper(dbClient);
      Draws newDraw;

      auto drawTimeDate = newRaffleResult.getValueOfDrawTime();
      time_t drawSecs = static_cast<time_t>(drawTimeDate.microSecondsSinceEpoch() / 1000000);
      struct tm tm_time;
      localtime_r(&drawSecs, &tm_time);

      char buf[32];
      snprintf(buf, sizeof(buf), "%04d%02d%02d-%02d%02d", tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min);

      newDraw.setDrawCode(std::string(buf));
      newDraw.setAlias(newRaffleResult.getValueOfSlug());
      newDraw.setRaffleId(newRaffleResult.getValueOfId());
      newDraw.setStatus(constants::DrawStatus::DRAW_SCHEDULED);
      newDraw.setDrawDate(newRaffleResult.getValueOfDrawTime());
      newDraw.setTotalEntries(0);

      co_await drawMapper.insert(newDraw);

      // Quartz Schedule Jobs
      auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
      auto &quartzApi = plugin->getQuartzApi();
      auto customConfig = drogon::app().getCustomConfig();

      // Activation Job
      std::string raffleActivationCallBackUrl = customConfig["QuartzSchedulerApi"]["RaffleActivationCallBackUrl"].asString();
      dto::QuartzJobDto raffleActivationQuartzJobDto;
      raffleActivationQuartzJobDto.name = "raffle-activation-" + newRaffleResult.getValueOfId();
      raffleActivationQuartzJobDto.description = "Activates raffle at scheduled start time";
      raffleActivationQuartzJobDto.customData.uniqueId = newRaffleResult.getValueOfId();
      raffleActivationQuartzJobDto.customData.callbackUrl = raffleActivationCallBackUrl + newRaffleResult.getValueOfId();

      const auto& activationStartTime = newRaffleResult.getValueOfStartTime();
      raffleActivationQuartzJobDto.schedule = utils::CronUtils::buildCronExpression(activationStartTime);
      raffleActivationQuartzJobDto.startDate = activationStartTime.after(-5 * 60).toDbStringLocal();
      raffleActivationQuartzJobDto.endDate = activationStartTime.after(5 * 60).toDbStringLocal();

      co_await quartzApi.scheduleJob(raffleActivationQuartzJobDto);

      // Closure Job
      std::string raffleClosureCallBackUrl = customConfig["QuartzSchedulerApi"]["RaffleClosureCallBackUrl"].asString();
      dto::QuartzJobDto raffleClosureQuartzJobDto;
      raffleClosureQuartzJobDto.name = "raffle-closure-" + newRaffleResult.getValueOfId();
      raffleClosureQuartzJobDto.description = "Closes raffle at scheduled end time";
      raffleClosureQuartzJobDto.customData.uniqueId = newRaffleResult.getValueOfId();
      raffleClosureQuartzJobDto.customData.callbackUrl = raffleClosureCallBackUrl + newRaffleResult.getValueOfId();

      const auto& closureStartTime = newRaffleResult.getValueOfEndTime();
      raffleClosureQuartzJobDto.schedule = utils::CronUtils::buildCronExpression(closureStartTime);
      raffleClosureQuartzJobDto.startDate = closureStartTime.after(-5 * 60).toDbStringLocal();
      raffleClosureQuartzJobDto.endDate = closureStartTime.after(5 * 60).toDbStringLocal();

      co_await quartzApi.scheduleJob(raffleClosureQuartzJobDto);

      // Draw Job
      std::string raffleDrawCallBackUrl = customConfig["QuartzSchedulerApi"]["RaffleDrawCallBackUrl"].asString();
      dto::QuartzJobDto raffleDrawQuartzJobDto;
      raffleDrawQuartzJobDto.name = "raffle-draw-" + newRaffleResult.getValueOfId();
      raffleDrawQuartzJobDto.description = "Initiates raffle draw at scheduled draw time";
      raffleDrawQuartzJobDto.customData.uniqueId = newRaffleResult.getValueOfId();
      raffleDrawQuartzJobDto.customData.callbackUrl = raffleDrawCallBackUrl + newRaffleResult.getValueOfId();

      const auto& drawStartTime = newRaffleResult.getValueOfDrawTime();
      raffleDrawQuartzJobDto.schedule = utils::CronUtils::buildCronExpression(drawStartTime);
      raffleDrawQuartzJobDto.startDate = drawStartTime.after(-5 * 60).toDbStringLocal();
      raffleDrawQuartzJobDto.endDate = drawStartTime.after(5 * 60).toDbStringLocal();

      co_await quartzApi.scheduleJob(raffleDrawQuartzJobDto);

    } catch (const std::exception &e) {
      LOG_ERROR << "Failed to create initial draw or schedule Quartz jobs: " << e.what();
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Raffle created successfully";
    response.result = newRaffleResult.getValueOfId();

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

drogon::Task<dto::BaseApiResponse> RaffleService::updateAsync(const dto::RaffleDto &dto, const std::string &id) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> mapper(dbClient);

  try {
    if (id.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Raffle ID is required for update.";
      co_return errorResponse;
    }

    auto existingRaffle = co_await mapper.findByPrimaryKey(id);

    bool drawTimeChanged = false;
    bool slugChanged = false;
    bool scheduleDatesChanged = false;

    if (!dto.getName().empty()) {
      existingRaffle.setName(dto.getName());
    }

    if (!dto.getSlug().empty() && existingRaffle.getValueOfSlug() != dto.getSlug()) {
      existingRaffle.setSlug(dto.getSlug());
      slugChanged = true;
    }

    existingRaffle.setLiablity(std::to_string(dto.getLiability()));
    existingRaffle.setExpectedRevenue(std::to_string(dto.getExpectedRevenue()));

    if (!dto.getStartTime().empty()) {
      auto newStartTime = trantor::Date::fromDbStringLocal(utils::DateUtils::normalizeDateString(dto.getStartTime()));
      if (existingRaffle.getValueOfStartTime() != newStartTime) {
        existingRaffle.setStartTime(newStartTime);
        scheduleDatesChanged = true;
      }
    }

    if (!dto.getEndTime().empty()) {
      auto newEndTime = trantor::Date::fromDbStringLocal(utils::DateUtils::normalizeDateString(dto.getEndTime()));
      if (existingRaffle.getValueOfEndTime() != newEndTime) {
        existingRaffle.setEndTime(newEndTime);
        scheduleDatesChanged = true;
      }
    }

    if (!dto.getDrawTime().empty()) {
      auto newDrawTime = trantor::Date::fromDbStringLocal(utils::DateUtils::normalizeDateString(dto.getDrawTime()));
      if (existingRaffle.getValueOfDrawTime() != newDrawTime) {
        existingRaffle.setDrawTime(newDrawTime);
        drawTimeChanged = true;
        scheduleDatesChanged = true;
      }
    }

    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["indentation"] = "";

    if (!dto.getOptions().isNull() && dto.getOptions().isArray()) {
      existingRaffle.setOptions(Json::writeString(writerBuilder, dto.getOptions()));
    }

    if (!dto.getPrizeConfiguration().isNull() && dto.getPrizeConfiguration().isArray()) {
      existingRaffle.setPrizeConfig(Json::writeString(writerBuilder, dto.getPrizeConfiguration()));
    }

    if (!dto.getExemptedPartners().empty()) {
      Json::Value exemptedPartnersJson(Json::arrayValue);
      for (const auto& partner : dto.getExemptedPartners()) {
        exemptedPartnersJson.append(partner);
      }
      existingRaffle.setExemptedPartners(Json::writeString(writerBuilder, exemptedPartnersJson));
    }

    auto result = co_await mapper.update(existingRaffle);

    // Update associated Draw
    if (drawTimeChanged || slugChanged) {
      try {
        CoroMapper<Draws> drawMapper(dbClient);
        auto draws = co_await drawMapper.findBy(Criteria(Draws::Cols::_raffle_id, CompareOperator::EQ, id));

        if (!draws.empty()) {
          auto draw = draws[0];

          if (drawTimeChanged) {
            auto drawTimeDate = existingRaffle.getValueOfDrawTime();
            time_t seconds = static_cast<time_t>(drawTimeDate.microSecondsSinceEpoch() / 1000000);
            struct tm tm_time;
            localtime_r(&seconds, &tm_time);

            char buf[32];
            snprintf(buf, sizeof(buf), "%04d%02d%02d-%02d%02d", tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min);

            draw.setDrawCode(std::string(buf));
            draw.setDrawDate(drawTimeDate);
          }

          if (slugChanged) {
            draw.setAlias(existingRaffle.getValueOfSlug());
          }

          co_await drawMapper.update(draw);
        }
      } catch (const std::exception &e) {
        LOG_ERROR << "Failed to update associated draw for raffle " << id << ": " << e.what();
      }
    }

    // Update Quartz Jobs if dates were updated
    if (scheduleDatesChanged) {
      try {
        auto plugin = drogon::app().getPlugin<raffle::plugins::RaffleServicePlugin>();
        auto &quartzApi = plugin->getQuartzApi();
        auto customConfig = drogon::app().getCustomConfig();

        // Activation Job Update
        std::string raffleActivationCallBackUrl = customConfig["QuartzSchedulerApi"]["RaffleActivationCallBackUrl"].asString();
        dto::QuartzJobDto raffleActivationQuartzJobDto;
        raffleActivationQuartzJobDto.name = "raffle-activation-" + id;
        raffleActivationQuartzJobDto.description = "Activates raffle at scheduled start time";
        raffleActivationQuartzJobDto.customData.uniqueId = id;
        raffleActivationQuartzJobDto.customData.callbackUrl = raffleActivationCallBackUrl + id;

        const auto& activationStartTime = existingRaffle.getValueOfStartTime();
        raffleActivationQuartzJobDto.schedule = utils::CronUtils::buildCronExpression(activationStartTime);
        raffleActivationQuartzJobDto.startDate = activationStartTime.after(-5 * 60).toDbStringLocal();
        raffleActivationQuartzJobDto.endDate = activationStartTime.after(5 * 60).toDbStringLocal();

        co_await quartzApi.scheduleJob(raffleActivationQuartzJobDto);

        // Closure Job Update
        std::string raffleClosureCallBackUrl = customConfig["QuartzSchedulerApi"]["RaffleClosureCallBackUrl"].asString();
        dto::QuartzJobDto raffleClosureQuartzJobDto;
        raffleClosureQuartzJobDto.name = "raffle-closure-" + id;
        raffleClosureQuartzJobDto.description = "Closes raffle at scheduled end time";
        raffleClosureQuartzJobDto.customData.uniqueId = id;
        raffleClosureQuartzJobDto.customData.callbackUrl = raffleClosureCallBackUrl + id;

        const auto& closureStartTime = existingRaffle.getValueOfEndTime();
        raffleClosureQuartzJobDto.schedule = utils::CronUtils::buildCronExpression(closureStartTime);
        raffleClosureQuartzJobDto.startDate = closureStartTime.after(-5 * 60).toDbStringLocal();
        raffleClosureQuartzJobDto.endDate = closureStartTime.after(5 * 60).toDbStringLocal();

        co_await quartzApi.scheduleJob(raffleClosureQuartzJobDto);

        // Draw Job Update
        std::string raffleDrawCallBackUrl = customConfig["QuartzSchedulerApi"]["RaffleDrawCallBackUrl"].asString();
        dto::QuartzJobDto raffleDrawQuartzJobDto;
        raffleDrawQuartzJobDto.name = "raffle-draw-" + id;
        raffleDrawQuartzJobDto.description = "Initiates raffle draw at scheduled draw time";
        raffleDrawQuartzJobDto.customData.uniqueId = id;
        raffleDrawQuartzJobDto.customData.callbackUrl = raffleDrawCallBackUrl + id;

        const auto& drawStartTime = existingRaffle.getValueOfDrawTime();
        raffleDrawQuartzJobDto.schedule = utils::CronUtils::buildCronExpression(drawStartTime);
        raffleDrawQuartzJobDto.startDate = drawStartTime.after(-5 * 60).toDbStringLocal();
        raffleDrawQuartzJobDto.endDate = drawStartTime.after(5 * 60).toDbStringLocal();

        co_await quartzApi.scheduleJob(raffleDrawQuartzJobDto);

      } catch (const std::exception &e) {
        LOG_ERROR << "Failed to reschedule Quartz jobs for raffle " << id << ": " << e.what();
      }
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Raffle updated successfully";
    response.result = existingRaffle.getValueOfId();

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
    if (existingRaffle.getValueOfStatus() == constants::RaffleStatus::ACTIVE) {
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
      LOG_ERROR << "Failed to delete associated draw for raffle " << raffleId  << ": " << e.what();
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




drogon::Task<dto::BaseApiResponse> RaffleService::executeDraw(std::string raffleId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> raffleMapper(dbClient);
  CoroMapper<Draws> drawMapper(dbClient);
  CoroMapper<Tickets> ticketMapper(dbClient);
  CoroMapper<Winners> winnerMapper(dbClient);

  try {
    LOG_DEBUG << "Starting draw execution for raffleId: " << raffleId;

    // 1. Get the active raffle
    auto activeRaffle = co_await raffleMapper.findByPrimaryKey(raffleId);

    // 2. Get the associated draw ID
    auto draw = co_await drawMapper.findOne(
        Criteria(Draws::Cols::_raffle_id, CompareOperator::EQ, activeRaffle.getValueOfId()));

    LOG_DEBUG << "Successfully fetched raffle and associated draw ID: " << draw.getValueOfId();

    // --- ALGORITHM STEP 1: Pull the options for that particular draw ---
    LOG_DEBUG << "Step 1: Pulling options for the draw.";
    Json::Value optionsArray(Json::arrayValue);
    const std::string &optionsText = activeRaffle.getValueOfOptions();
    if (!optionsText.empty()) {
      Json::CharReaderBuilder readerBuilder;
      std::string errs;
      std::istringstream inputStream(optionsText);
      Json::parseFromStream(readerBuilder, inputStream, &optionsArray, &errs);
    }
    LOG_DEBUG << "Options loaded. Total options count: " << optionsArray.size();

    // Initialize Random Number Generator
    std::random_device rd;
    std::mt19937 rng(rd());

    // --- ALGORITHM STEP 2: Pick an option at random as the winning option ---
    LOG_DEBUG << "Step 2: Picking a random winning option.";
    Json::Value winningOption;
    std::string winningOptionId = "";

    if (!optionsArray.empty() && optionsArray.isArray()) {
      std::uniform_int_distribution<int> optDist(0, optionsArray.size() - 1);
      winningOption = optionsArray[optDist(rng)];
      winningOptionId = winningOption["id"].asString();
      LOG_DEBUG << "Winning option selected: " << winningOption["name"].asString()
                << " (ID: " << winningOptionId << ")";
    } else {
      LOG_DEBUG << "No options available to pick from.";
    }

    // --- ALGORITHM STEP 3: Select all tickets linked to the raffleId ---
    LOG_DEBUG << "Step 3: Selecting all tickets for raffleId: " << raffleId;
    auto allTickets = co_await ticketMapper.findBy(
        Criteria(Tickets::Cols::_raffle_id, CompareOperator::EQ, raffleId) && Criteria(Tickets::Cols::_payment_status, CompareOperator::EQ, std::to_string(constants::PaymentStatus::SUCCESS))
    );
    int totalCount = allTickets.size();
    LOG_DEBUG << "Total tickets found: " << totalCount;

    Json::Value winnersResult = Json::arrayValue;

    if (totalCount > 0 && !winningOptionId.empty()) {

      // --- ALGORITHM STEP 4: Filter the ticket pool ---
      LOG_DEBUG << "Step 4: Filtering tickets to find those that selected the winning option.";
      std::vector<Tickets> filteredTickets;
      for (const auto& ticket : allTickets) {
          if (ticket.getValueOfSelectedOptionValue() == winningOptionId) {
              filteredTickets.push_back(ticket);
          }
      }
      int validCount = filteredTickets.size();
      LOG_DEBUG << "Filtering complete. " << validCount << " out of " << totalCount << " tickets guessed correctly.";

      if (validCount > 0) {
          // --- ALGORITHM STEP 5: Determine how many winners to draw ---
          int maximumWinners = activeRaffle.getValueOfMaximumWinners();
          int actualWinners = std::min(validCount, maximumWinners);
          LOG_DEBUG << "Step 5: Maximum winners set to " << maximumWinners
                    << ". Actual winners to draw from the filtered pool: " << actualWinners;

          // Pull prize configuration
          Json::Value prizeConfigArray(Json::arrayValue);
          const std::string &prizeText = activeRaffle.getValueOfPrizeConfig();
          if (!prizeText.empty()) {
            Json::CharReaderBuilder readerBuilder;
            std::string errs;
            std::istringstream inputStream(prizeText);
            Json::parseFromStream(readerBuilder, inputStream, &prizeConfigArray, &errs);
          }
          LOG_DEBUG << "Prize configuration loaded. Total prize tiers: " << prizeConfigArray.size();

          // --- ALGORITHM STEP 6: Shuffle the filtered ticket pool ---
          LOG_DEBUG << "Step 6: Shuffling the filtered ticket pool.";
          std::shuffle(filteredTickets.begin(), filteredTickets.end(), rng);

          // --- ALGORITHM STEP 7: Pick winners from the top of the shuffled valid pool ---
          LOG_DEBUG << "Step 7: Selecting winners and assigning prizes.";
          for (int i = 0; i < actualWinners; ++i) {
            auto winningTicket = filteredTickets[i];

            LOG_DEBUG << "--- Draw Pick " << (i + 1) << " ---";
            LOG_DEBUG << "Selected winning ticket ID: " << winningTicket.getValueOfId()
                      << " (Ticket Number: " << winningTicket.getValueOfTicketNumber() << ")";

            // Map the corresponding prize for the current winner's rank
            double prizeAmount = 0.0;
            if (prizeConfigArray.isArray() && i < (int)prizeConfigArray.size()) {
              prizeAmount = prizeConfigArray[i]["value"].asDouble();
            }

            // Write winner to the database
            Winners winner;
            winner.setDrawId(draw.getValueOfId());
            winner.setTicketId(winningTicket.getValueOfId());
            winner.setCustomerId(winningTicket.getValueOfCustomerId());

            winner.setSelectedOptionId(winningOptionId);
            winner.setSelectedOptionLabel(winningOption["name"].asString());

            winner.setPrizeAmount(std::to_string(prizeAmount));
            winner.setStatus("PENDING");
            winner.setTicketAlias(winningTicket.getValueOfTicketNumber());
            winner.setPhoneNumber(winningTicket.getValueOfCustomerPhone());

            auto savedWinner = co_await winnerMapper.insert(winner);
            winnersResult.append(utils::StringUtils::snakeCaseToCamelCase(savedWinner.toJson()));
            LOG_DEBUG << "Winner successfully saved to database. DB Record ID: " << savedWinner.getValueOfId();
          }
      } else {
          LOG_DEBUG << "No tickets guessed the winning option. No prizes awarded.";
      }
    } else {
      LOG_DEBUG << "No tickets found or no winning option determined. Skipping winner selection.";
    }

    // Mark the raffle lifecycle as complete
    LOG_DEBUG << "Updating raffle status to DRAW_COMPLETED.";
    activeRaffle.setStatus(constants::RaffleStatus::DRAW_COMPLETED);
    co_await raffleMapper.update(activeRaffle);

    LOG_DEBUG << "Draw execution fully completed for raffleId: " << raffleId;

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Draw executed successfully";
    response.result = winnersResult;

    co_return response;

  } catch (const DrogonDbException &e) {
    LOG_ERROR << "Database error while executing draw for raffleId: " << raffleId << ". Details: " << e.base().what();
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while executing draw.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    LOG_ERROR << "Internal error while executing draw for raffleId: " << raffleId << ". Details: " << e.what();
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

    // Removed the activeRaffles validation check that restricted multiple active raffles.
    auto raffle = co_await mapper.findByPrimaryKey(raffleId);

    // Update using the enum
    raffle.setStatus(constants::RaffleStatus::ACTIVE);
    co_await mapper.update(raffle);

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Raffle activated successfully";

    //notify admin via email

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
    raffle.setStatus(constants::RaffleStatus::CLOSED);
    co_await mapper.update(raffle);

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Raffle closed successfully";

    // notify admin via email

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

  drogon::Task<dto::BaseApiResponse> RaffleService::completeRaffle(const std::string &raffleId) {
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
    raffle.setStatus(constants::RaffleStatus::DRAW_COMPLETED);
    co_await mapper.update(raffle);

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Raffle closed successfully";

    // notify admin via email

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
      data.append(utils::StringUtils::snakeCaseToCamelCase(ticket.toJson()));
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



drogon::Task<dto::BaseApiResponse> RaffleService::getWeeklyRaffles() {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> mapper(dbClient);

  try {
    // 1. Calculate Sunday and Saturday bounds using the current local time
    auto now = trantor::Date::now();
    time_t seconds = static_cast<time_t>(now.microSecondsSinceEpoch() / 1000000);
    struct tm tm_now;
    localtime_r(&seconds, &tm_now);

    struct tm tm_sun = tm_now;
    tm_sun.tm_mday -= tm_now.tm_wday; // Subtract days to get to Sunday (wday: 0=Sun, ..., 6=Sat)
    tm_sun.tm_hour = 0;
    tm_sun.tm_min = 0;
    tm_sun.tm_sec = 0;
    tm_sun.tm_isdst = -1;
    time_t sun_secs = std::mktime(&tm_sun);
    trantor::Date sunday_date(sun_secs * 1000000);

    struct tm tm_sat = tm_now;
    tm_sat.tm_mday += (6 - tm_now.tm_wday); // Add days to get to Saturday
    tm_sat.tm_hour = 23;
    tm_sat.tm_min = 59;
    tm_sat.tm_sec = 59;
    tm_sat.tm_isdst = -1;
    time_t sat_secs = std::mktime(&tm_sat);
    trantor::Date saturday_date(sat_secs * 1000000 + 999999);

    // 2. Query Raffles starting between Sunday 00:00:00 and Saturday 23:59:59
    Criteria rangeCriteria =
        Criteria(Raffles::Cols::_start_time, CompareOperator::GE, sunday_date) &&
        Criteria(Raffles::Cols::_start_time, CompareOperator::LE, saturday_date);

    auto raffles = co_await mapper.orderBy(Raffles::Cols::_start_time, SortOrder::ASC).findBy(rangeCriteria);

    dto::BaseApiResponse response;
    response.success = true;

    Json::Value data = Json::arrayValue;
    const char* DAYS_OF_WEEK[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
    };

    for (const auto &raffle : raffles) {
      std::string formattedName = raffle.getValueOfName();
      int status = raffle.getValueOfStatus();

      if (status == constants::RaffleStatus::DRAW_COMPLETED || status == constants::RaffleStatus::CLOSED) {
        formattedName += " (Done)";
      } else if (status == constants::RaffleStatus::ACTIVE) {
        formattedName += " (Ongoing)";
      } else {
        // Retrieve weekday of start_time
        auto startTime = raffle.getValueOfStartTime();
        time_t start_secs = static_cast<time_t>(startTime.microSecondsSinceEpoch() / 1000000);
        struct tm tm_start;
        localtime_r(&start_secs, &tm_start);
        std::string dayName = DAYS_OF_WEEK[tm_start.tm_wday];
        formattedName += " (" + dayName + ")";
      }

      Json::Value raffleObj;
      raffleObj["name"] = formattedName;
      raffleObj["raffleId"] = raffle.getValueOfId();
      data.append(raffleObj);
    }

    response.result = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching weekly raffles.";
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


drogon::Task<dto::BaseApiResponse> RaffleService::getWeeklyRaffleSchedule() {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> mapper(dbClient);

  try {
    // 1. Calculate Sunday and Saturday bounds using the current local time
    auto now = trantor::Date::now();
    time_t seconds = static_cast<time_t>(now.microSecondsSinceEpoch() / 1000000);
    struct tm tm_now;
    localtime_r(&seconds, &tm_now);

    struct tm tm_sun = tm_now;
    tm_sun.tm_mday -= tm_now.tm_wday; // Subtract days to get to Sunday (wday: 0=Sun, ..., 6=Sat)
    tm_sun.tm_hour = 0;
    tm_sun.tm_min = 0;
    tm_sun.tm_sec = 0;
    tm_sun.tm_isdst = -1;
    time_t sun_secs = std::mktime(&tm_sun);
    trantor::Date sunday_date(sun_secs * 1000000);

    struct tm tm_sat = tm_now;
    tm_sat.tm_mday += (6 - tm_now.tm_wday); // Add days to get to Saturday
    tm_sat.tm_hour = 23;
    tm_sat.tm_min = 59;
    tm_sat.tm_sec = 59;
    tm_sat.tm_isdst = -1;
    time_t sat_secs = std::mktime(&tm_sat);
    trantor::Date saturday_date(sat_secs * 1000000 + 999999);

    // 2. Query Raffles starting between Sunday 00:00:00 and Saturday 23:59:59
    Criteria rangeCriteria =
        Criteria(Raffles::Cols::_start_time, CompareOperator::GE, sunday_date) &&
        Criteria(Raffles::Cols::_start_time, CompareOperator::LE, saturday_date);

    auto raffles = co_await mapper.orderBy(Raffles::Cols::_start_time, SortOrder::ASC).findBy(rangeCriteria);

    dto::BaseApiResponse response;
    response.success = true;

    Json::Value data = Json::arrayValue;
    const char* DAYS_OF_WEEK[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
    };

    for (const auto &raffle : raffles) {
      std::string formattedName = raffle.getValueOfName();
      int status = raffle.getValueOfStatus();
      std::string dayOfWeek;

      // Resolve weekday from start_time (always, so it is always available)
      auto startTime = raffle.getValueOfStartTime();
      time_t start_secs = static_cast<time_t>(startTime.microSecondsSinceEpoch() / 1000000);
      struct tm tm_start;
      localtime_r(&start_secs, &tm_start);
      dayOfWeek = DAYS_OF_WEEK[tm_start.tm_wday];

      if (status == constants::RaffleStatus::DRAW_COMPLETED || status == constants::RaffleStatus::CLOSED) {
        formattedName += " (Done)";
      } else if (status == constants::RaffleStatus::ACTIVE) {
        formattedName += " (Ongoing)";
      } else {
        formattedName += " (" + dayOfWeek + ")";
      }

      Json::Value raffleObj;
      raffleObj["raffleId"]      = raffle.getValueOfId();
      raffleObj["name"]          = formattedName;
      raffleObj["slug"]          = raffle.getValueOfSlug();
      raffleObj["status"]        = status;
      raffleObj["dayOfWeek"]     = dayOfWeek;
      raffleObj["totalEntries"]  = raffle.getValueOfTotalEntries();
      raffleObj["startsAt"]      = raffle.getValueOfStartTime().toDbStringLocal();
      raffleObj["endsAt"]        = raffle.getValueOfEndTime().toDbStringLocal();
      raffleObj["drawTime"]      = raffle.getValueOfDrawTime().toDbStringLocal();
      data.append(raffleObj);
    }

    response.result = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching weekly raffles.";
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



drogon::Task<dto::BaseApiResponse> RaffleService::getRaffleSummary(const std::string &raffleId) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> raffleMapper(dbClient);

  try {
    auto raffle = co_await raffleMapper.findByPrimaryKey(raffleId);
    int status = raffle.getValueOfStatus();
    
    dto::BaseApiResponse response;
    response.success = true;
    std::string message;

    if (status == constants::RaffleStatus::DRAW_COMPLETED || status == constants::RaffleStatus::CLOSED) {
      CoroMapper<Draws> drawMapper(dbClient);
      auto draws = co_await drawMapper.findBy(Criteria(Draws::Cols::_raffle_id, CompareOperator::EQ, raffleId));
      
      if (!draws.empty()) {
        std::string drawId = draws[0].getValueOfId();
        
        CoroMapper<Winners> winnerMapper(dbClient);
        auto winners = co_await winnerMapper.findBy(Criteria(Winners::Cols::_draw_id, CompareOperator::EQ, drawId));
        
        if (!winners.empty()) {
          std::string ticketId = winners[0].getValueOfTicketId();
          
          CoroMapper<Tickets> ticketMapper(dbClient);
          auto ticket = co_await ticketMapper.findByPrimaryKey(ticketId);
          
          std::string phone = ticket.getValueOfCustomerPhone();
          std::string maskedPhone = phone;
          if (maskedPhone.length() > 6) {
            maskedPhone = maskedPhone.substr(0, 3) + std::string(maskedPhone.length() - 6, '*') + maskedPhone.substr(maskedPhone.length() - 3);
          }
          
          message = "Raffle is closed. Winner: Ticket " + ticket.getValueOfTicketNumber() + " (" + maskedPhone + ")";
        } else {
          message = "Raffle is closed. No winner found.";
        }
      } else {
        message = "Raffle is closed. Draw pending.";
      }
    } else if (status == constants::RaffleStatus::ACTIVE) {
      message = "1. Participate in today's raffle";
    } else if (status == constants::RaffleStatus::SCHEDULED) {
      message = "This raffle hasn't started yet. Check back on the raffle date.";
    }

    response.result["text"] = message;
    co_return response;
    
  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching raffle summary.";
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
