//
// Created by Emmanuel Addo-Odame on 07/07/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CHANGECUSTOMERPINDTO_H
#define CHOPMONEYRAFFLEAPI_CHANGECUSTOMERPINDTO_H
#include <json/json.h>

namespace raffle::dto {

    class ChangeCustomerPinDto {

    public:

        ChangeCustomerPinDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getOldPin() const { return oldPin_; }
        [[nodiscard]] const std::string& getNewPin() const { return newPin_; }
        [[nodiscard]] const std::string& getCustomerPhoneNumber() const { return customerPhoneNumber_; }

        // Setters
        void setOldPin(const std::string& value) { oldPin_ = value; }
        void setNewPin(const std::string& value) { newPin_ = value; }
        void setCustomerPhoneNumber(const std::string& value) { customerPhoneNumber_ = value; }

    private:

        std::string oldPin_;
        std::string newPin_;
        std::string customerPhoneNumber_;

    };

    inline void ChangeCustomerPinDto::fromJson(const Json::Value& json) {

        if (json.isMember("oldPin") && !json["oldPin"].isNull()) {
            oldPin_ = json["oldPin"].asString();
        }

        if (json.isMember("newPin") && !json["newPin"].isNull()) {
            newPin_ = json["newPin"].asString();
        }

        if (json.isMember("phoneNumber") && !json["phoneNumber"].isNull()) {
            customerPhoneNumber_ = json["phoneNumber"].asString();
        }

    }

}
#endif //CHOPMONEYRAFFLEAPI_CHANGECUSTOMERPINDTO_H