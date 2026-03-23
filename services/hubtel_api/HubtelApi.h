//
// Created by Emmanuel Addo-Odame on 05/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_HUBTELAPI_H
#define CHOPMONEYRAFFLEAPI_HUBTELAPI_H

#include <drogon/drogon.h>
#include "dto/BaseApiResponse.h"
#include <string>

#include "dto/DirectReceiveMoneyDto.h"
#include "dto/WebCheckoutDto.h"
#include "dto/CheckoutResponse.h"

namespace raffle::services {

    class HubtelApi {

    public:

        drogon::Task<dto::CheckoutResponse> InitializeWebCheckout(const dto::WebCheckoutDto &dto);

        drogon::Task<dto::BaseApiResponse> WebCheckoutStatusCheck(const std::string &paymentReference);

        drogon::Task<dto::BaseApiResponse> InitializeDirectReceiveMoney(const dto::DirectReceiveMoneyDto &dto);
    };

}
#endif //CHOPMONEYRAFFLEAPI_HUBTELAPI_H