//
// Created by Emmanuel Addo-Odame on 01/05/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_AFFILIATEPARTNERSERVICE_H
#define CHOPMONEYRAFFLEAPI_AFFILIATEPARTNERSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>
#include <drogon/utils/coroutine.h>

#include "dto/AffiliatePartnerDraftDto.h"
#include "dto/AffiliatePartnerDto.h"

namespace raffle::services {

 class AffiliatePartnerService {

 public:
  drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

  drogon::Task<dto::BaseApiResponse> getPartnerStats();

  drogon::Task<dto::BaseApiResponse> create(const dto::AffiliatePartnerDto &dto);

  drogon::Task<dto::BaseApiResponse> createDraft(const dto::AffiliatePartnerDraftDto &dto);

  drogon::Task<dto::BaseApiResponse> update(const dto::AffiliatePartnerDto &dto, const std::string &partnerId);

  drogon::Task<dto::BaseApiResponse> deletePartner(const std::string &partnerId);

 };

}
#endif //CHOPMONEYRAFFLEAPI_AFFILIATEPARTNERSERVICE_H