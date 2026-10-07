//
// Created by Emmanuel Addo-Odame on 02/06/2026.
//

#include "SupportTicketService.h"
#include "SupportTickets.h"
#include "constants/ErrorCodes.h"
#include "dto/BaseApiResponse.h"
#include "dto/SupportTicketDto.h"
#include "utils/IdGeneratorUtils.h"
#include <drogon/orm/CoroMapper.h>
#include <trantor/utils/Date.h>

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;

namespace raffle::services {

drogon::Task<dto::BaseApiResponse>
SupportTicketService::create(const dto::SupportTicketDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<SupportTickets> mapper(dbClient);

  try {

    SupportTickets ticket;
    ticket.setUserName(dto.getName());

    if (!dto.getSubject().empty()) {
      ticket.setSubject(dto.getSubject());
    } else {
      ticket.setSubject("Support Ticket");
    }

    ticket.setTicketNumber(utils::IdGeneratorUtils::generateRandomSixDigit());

    if (!dto.getCategory().empty()) {
      ticket.setCategory(dto.getCategory());
    } else {
      ticket.setCategory("General");
    }

    if (!dto.getEmail().empty()) {
      ticket.setUserEmail(dto.getEmail());
    }

    if (!dto.getPhoneNumber().empty()) {
      ticket.setUserPhone(dto.getPhoneNumber());
    }

    if (!dto.getSource().empty()) {
      ticket.setSource(dto.getSource());
    } else {
      ticket.setSource("Web");
    }

    ticket.setMessage(dto.getMessage());
    ticket.setCreatedAt(trantor::Date::now());

    auto result = co_await mapper.insert(ticket);

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Support ticket created successfully";
    response.result["ticketNumber"] = result.getValueOfTicketNumber();
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] =
        "Database error while creating support ticket.";
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