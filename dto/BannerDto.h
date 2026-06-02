//
// Created by Emmanuel Addo-Odame on 02/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_BANNERDTO_H
#define CHOPMONEYRAFFLEAPI_BANNERDTO_H

#include <json/json.h>
#include <string>

namespace raffle::dto {

    class BannerDto {

    public:

        BannerDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] const std::string& getBannerImageUrl() const { return banner_image_url_; }
        [[nodiscard]] const std::string& getCtaText() const { return cta_text_; }
        [[nodiscard]] const std::string& getCtaLink() const { return cta_link_; }
        [[nodiscard]] const std::string& getStartDate() const { return start_date_; }
        [[nodiscard]] const std::string& getEndDate() const { return end_date_; }
        [[nodiscard]] bool isActive() const { return is_active_; }

        // Setters
        void setName(const std::string& value) { name_ = value; }
        void setDescription(const std::string& value) { description_ = value; }
        void setBannerImageUrl(const std::string& value) { banner_image_url_ = value; }
        void setCtaText(const std::string& value) { cta_text_ = value; }
        void setCtaLink(const std::string& value) { cta_link_ = value; }
        void setStartDate(const std::string& value) { start_date_ = value; }
        void setEndDate(const std::string& value) { end_date_ = value; }
        void setIsActive(bool value) { is_active_ = value; }

    private:

        std::string name_;
        std::string description_;
        std::string banner_image_url_;
        std::string cta_text_;
        std::string cta_link_;
        std::string start_date_;
        std::string end_date_;
        bool is_active_ = true;

    };

    inline void BannerDto::fromJson(const Json::Value& json) {


        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }

        if (json.isMember("description") && !json["description"].isNull()) {
            description_ = json["description"].asString();
        }

        if (json.isMember("bannerImageUrl") && !json["bannerImageUrl"].isNull()) {
            banner_image_url_ = json["bannerImageUrl"].asString();
        }

        if (json.isMember("ctaText") && !json["ctaText"].isNull()) {
            cta_text_ = json["ctaText"].asString();
        }

        if (json.isMember("ctaLink") && !json["ctaLink"].isNull()) {
            cta_link_ = json["ctaLink"].asString();
        }

        if (json.isMember("startDate") && !json["startDate"].isNull()) {
            start_date_ = json["startDate"].asString();
        }

        if (json.isMember("endDate") && !json["endDate"].isNull()) {
            end_date_ = json["endDate"].asString();
        }

        if (json.isMember("isActive") && !json["isActive"].isNull()) {
            is_active_ = json["isActive"].asBool();
        }

    }

}

#endif //CHOPMONEYRAFFLEAPI_BANNERDTO_H
