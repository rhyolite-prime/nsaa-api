//
// Created by Emmanuel Addo-Odame on 02/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_HEROSLIDERDTO_H
#define CHOPMONEYRAFFLEAPI_HEROSLIDERDTO_H
#include <json/json.h>
#include <string>

namespace raffle::dto {

    class HeroSliderDto {
    public:
        HeroSliderDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getHeadline() const { return headline_; }
        [[nodiscard]] const std::string& getCopy() const { return copy_; }
        [[nodiscard]] const std::string& getImageUrl() const { return image_url_; }
        [[nodiscard]] const std::string& getCtaPrimary() const { return cta_primary_; }
        [[nodiscard]] const std::string& getCtaSecondary() const { return cta_secondary_; }
        [[nodiscard]] int getDisplayOrder() const { return display_order_; }
        [[nodiscard]] bool isActive() const { return is_active_; }

        // Setters
        void setHeadline(const std::string& value) { headline_ = value; }
        void setCopy(const std::string& value) { copy_ = value; }
        void setImageUrl(const std::string& value) { image_url_ = value; }
        void setCtaPrimary(const std::string& value) { cta_primary_ = value; }
        void setCtaSecondary(const std::string& value) { cta_secondary_ = value; }
        void setDisplayOrder(int value) { display_order_ = value; }
        void setIsActive(bool value) { is_active_ = value; }

    private:

        std::string headline_;
        std::string copy_;
        std::string image_url_;
        std::string cta_primary_;
        std::string cta_secondary_;
        int display_order_ = 0;
        bool is_active_ = true;

    };

    inline void HeroSliderDto::fromJson(const Json::Value& json) {

        if (json.isMember("headline") && !json["headline"].isNull()) {
            headline_ = json["headline"].asString();
        }

        if (json.isMember("copy") && !json["copy"].isNull()) {
            copy_ = json["copy"].asString();
        }

        if (json.isMember("imageUrl") && !json["imageUrl"].isNull()) {
            image_url_ = json["imageUrl"].asString();
        }

        if (json.isMember("ctaPrimary") && !json["ctaPrimary"].isNull()) {
            cta_primary_ = json["ctaPrimary"].asString();
        }

        if (json.isMember("ctaSecondary") && !json["ctaSecondary"].isNull()) {
            cta_secondary_ = json["ctaSecondary"].asString();
        }

        if (json.isMember("displayOrder") && !json["displayOrder"].isNull()) {
            display_order_ = json["displayOrder"].asInt();
        }

        if (json.isMember("isActive") && !json["isActive"].isNull()) {
            is_active_ = json["isActive"].asBool();
        }

    }
}
#endif //CHOPMONEYRAFFLEAPI_HEROSLIDERDTO_H