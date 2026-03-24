//
// Created by Emmanuel Addo-Odame on 23/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_DIRECTRECEIVEMONEYDTO_H
#define CHOPMONEYRAFFLEAPI_DIRECTRECEIVEMONEYDTO_H

#include <json/json.h>
#include <string>

namespace raffle::dto {

  class DirectReceiveMoneyDto {
  public:
    DirectReceiveMoneyDto() = default;

    void fromJson(const Json::Value &json);

    [[nodiscard]] const std::string &getCustomerName() const { return customer_name_; }
    [[nodiscard]] const std::string &getCustomerMsisdn() const { return customer_msisdn_; }
    [[nodiscard]] const std::string &getCustomerEmail() const { return customer_email_; }
    [[nodiscard]] const std::string &getChannel() const { return channel_; }
    [[nodiscard]] double getAmount() const { return amount_; }
    [[nodiscard]] const std::string &getPrimaryCallbackUrl() const { return primary_callback_url_; }
    [[nodiscard]] const std::string &getDescription() const { return description_; }
    [[nodiscard]] const std::string &getClientReference() const { return client_reference_; }

    void setCustomerName(const std::string &value) { customer_name_ = value; }
    void setCustomerMsisdn(const std::string &value) { customer_msisdn_ = value; }
    void setCustomerEmail(const std::string &value) { customer_email_ = value; }
    void setChannel(const std::string &value) { channel_ = value; }
    void setAmount(double value) { amount_ = value; }
    void setPrimaryCallbackUrl(const std::string &value) { primary_callback_url_ = value; }
    void setDescription(const std::string &value) { description_ = value; }
    void setClientReference(const std::string &value) { client_reference_ = value; }

  private:
    std::string customer_name_;
    std::string customer_msisdn_;
    std::string customer_email_;
    std::string channel_;
    double amount_ = 0.0;
    std::string primary_callback_url_;
    std::string description_;
    std::string client_reference_;
  };

  inline void DirectReceiveMoneyDto::fromJson(const Json::Value &json) {
    if (json.isMember("CustomerName") && !json["CustomerName"].isNull()) {
      customer_name_ = json["CustomerName"].asString();
    }

    if (json.isMember("CustomerMsisdn") && !json["CustomerMsisdn"].isNull()) {
      customer_msisdn_ = json["CustomerMsisdn"].asString();
    }

    if (json.isMember("CustomerEmail") && !json["CustomerEmail"].isNull()) {
      customer_email_ = json["CustomerEmail"].asString();
    }

    if (json.isMember("Channel") && !json["Channel"].isNull()) {
      channel_ = json["Channel"].asString();
    }

    if (json.isMember("Amount") && !json["Amount"].isNull()) {
      amount_ = json["Amount"].asDouble();
    }

    if (json.isMember("PrimaryCallbackUrl") && !json["PrimaryCallbackUrl"].isNull()) {
      primary_callback_url_ = json["PrimaryCallbackUrl"].asString();
    }

    if (json.isMember("Description") && !json["Description"].isNull()) {
      description_ = json["Description"].asString();
    }

    if (json.isMember("ClientReference") && !json["ClientReference"].isNull()) {
      client_reference_ = json["ClientReference"].asString();
    }
  }
}

#endif //CHOPMONEYRAFFLEAPI_DIRECTRECEIVEMONEYDTO_H