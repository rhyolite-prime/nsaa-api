//
// Created by Emmanuel Addo-Odame on 02/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_SUPPORTTICKETDTO_H
#define CHOPMONEYRAFFLEAPI_SUPPORTTICKETDTO_H

#include <json/json.h>

namespace raffle::dto {

    class SupportTicketDto {

    public:

        SupportTicketDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getSubject() const { return subject_; }
        [[nodiscard]] const std::string& getCategory() const { return category_; }
        [[nodiscard]] const std::string& getEmail() const { return email_; }
        [[nodiscard]] const std::string& getPhoneNumber() const { return phone_number_; }
        [[nodiscard]] const std::string& getMessage() const { return message_; }
        [[nodiscard]] const std::string& getSource() const { return source_; }

        // Setters
        void setName(const std::string& value) { name_ = value; }
        void setSubject(const std::string& value) { subject_ = value; }
        void setCategory(const std::string& value) { category_ = value; }
        void setEmail(const std::string& value) { email_ = value; }
        void setPhoneNumber(const std::string& value) { phone_number_ = value; }
        void setMessage(const std::string& value) { message_ = value; }
        void setSource(const std::string& value) { source_ = value; }



    private:

        std::string name_;
        std::string email_;
        std::string subject_;
        std::string category_;
        std::string phone_number_;
        std::string message_;
        std::string source_;

    };

    inline void SupportTicketDto::fromJson(const Json::Value& json) {

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }

        if (json.isMember("subject") && !json["subject"].isNull()) {
            subject_ = json["subject"].asString();
        }

        if (json.isMember("category") && !json["category"].isNull()) {
            category_ = json["category"].asString();
        }


        if (json.isMember("source") && !json["source"].isNull()) {
            source_ = json["source"].asString();
        }

        if (json.isMember("email") && !json["email"].isNull()) {
            email_ = json["email"].asString();
        }

        if (json.isMember("phoneNumber") && !json["phoneNumber"].isNull()) {
            phone_number_ = json["phoneNumber"].asString();
        }

        if (json.isMember("message") && !json["message"].isNull()) {
            message_ = json["message"].asString();
        }


    }

}
#endif //CHOPMONEYRAFFLEAPI_SUPPORTTICKETDTO_H