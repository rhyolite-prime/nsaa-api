//
// Created by Emmanuel Addo-Odame on 06/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CREATETICKETDTO_H
#define CHOPMONEYRAFFLEAPI_CREATETICKETDTO_H

#include <json/json.h>
#include <string>

namespace raffle::dto {

class CreateTicketDto {

public:
  CreateTicketDto() = default;

  void fromJson(const Json::Value &json);

  // Getters
  [[nodiscard]] const std::string &getRaffleId() const { return raffle_id_; }
  [[nodiscard]] const std::string &getSelectedOptionValue() const { return selected_option_value_; }
  [[nodiscard]] const std::string &getTicketGroupId() const { return ticket_group_id_; }
  [[nodiscard]] const std::string& getCustomerPhone() const { return customer_phone_; }
  [[nodiscard]] const std::string& getPaymentMethod() const { return payment_method_; }
  [[nodiscard]] const std::string& getPurchaseChannel() const { return purchase_channel_; }
  [[nodiscard]] const std::string& getProvider() const { return provider_; }

  // Setters
  void setRaffleId(const std::string &value) { raffle_id_ = value; }
  void setSelectedOptionValue(const std::string &value) { selected_option_value_ = value; }
  void setTicketGroupId(const std::string &value) { ticket_group_id_ = value; }
  void setCustomerPhone(const std::string &value) { customer_phone_ = value; }
  void setPaymentMethod(const std::string &value) { payment_method_ = value; }
  void setPurchaseChannel(const std::string &value) { purchase_channel_ = value; }
  void setProvider(const std::string &value) { provider_ = value; }


  private:
    std::string raffle_id_;
    std::string selected_option_value_;
    std::string ticket_group_id_;
    std::string payment_method_;
    std::string customer_phone_;
    std::string purchase_channel_;
    std::string provider_;

};

inline void CreateTicketDto::fromJson(const Json::Value &json) {

  if (json.isMember("raffleId") && !json["raffleId"].isNull()) {
    setRaffleId(json["raffleId"].asString());
  }

  if (json.isMember("selectedOptionValue") && !json["selectedOptionValue"].isNull()) {
    setSelectedOptionValue(json["selectedOptionValue"].asString());
  }

  if (json.isMember("ticketGroupId") && !json["ticketGroupId"].isNull()) {
    setTicketGroupId(json["ticketGroupId"].asString());
  }

  if (json.isMember("customerPhone") && !json["customerPhone"].isNull()) {
    customer_phone_ = json["customerPhone"].asString();
  }


  if (json.isMember("paymentMethod") && !json["paymentMethod"].isNull()) {
    payment_method_ = json["paymentMethod"].asString();
  }

  if (json.isMember("purchaseChannel") && !json["purchaseChannel"].isNull()) {
    purchase_channel_ = json["purchaseChannel"].asString();
  }

  if (json.isMember("provider") && !json["provider"].isNull()) {
    provider_ = json["provider"].asString();
  }

}

} // namespace raffle::dto
#endif // CHOPMONEYRAFFLEAPI_CREATETICKETDTO_H