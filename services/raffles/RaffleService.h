//
// Created by Emmanuel Addo-Odame on 05/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_RAFFLESERVICE_H
#define CHOPMONEYRAFFLEAPI_RAFFLESERVICE_H
#include "dto/BaseApiResponse.h"
#include "dto/RaffleDto.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

namespace raffle::services {

class RaffleService {
public:
  drogon::Task<dto::BaseApiResponse>
  getAll(int pageNo, int pageSize, const std::string &query, int status,
         const std::string &startDate = "", const std::string &endDate = "");

  drogon::Task<dto::BaseApiResponse> createAsync(const dto::RaffleDto &dto);

  drogon::Task<dto::BaseApiResponse> updateAsync(const dto::RaffleDto &dto,
                                                 const std::string &id);

  drogon::Task<dto::BaseApiResponse> deleteRaffle(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> getActiveRaffles();

  drogon::Task<dto::BaseApiResponse>
  getActiveRaffleOptions(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse>
  incrementEntries(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> activate(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse>
  getRecentTicketHolders(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse>
  getTopTicketHolders(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse>
  getDrawDetails(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> getDetails(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> getTickets(const std::string &raffleId,
                                                int pageNo, int pageSize);

  drogon::Task<dto::BaseApiResponse>
  getTicketStats(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> getDrawLogs(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse>
  getAllRaffleTickets(const std::string &raffleId, int pageNo, int pageSize);

  drogon::Task<dto::BaseApiResponse> getDrawHistory(int pageNo, int pageSize);

  drogon::Task<dto::BaseApiResponse> executeDraw(std::string raffleId);

  drogon::Task<dto::BaseApiResponse> closeRaffle(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse>
  completeRaffle(const std::string &raffleId);

  // USSD
  drogon::Task<dto::BaseApiResponse> getWeeklyRaffles();

  drogon::Task<dto::BaseApiResponse>
  getRaffleSummary(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse>
  getRaffleStats(const std::string &raffleId);

  // public web
  drogon::Task<dto::BaseApiResponse> getWeeklyRaffleSchedule();
};

} // namespace raffle::services
#endif // CHOPMONEYRAFFLEAPI_RAFFLESERVICE_H