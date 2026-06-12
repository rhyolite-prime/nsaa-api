//
// Created by Emmanuel Addo-Odame on 12/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CUSTOMEROTPDTO_H
#define CHOPMONEYRAFFLEAPI_CUSTOMEROTPDTO_H

#include <json/json.h>

namespace raffle::dto {

    class CustomerOtpDto {

    public:

        CustomerOtpDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getPhoneNumber() const { return phone_number_; }
        [[nodiscard]] const std::string& getRequestId() const { return request_id_; }

        // Setters
        void setPhoneNumber(const std::string& value) { phone_number_ = value; }
        void setRequestId(const std::string& value) { request_id_ = value; }

    private:

        std::string phone_number_;
        std::string request_id_;

    };

    inline void CustomerOtpDto::fromJson(const Json::Value& json) {

        if (json.isMember("phoneNumber") && !json["phoneNumber"].isNull()) {
            phone_number_ = json["phoneNumber"].asString();
        }

        if (json.isMember("requestId") && !json["requestId"].isNull()) {
            request_id_ = json["requestId"].asString();
        }

    }
}


#endif //CHOPMONEYRAFFLEAPI_CUSTOMEROTPDTO_H