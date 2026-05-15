//
// Created by Emmanuel Addo-Odame on 03/03/2026.
//

#include "TicketService.h"
#include "Customers.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include "dto/CreateTicketDto.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>
#include <string>

#include "Colours.h"
#include "Draws.h"
#include "Raffles.h"
#include "Tickets.h"
#include <algorithm>
#include <uuid/uuid.h>

#include "dto/CheckoutResponse.h"
#include "dto/WebCheckoutDto.h"
#include "utils/IdGeneratorUtils.h"
#include <iomanip>
#include <sstream>

#include "services/hubtel_api/HubtelApi.h"

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
    searchCriteria = Criteria(Tickets::Cols::_ticket_number,
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
        "coalesce(sum(CASE WHEN is_active = true THEN total_amount ELSE 0 "
        "END), 0) as total_revenue "
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
    std::string ticketFee =
        customConfig["TicketConfiguration"]["Price"].asString();

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

    // initialize payment
    const auto &hubtelConfig = customConfig["HubtelPaymentApi"];
    raffle::services::HubtelApi hubtelApi;
    dto::BaseApiResponse response;

    if (dto.getPurchaseChannel() == "WSSD") {

      raffle::dto::WebCheckoutDto webCheckoutDto;
      std::string callbackUrl =
          hubtelConfig["WebCheckoutCallBackUrl"].asString();

      webCheckoutDto.setCallbackUrl(
          hubtelConfig.isMember("WebCheckoutCallBackUrl")
              ? hubtelConfig["WebCheckoutCallBackUrl"].asString()
              : "https://nsaa-api.rhyoliteprime.com/api/v1/"
                "hubtel-callback-processor/web-checkout");
      webCheckoutDto.setCancellationUrl(hubtelConfig["ReturnUrl"].asString());
      webCheckoutDto.setReturnUrl(hubtelConfig["ReturnUrl"].asString());
      webCheckoutDto.setTotalAmount(totalAmount);
      webCheckoutDto.setDescription("Ticket Purchase - " + raffleSlug);
      webCheckoutDto.setClientReference(clientReference);

      auto hubtelResult =
          co_await hubtelApi.InitializeWebCheckout(webCheckoutDto);

      LOG_INFO << "Hubtel WebCheckout result for clientReference="
               << clientReference
               << " responseCode=" << hubtelResult.responseCode
               << " status=" << hubtelResult.status
               << " checkoutUrl=" << hubtelResult.data.checkoutUrl;

      if (hubtelResult.responseCode == "0000") {
        response.success = true;
        response.message =
            "Ticket created successfully. Please complete payment.";
        response.result["ticketId"] = result.getValueOfId();
        response.result["ticketNumber"] = result.getValueOfTicketNumber();
        response.result["paymentReference"] = clientReference;
        response.result["checkoutUrl"] = hubtelResult.data.checkoutUrl;
        response.result["selectedColour"] = colourName;
      } else {
        response.success = false;
        response.message = "Ticket created but failed to initialize payment. " +
                           hubtelResult.status;
        response.error["code"] = constants::ERR_INTERNAL;
        response.error["message"] = "Payment initialization failed.";
        response.result["ticketId"] = result.getValueOfId();
      }
    } else {

      std::string provider = dto.getProvider();



      // convert to lowercase
      std::transform(provider.begin(), provider.end(), provider.begin(),
                     [](unsigned char c) { return std::tolower(c); });

      if (provider.find("mtn") != std::string::npos) {
        provider = "mtn-gh";
      } else if (provider.find("vodafone") != std::string::npos) {
        provider = "vodafone-gh";
      } else if (provider.find("airtel") != std::string::npos) {
        provider = "tigo-gh";
      } else if (provider.find("tigo") != std::string::npos) {
        provider = "tigo-gh";
      } else if (provider.find("telecel") != std::string::npos) {
        provider = "vodafone-gh";
      }

      dto::DirectReceiveMoneyDto directReceiveDto;
      directReceiveDto.setCustomerName("NSAA_" + dto.getCustomerPhone());
      directReceiveDto.setCustomerEmail(dto.getCustomerPhone() +"@nsaaplay.com");
      directReceiveDto.setCustomerMsisdn(dto.getCustomerPhone());
      directReceiveDto.setAmount(totalAmount);

      directReceiveDto.setChannel(provider);
      directReceiveDto.setClientReference(clientReference);
      directReceiveDto.setDescription("Ticket Purchase - " + raffleSlug);
      directReceiveDto.setPrimaryCallbackUrl(hubtelConfig["DirectReceiveMoneyCallBackUrl"].asString());

      auto hubtelResult =  co_await hubtelApi.QueueReceiveMoneyRequest(directReceiveDto);

      LOG_INFO << "Hubtel DirectReceive result for clientReference="
               << clientReference
               << " responseCode=" << hubtelResult.responseCode
               << " transactionId=" << hubtelResult.data.transactionId;

      if (hubtelResult.responseCode == "0001") {
        response.success = true;
        response.message =  "Authorize Payment by entering PIN on your mobile device.\nIf no bill prompt appears, use *170# (MTN) or *110# (AT & Telecel) to resume";
        response.result["ticketId"] = result.getValueOfId();
        response.result["ticketNumber"] = result.getValueOfTicketNumber();
        response.result["paymentReference"] = clientReference;
        response.result["transactionId"] = hubtelResult.data.transactionId;
        response.result["selectedColour"] = colourName;
      } else {
        response.success = false;
        response.message = "Ticket created but failed to initiate mobile money prompt. " + hubtelResult.message;
        response.error["code"] = constants::ERR_INTERNAL;
        response.error["message"] = "Direct payment initialization failed.";
        response.result["ticketId"] = result.getValueOfId();
      }
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

drogon::Task<dto::BaseApiResponse>
TicketService::activate(const std::string &paymentReference) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Tickets> ticketMapper(dbClient);
  CoroMapper<Raffles> raffleMapper(dbClient);
  CoroMapper<Draws> drawMapper(dbClient);

  try {
    // 1. Find the ticket by payment reference
    auto tickets = co_await ticketMapper.findBy(
        Criteria(Tickets::Cols::_payment_reference, CompareOperator::EQ,
                 paymentReference));

    if (tickets.empty()) {
      dto::BaseApiResponse errorResponse;
      errorResponse.success = false;
      errorResponse.error["code"] = constants::ERR_DB_NOT_FOUND;
      errorResponse.error["message"] =
          "Ticket not found with the provided payment reference.";
      co_return errorResponse;
    }

    auto ticket = tickets[0];

    // 2. Check if already active (idempotency)
    if (ticket.getValueOfIsActive() &&
        ticket.getValueOfPaymentStatus() == "PAID") {
      dto::BaseApiResponse response;
      response.success = true;
      response.message = "Ticket is already active.";
      response.result["ticketId"] = ticket.getValueOfId();
      co_return response;
    }

    // 3. Update the ticket
    ticket.setPaymentStatus("PAID");
    ticket.setIsActive(true);
    ticket.setUpdatedAt(trantor::Date::now());
    co_await ticketMapper.update(ticket);

    // 4. Update the associated Raffle
    auto raffleId = ticket.getValueOfRaffleId();
    try {
      auto raffle = co_await raffleMapper.findByPrimaryKey(raffleId);
      int currentEntries = raffle.getValueOfTotalEntries();
      raffle.setTotalEntries(currentEntries + ticket.getValueOfQuantity());

      raffle.setUpdatedAt(trantor::Date::now());

      co_await raffleMapper.update(raffle);
    } catch (const std::exception &e) {
      LOG_ERROR << "Failed to update associated raffle for ticket "
                << ticket.getValueOfId() << ": " << e.what();
    }

    // 5. Update the associated Draw
    auto drawId = ticket.getValueOfDrawId();
    try {
      auto draw = co_await drawMapper.findByPrimaryKey(drawId);
      int currentEntries = draw.getValueOfTotalEntries();
      draw.setTotalEntries(currentEntries + ticket.getValueOfQuantity());

      std::string currentPoolStr = draw.getValueOfTotalPool();
      double currentPool = 0;
      if (!currentPoolStr.empty()) {
        try {
          currentPool = std::stod(currentPoolStr);
        } catch (...) {
          currentPool = 0;
        }
      }
      double ticketAmount = std::stod(ticket.getValueOfTotalAmount());
      currentPool += ticketAmount;

      char buffer[64];
      snprintf(buffer, sizeof(buffer), "%.2f", currentPool);
      draw.setTotalPool(std::string(buffer));
      draw.setUpdatedAt(trantor::Date::now());

      co_await drawMapper.update(draw);
    } catch (const std::exception &e) {
      LOG_ERROR << "Failed to update associated draw for ticket "
                << ticket.getValueOfId() << ": " << e.what();
    }

    // 6. Update the associated Customer
    auto customerId = ticket.getValueOfCustomerId();
    if (!customerId.empty()) {
      try {
        CoroMapper<Customers> customerMapper(dbClient);
        auto customer = co_await customerMapper.findByPrimaryKey(customerId);

        // Update number of entries
        int32_t currentEntries = customer.getValueOfNumberOfEntries();
        customer.setNumberOfEntries(currentEntries +
                                    ticket.getValueOfQuantity());

        // Update total spent
        double currentSpent = 0;
        std::string currentSpentStr = customer.getValueOfTotalSpent();
        if (!currentSpentStr.empty()) {
          try {
            currentSpent = std::stod(currentSpentStr);
          } catch (...) {
            currentSpent = 0;
          }
        }
        double ticketAmount = std::stod(ticket.getValueOfTotalAmount());
        currentSpent += ticketAmount;

        char spentBuffer[64];
        snprintf(spentBuffer, sizeof(spentBuffer), "%.2f", currentSpent);
        customer.setTotalSpent(std::string(spentBuffer));

        customer.setUpdatedAt(trantor::Date::now());

        co_await customerMapper.update(customer);
      } catch (const std::exception &e) {
        LOG_ERROR << "Failed to update associated customer for ticket "
                  << ticket.getValueOfId() << ": " << e.what();
      }
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Ticket activated successfully.";
    response.result["ticketId"] = ticket.getValueOfId();
    co_return response;

  } catch (const drogon::orm::DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while activating ticket.";
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
