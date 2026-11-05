//
// Created by Emmanuel Addo-Odame on 05/11/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CLOUDINARYAPI_H
#define CHOPMONEYRAFFLEAPI_CLOUDINARYAPI_H
#include <drogon/utils/coroutine.h>

#include "dto/BaseApiResponse.h"

namespace raffle::services {

    class CloudinaryApi {

        drogon::Task<std::string> uploadFile();

    };


}
#endif //CHOPMONEYRAFFLEAPI_CLOUDINARYAPI_H
