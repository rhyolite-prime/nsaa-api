//
// Created by Emmanuel Addo-Odame on 07/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CREATEUSERDTO_H
#define CHOPMONEYRAFFLEAPI_CREATEUSERDTO_H
#include <json/json.h>

namespace raffle::dto {

    class CreateUserDto {

    public:

        CreateUserDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getFirstName() const { return first_name_; }
        [[nodiscard]] const std::string& getLastName() const { return last_name_; }
        [[nodiscard]] const std::string& getEmail() const { return email_; }
        [[nodiscard]] const std::string& getUsername() const { return username_; }
        [[nodiscard]] const std::string& getPassword() const { return password_; }
        [[nodiscard]] const std::string& getPhoneNumber() const { return phone_number_; }
        [[nodiscard]] bool isAdmin() const { return is_admin_; }
        [[nodiscard]] bool isActive() const { return is_active_; }
        [[nodiscard]] const std::vector<std::string>& getRoles() const { return roles_; }

        // Setters
        void setFirstName(const std::string& value) { first_name_ = value; }
        void setLastName(const std::string& value) { last_name_ = value; }
        void setEmail(const std::string& value) { email_ = value; }
        void setUsername(const std::string& value) { username_ = value; }
        void setPassword(const std::string& value) { password_ = value; }
        void setPhoneNumber(const std::string& value) { phone_number_ = value; }
        void setIsAdmin(bool value) { is_admin_ = value; }
        void setIsActive(bool value) { is_active_ = value; }
        void setRoles(const std::vector<std::string>& value) { roles_ = value; }

    private:

        std::string first_name_;
        std::string last_name_;
        std::string username_;
        std::string email_;
        std::string password_;
        std::string phone_number_;
        bool is_admin_ = false;
        bool is_active_ = false;
        std::vector<std::string> roles_;

    };

     inline void CreateUserDto::fromJson(const Json::Value& json) {

        if (json.isMember("firstName") && !json["firstName"].isNull()) {
            first_name_ = json["firstName"].asString();
        }

        if (json.isMember("lastName") && !json["lastName"].isNull()) {
            last_name_ = json["lastName"].asString();
        }

        if (json.isMember("username") && !json["username"].isNull()) {
            username_ = json["username"].asString();
        }

         if (json.isMember("email") && !json["email"].isNull()) {
             email_ = json["email"].asString();
         }

        if (json.isMember("password") && !json["password"].isNull()) {
            password_ = json["password"].asString();
        }

        if (json.isMember("phoneNumber") && !json["phoneNumber"].isNull()) {
            phone_number_ = json["phoneNumber"].asString();
        }

         if (json.isMember("isAdmin") && !json["isAdmin"].isNull()) {
             is_admin_ = json["isAdmin"].asBool();
         }

         if (json.isMember("isActive") && !json["isActive"].isNull()) {
             is_active_ = json["isActive"].asBool();
         }

         if (json.isMember("roles") && json["roles"].isArray()) {
             roles_.clear();
             for (const auto& role : json["roles"]) {
                 if (!role.isNull()) {
                     roles_.push_back(role.asString());
                 }
             }
         }



    }

}


#endif //CHOPMONEYRAFFLEAPI_CREATEUSERDTO_H