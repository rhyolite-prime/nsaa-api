//
// Created by Emmanuel Addo-Odame on 02/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_MEDIAASSETDTO_H
#define CHOPMONEYRAFFLEAPI_MEDIAASSETDTO_H

#include <json/json.h>
#include <string>

namespace raffle::dto {

    class MediaAssetDto {

    public:

        MediaAssetDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getId() const { return id_; }
        [[nodiscard]] const std::string& getTitle() const { return title_; }
        [[nodiscard]] const std::string& getFileUrl() const { return file_url_; }
        [[nodiscard]] const std::string& getFileType() const { return file_type_; }
        [[nodiscard]] int getFileSize() const { return file_size_; }
        [[nodiscard]] const std::string& getCategory() const { return category_; }
        [[nodiscard]] int getDownloadCount() const { return download_count_; }
        [[nodiscard]] bool isActive() const { return is_active_; }

        // Setters
        void setId(const std::string& value) { id_ = value; }
        void setTitle(const std::string& value) { title_ = value; }
        void setFileUrl(const std::string& value) { file_url_ = value; }
        void setFileType(const std::string& value) { file_type_ = value; }
        void setFileSize(int value) { file_size_ = value; }
        void setCategory(const std::string& value) { category_ = value; }
        void setDownloadCount(int value) { download_count_ = value; }
        void setIsActive(bool value) { is_active_ = value; }

    private:

        std::string id_;
        std::string title_;
        std::string file_url_;
        std::string file_type_;
        int file_size_ = 0;
        std::string category_;
        int download_count_ = 0;
        bool is_active_ = true;

    };

    inline void MediaAssetDto::fromJson(const Json::Value& json) {

        if (json.isMember("id") && !json["id"].isNull()) {
            id_ = json["id"].asString();
        }

        if (json.isMember("title") && !json["title"].isNull()) {
            title_ = json["title"].asString();
        }

        if (json.isMember("fileUrl") && !json["fileUrl"].isNull()) {
            file_url_ = json["fileUrl"].asString();
        }

        if (json.isMember("fileType") && !json["fileType"].isNull()) {
            file_type_ = json["fileType"].asString();
        }

        if (json.isMember("fileSize") && !json["fileSize"].isNull()) {
            file_size_ = json["fileSize"].asInt();
        }

        if (json.isMember("category") && !json["category"].isNull()) {
            category_ = json["category"].asString();
        }

        if (json.isMember("downloadCount") && !json["downloadCount"].isNull()) {
            download_count_ = json["downloadCount"].asInt();
        }

        if (json.isMember("isActive") && !json["isActive"].isNull()) {
            is_active_ = json["isActive"].asBool();
        }

    }

}

#endif //CHOPMONEYRAFFLEAPI_MEDIAASSETDTO_H
