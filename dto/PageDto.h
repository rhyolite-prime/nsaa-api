//
// Created by Emmanuel Addo-Odame on 02/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_PAGEDTO_H
#define CHOPMONEYRAFFLEAPI_PAGEDTO_H

#include <json/json.h>
#include <string>

namespace raffle::dto {

    class PageDto {

    public:

        PageDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getId() const { return id_; }
        [[nodiscard]] const std::string& getSlug() const { return slug_; }
        [[nodiscard]] const std::string& getTitle() const { return title_; }
        [[nodiscard]] const std::string& getContent() const { return content_; }
        [[nodiscard]] const std::string& getMetaDescription() const { return meta_description_; }
        [[nodiscard]] const std::string& getMetaKeywords() const { return meta_keywords_; }
        [[nodiscard]] int getVersion() const { return version_; }
        [[nodiscard]] const std::string& getPublishedAt() const { return published_at_; }
        [[nodiscard]] bool isActive() const { return is_active_; }

        // Setters
        void setId(const std::string& value) { id_ = value; }
        void setSlug(const std::string& value) { slug_ = value; }
        void setTitle(const std::string& value) { title_ = value; }
        void setContent(const std::string& value) { content_ = value; }
        void setMetaDescription(const std::string& value) { meta_description_ = value; }
        void setMetaKeywords(const std::string& value) { meta_keywords_ = value; }
        void setVersion(int value) { version_ = value; }
        void setPublishedAt(const std::string& value) { published_at_ = value; }
        void setIsActive(bool value) { is_active_ = value; }

    private:

        std::string id_;
        std::string slug_;
        std::string title_;
        std::string content_;
        std::string meta_description_;
        std::string meta_keywords_;
        int version_ = 1;
        std::string published_at_;
        bool is_active_ = true;

    };

    inline void PageDto::fromJson(const Json::Value& json) {

        if (json.isMember("id") && !json["id"].isNull()) {
            id_ = json["id"].asString();
        }

        if (json.isMember("slug") && !json["slug"].isNull()) {
            slug_ = json["slug"].asString();
        }

        if (json.isMember("title") && !json["title"].isNull()) {
            title_ = json["title"].asString();
        }

        if (json.isMember("content") && !json["content"].isNull()) {
            content_ = json["content"].asString();
        }

        if (json.isMember("metaDescription") && !json["metaDescription"].isNull()) {
            meta_description_ = json["metaDescription"].asString();
        }

        if (json.isMember("metaKeywords") && !json["metaKeywords"].isNull()) {
            meta_keywords_ = json["metaKeywords"].asString();
        }

        if (json.isMember("version") && !json["version"].isNull()) {
            version_ = json["version"].asInt();
        }

        if (json.isMember("publishedAt") && !json["publishedAt"].isNull()) {
            published_at_ = json["publishedAt"].asString();
        }

        if (json.isMember("isActive") && !json["isActive"].isNull()) {
            is_active_ = json["isActive"].asBool();
        }

    }

}

#endif //CHOPMONEYRAFFLEAPI_PAGEDTO_H
