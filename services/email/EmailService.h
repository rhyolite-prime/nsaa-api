//
// Created by Emmanuel Addo-Odame on 29/04/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_EMAILSERVICE_H
#define CHOPMONEYRAFFLEAPI_EMAILSERVICE_H

#include "dto/SendEmailDto.h"
#include <drogon/drogon.h>
#include "dto/BaseApiResponse.h"

namespace raffle::services {

    class EmailService {

    public:

        drogon::Task<dto::BaseApiResponse> sendEmail(const dto::SendEmailDto &dto);

        drogon::Task<dto::BaseApiResponse> sendOtpEmail(const dto::SendEmailDto &dto);
    };

}

#endif //CHOPMONEYRAFFLEAPI_EMAILSERVICE_H