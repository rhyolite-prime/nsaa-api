//
// Created by Emmanuel Addo-Odame on 05/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_RAFFLESERVICE_H
#define CHOPMONEYRAFFLEAPI_RAFFLESERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

#include "dto/CreateRaffleDto.h"
#include "dto/UpdateRaffleDto.h"

namespace raffle::services {

class RaffleService {
public:
  drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize,
                                            const std::string &query,
                                            const std::string &startDate = "",
                                            const std::string &endDate = "",
                                            const std::string &status = "");

  drogon::Task<dto::BaseApiResponse> createAsync(const dto::CreateRaffleDto &dto);

  drogon::Task<dto::BaseApiResponse> updateAsync(const dto::UpdateRaffleDto &dto);

  drogon::Task<dto::BaseApiResponse> deleteRaffle(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> getActiveRaffle();

  drogon::Task<dto::BaseApiResponse> incrementEntries(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> incrementPricePool(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> updateWinningColour(const std::string &raffleId);

  drogon::Task<dto::BaseApiResponse> activate(const std::string &raffleId);
};

} // namespace raffle::services
#endif // CHOPMONEYRAFFLEAPI_RAFFLESERVICE_H