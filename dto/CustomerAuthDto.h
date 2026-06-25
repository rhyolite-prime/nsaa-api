//
// Created by Emmanuel Addo-Odame on 12/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CUSTOMERAUTHDTO_H
#define CHOPMONEYRAFFLEAPI_CUSTOMERAUTHDTO_H
#include <json/json.h>

namespace raffle::dto {

    class CustomerAuthDto {

        public:

        CustomerAuthDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getPhoneNumber() const { return phone_number_; }
        [[nodiscard]] const std::string& getPin() const { return pin_; }
        [[nodiscard]] const std::string& getRequestId() const { return request_id_; }

        // Setters
        void setPhoneNumber(const std::string& value) { phone_number_ = value; }
        void setPin(const std::string& value) { pin_ = value; }
        void setRequestId(const std::string& value) { request_id_ = value; }

    private:

        std::string phone_number_;
        std::string pin_;
        std::string request_id_;

    };

    inline void CustomerAuthDto::fromJson(const Json::Value& json) {

        if (json.isMember("phoneNumber") && !json["phoneNumber"].isNull()) {
            phone_number_ = json["phoneNumber"].asString();
        }

        if (json.isMember("pin") && !json["pin"].isNull()) {
            pin_ = json["pin"].asString();
        }

        if (json.isMember("requestId") && !json["requestId"].isNull()) {
            request_id_ = json["requestId"].asString();
        }

    }
}


#endif //CHOPMONEYRAFFLEAPI_CUSTOMERAUTHDTO_H