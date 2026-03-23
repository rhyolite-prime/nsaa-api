//
// Created by CHOPMONEY AI on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CREATEPRICETIERDTO_H
#define CHOPMONEYRAFFLEAPI_CREATEPRICETIERDTO_H

#include <json/json.h>

namespace raffle::dto {

    class CreatePriceTierDto {
    public:
        CreatePriceTierDto() = default;

        void fromJson(const Json::Value& json);

        [[nodiscard]] const std::string& getTierName() const { return tier_name_; }
        [[nodiscard]] int getTierLevel() const { return tier_level_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] const std::string& getPrizeAmount() const { return prize_amount_; }
        [[nodiscard]] const std::string& getPrizePercentage() const { return prize_percentage_; }
        [[nodiscard]] int getNumberOfWinners() const { return number_of_winners_; }
        [[nodiscard]] bool getIsCashback() const { return is_cashback_; }

        void setTierName(const std::string& value) { tier_name_ = value; }
        void setTierLevel(int value) { tier_level_ = value; }
        void setDescription(const std::string& value) { description_ = value; }
        void setPrizeAmount(const std::string& value) { prize_amount_ = value; }
        void setPrizePercentage(const std::string& value) { prize_percentage_ = value; }
        void setNumberOfWinners(int value) { number_of_winners_ = value; }
        void setIsCashback(bool value) { is_cashback_ = value; }

    private:
        std::string tier_name_;
        int tier_level_ = 0;
        std::string description_;
        std::string prize_amount_;
        std::string prize_percentage_;
        int number_of_winners_ = 0;
        bool is_cashback_ = false;
    };

    inline void CreatePriceTierDto::fromJson(const Json::Value& json) {
        if (json.isMember("tierName") && !json["tierName"].isNull()) {
            tier_name_ = json["tierName"].asString();
        }
        if (json.isMember("tierLevel") && !json["tierLevel"].isNull()) {
            tier_level_ = json["tierLevel"].asInt();
        }
        if (json.isMember("description") && !json["description"].isNull()) {
            description_ = json["description"].asString();
        }
        if (json.isMember("prizeAmount") && !json["prizeAmount"].isNull()) {
            prize_amount_ = json["prizeAmount"].asString();
        }
        if (json.isMember("prizePercentage") && !json["prizePercentage"].isNull()) {
            prize_percentage_ = json["prizePercentage"].asString();
        }
        if (json.isMember("numberOfWinners") && !json["numberOfWinners"].isNull()) {
            number_of_winners_ = json["numberOfWinners"].asInt();
        }
        if (json.isMember("isCashback") && !json["isCashback"].isNull()) {
            is_cashback_ = json["isCashback"].asBool();
        }
    }
}

#endif //CHOPMONEYRAFFLEAPI_CREATEPRICETIERDTO_H
