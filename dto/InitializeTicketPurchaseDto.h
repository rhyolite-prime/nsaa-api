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
        [[nodiscard]] const std::string &getColourId() const { return colour_id_; }
        [[nodiscard]] int32_t getQuantity() const { return quantity_; }
        [[nodiscard]] const std::string &getPhoneNumber() const { return phone_number_; }

        // Setters
        void setColourId(const std::string &value) { colour_id_ = value; }
        void setQuantity(int32_t value) { quantity_ = value; }
        void setPhoneNumber(const std::string &value) { phone_number_ = value; }


    private:

        std::string colour_id_;
        std::string phone_number_;
        int32_t quantity_ {1};
    };

    inline void InitializeTicketPurchaseDto::fromJson(const Json::Value& json) {

        if (json.isMember("colourId") && !json["colourId"].isNull()) {
            colour_id_ = json["colourId"].asString();
        }

        if (json.isMember("quantity") && !json["quantity"].isNull()) {
            quantity_ = json["quantity"].asInt();
        }

        if (json.isMember("phoneNumber") && !json["phoneNumber"].isNull()) {
            phone_number_ = json["phoneNumber"].asString();
        }

    }
}
#endif //CHOPMONEYRAFFLEAPI_INITIALIZETICKETPURCHASEDTO_H