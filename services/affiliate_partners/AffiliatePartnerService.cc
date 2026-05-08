//
// Created by Emmanuel Addo-Odame on 07/05/2026.
//
#include "AffiliatePartnerService.h"
#include "constants/ErrorCodes.h"
#include "models/Colours.h"
#include <drogon/orm/CoroMapper.h>
#include <drogon/orm/Mapper.h>

#include "AffiliatePartners.h"
#include <drogon/utils/Utilities.h>
#include "utils/DatabaseUtils.h"

using namespace drogon::orm;
using namespace drogon_model::ChopMoneyRaffle;


namespace raffle::services {


drogon::Task<dto::BaseApiResponse> AffiliatePartnerService::getAll(int pageNo, int pageSize, const std::string &query) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<AffiliatePartners> mapper(dbClient);

  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria =
        Criteria(AffiliatePartners::Cols::_name, CompareOperator::Like, likeQuery) ||
        Criteria(AffiliatePartners::Cols::_identifier, CompareOperator::Like, likeQuery) ||
        Criteria(AffiliatePartners::Cols::_contact_email, CompareOperator::Like, likeQuery) ||
        Criteria(AffiliatePartners::Cols::_contact_phone, CompareOperator::Like, likeQuery) ||
        Criteria(AffiliatePartners::Cols::_legal_entity_name, CompareOperator::Like, likeQuery) ||
        Criteria(AffiliatePartners::Cols::_contact_name, CompareOperator::Like, likeQuery);
  }

  try {
    auto totalCount = co_await mapper.count(searchCriteria);

    if (totalCount == 0) {
      dto::BaseApiResponse response;
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    int offset = (pageNo - 1) * pageSize;
    auto affiliatePartners = co_await mapper.orderBy(Colours::Cols::_created_at, SortOrder::DESC)
            .limit(pageSize)
            .offset(offset)
            .findBy(searchCriteria);

    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    dto::BaseApiResponse response;
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] = (int)totalPages;
    response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] = (int)totalPages == pageNo  ? (Json::UInt64)totalCount : (Json::UInt64)(pageNo * pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &affiliatePartner : affiliatePartners) {
      Json::Value j = affiliatePartner.toJson();
      Json::Value item;
      item["id"] = j["id"];
      item["name"] = j["name"];
      item["identifier"] = j["identifier"];
      item["contact_name"] = j["contact_name"];
      item["contactEmail"] = j["contact_email"];
      item["contactPhone"] = j["contact_phone"];
      item["billingEmail"] = j["billing_email"];
      item["country"] = j["country"];
      item["currency"] = j["currency"];
      item["status"] = j["status"];
      item["ussdCode"] = j["ussd_code"];
      item["wssdServiceUrl"] = j["wssd_service_url"];
      item["subAccountEnabled"] = j["sub_account_enabled"];
      item["requireTwoFactorAuth"] = j["require_two_factor_auth"];
      item["organizationLogo"] = j["organization_logo"];
      item["commissionRate"] = j["commission_rate"];
      item["commissionType"] = j["commission_type"];
      item["minimumPayoutThreshold"] = j["minimum_payout_threshold"];
      item["paymentMethod"] = j["payment_method"];

      // Handle settlement_details parsing
      std::string settlementDetailsStr = j["settlement_details"].asString();
      if (!settlementDetailsStr.empty()) {
          Json::Value settlementDetailsJson;
          Json::Reader reader;
          if (reader.parse(settlementDetailsStr, settlementDetailsJson)) {
              item["settlementDetails"] = settlementDetailsJson;
          } else {
              item["settlementDetails"] = settlementDetailsStr; // Fallback to string if parsing fails
          }
      } else {
          item["settlementDetails"] = Json::objectValue;
      }
      item["maxRafflesPerDay"] = j["max_raffles_per_day"];
      item["requiresApproval"] = j["requires_approval"];
      item["totalTicketsSold"] = j["total_tickets_sold"];
      item["totalRevenueGenerated"] = j["total_revenue_generated"];
      item["totalCommissionEarned"] = j["total_commission_earned"];
      item["lastPayoutDate"] = j["last_payout_date"];
      item["ipWhitelist"] = j["ip_whitelist"];
      item["taxId"] = j["tax_id"];
      item["legalEntityName"] = j["legal_entity_name"];
      item["kycVerified"] = j["kyc_verified"];
      item["agreementAcceptedAt"] = j["agreement_accepted_at"];
      item["agreementVersion"] = j["agreement_version"];
      item["supportPriority"] = j["support_priority"];
      item["notes"] = j["notes"];
      item["createdAt"] = j["created_at"];
      data.append(item);
    }
    response.result["data"] = data;
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching affiliate partners.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}


drogon::Task<dto::BaseApiResponse> AffiliatePartnerService::getPartnerStats() {

  auto dbClient = drogon::app().getDbClient();

  try {
    std::string sql = "SELECT "
                      "COUNT(*) as total, "
                      "COUNT(*) FILTER (WHERE kyc_verified = true) as verified, "
                      "COUNT(*) FILTER (WHERE status = 'active') as active, "
                      "COALESCE(SUM(total_commission_earned::numeric), 0) as commission "
                      "FROM affiliate_partners";

    auto result = co_await dbClient->execSqlCoro(sql);

    dto::BaseApiResponse response;
    response.success = true;

    if (!result.empty()) {
      auto row = result[0];
      long long totalCount = row["total"].as<long long>();
      long long verifiedCount = row["verified"].as<long long>();
      long long activeCount = row["active"].as<long long>();
      double totalEarned = row["commission"].as<double>();

      double complianceRate = 0.0;
      if (totalCount > 0) {
        complianceRate = (static_cast<double>(verifiedCount) / static_cast<double>(totalCount)) * 100.0;
      }

      response.result["verifiedPartners"] = verifiedCount;
      response.result["activeStatus"] = activeCount;
      response.result["totalEarned"] = totalEarned;
      response.result["complianceRate"] = std::round(complianceRate); // Rounding to match the UI style (e.g., 94%)
    } else {
      response.result["verifiedPartners"] = 0;
      response.result["activeStatus"] = 0;
      response.result["totalEarned"] = 0.0;
      response.result["complianceRate"] = 0.0;
    }

    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while fetching partner stats.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}

drogon::Task<dto::BaseApiResponse> AffiliatePartnerService::create(const dto::AffiliatePartnerDto &dto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<AffiliatePartners> mapper(dbClient);

  try {
    AffiliatePartners newAffiliatePartner;
    newAffiliatePartner.setName(dto.getName());

    // Set identifier (if provided, otherwise DB will auto-generate)
    if (!dto.getIdentifier().empty()) {
        newAffiliatePartner.setIdentifier(dto.getIdentifier());
    }

    // Set contact information
    if (!dto.getContactName().empty()) {
        newAffiliatePartner.setContactName(dto.getContactName());
    }
    if (!dto.getContactEmail().empty()) {
        newAffiliatePartner.setContactEmail(dto.getContactEmail());
    }
    if (!dto.getContactPhone().empty()) {
        newAffiliatePartner.setContactPhone(dto.getContactPhone());
    }
    if (!dto.getBillingEmail().empty()) {
        newAffiliatePartner.setBillingEmail(dto.getBillingEmail());
    }

    // Set location and currency (with defaults if not provided)
    newAffiliatePartner.setCountry(dto.getCountry().empty() ? "Ghana" : dto.getCountry());
    newAffiliatePartner.setCurrency(dto.getCurrency().empty() ? "GHS" : dto.getCurrency());

    // Set status (default to 'active' if not provided)
    newAffiliatePartner.setStatus(dto.getStatus().empty() ? "active" : dto.getStatus());

    // Set USSD and WSSD configuration
    if (!dto.getUssdCode().empty()) {
        newAffiliatePartner.setUssdCode(dto.getUssdCode());
    }
    if (!dto.getWssdServiceUrl().empty()) {
        newAffiliatePartner.setWssdServiceUrl(dto.getWssdServiceUrl());
    }

    // Set feature flags
    newAffiliatePartner.setSubAccountEnabled(dto.isSubAccountEnabled());
    newAffiliatePartner.setRequireTwoFactorAuth(dto.isRequireTwoFactorAuth());

    // Set commission configuration
    if (dto.getCommissionRate() > 0) {
        newAffiliatePartner.setCommissionRate(utils::DatabaseUtils::formatDecimal(dto.getCommissionRate()));
    }
    if (!dto.getCommissionType().empty()) {
        newAffiliatePartner.setCommissionType(dto.getCommissionType());
    }
    if (dto.getPaymentDueDate() > 0) {
        newAffiliatePartner.setPaymentDueDate(dto.getPaymentDueDate());
    }
    if (dto.getMinimumPayoutThreshold() > 0) {
        newAffiliatePartner.setMinimumPayoutThreshold(utils::DatabaseUtils::formatDecimal(dto.getMinimumPayoutThreshold()));
    }

    // Set payment configuration
    if (!dto.getPaymentMethod().empty()) {
        newAffiliatePartner.setPaymentMethod(dto.getPaymentMethod());
    }
    if (!dto.getSettlementDetails().isNull()) {
        newAffiliatePartner.setSettlementDetails(dto.getSettlementDetails().toStyledString());
    }

    // Set raffle limits and categories
    if (dto.getMaxRafflesPerDay() > 0) {
        newAffiliatePartner.setMaxRafflesPerDay(dto.getMaxRafflesPerDay());
    }

    if (!dto.getAllowedRaffleCategories().empty()) {
        newAffiliatePartner.setAllowedRaffleCategories(utils::DatabaseUtils::vectorToPgArray(dto.getAllowedRaffleCategories()));
    }

    newAffiliatePartner.setRequiresApproval(dto.isRequiresApproval());

    // Initialize financial counters (always start at zero)
    newAffiliatePartner.setTotalTicketsSold(0);
    newAffiliatePartner.setTotalRevenueGenerated("0.00");
    newAffiliatePartner.setTotalCommissionEarned("0.00");

    // Set API credentials if provided
    if (!dto.getApiKey().empty()) {
        newAffiliatePartner.setApiKey(dto.getApiKey());
    }

    if (!dto.getApiSecret().empty()) {
        newAffiliatePartner.setApiSecret(dto.getApiSecret());
    }

    // Set IP whitelist if provided
    if (!dto.getIpWhitelist().empty()) {
        newAffiliatePartner.setIpWhitelist(utils::DatabaseUtils::vectorToPgArray(dto.getIpWhitelist()));
    }

    // Set webhook configuration
    if (!dto.getWebhookSecret().empty()) {
        newAffiliatePartner.setWebhookSecret(dto.getWebhookSecret());
    }

    // Set legal and KYC information
    if (!dto.getTaxId().empty()) {
        newAffiliatePartner.setTaxId(dto.getTaxId());
    }
    if (!dto.getLegalEntityName().empty()) {
        newAffiliatePartner.setLegalEntityName(dto.getLegalEntityName());
    }

    newAffiliatePartner.setKycVerified(dto.isKycVerified());

    // Set agreement information
    if (!dto.getAgreementAcceptedAt().empty()) {
        newAffiliatePartner.setAgreementAcceptedAt(trantor::Date::fromDbStringLocal(dto.getAgreementAcceptedAt()));
    }

    if (!dto.getAgreementVersion().empty()) {
        newAffiliatePartner.setAgreementVersion(dto.getAgreementVersion());
    }

    // Set support and notes
    if (!dto.getSupportPriority().empty()) {
        newAffiliatePartner.setSupportPriority(dto.getSupportPriority());
    }
    
    if (!dto.getNotes().empty()) {
        newAffiliatePartner.setNotes(dto.getNotes());
    }

    auto result = co_await mapper.insert(newAffiliatePartner);

    //send email to partner congratulating them about their onboarding
    //generating login credentials and sending alongside the same mail

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Affiliate partner created successfully";
    response.result["id"] = result.getValueOfId();
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while creating affiliate partner.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }

}


drogon::Task<dto::BaseApiResponse> AffiliatePartnerService::update(const dto::AffiliatePartnerDto &dto, const std::string &partnerId) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<AffiliatePartners> mapper(dbClient);

  try {
    auto partner = co_await mapper.findByPrimaryKey(partnerId);

    if (!dto.getName().empty()) {
        partner.setName(dto.getName());
    }
    if (!dto.getIdentifier().empty()) {
        partner.setIdentifier(dto.getIdentifier());
    }
    if (!dto.getContactName().empty()) {
        partner.setContactName(dto.getContactName());
    }
    if (!dto.getContactEmail().empty()) {
        partner.setContactEmail(dto.getContactEmail());
    }
    if (!dto.getContactPhone().empty()) {
        partner.setContactPhone(dto.getContactPhone());
    }
    if (!dto.getBillingEmail().empty()) {
        partner.setBillingEmail(dto.getBillingEmail());
    }
    if (!dto.getCountry().empty()) {
        partner.setCountry(dto.getCountry());
    }
    if (!dto.getCurrency().empty()) {
        partner.setCurrency(dto.getCurrency());
    }
    if (!dto.getStatus().empty()) {
        partner.setStatus(dto.getStatus());
    }
    if (!dto.getUssdCode().empty()) {
        partner.setUssdCode(dto.getUssdCode());
    }
    if (!dto.getWssdServiceUrl().empty()) {
        partner.setWssdServiceUrl(dto.getWssdServiceUrl());
    }

    partner.setSubAccountEnabled(dto.isSubAccountEnabled());
    partner.setRequireTwoFactorAuth(dto.isRequireTwoFactorAuth());

    if (dto.getCommissionRate() > 0) {
        partner.setCommissionRate(utils::DatabaseUtils::formatDecimal(dto.getCommissionRate()));
    }
    if (!dto.getCommissionType().empty()) {
        partner.setCommissionType(dto.getCommissionType());
    }
    if (dto.getPaymentDueDate() > 0) {
        partner.setPaymentDueDate(dto.getPaymentDueDate());
    }
    if (dto.getMinimumPayoutThreshold() > 0) {
        partner.setMinimumPayoutThreshold(utils::DatabaseUtils::formatDecimal(dto.getMinimumPayoutThreshold()));
    }
    if (!dto.getPaymentMethod().empty()) {
        partner.setPaymentMethod(dto.getPaymentMethod());
    }
    if (!dto.getSettlementDetails().isNull()) {
        partner.setSettlementDetails(dto.getSettlementDetails().toStyledString());
    }
    if (dto.getMaxRafflesPerDay() > 0) {
        partner.setMaxRafflesPerDay(dto.getMaxRafflesPerDay());
    }
    if (!dto.getAllowedRaffleCategories().empty()) {
        partner.setAllowedRaffleCategories(utils::DatabaseUtils::vectorToPgArray(dto.getAllowedRaffleCategories()));
    }

    partner.setRequiresApproval(dto.isRequiresApproval());

    if (!dto.getApiKey().empty()) {
        partner.setApiKey(dto.getApiKey());
    }
    if (!dto.getApiSecret().empty()) {
        partner.setApiSecret(dto.getApiSecret());
    }
    if (!dto.getIpWhitelist().empty()) {
        partner.setIpWhitelist(utils::DatabaseUtils::vectorToPgArray(dto.getIpWhitelist()));
    }
    if (!dto.getWebhookSecret().empty()) {
        partner.setWebhookSecret(dto.getWebhookSecret());
    }
    if (!dto.getTaxId().empty()) {
        partner.setTaxId(dto.getTaxId());
    }
    if (!dto.getLegalEntityName().empty()) {
        partner.setLegalEntityName(dto.getLegalEntityName());
    }
    partner.setKycVerified(dto.isKycVerified());

    if (!dto.getAgreementAcceptedAt().empty()) {
        partner.setAgreementAcceptedAt(trantor::Date::fromDbStringLocal(dto.getAgreementAcceptedAt()));
    }
    if (!dto.getAgreementVersion().empty()) {
        partner.setAgreementVersion(dto.getAgreementVersion());
    }
    if (!dto.getSupportPriority().empty()) {
        partner.setSupportPriority(dto.getSupportPriority());
    }
    if (!dto.getNotes().empty()) {
        partner.setNotes(dto.getNotes());
    }

    co_await mapper.update(partner);

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Affiliate partner updated successfully";
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while updating affiliate partner.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}


drogon::Task<dto::BaseApiResponse> AffiliatePartnerService::deletePartner(const std::string &partnerId) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<AffiliatePartners> mapper(dbClient);

  try {
    auto deletedCount = co_await mapper.deleteByPrimaryKey(partnerId);

    if (deletedCount == 0) {
        dto::BaseApiResponse errorResponse;
        errorResponse.success = false;
        errorResponse.error["code"] = constants::ERR_DB_NOT_FOUND;
        errorResponse.error["message"] = "Affiliate partner not found.";
        co_return errorResponse;
    }

    dto::BaseApiResponse response;
    response.success = true;
    response.message = "Affiliate partner deleted successfully";
    co_return response;

  } catch (const DrogonDbException &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_DB_QUERY;
    errorResponse.error["message"] = "Database error while deleting affiliate partner.";
    errorResponse.error["detail"] = e.base().what();
    co_return errorResponse;
  } catch (const std::exception &e) {
    dto::BaseApiResponse errorResponse;
    errorResponse.success = false;
    errorResponse.error["code"] = constants::ERR_INTERNAL;
    errorResponse.error["message"] = "Internal error.";
    errorResponse.error["detail"] = e.what();
    co_return errorResponse;
  }
}


}