//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_ROLESERVICE_H
#define CHOPMONEYRAFFLEAPI_ROLESERVICE_H

#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/CreateRoleDto.h"
#include "dto/UpdateRoleDto.h"

namespace raffle::services {

    class RoleService {
    public:
        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> createAsync(const dto::CreateRoleDto &dto);

        drogon::Task<dto::BaseApiResponse> updateAsync(const dto::UpdateRoleDto &dto);

        drogon::Task<dto::BaseApiResponse> deleteRole(const std::string &roleId);

    };

}

#endif //CHOPMONEYRAFFLEAPI_ROLESERVICE_H