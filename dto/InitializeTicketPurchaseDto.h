//
// Created by Emmanuel Addo-Odame on 06/07/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_INITIALIZETICKETPURCHASEDTO_H
#define CHOPMONEYRAFFLEAPI_INITIALIZETICKETPURCHASEDTO_H
#include <json/json.h>
#include <string>

namespace raffle::dto {

    class InitializeTicketPurchaseDto {

    public:
        InitializeTicketPurchaseDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string &getSelectedOption() const { return selected_option_; }
        [[nodiscard]] const std::string &getTicketGroupId() const { return ticket_group_id_; }
        [[nodiscard]] const std::string &getPhoneNumber() const { return phone_number_; }

        // Setters
        void setSelectedOption(const std::string &value) { selected_option_ = value; }
        void setTicketGroupId(const std::string &value) { ticket_group_id_ = value; }
        void setPhoneNumber(const std::string &value) { phone_number_ = value; }


    private:

        std::string selected_option_;
        std::string ticket_group_id_;
        std::string phone_number_;

    };

    inline void InitializeTicketPurchaseDto::fromJson(const Json::Value& json) {

        if (json.isMember("selectedOption") && !json["selectedOption"].isNull()) {
           setSelectedOption(json["selectedOption"].asString());
        }

        if (json.isMember("ticketGroupId") && !json["ticketGroupId"].isNull()) {
            setTicketGroupId(json["ticketGroupId"].asString());
        }

        if (json.isMember("phoneNumber") && !json["phoneNumber"].isNull()) {
            setPhoneNumber(json["phoneNumber"].asString());
        }

    }
}
#endif //CHOPMONEYRAFFLEAPI_INITIALIZETICKETPURCHASEDTO_H