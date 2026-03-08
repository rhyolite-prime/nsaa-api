//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_USERSERVICE_H
#define CHOPMONEYRAFFLEAPI_USERSERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/CreateUserDto.h"
#include "dto/UpdateUserDto.h"

namespace raffle::services {

    class UserService {
    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> getStats();

        drogon::Task<dto::BaseApiResponse> createAsync(const dto::CreateUserDto &dto);

        drogon::Task<dto::BaseApiResponse> updateAsync(const dto::UpdateUserDto &dto);

        drogon::Task<dto::BaseApiResponse> deleteUser(const std::string &userId);



    };

}
#endif //CHOPMONEYRAFFLEAPI_USERSERVICE_H