//
// Created by Emmanuel Addo-Odame on 18/08/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_PRIZECATEGORYDTO_H
#define CHOPMONEYRAFFLEAPI_PRIZECATEGORYDTO_H
#include <json/json.h>
#include <string>

namespace raffle::dto {

    class PrizeCategoryDto {

    public:

        PrizeCategoryDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getCode() const { return code_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] int getRewardType() const { return reward_type_; }
        [[nodiscard]] int getPriority() const { return priority_; }
        [[nodiscard]] int getMaxWinners() const { return max_winners_; }
        [[nodiscard]] bool isActive() const { return is_active_; }
        [[nodiscard]] bool getIsRepeatable() const { return is_repeatable_; }

        // Setters
        void setName(const std::string& value) { name_ = value; }
        void setCode(const std::string& value) { code_ = value; }
        void setDescription(const std::string& value) { description_ = value; }
        void setRewardType(const int value) { reward_type_ = value; }
        void setPriority(const int value) { priority_ = value; }
        void setMaxWinners(const int value) { max_winners_ = value; }
        void setIsActive(const bool value) { is_active_ = value; }
        void setIsRepeatable(const bool value) { is_repeatable_ = value; }

    private:

        std::string name_;
        std::string code_;
        std::string description_;
        int reward_type_ {0};
        int priority_ = 1;
        int max_winners_ = 1;
        bool is_active_ {false};
        bool is_repeatable_ {false};

    };

    inline void PrizeCategoryDto::fromJson(const Json::Value& json) {


        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }

        if (json.isMember("code") && !json["code"].isNull()) {
            code_ = json["code"].asString();
        }

        if (json.isMember("description") && !json["description"].isNull()) {
            description_ = json["description"].asString();
        }

        if (json.isMember("priority") && !json["priority"].isNull()) {
            priority_ = json["priority"].asInt();
        }

        if (json.isMember("maxWinners") && !json["maxWinners"].isNull()) {
            max_winners_ = json["maxWinners"].asInt();
        }

        if (json.isMember("rewardType") && !json["rewardType"].isNull()) {
            reward_type_ = json["rewardType"].asInt();
        }

        if (json.isMember("isActive") && !json["isActive"].isNull()) {
            is_active_ = json["isActive"].asBool();
        }

        if (json.isMember("isRepeatable") && !json["isRepeatable"].isNull()) {
            is_repeatable_ = json["isRepeatable"].asBool();
        }

    }

}

#endif //CHOPMONEYRAFFLEAPI_PRIZECATEGORYDTO_H
