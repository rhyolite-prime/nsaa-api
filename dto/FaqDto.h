//
// Created by Emmanuel Addo-Odame on 02/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_FAQDTO_H
#define CHOPMONEYRAFFLEAPI_FAQDTO_H

#include <json/json.h>
#include <string>

namespace raffle::dto {

    class FaqDto {

    public:

        FaqDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getQuestion() const { return question_; }
        [[nodiscard]] const std::string& getAnswer() const { return answer_; }
        [[nodiscard]] const std::string& getCategory() const { return category_; }
        [[nodiscard]] int getDisplayOrder() const { return display_order_; }
        [[nodiscard]] bool isActive() const { return is_active_; }

        // Setters
        void setQuestion(const std::string& value) { question_ = value; }
        void setAnswer(const std::string& value) { answer_ = value; }
        void setCategory(const std::string& value) { category_ = value; }
        void setDisplayOrder(int value) { display_order_ = value; }
        void setIsActive(bool value) { is_active_ = value; }

    private:

        std::string question_;
        std::string answer_;
        std::string category_;
        int display_order_ = 1;
        bool is_active_ = true;

    };

    inline void FaqDto::fromJson(const Json::Value& json) {


        if (json.isMember("question") && !json["question"].isNull()) {
            question_ = json["question"].asString();
        }

        if (json.isMember("answer") && !json["answer"].isNull()) {
            answer_ = json["answer"].asString();
        }

        if (json.isMember("category") && !json["category"].isNull()) {
            category_ = json["category"].asString();
        }

        if (json.isMember("displayOrder") && !json["displayOrder"].isNull()) {
            display_order_ = json["displayOrder"].asInt();
        }

        if (json.isMember("isActive") && !json["isActive"].isNull()) {
            is_active_ = json["isActive"].asBool();
        }

    }

}

#endif //CHOPMONEYRAFFLEAPI_FAQDTO_H
