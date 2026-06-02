//
// Created by Emmanuel Addo-Odame on 01/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_MEDIALIBRARYSERVICE_H
#define CHOPMONEYRAFFLEAPI_MEDIALIBRARYSERVICE_H
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>
#include "dto/BaseApiResponse.h"
#include "dto/MediaAssetDto.h"

namespace raffle::services {

    class MediaLibraryService {
    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> create(const dto::MediaAssetDto &dto);

        drogon::Task<dto::BaseApiResponse> update(const dto::MediaAssetDto &dto, const std::string &id);

        drogon::Task<dto::BaseApiResponse> deleteMediaAsset(const std::string &id);

    };
}

#endif //CHOPMONEYRAFFLEAPI_MEDIALIBRARYSERVICE_H