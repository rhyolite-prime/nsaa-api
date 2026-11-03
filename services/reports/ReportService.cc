//
// Created by Emmanuel Addo-Odame on 02/11/2026.
//

#include "ReportService.h"

#include "constants/ErrorCodes.h"
#include "constants/RewardTypes.h"
#include "dto/BaseApiResponse.h"

#include <drogon/orm/DbClient.h>
#include <drogon/utils/coroutine.h>

#include <string>

using namespace drogon::orm;

namespace raffle::services {

namespace {

std::string winnerStatusLabel(int32_t status) {
  switch (status) {
  case constants::WinnerStatus::PENDING_PAYOUT:
    return "Pending Payout";
  case constants::WinnerStatus::PROCESSING_PAYOUT:
    return "Processing Payout";
  case constants::WinnerStatus::PAYOUT_COMPLETED:
    return "Payout Completed";
  case constants::WinnerStatus::PAYOUT_FAILED:
    return "Payout Failed";
  case constants::WinnerStatus::CLAIMED:
    return "Claimed";
  case constants::WinnerStatus::DISQUALIFIED:
    return "Disqualified";
  case constants::WinnerStatus::UNDER_REVIEW:
    return "Under Review";
  case constants::WinnerStatus::REPLACED:
    return "Replaced";
  case constants::WinnerStatus::NOTIFIED:
    return "Notified";
  case constants::WinnerStatus::ACKNOWLEDGED:
    return "Acknowledged";
  default:
    return "Unknown";
  }
}

std::string ticketStatusLabel(int32_t status) {
  switch (status) {
  case constants::TicketStatus::PENDING:
    return "Pending";
  case constants::TicketStatus::PAID:
    return "Paid";
  case constants::TicketStatus::FAILED:
    return "Failed";
  case constants::TicketStatus::REFUNDED:
    return "Refunded";
  default:
    return "Unknown";
  }
}

} // namespace

drogon::Task<dto::BaseApiResponse>
ReportService::getSummary(const std::string &startDate,
                          const std::string &endDate) {
  auto dbClient = drogon::app().getDbClient();

  try {
    auto ticketRows = co_await dbClient->execSqlCoro(
        "SELECT COALESCE(SUM(quantity), 0) AS total_tickets, "
        "COALESCE(SUM(quantity) FILTER (WHERE is_active = true), 0) AS paid_tickets, "
        "COALESCE(SUM(quantity) FILTER (WHERE is_active = false), 0) AS pending_tickets, "
        "COALESCE(SUM(total_amount::numeric) FILTER (WHERE is_active = true), 0) AS total_revenue, "
        "COALESCE(SUM(quantity), 0) AS total_quantity "
        "FROM tickets "
        "WHERE ($1 = '' OR created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR created_at < (NULLIF($2,'')::date + interval '1 day'))",
        startDate, endDate);

    auto winnerRows = co_await dbClient->execSqlCoro(
        "SELECT COUNT(*) AS total_winners, "
        "COUNT(*) FILTER (WHERE status = 0) AS pending_payouts "
        "FROM winners "
        "WHERE ($1 = '' OR created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR created_at < (NULLIF($2,'')::date + interval '1 day'))",
        startDate, endDate);

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

    dto::BaseApiResponse response;
    response.success = true;

    if (!ticketRows.empty()) {
      const auto &r = ticketRows[0];
      response.result["totalTickets"] = (Json::UInt64)r["total_tickets"].as<uint64_t>();
      response.result["paidTickets"] = (Json::UInt64)r["paid_tickets"].as<uint64_t>();
      response.result["pendingTickets"] = (Json::UInt64)r["pending_tickets"].as<uint64_t>();
      response.result["totalRevenue"] = r["total_revenue"].isNull() ? 0.0 : r["total_revenue"].as<double>();
      response.result["totalQuantity"] = (Json::UInt64)r["total_quantity"].as<uint64_t>();
    }
    if (!winnerRows.empty()) {
      const auto &r = winnerRows[0];
      response.result["totalWinners"] = (Json::UInt64)r["total_winners"].as<uint64_t>();
      response.result["pendingPayouts"] = (Json::UInt64)r["pending_payouts"].as<uint64_t>();
    }
    if (!customerRows.empty()) {
      const auto &r = customerRows[0];
      response.result["totalCustomers"] = (Json::UInt64)r["total_customers"].as<uint64_t>();
      response.result["activeCustomers"] = (Json::UInt64)r["active_customers"].as<uint64_t>();
    }
    if (!raffleRows.empty()) {
      const auto &r = raffleRows[0];
      response.result["totalRaffles"] = (Json::UInt64)r["total_raffles"].as<uint64_t>();
      response.result["activeRaffles"] = (Json::UInt64)r["active_raffles"].as<uint64_t>();
    }
    if (!drawRows.empty()) {
      const auto &r = drawRows[0];
      response.result["totalDraws"] = (Json::UInt64)r["total_draws"].as<uint64_t>();
      response.result["completedDraws"] = (Json::UInt64)r["completed_draws"].as<uint64_t>();
    }

    response.result["startDate"] = startDate;
    response.result["endDate"] = endDate;
    co_return response;
  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse err;
    err.success = false;
    err.error["code"] = constants::ERR_DB_QUERY;
    err.error["message"] = "Database error while fetching report summary.";
    err.error["detail"] = e.base().what();
    co_return err;
  } catch (const std::exception &e) {
    dto::BaseApiResponse err;
    err.success = false;
    err.error["code"] = constants::ERR_INTERNAL;
    err.error["message"] = "Internal error.";
    err.error["detail"] = e.what();
    co_return err;
  }
}

drogon::Task<dto::BaseApiResponse> ReportService::getTicketSalesReport(
    const std::string &startDate, const std::string &endDate,
    const std::string &raffleId, const std::string &paymentStatus,
    const std::string &query, int pageNo, int pageSize) {
  auto dbClient = drogon::app().getDbClient();

  try {
    auto countRows = co_await dbClient->execSqlCoro(
        "SELECT COUNT(*) AS total FROM tickets t "
        "WHERE ($1 = '' OR t.created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR t.created_at < (NULLIF($2,'')::date + interval '1 day')) "
        "AND ($3 = '' OR t.raffle_id = NULLIF($3,'')::uuid) "
        "AND ($4 = '' OR t.payment_status = NULLIF($4,'')::int) "
        "AND ($5 = '' OR t.ticket_number ILIKE '%' || $5 || '%' "
        "OR t.customer_phone ILIKE '%' || $5 || '%')",
        startDate, endDate, raffleId, paymentStatus, query);

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
        "SELECT t.id, t.ticket_number, t.ticket_alias, t.draw_id, t.raffle_id, "
        "t.customer_id, t.customer_phone, t.selected_option_name, "
        "t.selected_option_value, t.entry_fee, t.quantity, t.total_amount, "
        "t.payment_reference, t.payment_status, t.payment_method, "
        "t.purchase_channel, t.is_active, t.created_at "
        "FROM tickets t "
        "WHERE ($1 = '' OR t.created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR t.created_at < (NULLIF($2,'')::date + interval '1 day')) "
        "AND ($3 = '' OR t.raffle_id = NULLIF($3,'')::uuid) "
        "AND ($4 = '' OR t.payment_status = NULLIF($4,'')::int) "
        "AND ($5 = '' OR t.ticket_number ILIKE '%' || $5 || '%' "
        "OR t.customer_phone ILIKE '%' || $5 || '%') "
        "ORDER BY t.created_at DESC LIMIT $6 OFFSET $7",
        startDate, endDate, raffleId, paymentStatus, query, (long long)pageSize, (long long)offset);

    double totalRevenue = 0.0;
    for (const auto &row : rows) {
      Json::Value item;
      item["id"] = row["id"].as<std::string>();
      item["ticketNumber"] = row["ticket_number"].as<std::string>();
      item["ticketAlias"] =
          row["ticket_alias"].isNull() ? "" : row["ticket_alias"].as<std::string>();
      item["drawId"] = row["draw_id"].isNull() ? "" : row["draw_id"].as<std::string>();
      item["raffleId"] =
          row["raffle_id"].isNull() ? "" : row["raffle_id"].as<std::string>();
      item["customerId"] =
          row["customer_id"].isNull() ? "" : row["customer_id"].as<std::string>();
      item["customerPhone"] =
          row["customer_phone"].isNull() ? "" : row["customer_phone"].as<std::string>();
      item["selectedOptionName"] =
          row["selected_option_name"].isNull() ? "" : row["selected_option_name"].as<std::string>();
      item["selectedOptionValue"] =
          row["selected_option_value"].isNull() ? "" : row["selected_option_value"].as<std::string>();
      item["entryFee"] = row["entry_fee"].as<std::string>();
      item["quantity"] = row["quantity"].as<int32_t>();
      item["totalAmount"] = row["total_amount"].as<std::string>();
      item["paymentReference"] =
          row["payment_reference"].isNull() ? "" : row["payment_reference"].as<std::string>();
      int32_t status =
          row["payment_status"].isNull() ? -1 : row["payment_status"].as<int32_t>();
      item["paymentStatus"] = ticketStatusLabel(status);
      item["paymentStatusValue"] = status;
      item["paymentMethod"] =
          row["payment_method"].isNull() ? "" : row["payment_method"].as<std::string>();
      item["purchaseChannel"] =
          row["purchase_channel"].isNull() ? "" : row["purchase_channel"].as<std::string>();
      item["isActive"] = row["is_active"].as<bool>();
      item["createdAt"] =
          row["created_at"].isNull() ? "" : row["created_at"].as<std::string>();
      if (status == constants::TicketStatus::PAID) {
        try {
          totalRevenue += std::stod(row["total_amount"].as<std::string>());
        } catch (...) {
          // ignore malformed amounts
        }
      }
      data.append(item);
    }
    response.result["data"] = data;
    response.result["totalRevenue"] = totalRevenue;
    co_return response;
  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse err;
    err.success = false;
    err.error["code"] = constants::ERR_DB_QUERY;
    err.error["message"] = "Database error while fetching ticket sales report.";
    err.error["detail"] = e.base().what();
    co_return err;
  } catch (const std::exception &e) {
    dto::BaseApiResponse err;
    err.success = false;
    err.error["code"] = constants::ERR_INTERNAL;
    err.error["message"] = "Internal error.";
    err.error["detail"] = e.what();
    co_return err;
  }
}

drogon::Task<dto::BaseApiResponse> ReportService::getRevenueReport(
    const std::string &startDate, const std::string &endDate,
    const std::string &groupBy, const std::string &raffleId) {
  auto dbClient = drogon::app().getDbClient();

  std::string bucket = groupBy.empty() ? "day" : groupBy;

  try {
    auto rows = co_await dbClient->execSqlCoro(
        "SELECT date_trunc($3, t.created_at) AS bucket, "
        "COALESCE(SUM(t.total_amount::numeric), 0) AS revenue, "
        "COALESCE(SUM(t.quantity), 0) AS entries, "
        "COUNT(*) AS tickets, "
        "COUNT(DISTINCT t.customer_id) AS participants "
        "FROM tickets t "
        "WHERE t.payment_status = 1 "
        "AND ($1 = '' OR t.created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR t.created_at < (NULLIF($2,'')::date + interval '1 day')) "
        "AND ($4 = '' OR t.raffle_id = NULLIF($4,'')::uuid) "
        "GROUP BY bucket ORDER BY bucket ASC",
        startDate, endDate, bucket, raffleId);

    Json::Value data = Json::arrayValue;
    double totalRevenue = 0.0;
    uint64_t totalTickets = 0, totalEntries = 0;
    for (const auto &row : rows) {
      Json::Value item;
      item["period"] = row["bucket"].isNull() ? "" : row["bucket"].as<std::string>();
      double revenue = row["revenue"].isNull() ? 0.0 : row["revenue"].as<double>();
      item["revenue"] = revenue;
      item["tickets"] = (Json::UInt64)row["tickets"].as<uint64_t>();
      item["entries"] = (Json::UInt64)row["entries"].as<uint64_t>();
      item["participants"] = (Json::UInt64)row["participants"].as<uint64_t>();
      totalRevenue += revenue;
      totalTickets += row["tickets"].as<uint64_t>();
      totalEntries += row["entries"].as<uint64_t>();
      data.append(item);
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.result["groupBy"] = bucket;
    response.result["data"] = data;
    response.result["totalRevenue"] = totalRevenue;
    response.result["totalTickets"] = (Json::UInt64)totalTickets;
    response.result["totalEntries"] = (Json::UInt64)totalEntries;
    co_return response;
  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse err;
    err.success = false;
    err.error["code"] = constants::ERR_DB_QUERY;
    err.error["message"] = "Database error while fetching revenue report.";
    err.error["detail"] = e.base().what();
    co_return err;
  } catch (const std::exception &e) {
    dto::BaseApiResponse err;
    err.success = false;
    err.error["code"] = constants::ERR_INTERNAL;
    err.error["message"] = "Internal error.";
    err.error["detail"] = e.what();
    co_return err;
  }
}

drogon::Task<dto::BaseApiResponse> ReportService::getWinnersReport(
    const std::string &startDate, const std::string &endDate,
    const std::string &drawId, const std::string &status,
    const std::string &query, int pageNo, int pageSize) {
  auto dbClient = drogon::app().getDbClient();

  try {
    auto countRows = co_await dbClient->execSqlCoro(
        "SELECT COUNT(*) AS total FROM winners w "
        "LEFT JOIN customers c ON c.id = w.customer_id "
        "WHERE ($1 = '' OR w.created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR w.created_at < (NULLIF($2,'')::date + interval '1 day')) "
        "AND ($3 = '' OR w.draw_id = NULLIF($3,'')::uuid) "
        "AND ($4 = '' OR w.status = NULLIF($4,'')::int) "
        "AND ($5 = '' OR w.phone_number ILIKE '%' || $5 || '%' "
        "OR c.msisdn ILIKE '%' || $5 || '%' "
        "OR w.prize_description ILIKE '%' || $5 || '%')",
        startDate, endDate, drawId, status, query);

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
        "SELECT w.id, w.draw_id, d.draw_code, w.ticket_id, w.ticket_alias, "
        "w.customer_id, w.phone_number, w.selected_option_label, "
        "w.prize_description, w.payout_method, w.payout_reference, "
        "w.payout_date, w.status, w.created_at, "
        "c.msisdn, c.first_name, c.last_name "
        "FROM winners w "
        "LEFT JOIN customers c ON c.id = w.customer_id "
        "LEFT JOIN draws d ON d.id = w.draw_id "
        "WHERE ($1 = '' OR w.created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR w.created_at < (NULLIF($2,'')::date + interval '1 day')) "
        "AND ($3 = '' OR w.draw_id = NULLIF($3,'')::uuid) "
        "AND ($4 = '' OR w.status = NULLIF($4,'')::int) "
        "AND ($5 = '' OR w.phone_number ILIKE '%' || $5 || '%' "
        "OR c.msisdn ILIKE '%' || $5 || '%' "
        "OR w.prize_description ILIKE '%' || $5 || '%') "
        "ORDER BY w.created_at DESC LIMIT $6 OFFSET $7",
        startDate, endDate, drawId, status, query, (long long)pageSize, (long long)offset);

    for (const auto &row : rows) {
      Json::Value item;
      item["id"] = row["id"].as<std::string>();
      item["drawId"] = row["draw_id"].isNull() ? "" : row["draw_id"].as<std::string>();
      item["drawCode"] =
          row["draw_code"].isNull() ? "" : row["draw_code"].as<std::string>();
      item["ticketId"] =
          row["ticket_id"].isNull() ? "" : row["ticket_id"].as<std::string>();
      item["ticketAlias"] =
          row["ticket_alias"].isNull() ? "" : row["ticket_alias"].as<std::string>();
      item["customerId"] =
          row["customer_id"].isNull() ? "" : row["customer_id"].as<std::string>();
      item["phoneNumber"] =
          row["phone_number"].isNull() ? "" : row["phone_number"].as<std::string>();
      item["msisdn"] = row["msisdn"].isNull() ? "" : row["msisdn"].as<std::string>();
      std::string firstName =
          row["first_name"].isNull() ? "" : row["first_name"].as<std::string>();
      std::string lastName =
          row["last_name"].isNull() ? "" : row["last_name"].as<std::string>();
      item["customerName"] = firstName + " " + lastName;
      item["selectedOptionLabel"] =
          row["selected_option_label"].isNull() ? "" : row["selected_option_label"].as<std::string>();
      item["prizeDescription"] =
          row["prize_description"].isNull() ? "" : row["prize_description"].as<std::string>();
      item["payoutMethod"] =
          row["payout_method"].isNull() ? "" : row["payout_method"].as<std::string>();
      item["payoutReference"] =
          row["payout_reference"].isNull() ? "" : row["payout_reference"].as<std::string>();
      item["payoutDate"] =
          row["payout_date"].isNull() ? "" : row["payout_date"].as<std::string>();
      int32_t st = row["status"].isNull() ? -1 : row["status"].as<int32_t>();
      item["status"] = winnerStatusLabel(st);
      item["statusValue"] = st;
      item["createdAt"] =
          row["created_at"].isNull() ? "" : row["created_at"].as<std::string>();
      data.append(item);
    }
    response.result["data"] = data;
    co_return response;
  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse err;
    err.success = false;
    err.error["code"] = constants::ERR_DB_QUERY;
    err.error["message"] = "Database error while fetching winners report.";
    err.error["detail"] = e.base().what();
    co_return err;
  } catch (const std::exception &e) {
    dto::BaseApiResponse err;
    err.success = false;
    err.error["code"] = constants::ERR_INTERNAL;
    err.error["message"] = "Internal error.";
    err.error["detail"] = e.what();
    co_return err;
  }
}

drogon::Task<dto::BaseApiResponse> ReportService::getCustomerReport(
    const std::string &startDate, const std::string &endDate,
    const std::string &query, const std::string &isActive, int pageNo,
    int pageSize) {
  auto dbClient = drogon::app().getDbClient();

  try {
    auto countRows = co_await dbClient->execSqlCoro(
        "SELECT COUNT(*) AS total FROM customers c "
        "WHERE ($1 = '' OR c.created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR c.created_at < (NULLIF($2,'')::date + interval '1 day')) "
        "AND ($3 = '' OR c.msisdn ILIKE '%' || $3 || '%' "
        "OR c.first_name ILIKE '%' || $3 || '%' "
        "OR c.last_name ILIKE '%' || $3 || '%' "
        "OR c.email ILIKE '%' || $3 || '%') "
        "AND ($4 = '' OR c.is_active = NULLIF($4,'')::boolean)",
        startDate, endDate, query, isActive);

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
        "SELECT c.id, c.msisdn, c.first_name, c.last_name, c.email, "
        "c.account_provider, c.account_name, c.account_number, "
        "c.is_active, c.is_locked_out, c.total_spent, c.total_won, "
        "c.number_of_wins, c.number_of_entries, c.last_active, c.created_at "
        "FROM customers c "
        "WHERE ($1 = '' OR c.created_at >= NULLIF($1,'')::timestamp) "
        "AND ($2 = '' OR c.created_at < (NULLIF($2,'')::date + interval '1 day')) "
        "AND ($3 = '' OR c.msisdn ILIKE '%' || $3 || '%' "
        "OR c.first_name ILIKE '%' || $3 || '%' "
        "OR c.last_name ILIKE '%' || $3 || '%' "
        "OR c.email ILIKE '%' || $3 || '%') "
        "AND ($4 = '' OR c.is_active = NULLIF($4,'')::boolean) "
        "ORDER BY c.created_at DESC LIMIT $5 OFFSET $6",
        startDate, endDate, query, isActive, (long long)pageSize, (long long)offset);

    for (const auto &row : rows) {
      Json::Value item;
      item["id"] = row["id"].as<std::string>();
      item["msisdn"] = row["msisdn"].as<std::string>();
      item["firstName"] =
          row["first_name"].isNull() ? "" : row["first_name"].as<std::string>();
      item["lastName"] =
          row["last_name"].isNull() ? "" : row["last_name"].as<std::string>();
      item["email"] = row["email"].isNull() ? "" : row["email"].as<std::string>();
      item["accountProvider"] =
          row["account_provider"].isNull() ? "" : row["account_provider"].as<std::string>();
      item["accountName"] =
          row["account_name"].isNull() ? "" : row["account_name"].as<std::string>();
      item["accountNumber"] =
          row["account_number"].isNull() ? "" : row["account_number"].as<std::string>();
      item["isActive"] = row["is_active"].as<bool>();
      item["isLockedOut"] = row["is_locked_out"].as<bool>();
      item["totalSpent"] =
          row["total_spent"].isNull() ? "0" : row["total_spent"].as<std::string>();
      item["totalWon"] =
          row["total_won"].isNull() ? "0" : row["total_won"].as<std::string>();
      item["numberOfWins"] = (Json::UInt64)row["number_of_wins"].as<uint64_t>();
      item["numberOfEntries"] =
          (Json::UInt64)row["number_of_entries"].as<uint64_t>();
      item["lastActive"] =
          row["last_active"].isNull() ? "" : row["last_active"].as<std::string>();
      item["createdAt"] =
          row["created_at"].isNull() ? "" : row["created_at"].as<std::string>();
      data.append(item);
    }
    response.result["data"] = data;
    co_return response;
  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse err;
    err.success = false;
    err.error["code"] = constants::ERR_DB_QUERY;
    err.error["message"] = "Database error while fetching customer report.";
    err.error["detail"] = e.base().what();
    co_return err;
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
