//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_TICKETSERVICE_H
#define CHOPMONEYRAFFLEAPI_TICKETSERVICE_H

#include "dto/BaseApiResponse.h"
#include "dto/CreateTicketDto.h"
#include <drogon/drogon.h>
#include <string>

namespace raffle::services {

class TicketService {
public:
  drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize,const std::string &query);

  drogon::Task<dto::BaseApiResponse> activate(const std::string &paymentReference);

  drogon::Task<dto::BaseApiResponse> create(const dto::CreateTicketDto &dto);
};

} // namespace raffle::services

#endif // CHOPMONEYRAFFLEAPI_TICKETSERVICE_H