//
// Created by Emmanuel Addo-Odame on 23/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_WEBCHECKOUTDTO_H
#define CHOPMONEYRAFFLEAPI_WEBCHECKOUTDTO_H

#include <json/json.h>
#include <string>

namespace raffle::dto {

    class WebCheckoutDto {

public:
    WebCheckoutDto() = default;

    void fromJson(const Json::Value &json);

    [[nodiscard]] const std::string &getPhoneNo() const { return phone_no_; }
    [[nodiscard]] double getTotalAmount() const { return total_amount_; }
    [[nodiscard]] const std::string &getDescription() const { return description_; }
    [[nodiscard]] const std::string &getCallbackUrl() const { return callback_url_; }
    [[nodiscard]] const std::string &getReturnUrl() const { return return_url_; }
    [[nodiscard]] const std::string &getCancellationUrl() const { return cancellation_url_; }
    [[nodiscard]] const std::string &getClientReference() const { return client_reference_; }

    void setPhoneNo(const std::string &value) { phone_no_ = value; }
    void setTotalAmount(double value) { total_amount_ = value; }
    void setDescription(const std::string &value) { description_ = value; }
    void setCallbackUrl(const std::string &value) { callback_url_ = value; }
    void setReturnUrl(const std::string &value) { return_url_ = value; }
    void setCancellationUrl(const std::string &value) { cancellation_url_ = value; }
    void setClientReference(const std::string &value) { client_reference_ = value; }

private:
    std::string phone_no_;
    double total_amount_ = 0.0;
    std::string description_;
    std::string callback_url_;
    std::string return_url_;
    std::string cancellation_url_;
    std::string client_reference_;

    };

    inline void WebCheckoutDto::fromJson(const Json::Value &json) {
        if (json.isMember("phoneNo") && !json["phoneNo"].isNull()) {
            phone_no_ = json["phoneNo"].asString();
        }

        if (json.isMember("totalAmount") && !json["totalAmount"].isNull()) {
            total_amount_ = json["totalAmount"].asDouble();
        }

        if (json.isMember("description") && !json["description"].isNull()) {
            description_ = json["description"].asString();
        }

        if (json.isMember("callbackUrl") && !json["callbackUrl"].isNull()) {
            callback_url_ = json["callbackUrl"].asString();
        }

        if (json.isMember("returnUrl") && !json["returnUrl"].isNull()) {
            return_url_ = json["returnUrl"].asString();
        }

        if (json.isMember("cancellationUrl") && !json["cancellationUrl"].isNull()) {
            cancellation_url_ = json["cancellationUrl"].asString();
        }

        if (json.isMember("clientReference") && !json["clientReference"].isNull()) {
            client_reference_ = json["clientReference"].asString();
        }

    }


}
#endif //CHOPMONEYRAFFLEAPI_WEBCHECKOUTDTO_H