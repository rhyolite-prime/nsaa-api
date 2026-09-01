//
// Created by Emmanuel Addo-Odame on 18/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_TICKETCOLLECTIONDTO_H
#define CHOPMONEYRAFFLEAPI_TICKETCOLLECTIONDTO_H
#include <drogon/drogon.h>
#include <string>
#include <json/json.h>

namespace raffle::dto {

    class TicketCollectionDto {

    public:

        TicketCollectionDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] int32_t getQuantity() const { return quantity_; }
        [[nodiscard]] double getUnitPrice() const { return unit_price_; }
        [[nodiscard]] bool getIsActive() const { return is_active_; }


        // Setters
        void setName(const std::string& value) { name_ = value; }
        void setQuantity(const int32_t value) { quantity_ = value; }
        void setUnitPrice(const double value) { unit_price_ = value; }
        void setIsActive(const bool value) { is_active_ = value; }

    private:

        std::string name_;
        int32_t quantity_ {0};
        double unit_price_ {0};
        bool is_active_ {false};

    };

    inline void TicketCollectionDto::fromJson(const Json::Value& json) {

        if (json.isMember("name") && !json["name"].isNull()) {
            setName(json["name"].asString());
        }

        if (json.isMember("quantity") && !json["quantity"].isNull()) {
            setQuantity(json["quantity"].asInt());
        }

        if (json.isMember("unitPrice") && !json["unitPrice"].isNull()) {
            setUnitPrice(json["unitPrice"].asDouble());
        }

        if (json.isMember("isActive") && !json["isActive"].isNull()) {
            setIsActive(json["isActive"].asBool());
        }

    }


}
#endif //CHOPMONEYRAFFLEAPI_TICKETCOLLECTIONDTO_H