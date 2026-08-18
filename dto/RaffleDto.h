//
// Created by Emmanuel Addo-Odame on 06/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CREATERAFFLEDTO_H
#define CHOPMONEYRAFFLEAPI_CREATERAFFLEDTO_H

#include <json/json.h>
#include <string>

namespace raffle::dto {

class RaffleDto {

public:
  RaffleDto() = default;

  void fromJson(const Json::Value &json);

  // Getters

  [[nodiscard]] const std::string &getName() const { return name_; }
  [[nodiscard]] const std::string &getSlug() const { return slug_; }
  [[nodiscard]] const std::string &getStartTime() const { return start_time_; }
  [[nodiscard]] const std::string &getEndTime() const { return end_time_; }
  [[nodiscard]] const std::string &getDrawTime() const { return draw_time_; }
  [[nodiscard]] double getLiability() const { return liability_; }
  [[nodiscard]] int32_t getMaximumWinners() const { return maximum_winners_; }
  [[nodiscard]] double getExpectedRevenue() const { return expected_revenue_; }
  [[nodiscard]] const std::vector<std::string> &getExemptedPartners() const { return exempted_partner_ids_; }
  [[nodiscard]] const Json::Value &getOptions() const { return options_; }

  // Setters
  void setName(const std::string &value) { name_ = value; }
  void setSlug(const std::string &value) { slug_ = value; }
  void setStartTime(const std::string &value) { start_time_ = value; }
  void setEndTime(const std::string &value) { end_time_ = value; }
  void setDrawTime(const std::string &value) { draw_time_ = value; }
  void setLiability(const double value) { liability_ = value; }
  void setMaximumWinners(const int32_t value) { maximum_winners_ = value; }
  void setExpectedRevenue(const double &value) { expected_revenue_ = value; }
  void setExemptedPartners(const std::vector<std::string> &value) { exempted_partner_ids_ = value; }
  void setOptions(const Json::Value &value) { options_ = value; }

private:
  std::string name_;
  std::string slug_;
  std::string start_time_;
  std::string end_time_;
  std::string draw_time_;
  int32_t maximum_winners_ {1};
  double liability_ {0};
  double expected_revenue_ {0};
  std::vector<std::string> exempted_partner_ids_;
  Json::Value options_;
};

inline void RaffleDto::fromJson(const Json::Value &json) {

  if (json.isMember("name") && !json["name"].isNull()) {
    setName(json["name"].asString());
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

  if (json.isMember("liability") && !json["liability"].isNull()) {
    liability_ = json["liability"].asDouble();
  }

  if (json.isMember("maximumWinners") && !json["maximumWinners"].isNull()) {
    maximum_winners_ = json["maximumWinners"].asInt();
  }

  if (json.isMember("expectedRevenue") && !json["expectedRevenue"].isNull()) {
    expected_revenue_ = json["expectedRevenue"].asDouble();
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
#endif // CHOPMONEYRAFFLEAPI_CREATERAFFLEDTO_H