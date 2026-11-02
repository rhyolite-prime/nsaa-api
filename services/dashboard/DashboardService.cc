//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#include "DashboardService.h"

#include "constants/ErrorCodes.h"
#include "constants/RewardTypes.h"
#include "dto/BaseApiResponse.h"

#include <drogon/orm/DbClient.h>
#include <drogon/utils/coroutine.h>

#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

using namespace drogon::orm;

namespace raffle::services {

namespace {

// Parse a YYYY-MM-DD string into a std::tm (date part only).
bool parseDate(const std::string &dateStr, std::tm &out) {
  if (dateStr.size() < 10)
    return false;
  std::istringstream ss(dateStr.substr(0, 10));
  ss >> std::get_time(&out, "%Y-%m-%d");
  return !ss.fail();
}

std::string formatDate(const std::tm &t) {
  std::ostringstream ss;
  ss << std::put_time(&t, "%Y-%m-%d");
  return ss.str();
}

// Shift a YYYY-MM-DD date by the given number of days.
std::string shiftDate(const std::string &dateStr, int days) {
  std::tm t{};
  if (!parseDate(dateStr, t))
    return "";
  std::time_t time = std::mktime(&t);
  time += static_cast<std::time_t>(days) * 86400;
  std::tm shifted{};
  localtime_r(&time, &shifted);
  return formatDate(shifted);
}

// Compute the previous period of equal length immediately before
// [startDate, endDate]. Returns false when dates are missing/invalid.
bool previousPeriod(const std::string &startDate, const std::string &endDate,
                    std::string &prevStart, std::string &prevEnd) {
  if (startDate.empty() || endDate.empty())
    return false;
  std::tm s{}, e{};
  if (!parseDate(startDate, s) || !parseDate(endDate, e))
    return false;
  std::time_t st = std::mktime(&s);
  std::time_t et = std::mktime(&e);
  if (st == -1 || et == -1 || et < st)
    return false;
  long days = static_cast<long>(std::difftime(et, st) / 86400) + 1;
  prevEnd = shiftDate(startDate, -1);
  prevStart = shiftDate(startDate, -static_cast<int>(days));
  return !prevStart.empty() && !prevEnd.empty();
}

// Percentage change from previous to current value plus a trend label.
void setChange(Json::Value &target, const std::string &changeKey,
               const std::string &trendKey, double current, double previous) {
  double change = 0.0;
  std::string trend = "flat";
  if (previous > 0.0) {
    change = ((current - previous) / previous) * 100.0;
  } else if (current > 0.0) {
    change = 100.0;
  }
  if (change > 0.0)
    trend = "up";
  else if (change < 0.0)
    trend = "down";
  target[changeKey] = std::round(change * 100.0) / 100.0;
  target[trendKey] = trend;
}

std::string drawStatusLabel(int32_t status) {
  switch (status) {
  case constants::DrawStatus::IN_PROGRESS:
    return "In Progress";
  case constants::DrawStatus::DRAW_SCHEDULED:
    return "Scheduled";
  case constants::DrawStatus::COMPLETED:
    return "Completed";
  case constants::DrawStatus::CANCELLED:
    return "Cancelled";
  default:
    return "Unknown";
  }
}

drogon::Task<dto::BaseApiResponse> errorResponse(const std::string &message,
                                                 const std::string &detail) {
  dto::BaseApiResponse response;
  response.success = false;
  response.error["code"] = constants::ERR_DB_QUERY;
  response.error["message"] = message;
  response.error["detail"] = detail;
  co_return response;
}

} // namespace

drogon::Task<dto::BaseApiResponse>
DashboardService::getOverview(const std::string &startDate,
                              const std::string &endDate) {
  auto dbClient = drogon::app().getDbClient();

  try {
    // Current-period ticket metrics (paid tickets only).
    auto ticketRows = co_await dbClient->execSqlCoro(
        "SELECT COUNT(*) AS entries, COALESCE(SUM(quantity), 0) AS quantity, "
        "COALESCE(SUM(total_amount::numeric), 0) AS revenue "
        "FROM tickets "
        "WHERE payment_status = 1 "
        "AND ($1 = '' OR created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR created_at < (NULLIF($2,'')::date + interval '1 day'))",
        startDate, endDate);

    // Current-period winner metrics.
    auto winnerRows = co_await dbClient->execSqlCoro(
        "SELECT COUNT(*) AS winners, "
        "COUNT(*) FILTER (WHERE status = 0) AS pending_payouts "
        "FROM winners "
        "WHERE ($1 = '' OR created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR created_at < (NULLIF($2,'')::date + interval '1 day'))",
        startDate, endDate);

    // Customer, raffle and draw totals.
    auto customerRows = co_await dbClient->execSqlCoro(
        "SELECT COUNT(*) AS total_customers, "
        "COUNT(*) FILTER (WHERE is_active = true) AS active_customers "
        "FROM customers "
        "WHERE ($1 = '' OR created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR created_at < (NULLIF($2,'')::date + interval '1 day'))",
        startDate, endDate);

    auto raffleRows = co_await dbClient->execSqlCoro(
        "SELECT COUNT(*) AS total_raffles, "
        "COUNT(*) FILTER (WHERE status = 0) AS active_raffles "
        "FROM raffles");

    auto drawRows = co_await dbClient->execSqlCoro(
        "SELECT COUNT(*) AS total_draws, "
        "COUNT(*) FILTER (WHERE status = 2) AS completed_draws "
        "FROM draws "
        "WHERE ($1 = '' OR draw_date >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR draw_date < (NULLIF($2,'')::date + interval '1 day'))",
        startDate, endDate);

    // Previous-period metrics for trend comparison (only when a full date
    // range is supplied).
    std::string prevStart, prevEnd;
    double prevRevenue = 0.0, prevEntries = 0.0, prevWinners = 0.0,
           prevPending = 0.0;
    bool hasPrev = previousPeriod(startDate, endDate, prevStart, prevEnd);
    if (hasPrev) {
      auto prevTickets = co_await dbClient->execSqlCoro(
          "SELECT COUNT(*) AS entries, "
          "COALESCE(SUM(total_amount::numeric), 0) AS revenue "
          "FROM tickets "
          "WHERE payment_status = 1 "
          "AND created_at >= NULLIF($1,'')::timestamp "
          "AND created_at < (NULLIF($2,'')::date + interval '1 day')",
          prevStart, prevEnd);
      auto prevWinnersRows = co_await dbClient->execSqlCoro(
          "SELECT COUNT(*) AS winners, "
          "COUNT(*) FILTER (WHERE status = 0) AS pending_payouts "
          "FROM winners "
          "WHERE created_at >= NULLIF($1,'')::timestamp "
          "AND created_at < (NULLIF($2,'')::date + interval '1 day')",
          prevStart, prevEnd);

      if (!prevTickets.empty()) {
        prevEntries = prevTickets[0]["entries"].as<double>();
        prevRevenue = prevTickets[0]["revenue"].isNull()
                          ? 0.0
                          : prevTickets[0]["revenue"].as<double>();
      }
      if (!prevWinnersRows.empty()) {
        prevWinners = prevWinnersRows[0]["winners"].as<double>();
        prevPending = prevWinnersRows[0]["pending_payouts"].isNull()
                          ? 0.0
                          : prevWinnersRows[0]["pending_payouts"].as<double>();
      }
    }

    double revenue = 0.0, entries = 0.0;
    if (!ticketRows.empty()) {
      entries = ticketRows[0]["entries"].as<double>();
      revenue = ticketRows[0]["revenue"].isNull()
                    ? 0.0
                    : ticketRows[0]["revenue"].as<double>();
    }
    double winners = 0.0, pendingPayouts = 0.0;
    if (!winnerRows.empty()) {
      winners = winnerRows[0]["winners"].as<double>();
      pendingPayouts = winnerRows[0]["pending_payouts"].isNull()
                           ? 0.0
                           : winnerRows[0]["pending_payouts"].as<double>();
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.result["totalRevenue"] = revenue;
    response.result["activeEntries"] = (Json::UInt64)entries;
    response.result["totalWinners"] = (Json::UInt64)winners;
    response.result["pendingPayouts"] = (Json::UInt64)pendingPayouts;

    setChange(response.result, "revenueChange", "revenueTrend", revenue,
              prevRevenue);
    setChange(response.result, "entriesChange", "entriesTrend", entries,
              prevEntries);
    setChange(response.result, "winnersChange", "winnersTrend", winners,
              prevWinners);
    setChange(response.result, "pendingPayoutsChange", "pendingPayoutsTrend",
              pendingPayouts, prevPending);

    if (!customerRows.empty()) {
      response.result["totalCustomers"] =
          (Json::UInt64)customerRows[0]["total_customers"].as<uint64_t>();
      response.result["activeCustomers"] =
          (Json::UInt64)customerRows[0]["active_customers"].as<uint64_t>();
    } else {
      response.result["totalCustomers"] = 0;
      response.result["activeCustomers"] = 0;
    }

    if (!raffleRows.empty()) {
      response.result["totalRaffles"] =
          (Json::UInt64)raffleRows[0]["total_raffles"].as<uint64_t>();
      response.result["activeRaffles"] =
          (Json::UInt64)raffleRows[0]["active_raffles"].as<uint64_t>();
    } else {
      response.result["totalRaffles"] = 0;
      response.result["activeRaffles"] = 0;
    }

    if (!drawRows.empty()) {
      response.result["totalDraws"] =
          (Json::UInt64)drawRows[0]["total_draws"].as<uint64_t>();
      response.result["completedDraws"] =
          (Json::UInt64)drawRows[0]["completed_draws"].as<uint64_t>();
    } else {
      response.result["totalDraws"] = 0;
      response.result["completedDraws"] = 0;
    }

    response.result["startDate"] = startDate;
    response.result["endDate"] = endDate;

    co_return response;
  } catch (const DrogonDbException &e) {
    co_return co_await errorResponse(
        "Database error while fetching dashboard overview.", e.base().what());
  } catch (const std::exception &e) {
    dto::BaseApiResponse err;
    err.success = false;
    err.error["code"] = constants::ERR_INTERNAL;
    err.error["message"] = "Internal error.";
    err.error["detail"] = e.what();
    co_return err;
  }
}

drogon::Task<dto::BaseApiResponse> DashboardService::getColourBreakdown(
    const std::string &startDate, const std::string &endDate,
    const std::string &raffleId) {
  auto dbClient = drogon::app().getDbClient();

  try {
    auto rows = co_await dbClient->execSqlCoro(
        "SELECT t.selected_option_name AS name, COUNT(*) AS count, "
        "COALESCE(SUM(t.total_amount::numeric), 0) AS revenue, "
        "c.hex_color AS hex_color, c.emoji AS emoji "
        "FROM tickets t "
        "LEFT JOIN colours c ON c.name = t.selected_option_name "
        "WHERE t.payment_status = 1 "
        "AND ($1 = '' OR t.created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR t.created_at < (NULLIF($2,'')::date + interval '1 day')) "
        "AND ($3 = '' OR t.raffle_id = $3) "
        "GROUP BY t.selected_option_name, c.hex_color, c.emoji "
        "ORDER BY count DESC",
        startDate, endDate, raffleId);

    Json::Value data = Json::arrayValue;
    uint64_t totalEntries = 0;
    for (const auto &row : rows) {
      Json::Value item;
      item["name"] = row["name"].isNull() ? "" : row["name"].as<std::string>();
      uint64_t count = row["count"].as<uint64_t>();
      item["count"] = (Json::UInt64)count;
      item["revenue"] = row["revenue"].isNull() ? 0.0 : row["revenue"].as<double>();
      item["hexColor"] =
          row["hex_color"].isNull() ? "" : row["hex_color"].as<std::string>();
      item["emoji"] =
          row["emoji"].isNull() ? "" : row["emoji"].as<std::string>();
      totalEntries += count;
      data.append(item);
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.result["data"] = data;
    response.result["totalEntries"] = (Json::UInt64)totalEntries;
    co_return response;
  } catch (const DrogonDbException &e) {
    co_return co_await errorResponse(
        "Database error while fetching colour breakdown.", e.base().what());
  } catch (const std::exception &e) {
    dto::BaseApiResponse err;
    err.success = false;
    err.error["code"] = constants::ERR_INTERNAL;
    err.error["message"] = "Internal error.";
    err.error["detail"] = e.what();
    co_return err;
  }
}

drogon::Task<dto::BaseApiResponse> DashboardService::getDrawStats(
    const std::string &startDate, const std::string &endDate,
    const std::string &raffleId, int pageNo, int pageSize) {
  auto dbClient = drogon::app().getDbClient();

  try {
    auto countRows = co_await dbClient->execSqlCoro(
        "SELECT COUNT(*) AS total FROM draws d "
        "WHERE ($1 = '' OR d.draw_date >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR d.draw_date < (NULLIF($2,'')::date + interval '1 day')) "
        "AND ($3 = '' OR d.raffle_id = $3)",
        startDate, endDate, raffleId);

    uint64_t totalCount =
        countRows.empty() ? 0 : countRows[0]["total"].as<uint64_t>();

    dto::BaseApiResponse response;
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    int totalPages = pageSize > 0 ? (int)((totalCount + pageSize - 1) / pageSize) : 0;
    response.result["totalPages"] = totalPages;
    response.result["lowerBound"] = totalCount == 0 ? 0 : pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] =
        (Json::UInt64)(totalPages == pageNo ? totalCount : (uint64_t)pageNo * pageSize);

    Json::Value data = Json::arrayValue;
    response.result["data"] = data;
    if (totalCount == 0)
      co_return response;

    int offset = (pageNo - 1) * pageSize;
    auto rows = co_await dbClient->execSqlCoro(
        "SELECT d.id, d.draw_code, d.draw_date, d.status, d.total_entries, "
        "d.alias, d.winning_option_name AS winning_colour, "
        "COALESCE(tk.total_pool, 0) AS total_pool, "
        "COALESCE(tk.entries, 0) AS entries, "
        "COALESCE(tk.unique_participants, 0) AS unique_participants, "
        "COALESCE(we.winning_colour_entries, 0) AS winning_colour_entries, "
        "COALESCE(w.total_winners, 0) AS total_winners, "
        "COALESCE(w.ultimate_winners, 0) AS ultimate_winners, "
        "COALESCE(w.tier2_winners, 0) AS tier2_winners, "
        "COALESCE(w.tier3_winners, 0) AS tier3_winners, "
        "COALESCE(w.cashback_recipients, 0) AS cashback_recipients, "
        "COALESCE(w.total_prizes, 0) AS total_prizes "
        "FROM draws d "
        "LEFT JOIN ("
        "  SELECT draw_id, COUNT(*) AS entries, "
        "  COUNT(DISTINCT customer_id) AS unique_participants, "
        "  COALESCE(SUM(total_amount::numeric), 0) AS total_pool "
        "  FROM tickets WHERE payment_status = 1 GROUP BY draw_id"
        ") tk ON tk.draw_id = d.id "
        "LEFT JOIN ("
        "  SELECT t.draw_id, COUNT(*) AS winning_colour_entries "
        "  FROM tickets t JOIN draws d2 ON d2.id = t.draw_id "
        "  WHERE t.payment_status = 1 "
        "  AND t.selected_option_name = d2.winning_option_name "
        "  GROUP BY t.draw_id"
        ") we ON we.draw_id = d.id "
        "LEFT JOIN ("
        "  SELECT rw.draw_id, COUNT(*) AS total_winners, "
        "  COUNT(*) FILTER (WHERE rw.prize_category_name ILIKE '%ultimate%') AS ultimate_winners, "
        "  COUNT(*) FILTER (WHERE rw.prize_category_name ILIKE '%tier 2%' OR rw.prize_category_name ILIKE '%tier2%') AS tier2_winners, "
        "  COUNT(*) FILTER (WHERE rw.prize_category_name ILIKE '%tier 3%' OR rw.prize_category_name ILIKE '%tier3%') AS tier3_winners, "
        "  COUNT(*) FILTER (WHERE rw.prize_category_name ILIKE '%cashback%' OR rw.prize_category_name ILIKE '%consolation%') AS cashback_recipients, "
        "  COALESCE(SUM(p.value), 0) AS total_prizes "
        "  FROM raffle_winners rw "
        "  LEFT JOIN prizes p ON p.id = rw.prize_id "
        "  GROUP BY rw.draw_id"
        ") w ON w.draw_id = d.id "
        "WHERE ($1 = '' OR d.draw_date >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR d.draw_date < (NULLIF($2,'')::date + interval '1 day')) "
        "AND ($3 = '' OR d.raffle_id = $3) "
        "ORDER BY d.draw_date DESC LIMIT $4 OFFSET $5",
        startDate, endDate, raffleId, pageSize, offset);

    for (const auto &row : rows) {
      Json::Value item;
      item["id"] = row["id"].as<std::string>();
      item["drawCode"] = row["draw_code"].as<std::string>();
      item["drawDate"] = row["draw_date"].isNull()
                             ? ""
                             : row["draw_date"].as<std::string>();
      int32_t status = row["status"].isNull() ? -1 : row["status"].as<int32_t>();
      item["status"] = drawStatusLabel(status);
      item["statusValue"] = status;
      item["alias"] = row["alias"].isNull() ? "" : row["alias"].as<std::string>();
      item["winningColour"] =
          row["winning_colour"].isNull() ? "" : row["winning_colour"].as<std::string>();
      item["totalEntries"] = (Json::UInt64)row["total_entries"].as<uint64_t>();
      item["entries"] = (Json::UInt64)row["entries"].as<uint64_t>();
      item["totalPool"] = row["total_pool"].isNull() ? 0.0 : row["total_pool"].as<double>();
      item["totalPrizes"] = row["total_prizes"].isNull() ? 0.0 : row["total_prizes"].as<double>();
      item["uniqueParticipants"] =
          (Json::UInt64)row["unique_participants"].as<uint64_t>();
      item["winningColourEntries"] =
          (Json::UInt64)row["winning_colour_entries"].as<uint64_t>();
      item["totalWinners"] = (Json::UInt64)row["total_winners"].as<uint64_t>();
      item["ultimateWinners"] =
          (Json::UInt64)row["ultimate_winners"].as<uint64_t>();
      item["tier2Winners"] = (Json::UInt64)row["tier2_winners"].as<uint64_t>();
      item["tier3Winners"] = (Json::UInt64)row["tier3_winners"].as<uint64_t>();
      item["cashbackRecipients"] =
          (Json::UInt64)row["cashback_recipients"].as<uint64_t>();
      data.append(item);
    }
    response.result["data"] = data;
    co_return response;
  } catch (const DrogonDbException &e) {
    co_return co_await errorResponse(
        "Database error while fetching draw statistics.", e.base().what());
  } catch (const std::exception &e) {
    dto::BaseApiResponse err;
    err.success = false;
    err.error["code"] = constants::ERR_INTERNAL;
    err.error["message"] = "Internal error.";
    err.error["detail"] = e.what();
    co_return err;
  }
}

drogon::Task<dto::BaseApiResponse> DashboardService::getTopParticipants(
    const std::string &startDate, const std::string &endDate, int limit) {
  auto dbClient = drogon::app().getDbClient();

  if (limit <= 0)
    limit = 10;

  try {
    auto rows = co_await dbClient->execSqlCoro(
        "SELECT c.id, c.msisdn, c.first_name, c.last_name, "
        "COUNT(DISTINCT t.draw_id) AS draws_participated, "
        "COUNT(t.id) AS total_tickets, "
        "COALESCE(SUM(t.total_amount::numeric), 0) AS total_spent, "
        "COALESCE(c.number_of_wins, 0) AS times_won, "
        "COALESCE(c.total_won, 0) AS total_won, "
        "(SELECT COUNT(*) FROM raffle_winners rw "
        " JOIN tickets tk ON tk.draw_id = rw.draw_id "
        "   AND tk.ticket_number = rw.ticket_number "
        " WHERE tk.customer_id = c.id "
        " AND (rw.prize_category_name ILIKE '%cashback%' "
        "      OR rw.prize_category_name ILIKE '%consolation%')) AS cashbacks_received "
        "FROM customers c "
        "JOIN tickets t ON t.customer_id = c.id AND t.payment_status = 1 "
        "WHERE ($1 = '' OR t.created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR t.created_at < (NULLIF($2,'')::date + interval '1 day')) "
        "GROUP BY c.id, c.msisdn, c.first_name, c.last_name, "
        "c.number_of_wins, c.total_won "
        "ORDER BY total_tickets DESC, total_spent DESC "
        "LIMIT $3",
        startDate, endDate, limit);

    Json::Value data = Json::arrayValue;
    for (const auto &row : rows) {
      Json::Value item;
      item["id"] = row["id"].as<std::string>();
      item["msisdn"] = row["msisdn"].as<std::string>();
      std::string firstName =
          row["first_name"].isNull() ? "" : row["first_name"].as<std::string>();
      std::string lastName =
          row["last_name"].isNull() ? "" : row["last_name"].as<std::string>();
      item["customerName"] = (firstName + " " + lastName).empty()
                                 ? row["msisdn"].as<std::string>()
                                 : firstName + " " + lastName;
      item["firstName"] = firstName;
      item["lastName"] = lastName;
      item["drawsParticipated"] =
          (Json::UInt64)row["draws_participated"].as<uint64_t>();
      item["totalTickets"] = (Json::UInt64)row["total_tickets"].as<uint64_t>();
      item["totalSpent"] =
          row["total_spent"].isNull() ? 0.0 : row["total_spent"].as<double>();
      item["timesWon"] = (Json::UInt64)row["times_won"].as<uint64_t>();
      item["totalWon"] =
          row["total_won"].isNull() ? "0" : row["total_won"].as<std::string>();
      item["cashbacksReceived"] =
          (Json::UInt64)row["cashbacks_received"].as<uint64_t>();
      data.append(item);
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.result["data"] = data;
    response.result["totalCount"] = (Json::UInt64)data.size();
    co_return response;
  } catch (const DrogonDbException &e) {
    co_return co_await errorResponse(
        "Database error while fetching top participants.", e.base().what());
  } catch (const std::exception &e) {
    dto::BaseApiResponse err;
    err.success = false;
    err.error["code"] = constants::ERR_INTERNAL;
    err.error["message"] = "Internal error.";
    err.error["detail"] = e.what();
    co_return err;
  }
}

drogon::Task<dto::BaseApiResponse> DashboardService::getRevenueTrend(
    const std::string &startDate, const std::string &endDate,
    const std::string &interval, const std::string &raffleId) {
  auto dbClient = drogon::app().getDbClient();

  std::string bucket = interval.empty() ? "day" : interval;

  try {
    auto rows = co_await dbClient->execSqlCoro(
        "SELECT date_trunc($3, t.created_at) AS bucket, "
        "COALESCE(SUM(t.total_amount::numeric), 0) AS revenue, "
        "COUNT(*) AS tickets, "
        "COUNT(DISTINCT t.customer_id) AS participants "
        "FROM tickets t "
        "WHERE t.payment_status = 1 "
        "AND ($1 = '' OR t.created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR t.created_at < (NULLIF($2,'')::date + interval '1 day')) "
        "AND ($4 = '' OR t.raffle_id = $4) "
        "GROUP BY bucket ORDER BY bucket ASC",
        startDate, endDate, bucket, raffleId);

    Json::Value data = Json::arrayValue;
    double totalRevenue = 0.0;
    uint64_t totalTickets = 0;
    for (const auto &row : rows) {
      Json::Value item;
      item["period"] = row["bucket"].isNull() ? "" : row["bucket"].as<std::string>();
      double revenue =
          row["revenue"].isNull() ? 0.0 : row["revenue"].as<double>();
      uint64_t tickets = row["tickets"].as<uint64_t>();
      item["revenue"] = revenue;
      item["tickets"] = (Json::UInt64)tickets;
      item["participants"] = (Json::UInt64)row["participants"].as<uint64_t>();
      totalRevenue += revenue;
      totalTickets += tickets;
      data.append(item);
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.result["interval"] = bucket;
    response.result["data"] = data;
    response.result["totalRevenue"] = totalRevenue;
    response.result["totalTickets"] = (Json::UInt64)totalTickets;
    co_return response;
  } catch (const DrogonDbException &e) {
    co_return co_await errorResponse(
        "Database error while fetching revenue trend.", e.base().what());
  } catch (const std::exception &e) {
    dto::BaseApiResponse err;
    err.success = false;
    err.error["code"] = constants::ERR_INTERNAL;
    err.error["message"] = "Internal error.";
    err.error["detail"] = e.what();
    co_return err;
  }
}

} // namespace raffle::services
