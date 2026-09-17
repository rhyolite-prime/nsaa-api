//
// Created by Emmanuel Addo-Odame on 12/09/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_TICKETSUMMARYREQUESTDTO_H
#define CHOPMONEYRAFFLEAPI_TICKETSUMMARYREQUESTDTO_H
#include <json/json.h>

namespace raffle::dto {

    class TicketSummaryRequestDto {

    public:

        TicketSummaryRequestDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getPhoneNo() const { return phoneNo_; }
        [[nodiscard]] const std::string& getPin() const { return pin_; }


        // Setters
        void setPhoneNo(const std::string& value) { phoneNo_ = value; }
        void setPin(const std::string& value) { pin_ = value; }


    private:

        std::string phoneNo_;
        std::string pin_;

    };

    inline void TicketSummaryRequestDto::fromJson(const Json::Value& json) {

        if (json.isMember("phoneNo") && !json["phoneNo"].isNull()) {
            setPhoneNo(json["phoneNo"].asString());
        }

        if (json.isMember("pin") && !json["pin"].isNull()) {
            setPin(json["pin"].asString());
        }

    }

}

#endif //CHOPMONEYRAFFLEAPI_TICKETSUMMARYREQUESTDTO_H
