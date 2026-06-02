//
// Created by Emmanuel Addo-Odame on 01/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_AFFILIATEPARTNERDRAFTDTO_H
#define CHOPMONEYRAFFLEAPI_AFFILIATEPARTNERDRAFTDTO_H

#include <json/json.h>

namespace raffle::dto {

    class AffiliatePartnerDraftDto {

    public:

        AffiliatePartnerDraftDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getContactName() const { return contact_name_; }
        [[nodiscard]] const std::string& getContactEmail() const { return contact_email_; }
        [[nodiscard]] const std::string& getContactPhone() const { return contact_phone_; }
        [[nodiscard]] const std::string& getPartnershipType() const { return partnership_type_; }
        [[nodiscard]] const std::string& getCountry() const { return country_; }
        [[nodiscard]] const std::string& getRegionOrState() const { return region_or_state_; }
        [[nodiscard]] const std::string& getNotes() const { return notes_; }


        // Setters
        void setName(const std::string& value) { name_ = value; }
        void setContactName(const std::string& value) { contact_name_ = value; }
        void setContactEmail(const std::string& value) { contact_email_ = value; }
        void setContactPhone(const std::string& value) { contact_phone_ = value; }
        void setPartnershipType(const std::string& value) { partnership_type_ = value; }
        void setCountry(const std::string& value) { country_ = value; }
        void setRegionOrState(const std::string& value) { region_or_state_ = value; }
        void setNotes(const std::string& value) { notes_ = value; }

    private:

        std::string name_;
        std::string contact_name_;
        std::string contact_email_;
        std::string contact_phone_;
        std::string legal_entity_name_;
        std::string partnership_type_;
        std::string country_;
        std::string region_or_state_;
        std::string notes_;

    };

    inline void AffiliatePartnerDraftDto::fromJson(const Json::Value& json) {

        if (json.isMember("businessName") && !json["businessName"].isNull()) {
            name_ = json["businessName"].asString();
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

        if (json.isMember("partnershipType") && !json["partnershipType"].isNull()) {
            partnership_type_ = json["partnershipType"].asString();
        }

        if (json.isMember("country") && !json["country"].isNull()) {
            country_ = json["country"].asString();
        }

        if (json.isMember("regionOrState") && !json["regionOrState"].isNull()) {
            region_or_state_ = json["regionOrState"].asString();
        }

        if (json.isMember("message") && !json["message"].isNull()) {
            notes_ = json["message"].asString();
        }

    }

}



#endif //CHOPMONEYRAFFLEAPI_AFFILIATEPARTNERDRAFTDTO_H