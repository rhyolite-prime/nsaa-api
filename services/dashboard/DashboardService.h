//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_DASHBOARDSERVICE_H
#define CHOPMONEYRAFFLEAPI_DASHBOARDSERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include <string>

namespace raffle::services {

    class DashboardService {
    public:

        // KPI cards for the index/dashboard page (revenue, entries, winners,
        // pending payouts) including period-over-period change percentages.
        // startDate / endDate are optional ISO dates (YYYY-MM-DD).
        drogon::Task<dto::BaseApiResponse> getOverview(const std::string &startDate, const std::string &endDate);

        // Entry breakdown by selected colour/option.
        drogon::Task<dto::BaseApiResponse> getColourBreakdown(const std::string &startDate, const std::string &endDate, const std::string &raffleId);

        // Per-draw statistics ledger (paginated).
        drogon::Task<dto::BaseApiResponse> getDrawStats(const std::string &startDate, const std::string &endDate, const std::string &raffleId, int pageNo, int pageSize);

        // Top participating customers ledger.
        drogon::Task<dto::BaseApiResponse> getTopParticipants(const std::string &startDate, const std::string &endDate, int limit);

        // Revenue time series for charts. interval: day | week | month.
        drogon::Task<dto::BaseApiResponse> getRevenueTrend(const std::string &startDate, const std::string &endDate, const std::string &interval, const std::string &raffleId);

    };

}

#endif //CHOPMONEYRAFFLEAPI_DASHBOARDSERVICE_H
