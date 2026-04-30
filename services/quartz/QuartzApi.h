//
// Created by Emmanuel Addo-Odame on 06/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_QUARTZAPI_H
#define CHOPMONEYRAFFLEAPI_QUARTZAPI_H

#include "dto/QuartzJobDto.h"
#include <drogon/drogon.h>

namespace raffle::services {

    class QuartzApi {
    public:
        drogon::Task<bool> scheduleJob(const dto::QuartzJobDto &dto);
    };

}
#endif //CHOPMONEYRAFFLEAPI_QUARTZAPI_H