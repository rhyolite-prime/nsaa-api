//
// Created by Emmanuel Addo-Odame on 06/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CREATERAFFLEDTO_H
#define CHOPMONEYRAFFLEAPI_CREATERAFFLEDTO_H

#include <json/json.h>
#include <string>

namespace raffle::dto {

class CreateRaffleDto {

public:
  CreateRaffleDto() = default;

  void fromJson(const Json::Value &json);

  // Getters

  [[nodiscard]] const std::string &getName() const { return name_; }
  [[nodiscard]] const std::string &getSlug() const { return slug_; }
  [[nodiscard]] const std::string &getStartTime() const { return start_time_; }
  [[nodiscard]] const std::string &getEndTime() const { return end_time_; }
  [[nodiscard]] const std::string &getDrawTime() const { return draw_time_; }
  [[nodiscard]] const std::string &getStatus() const { return status_; }
  [[nodiscard]] const std::string &getPrizePool() const { return prize_pool_; }

  // Setters
  void setName(const std::string &value) { name_ = value; }
  void setSlug(const std::string &value) { slug_ = value; }
  void setStartTime(const std::string &value) { start_time_ = value; }
  void setEndTime(const std::string &value) { end_time_ = value; }
  void setDrawTime(const std::string &value) { draw_time_ = value; }
  void setStatus(const std::string &value) { status_ = value; }
  void setPrizePool(const std::string &value) { prize_pool_ = value; }

private:
  std::string name_;
  std::string slug_;
  std::string start_time_;
  std::string end_time_;
  std::string draw_time_;
  std::string status_;
  std::string prize_pool_;
};

inline void CreateRaffleDto::fromJson(const Json::Value &json) {

  if (json.isMember("name") && !json["name"].isNull()) {
    name_ = json["name"].asString();
  }

  if (json.isMember("slug") && !json["slug"].isNull()) {
    slug_ = json["slug"].asString();
  }

  if (json.isMember("startTime") && !json["startTime"].isNull()) {
    start_time_ = json["startTime"].asString();
  }

  if (json.isMember("endTime") && !json["endTime"].isNull()) {
    end_time_ = json["endTime"].asString();
  }

  if (json.isMember("drawTime") && !json["drawTime"].isNull()) {
    draw_time_ = json["drawTime"].asString();
  }

  if (json.isMember("status") && !json["status"].isNull()) {
    status_ = json["status"].asString();
  }

  if (json.isMember("prizePool") && !json["prizePool"].isNull()) {
    prize_pool_ = json["prizePool"].asString();
  }
}

} // namespace raffle::dto
#endif // CHOPMONEYRAFFLEAPI_CREATERAFFLEDTO_H