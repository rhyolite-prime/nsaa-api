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

        /**
         * @brief Populates the dto from a json body.
         * Every field present in the json body is flagged as "provided" so that
         * partial updates can tell an omitted field from an empty one.
         */
        void fromJson(const Json::Value& json);

        // Presence flags (true when the field was supplied by the client)
        [[nodiscard]] bool hasHeadline() const { return has_headline_; }
        [[nodiscard]] bool hasCopy() const { return has_copy_; }
        [[nodiscard]] bool hasImageUrl() const { return has_image_url_; }
        [[nodiscard]] bool hasCtaPrimary() const { return has_cta_primary_; }
        [[nodiscard]] bool hasCtaSecondary() const { return has_cta_secondary_; }
        [[nodiscard]] bool hasDisplayOrder() const { return has_display_order_; }
        [[nodiscard]] bool hasIsActive() const { return has_is_active_; }

        // Getters
        [[nodiscard]] const std::string& getHeadline() const { return headline_; }
        [[nodiscard]] const std::string& getCopy() const { return copy_; }
        [[nodiscard]] const std::string& getImageUrl() const { return image_url_; }
        [[nodiscard]] const std::string& getCtaPrimary() const { return cta_primary_; }
        [[nodiscard]] const std::string& getCtaSecondary() const { return cta_secondary_; }
        [[nodiscard]] int getDisplayOrder() const { return display_order_; }
        [[nodiscard]] bool isActive() const { return is_active_; }

        // Setters
        void setHeadline(const std::string& value) { headline_ = value; has_headline_ = true; }
        void setCopy(const std::string& value) { copy_ = value; has_copy_ = true; }
        void setImageUrl(const std::string& value) { image_url_ = value; has_image_url_ = true; }
        void setCtaPrimary(const std::string& value) { cta_primary_ = value; has_cta_primary_ = true; }
        void setCtaSecondary(const std::string& value) { cta_secondary_ = value; has_cta_secondary_ = true; }
        void setDisplayOrder(int value) { display_order_ = value; has_display_order_ = true; }
        void setIsActive(bool value) { is_active_ = value; has_is_active_ = true; }

    private:

        std::string headline_;
        std::string copy_;
        std::string image_url_;
        std::string cta_primary_;
        std::string cta_secondary_;
        int display_order_ = 0;
        bool is_active_ = true;

        bool has_headline_ = false;
        bool has_copy_ = false;
        bool has_image_url_ = false;
        bool has_cta_primary_ = false;
        bool has_cta_secondary_ = false;
        bool has_display_order_ = false;
        bool has_is_active_ = false;

    };

    inline void HeroSliderDto::fromJson(const Json::Value& json) {

        if (json.isMember("headline") && !json["headline"].isNull()) {
            setHeadline(json["headline"].asString());
        }

        if (json.isMember("copy") && !json["copy"].isNull()) {
            setCopy(json["copy"].asString());
        }

        if (json.isMember("imageUrl") && !json["imageUrl"].isNull()) {
            setImageUrl(json["imageUrl"].asString());
        }

        if (json.isMember("ctaPrimary") && !json["ctaPrimary"].isNull()) {
            setCtaPrimary(json["ctaPrimary"].asString());
        }

        if (json.isMember("ctaSecondary") && !json["ctaSecondary"].isNull()) {
            setCtaSecondary(json["ctaSecondary"].asString());
        }

        if (json.isMember("displayOrder") && !json["displayOrder"].isNull()) {
            setDisplayOrder(json["displayOrder"].asInt());
        }

        if (json.isMember("isActive") && !json["isActive"].isNull()) {
            setIsActive(json["isActive"].asBool());
        }

    }
}
#endif //CHOPMONEYRAFFLEAPI_HEROSLIDERDTO_H
