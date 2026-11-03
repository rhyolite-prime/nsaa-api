//
// Created by Emmanuel Addo-Odame on 02/11/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_REPORTSERVICE_H
#define CHOPMONEYRAFFLEAPI_REPORTSERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include <string>

namespace raffle::services {

    class ReportService {
    public:

        // Aggregate summary of all core entities within an optional date range.
        drogon::Task<dto::BaseApiResponse> getSummary(const std::string &startDate, const std::string &endDate);

        // Paginated ticket sales report.
        drogon::Task<dto::BaseApiResponse> getTicketSalesReport(const std::string &startDate, const std::string &endDate, const std::string &raffleId, const std::string &paymentStatus, const std::string &query, int pageNo, int pageSize);

        // Ticket distribution grouped by selected option (name/value) with
        // ticket counts, entry quantities, revenue and percentage share.
        drogon::Task<dto::BaseApiResponse> getTicketDistribution(const std::string &startDate, const std::string &endDate, const std::string &raffleId);

        // Per-draw statistics ledger (mirrors the draw_statistics view shape).
        drogon::Task<dto::BaseApiResponse> getDrawStatistics(const std::string &startDate, const std::string &endDate, const std::string &raffleId, int pageNo, int pageSize);

        // Customer participation ledger (mirrors the customer_participation view shape).
        drogon::Task<dto::BaseApiResponse> getCustomerParticipation(const std::string &startDate, const std::string &endDate, const std::string &query, int pageNo, int pageSize);

        // Revenue grouped by day | week | month.
        drogon::Task<dto::BaseApiResponse> getRevenueReport(const std::string &startDate, const std::string &endDate, const std::string &groupBy, const std::string &raffleId);

        // Paginated winners report.
        drogon::Task<dto::BaseApiResponse> getWinnersReport(const std::string &startDate, const std::string &endDate, const std::string &drawId, const std::string &status, const std::string &query, int pageNo, int pageSize);

        // Paginated customer activity report.
        drogon::Task<dto::BaseApiResponse> getCustomerReport(const std::string &startDate, const std::string &endDate, const std::string &query, const std::string &isActive, int pageNo, int pageSize);

    };

}

#endif //CHOPMONEYRAFFLEAPI_REPORTSERVICE_H
