//
// Created by Emmanuel Addo-Odame on 07/05/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_AFFILIATEPARTNERDTO_H
#define CHOPMONEYRAFFLEAPI_AFFILIATEPARTNERDTO_H

#include <json/json.h>

namespace raffle::dto {

    class AffiliatePartnerDto {

    public:

        AffiliatePartnerDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getIdentifier() const { return identifier_; }
        [[nodiscard]] const std::string& getContactName() const { return contact_name_; }
        [[nodiscard]] const std::string& getContactEmail() const { return contact_email_; }
        [[nodiscard]] const std::string& getContactPhone() const { return contact_phone_; }
        [[nodiscard]] const std::string& getBillingEmail() const { return billing_email_; }
        [[nodiscard]] const std::string& getCountry() const { return country_; }
        [[nodiscard]] const std::string& getCurrency() const { return currency_; }
        [[nodiscard]] const std::string& getStatus() const { return status_; }
        [[nodiscard]] const std::string& getUssdCode() const { return ussd_code_; }
        [[nodiscard]] const std::string& getWssdServiceUrl() const { return wssd_service_url_; }
        [[nodiscard]] bool isSubAccountEnabled() const { return sub_account_enabled_; }
        [[nodiscard]] bool isRequireTwoFactorAuth() const { return require_two_factor_auth_; }
        [[nodiscard]] double getCommissionRate() const { return commission_rate_; }
        [[nodiscard]] const std::string& getCommissionType() const { return commission_type_; }
        [[nodiscard]] int getPaymentDueDate() const { return payment_due_date_; }
        [[nodiscard]] double getMinimumPayoutThreshold() const { return minimum_payout_threshold_; }
        [[nodiscard]] const std::string& getPaymentMethod() const { return payment_method_; }
        [[nodiscard]] const Json::Value& getSettlementDetails() const { return settlement_details_; }
        [[nodiscard]] int getMaxRafflesPerDay() const { return max_raffles_per_day_; }
        [[nodiscard]] const std::vector<std::string>& getAllowedRaffleCategories() const { return allowed_raffle_categories_; }
        [[nodiscard]] bool isRequiresApproval() const { return requires_approval_; }
        [[nodiscard]] int getTotalTicketsSold() const { return total_tickets_sold_; }
        [[nodiscard]] double getTotalRevenueGenerated() const { return total_revenue_generated_; }
        [[nodiscard]] double getTotalCommissionEarned() const { return total_commission_earned_; }
        [[nodiscard]] const std::string& getLastPayoutDate() const { return last_payout_date_; }
        [[nodiscard]] const std::string& getApiKey() const { return api_key_; }
        [[nodiscard]] const std::string& getApiSecret() const { return api_secret_; }
        [[nodiscard]] const std::vector<std::string>& getIpWhitelist() const { return ip_whitelist_; }
        [[nodiscard]] const std::string& getWebhookSecret() const { return webhook_secret_; }
        [[nodiscard]] const std::string& getTaxId() const { return tax_id_; }
        [[nodiscard]] const std::string& getLegalEntityName() const { return legal_entity_name_; }
        [[nodiscard]] bool isKycVerified() const { return kyc_verified_; }
        [[nodiscard]] const std::string& getAgreementAcceptedAt() const { return agreement_accepted_at_; }
        [[nodiscard]] const std::string& getAgreementVersion() const { return agreement_version_; }
        [[nodiscard]] const std::string& getSupportPriority() const { return support_priority_; }
        [[nodiscard]] const std::string& getNotes() const { return notes_; }
        [[nodiscard]] const std::string& getCreatedAt() const { return created_at_; }
        [[nodiscard]] const std::string& getUpdatedAt() const { return updated_at_; }

        // Setters
        void setName(const std::string& value) { name_ = value; }
        void setIdentifier(const std::string& value) { identifier_ = value; }
        void setContactName(const std::string& value) { contact_name_ = value; }
        void setContactEmail(const std::string& value) { contact_email_ = value; }
        void setContactPhone(const std::string& value) { contact_phone_ = value; }
        void setBillingEmail(const std::string& value) { billing_email_ = value; }
        void setCountry(const std::string& value) { country_ = value; }
        void setCurrency(const std::string& value) { currency_ = value; }
        void setStatus(const std::string& value) { status_ = value; }
        void setUssdCode(const std::string& value) { ussd_code_ = value; }
        void setWssdServiceUrl(const std::string& value) { wssd_service_url_ = value; }
        void setSubAccountEnabled(bool value) { sub_account_enabled_ = value; }
        void setRequireTwoFactorAuth(bool value) { require_two_factor_auth_ = value; }
        void setCommissionRate(double value) { commission_rate_ = value; }
        void setCommissionType(const std::string& value) { commission_type_ = value; }
        void setPaymentDueDate(int value) { payment_due_date_ = value; }
        void setMinimumPayoutThreshold(double value) { minimum_payout_threshold_ = value; }
        void setPaymentMethod(const std::string& value) { payment_method_ = value; }
        void setSettlementDetails(const Json::Value& value) { settlement_details_ = value; }
        void setMaxRafflesPerDay(int value) { max_raffles_per_day_ = value; }
        void setAllowedRaffleCategories(const std::vector<std::string>& value) { allowed_raffle_categories_ = value; }
        void setRequiresApproval(bool value) { requires_approval_ = value; }
        void setTotalTicketsSold(int value) { total_tickets_sold_ = value; }
        void setTotalRevenueGenerated(double value) { total_revenue_generated_ = value; }
        void setTotalCommissionEarned(double value) { total_commission_earned_ = value; }
        void setLastPayoutDate(const std::string& value) { last_payout_date_ = value; }
        void setApiKey(const std::string& value) { api_key_ = value; }
        void setApiSecret(const std::string& value) { api_secret_ = value; }
        void setIpWhitelist(const std::vector<std::string>& value) { ip_whitelist_ = value; }
        void setWebhookSecret(const std::string& value) { webhook_secret_ = value; }
        void setTaxId(const std::string& value) { tax_id_ = value; }
        void setLegalEntityName(const std::string& value) { legal_entity_name_ = value; }
        void setKycVerified(bool value) { kyc_verified_ = value; }
        void setAgreementAcceptedAt(const std::string& value) { agreement_accepted_at_ = value; }
        void setAgreementVersion(const std::string& value) { agreement_version_ = value; }
        void setSupportPriority(const std::string& value) { support_priority_ = value; }
        void setNotes(const std::string& value) { notes_ = value; }
        void setCreatedAt(const std::string& value) { created_at_ = value; }
        void setUpdatedAt(const std::string& value) { updated_at_ = value; }

    private:

        std::string name_;
        std::string identifier_;
        std::string contact_name_;
        std::string contact_email_;
        std::string contact_phone_;
        std::string billing_email_;
        std::string country_ = "Ghana";
        std::string currency_ = "GHS";
        std::string status_ = "active";
        std::string ussd_code_;
        std::string wssd_service_url_;
        bool sub_account_enabled_ = true;
        bool require_two_factor_auth_ = false;
        double commission_rate_ = 0.00;
        std::string commission_type_ = "percentage";
        int payment_due_date_ = 0;
        double minimum_payout_threshold_ = 100.00;
        std::string payment_method_;
        Json::Value settlement_details_;
        int max_raffles_per_day_ = 0;
        std::vector<std::string> allowed_raffle_categories_;
        bool requires_approval_ = true;
        int total_tickets_sold_ = 0;
        double total_revenue_generated_ = 0.00;
        double total_commission_earned_ = 0.00;
        std::string last_payout_date_;
        std::string api_key_;
        std::string api_secret_;
        std::vector<std::string> ip_whitelist_;
        std::string webhook_secret_;
        std::string tax_id_;
        std::string legal_entity_name_;
        bool kyc_verified_ = false;
        std::string agreement_accepted_at_;
        std::string agreement_version_;
        std::string support_priority_ = "normal";
        std::string notes_;
        std::string created_at_;
        std::string updated_at_;

    };

    inline void AffiliatePartnerDto::fromJson(const Json::Value& json) {

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }
        if (json.isMember("identifier") && !json["identifier"].isNull()) {
            identifier_ = json["identifier"].asString();
        }
        if (json.isMember("contactName") && !json["contactName"].isNull()) {
            contact_name_ = json["contactName"].asString();
        }
        if (json.isMember("contactEmail") && !json["contactEmail"].isNull()) {
            contact_email_ = json["contactEmail"].asString();
        }
        if (json.isMember("contactPhone") && !json["contactPhone"].isNull()) {
            contact_phone_ = json["contactPhone"].asString();
        }
        if (json.isMember("billingEmail") && !json["billingEmail"].isNull()) {
            billing_email_ = json["billingEmail"].asString();
        }
        if (json.isMember("country") && !json["country"].isNull()) {
            country_ = json["country"].asString();
        }
        if (json.isMember("currency") && !json["currency"].isNull()) {
            currency_ = json["currency"].asString();
        }
        if (json.isMember("status") && !json["status"].isNull()) {
            status_ = json["status"].asString();
        }
        if (json.isMember("ussdCode") && !json["ussdCode"].isNull()) {
            ussd_code_ = json["ussdCode"].asString();
        }
        if (json.isMember("wssdServiceUrl") && !json["wssdServiceUrl"].isNull()) {
            wssd_service_url_ = json["wssdServiceUrl"].asString();
        }
        if (json.isMember("subAccountEnabled") && !json["subAccountEnabled"].isNull()) {
            sub_account_enabled_ = json["subAccountEnabled"].asBool();
        }
        if (json.isMember("requireTwoFactorAuth") && !json["requireTwoFactorAuth"].isNull()) {
            require_two_factor_auth_ = json["requireTwoFactorAuth"].asBool();
        }
        if (json.isMember("commissionRate") && !json["commissionRate"].isNull()) {
            commission_rate_ = json["commissionRate"].asDouble();
        }
        if (json.isMember("commissionType") && !json["commissionType"].isNull()) {
            commission_type_ = json["commissionType"].asString();
        }
        if (json.isMember("paymentDueDate") && !json["paymentDueDate"].isNull()) {
            payment_due_date_ = json["paymentDueDate"].asInt();
        }
        if (json.isMember("minimumPayoutThreshold") && !json["minimumPayoutThreshold"].isNull()) {
            minimum_payout_threshold_ = json["minimumPayoutThreshold"].asDouble();
        }
        if (json.isMember("paymentMethod") && !json["paymentMethod"].isNull()) {
            payment_method_ = json["paymentMethod"].asString();
        }
        if (json.isMember("settlementDetails") && !json["settlementDetails"].isNull()) {
            settlement_details_ = json["settlementDetails"];
        }
        if (json.isMember("maxRafflesPerDay") && !json["maxRafflesPerDay"].isNull()) {
            max_raffles_per_day_ = json["maxRafflesPerDay"].asInt();
        }
        if (json.isMember("allowedRaffleCategories") && !json["allowedRaffleCategories"].isNull()) {
            allowed_raffle_categories_.clear();
            for (const auto& category : json["allowedRaffleCategories"]) {
                allowed_raffle_categories_.push_back(category.asString());
            }
        }
        if (json.isMember("requiresApproval") && !json["requiresApproval"].isNull()) {
            requires_approval_ = json["requiresApproval"].asBool();
        }
        if (json.isMember("totalTicketsSold") && !json["totalTicketsSold"].isNull()) {
            total_tickets_sold_ = json["totalTicketsSold"].asInt();
        }
        if (json.isMember("totalRevenueGenerated") && !json["totalRevenueGenerated"].isNull()) {
            total_revenue_generated_ = json["totalRevenueGenerated"].asDouble();
        }
        if (json.isMember("totalCommissionEarned") && !json["totalCommissionEarned"].isNull()) {
            total_commission_earned_ = json["totalCommissionEarned"].asDouble();
        }
        if (json.isMember("lastPayoutDate") && !json["lastPayoutDate"].isNull()) {
            last_payout_date_ = json["lastPayoutDate"].asString();
        }
        if (json.isMember("apiKey") && !json["apiKey"].isNull()) {
            api_key_ = json["apiKey"].asString();
        }
        if (json.isMember("apiSecret") && !json["apiSecret"].isNull()) {
            api_secret_ = json["apiSecret"].asString();
        }
        if (json.isMember("ipWhitelist") && !json["ipWhitelist"].isNull()) {
            ip_whitelist_.clear();
            for (const auto& ip : json["ipWhitelist"]) {
                ip_whitelist_.push_back(ip.asString());
            }
        }
        if (json.isMember("webhookSecret") && !json["webhookSecret"].isNull()) {
            webhook_secret_ = json["webhookSecret"].asString();
        }
        if (json.isMember("taxId") && !json["taxId"].isNull()) {
            tax_id_ = json["taxId"].asString();
        }
        if (json.isMember("legalEntityName") && !json["legalEntityName"].isNull()) {
            legal_entity_name_ = json["legalEntityName"].asString();
        }
        if (json.isMember("kycVerified") && !json["kycVerified"].isNull()) {
            kyc_verified_ = json["kycVerified"].asBool();
        }
        if (json.isMember("agreementAcceptedAt") && !json["agreementAcceptedAt"].isNull()) {
            agreement_accepted_at_ = json["agreementAcceptedAt"].asString();
        }
        if (json.isMember("agreementVersion") && !json["agreementVersion"].isNull()) {
            agreement_version_ = json["agreementVersion"].asString();
        }
        if (json.isMember("supportPriority") && !json["supportPriority"].isNull()) {
            support_priority_ = json["supportPriority"].asString();
        }
        if (json.isMember("notes") && !json["notes"].isNull()) {
            notes_ = json["notes"].asString();
        }
        if (json.isMember("createdAt") && !json["createdAt"].isNull()) {
            created_at_ = json["createdAt"].asString();
        }
        if (json.isMember("updatedAt") && !json["updatedAt"].isNull()) {
            updated_at_ = json["updatedAt"].asString();
        }
    }

}


#endif //CHOPMONEYRAFFLEAPI_AFFILIATEPARTNERDTO_H