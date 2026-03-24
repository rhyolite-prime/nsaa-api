//
// Created by Emmanuel Addo-Odame on 09/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CREATEROLEDTO_H
#define CHOPMONEYRAFFLEAPI_CREATEROLEDTO_H
#include <json/json.h>
#include <string>

#include "PermissionDto.h"

namespace raffle::dto {

    class CreateRoleDto
    {
    public:
        // Default constructor
        CreateRoleDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getDisplayName() const { return display_name_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] const std::vector<PermissionDto> &getPermissions() const { return permissions_; }

        // Setters
        void setName(const std::string& name) { name_ = name; }
        void setDisplayName(const std::string& displayName) { display_name_ = displayName; }
        void setDescription(const std::string& description) { description_ = description; }
        void setPermissions(const std::vector<PermissionDto> &value) { permissions_ = value; }

    private:

        std::string name_;
        std::string display_name_;
        std::string description_;
        std::vector<PermissionDto> permissions_;
    };

    inline void CreateRoleDto::fromJson(const Json::Value& json) {

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }

        if (json.isMember("displayName") && !json["displayName"].isNull()) {
            display_name_ = json["displayName"].asString();
        }

        if (json.isMember("description") && !json["description"].isNull()) {
            description_ = json["description"].asString();
        }

        if (json.isMember("permissions") && json["permissions"].isArray()) {
            permissions_.clear();
            for (const auto& permissionJson : json["permissions"]) {
                if (permissionJson.isObject()) {
                    PermissionDto permission;
                    permission.fromJson(permissionJson);
                    permissions_.push_back(permission);
                }
            }
        }
    }
}
#endif //CHOPMONEYRAFFLEAPI_CREATEROLEDTO_H