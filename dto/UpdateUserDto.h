//
// Created by Emmanuel Addo-Odame on 07/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_UPDATEUSERDTO_H
#define CHOPMONEYRAFFLEAPI_UPDATEUSERDTO_H
#include <json/json.h>

namespace raffle::dto {

class UpdateUserDto {

public:
  UpdateUserDto() = default;

  void fromJson(const Json::Value &json);

  [[nodiscard]] const std::string &getId() const { return id_; }
  [[nodiscard]] const std::string &getFirstName() const { return first_name_; }
  [[nodiscard]] const std::string &getLastName() const { return last_name_; }
  [[nodiscard]] const std::string &getEmail() const { return email_; }
  [[nodiscard]] const std::string &getUsername() const { return username_; }
  [[nodiscard]] const std::string &getPhoneNumber() const {
    return phone_number_;
  }
  [[nodiscard]] const std::vector<std::string> &getRoles() const {
    return roles_;
  }

  void setId(const std::string &value) { id_ = value; }
  void setFirstName(const std::string &value) { first_name_ = value; }
  void setLastName(const std::string &value) { last_name_ = value; }
  void setEmail(const std::string &value) { email_ = value; }
  void setUsername(const std::string &value) { username_ = value; }
  void setPhoneNumber(const std::string &value) { phone_number_ = value; }
  void setRoles(const std::vector<std::string> &value) { roles_ = value; }

private:
  std::string id_;
  std::string first_name_;
  std::string last_name_;
  std::string username_;
  std::string email_;
  std::string password_;
  std::string phone_number_;
  std::vector<std::string> roles_;
};

inline void UpdateUserDto::fromJson(const Json::Value &json) {

  if (json.isMember("id") && !json["id"].isNull()) {
    id_ = json["id"].asString();
  }

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

  if (json.isMember("phoneNumber") && !json["phoneNumber"].isNull()) {
    phone_number_ = json["phoneNumber"].asString();
  }

  if (json.isMember("roles") && json["roles"].isArray()) {
    roles_.clear();
    for (const auto &role : json["roles"]) {
      if (!role.isNull()) {
        roles_.push_back(role.asString());
      }
    }
  }
}

} // namespace raffle::dto

#endif // CHOPMONEYRAFFLEAPI_UPDATEUSERDTO_H