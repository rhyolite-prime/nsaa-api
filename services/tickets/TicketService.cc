//
// Created by Emmanuel Addo-Odame on 03/03/2026.
//

#include "TicketService.h"
#include "../../dto/BaseApiResponse.h"
#include "../../dto/CreateTicketDto.h"
#include "Customers.h"
#include "constants/ErrorCodes.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>

#include "Colours.h"
#include "Draws.h"
#include "Raffles.h"
#include "Tickets.h"
#include <algorithm>
#include <uuid/uuid.h>

#include "utils/IdGeneratorUtils.h"
#include "../hubtel_api/HubtelApi.h"
#include "../../dto/CheckoutResponse.h"
#include "../../dto/WebCheckoutDto.h"
#include <iomanip>
#include <sstream>

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

drogon::Task<dto::BaseApiResponse>
TicketService::getAll(int pageNo, int pageSize, const std::string &query) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Tickets> mapper(dbClient);

  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria = Criteria(Tickets::Cols::_customer_phone,
                              CompareOperator::Like, likeQuery);
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
    auto result =
        co_await mapper.orderBy(Tickets::Cols::_created_at, SortOrder::DESC)
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
    for (const auto &ticket : result) {
      Json::Value ticketJson;
      ticketJson["id"] = ticket.getValueOfId();
      ticketJson["ticketNumber"] = ticket.getValueOfTicketNumber();
      ticketJson["drawId"] = ticket.getValueOfDrawId();
      ticketJson["customerId"] = ticket.getValueOfCustomerId();
      ticketJson["colourId"] = ticket.getValueOfColourId();
      ticketJson["entryFee"] = ticket.getValueOfEntryFee();
      ticketJson["quantity"] = ticket.getValueOfQuantity();
      ticketJson["totalAmount"] = ticket.getValueOfTotalAmount();
      ticketJson["paymentReference"] = ticket.getValueOfPaymentReference();
      ticketJson["paymentStatus"] = ticket.getValueOfPaymentStatus();
      ticketJson["paymentMethod"] = ticket.getValueOfPaymentMethod();
      ticketJson["purchaseChannel"] = ticket.getValueOfPurchaseChannel();
      ticketJson["isActive"] = ticket.getValueOfIsActive();
      ticketJson["createdAt"] = ticket.getValueOfCreatedAt().toDbStringLocal();
      ticketJson["updatedAt"] = ticket.getValueOfUpdatedAt().toDbStringLocal();
      ticketJson["customerPhone"] = ticket.getValueOfCustomerPhone();
      ticketJson["colourName"] = ticket.getValueOfColourName();
      ticketJson["raffleId"] = ticket.getValueOfRaffleId();

      data.append(ticketJson);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching tickets.";
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

drogon::Task<dto::BaseApiResponse> TicketService::getStats() {
  auto dbClient = drogon::app().getDbClient();

  try {
    auto result = co_await dbClient->execSqlCoro(
        "SELECT count(*) as total_tickets, "
        "count(CASE WHEN is_active = true THEN 1 END) as active_tickets, "
        "count(CASE WHEN payment_status = 'PAID' THEN 1 END) as paid_tickets, "
        "sum(total_amount) as total_revenue "
        "FROM tickets");

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Ticket stats retrieved successfully";
    if (result.size() > 0) {
      auto totalTickets = result[0]["total_tickets"].as<int>();
      auto paidTickets = result[0]["paid_tickets"].as<int>();

      response.result["totalTickets"] = totalTickets;
      response.result["activeTickets"] = result[0]["active_tickets"].as<int>();
      response.result["paidTickets"] = paidTickets;
      response.result["totalRevenue"] = result[0]["total_revenue"].as<double>();

      double conversionRate = 0.0;
      if (totalTickets > 0) {
        conversionRate =
            (static_cast<double>(paidTickets) / totalTickets) * 100.0;
      }
      response.result["conversionRate"] = conversionRate;
    } else {
      response.result["totalTickets"] = 0;
      response.result["activeTickets"] = 0;
      response.result["paidTickets"] = 0;
      response.result["totalRevenue"] = 0.0;
      response.result["conversionRate"] = 0.0;
    }
    co_return response;
  } catch (const drogon::orm::DrogonDbException &e) {
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

drogon::Task<dto::BaseApiResponse>
TicketService::create(const dto::CreateTicketDto &dto) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Raffles> raffleMapper(dbClient);
  CoroMapper<Draws> drawMapper(dbClient);
  CoroMapper<Tickets> ticketMapper(dbClient);

  try {
    // 1. Find the currently ACTIVE raffle
    auto activeRaffles = co_await raffleMapper.findBy(
        Criteria(Raffles::Cols::_status, CompareOperator::EQ, "ACTIVE"));

    if (activeRaffles.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "No active raffle found.";
      co_return errorResponse;
    }

    auto activeRaffle = activeRaffles[0];
    std::string raffleSlug = activeRaffle.getValueOfSlug();

    // 2. Find the corresponding Draw using the raffle's slug (alias in Draws)
    auto draws = co_await drawMapper.findBy(
        Criteria(Draws::Cols::_alias, CompareOperator::EQ, raffleSlug));

    if (draws.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] =
          "No corresponding draw found for the active raffle.";
      co_return errorResponse;
    }

    auto activeDraw = draws[0];

    // 3. Customer Management
    CoroMapper<Customers> customerMapper(dbClient);
    std::string customerId;
    auto existingCustomers = co_await customerMapper.findBy(Criteria(
        Customers::Cols::_msisdn, CompareOperator::EQ, dto.getCustomerPhone()));

    if (!existingCustomers.empty()) {
      customerId = existingCustomers[0].getValueOfId();
    } else {
      Customers newCustomer;
      newCustomer.setMsisdn(dto.getCustomerPhone());
      newCustomer.setIsActive(true);
      auto insertedCustomer = co_await customerMapper.insert(newCustomer);
      customerId = insertedCustomer.getValueOfId();
    }

    // 4. Color Management
    CoroMapper<Colours> colourMapper(dbClient);
    std::string colourName;
    try {
      auto colour = co_await colourMapper.findByPrimaryKey(dto.getColourId());
      colourName = colour.getValueOfName();
    } catch (const drogon::orm::DrogonDbException &e) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_VALIDATION;
      errorResponse.error["message"] = "Invalid colour ID.";
      co_return errorResponse;
    }

    // 5. Create the ticket
    Tickets newTicket;
    newTicket.setDrawId(activeDraw.getValueOfId());
    newTicket.setColourId(dto.getColourId());
    newTicket.setColourName(colourName);
    newTicket.setQuantity(dto.getQuantity());
    newTicket.setCustomerPhone(dto.getCustomerPhone());
    newTicket.setRaffleId(activeRaffle.getValueOfId());

    std::string clientReference =
        raffle::utils::IdGeneratorUtils::generateGuid();

    newTicket.setPaymentReference(clientReference);
    newTicket.setCustomerId(customerId);

    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
    localtime_r(&nowTime, &localTime);

    std::ostringstream ticketNumberStream;
    ticketNumberStream << std::put_time(&localTime, "%Y%m%d%H%M%S");
    newTicket.setTicketNumber(ticketNumberStream.str());

    auto &app = drogon::app();
    auto customConfig = app.getCustomConfig();
    std::string ticketFee = customConfig["TicketConfiguration"]["Price"].asString();

    // Placeholder for entry fee
    newTicket.setEntryFee(ticketFee);

    double entryFee = std::stod(newTicket.getValueOfEntryFee());
    double totalAmount = entryFee * dto.getQuantity();
    newTicket.setTotalAmount(std::to_string(totalAmount));

    newTicket.setPaymentStatus("PENDING");
    newTicket.setPurchaseChannel(dto.getPurchaseChannel()); // USSD, WSSD
    newTicket.setPaymentMethod(dto.getPaymentMethod());

    newTicket.setIsActive(false); // will be updated and set to true when the
                                  // payment status is marked as Paid.

    auto result = co_await ticketMapper.insert(newTicket);

    // initialize checkout
    raffle::services::HubtelApi hubtelApi;
    raffle::dto::WebCheckoutDto webCheckoutDto;
    webCheckoutDto.setCallbackUrl("https://webhook.site/93972647-7e6d-40dc-af87-5c3d9f6f696b");
    webCheckoutDto.setCancellationUrl("https://wssd.rhyoliteprime.com/019c4d8c-58be-73a9-80e9-2a44d4e556f3");
    webCheckoutDto.setReturnUrl("https://wssd.rhyoliteprime.com/019c4d8c-58be-73a9-80e9-2a44d4e556f3");
    webCheckoutDto.setTotalAmount(totalAmount);
    webCheckoutDto.setDescription("Ticket Purchase - " + raffleSlug);
    webCheckoutDto.setClientReference(clientReference);
    
    const auto &hubtelConfig = customConfig["HubtelPaymentApi"];
    if (hubtelConfig.isMember("WebCheckoutCallBackUrl")) {
        webCheckoutDto.setCallbackUrl(hubtelConfig["WebCheckoutCallBackUrl"].asString());
    }

    auto hubtelResult = co_await hubtelApi.InitializeWebCheckout(webCheckoutDto);

    dto::BaseApiResponse response;
    if (hubtelResult.responseCode == "0000") {
        response.success = true;
        response.message = "Ticket created successfully. Please complete payment.";
        response.result["ticketId"] = result.getValueOfId();
        response.result["ticketNumber"] = result.getValueOfTicketNumber();
        response.result["paymentReference"] = clientReference;
        response.result["checkoutUrl"] = hubtelResult.data.checkoutUrl;
    } else {
        response.success = false;
        response.message = "Ticket created but failed to initialize payment. " + hubtelResult.status;
        response.error["code"] = constants::ERR_INTERNAL;
        response.error["message"] = "Payment initialization failed.";
        response.result["ticketId"] = result.getValueOfId();
    }

    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while creating ticket.";
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
