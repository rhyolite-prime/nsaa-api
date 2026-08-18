//
// Created by Emmanuel Addo-Odame on 05/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_RAFFLESERVICE_H
#define CHOPMONEYRAFFLEAPI_RAFFLESERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/RaffleDto.h"

namespace raffle::services {

class RaffleService {
public:
  drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize,
                                            const std::string &query,
                                            const std::string &startDate = "",
                                            const std::string &endDate = "",
                                            const std::string &status = "");

  drogon::Task<dto::BaseApiResponse> createAsync(const dto::RaffleDto &dto);

  drogon::Task<dto::BaseApiResponse> updateAsync(const dto::RaffleDto &dto, const std::string &id);

  drogon::Task<dto::BaseApiResponse> deleteRaffle(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> getActiveRaffles();

  drogon::Task<dto::BaseApiResponse> incrementEntries(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> activate(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> getRecentTicketHolders(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> getTopTicketHolders(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> getTickets(const std::string &raffleId, int pageNo, int pageSize);

  drogon::Task<dto::BaseApiResponse> getTicketStats(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> getRngLogs(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> getAllRaffleTickets(const std::string &raffleId, int pageNo, int pageSize);

  drogon::Task<dto::BaseApiResponse> executeDraw();

  drogon::Task<dto::BaseApiResponse> runDraw(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> closeRaffle(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> completeRaffle(const std::string &raffleId);

   //USSD
  drogon::Task<dto::BaseApiResponse> getWeeklyRaffles();

  drogon::Task<dto::BaseApiResponse> getRaffleSummary(const std::string &raffleId);

    //public web
    drogon::Task<dto::BaseApiResponse> getWeeklyRaffleSchedule();
};

} // namespace raffle::services
#endif // CHOPMONEYRAFFLEAPI_RAFFLESERVICE_H