//
// Created by Emmanuel Addo-Odame on 05/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_UPDATERAFFLEDTO_H
#define CHOPMONEYRAFFLEAPI_UPDATERAFFLEDTO_H
#include <json/json.h>
#include <string>

namespace raffle::dto {

class UpdateRaffleDto {

public:
  UpdateRaffleDto() = default;

  void fromJson(const Json::Value &json);

  [[nodiscard]] const std::string &getId() const { return id_; }
  [[nodiscard]] const std::string &getName() const { return name_; }
  [[nodiscard]] const std::string &getSlug() const { return slug_; }
  [[nodiscard]] const std::string &getStartTime() const { return start_time_; }
  [[nodiscard]] const std::string &getEndTime() const { return end_time_; }
  [[nodiscard]] const std::string &getDrawTime() const { return draw_time_; }
  [[nodiscard]] const std::string &getStatus() const { return status_; }
  [[nodiscard]] const std::string &getPrizePool() const { return prize_pool_; }
  [[nodiscard]] const std::vector<std::string> &getExemptedPartners() const { return exempted_partner_ids_; }
  [[nodiscard]] const Json::Value &getOptions() const { return options_; }

  // Setters
  void setId(const std::string &value) { id_ = value; }
  void setName(const std::string &value) { name_ = value; }
  void setSlug(const std::string &value) { slug_ = value; }
  void setStartTime(const std::string &value) { start_time_ = value; }
  void setEndTime(const std::string &value) { end_time_ = value; }
  void setDrawTime(const std::string &value) { draw_time_ = value; }
  void setStatus(const std::string &value) { status_ = value; }
  void setPrizePool(const std::string &value) { prize_pool_ = value; }
  void setExemptedPartners(const std::vector<std::string> &value) { exempted_partner_ids_ = value; }
  void setOptions(const Json::Value &value) { options_ = value; }

private:
  std::string id_;
  std::string name_;
  std::string slug_;
  std::string start_time_;
  std::string end_time_;
  std::string draw_time_;
  std::string status_;
  std::string prize_pool_;
  std::vector<std::string> exempted_partner_ids_;
  Json::Value options_;
};

inline void UpdateRaffleDto::fromJson(const Json::Value &json) {

  if (json.isMember("id") && !json["id"].isNull()) {
    id_ = json["id"].asString();
  }

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

  if (json.isMember("exemptedPartnerIds") && !json["exemptedPartnerIds"].isNull()) {
    exempted_partner_ids_.clear();
    for (const auto& partnerId : json["exemptedPartnerIds"]) {
      if (!partnerId.isNull()) {
        exempted_partner_ids_.push_back(partnerId.asString());
      }
    }
  }

  if (json.isMember("options") && !json["options"].isNull()) {
    setOptions(json["options"]);
  }
}

} // namespace raffle::dto

#endif // CHOPMONEYRAFFLEAPI_UPDATERAFFLEDTO_H