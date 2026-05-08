//
// Created by Emmanuel Addo-Odame on 24/02/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CREATECOLORDTO_H
#define CHOPMONEYRAFFLEAPI_CREATECOLORDTO_H

#include <json/json.h>

namespace raffle::dto {

    class CreateColorDto {

    public:

        CreateColorDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getCode() const { return code_; }
        [[nodiscard]] const std::string& getHexColor() const { return hex_color_; }
        [[nodiscard]] int getSortOrder() const { return sort_order_; }

        // Setters
        void setName(const std::string& value) { name_ = value; }
        void setCode(const std::string& value) { code_ = value; }
        void setHexColor(const std::string& value) { hex_color_ = value; }
        void setSortOrder(int value) { sort_order_ = value; }



    private:

        std::string name_;
        std::string code_;
        std::string hex_color_;
        int sort_order_ = 1;

    };

    inline void CreateColorDto::fromJson(const Json::Value& json) {

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }

        if (json.isMember("code") && !json["code"].isNull()) {
            code_ = json["code"].asString();
        }

        if (json.isMember("hexColor") && !json["hexColor"].isNull()) {
            hex_color_ = json["hexColor"].asString();
        }

        if (json.isMember("sortOrder") && !json["sortOrder"].isNull()) {
            sort_order_ = json["sortOrder"].asInt();
        }


    }

}


#endif //CHOPMONEYRAFFLEAPI_CREATECOLORDTO_H