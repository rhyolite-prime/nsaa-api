//
// Created by Emmanuel Addo-Odame on 18/08/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_PRIZEDTO_H
#define CHOPMONEYRAFFLEAPI_PRIZEDTO_H

#include <json/json.h>
#include <string>

namespace raffle::dto {

    class PrizeDto {

    public:

        PrizeDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getCategoryId() const { return category_id_; }
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] const std::string& getCurrency() const { return currency_; }
        [[nodiscard]] double getValue() const { return value_; }
        [[nodiscard]] int getQuantity() const { return quantity_; }
        [[nodiscard]] int getRemainingQuantity() const { return remaining_quantity_; }
        [[nodiscard]] double getWinProbability() const { return win_probability_; }
        [[nodiscard]] Json::Value getMetaData() const { return meta_data_; }


        // Setters
        void setCategoryId(const std::string& value) { category_id_ = value; }
        void setName(const std::string& value) { name_ = value; }
        void setDescription(const std::string& value) { description_ = value; }
        void setCurrency(const std::string& value) { currency_ = value; }
        void setValue(const double value) { value_ = value; }
        void setQuantity(const int value) { quantity_ = value; }
        void setRemainingQuantity(const int value) { remaining_quantity_ = value; }
        void setWinProbability(const double value) { win_probability_ = value; }
        void setMetaData(const Json::Value value) { meta_data_ = value; }


    private:

        std::string category_id_;
        std::string name_;
        std::string description_;
        std::string currency_;
        double value_ = 1;
        int quantity_ = 1;
        int remaining_quantity_ = 1;
        double win_probability_ {0};
        Json::Value meta_data_;

    };

    inline void PrizeDto::fromJson(const Json::Value& json) {


        if (json.isMember("categoryId") && !json["categoryId"].isNull()) {
            category_id_ = json["categoryId"].asString();
        }

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }

        if (json.isMember("description") && !json["description"].isNull()) {
            description_ = json["description"].asString();
        }

        if (json.isMember("value") && !json["value"].isNull()) {
            value_ = json["value"].asDouble();
        }

        if (json.isMember("probability") && !json["probability"].isNull()) {
            win_probability_ = json["probability"].asDouble();
        }

        if (json.isMember("currency") && !json["currency"].isNull()) {
            currency_ = json["currency"].asString();
        }

        if (json.isMember("quantity") && !json["quantity"].isNull()) {
            quantity_ = json["quantity"].asInt();
        }

        if (json.isMember("remainingQuantity") && !json["remainingQuantity"].isNull()) {
            remaining_quantity_ = json["remainingQuantity"].asInt();

        }

        if (json.isMember("metaData") && !json["metaData"].isNull()) {
            setMetaData(json["metaData"]);
        }



    }

}


#endif //CHOPMONEYRAFFLEAPI_PRIZEDTO_H
