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
  [[nodiscard]] const std::string &getColourId() const { return colour_id_; }
  [[nodiscard]] int32_t getQuantity() const { return quantity_; }
  [[nodiscard]] const std::string& getCustomerPhone() const { return customer_phone_; }
  [[nodiscard]] const std::string& getPaymentMethod() const { return payment_method_; }
  [[nodiscard]] const std::string& getPurchaseChannel() const { return purchase_channel_; }

  // Setters
  void setColourId(const std::string &value) { colour_id_ = value; }
  void setQuantity(int32_t value) { quantity_ = value; }
  void setCustomerPhone(const std::string &value) { customer_phone_ = value; }
  void setPaymentMethod(const std::string &value) { payment_method_ = value; }
  void setPurchaseChannel(const std::string &value) { purchase_channel_ = value; }


  private:
    std::string colour_id_;
    std::string payment_method_;
    int32_t quantity_ = 1;
    std::string customer_phone_;
    std::string purchase_channel_;

};

inline void CreateTicketDto::fromJson(const Json::Value &json) {

  if (json.isMember("colourId") && !json["colourId"].isNull()) {
    colour_id_ = json["colourId"].asString();
  }

  if (json.isMember("quantity") && !json["quantity"].isNull()) {
    quantity_ = json["quantity"].asInt();
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


}

} // namespace raffle::dto
#endif // CHOPMONEYRAFFLEAPI_CREATETICKETDTO_H