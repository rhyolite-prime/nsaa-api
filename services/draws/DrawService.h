//
// Created by Emmanuel Addo-Odame on 26/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_DRAWSERVICE_H
#define CHOPMONEYRAFFLEAPI_DRAWSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

namespace raffle::services {

    class DrawService {
    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);
        //drogon::Task<dto::BaseApiResponse> executeDraw();


    };

}

#endif //CHOPMONEYRAFFLEAPI_DRAWSERVICE_H