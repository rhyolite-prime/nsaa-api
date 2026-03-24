//
// Created by Emmanuel Addo-Odame on 09/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_UPDATEROLEDTO_H
#define CHOPMONEYRAFFLEAPI_UPDATEROLEDTO_H

#include <json/json.h>
#include <string>

namespace raffle::dto {

    class UpdateRoleDto
    {
    public:
        // Default constructor
        UpdateRoleDto() = default;

        void fromJson(const Json::Value& json);

        // Getters

        const std::string& getId() const { return id_; }
        const std::string& getName() const { return name_; }
        const std::string& getDisplayName() const { return display_name_; }
        const std::string& getDescription() const { return description_; }
        [[nodiscard]] const std::vector<PermissionDto> &getPermissions() const { return permissions_; }
        // Setters
        void setId(const std::string& id) { id_ = id; }
        void setName(const std::string& name) { name_ = name; }
        void setDisplayName(const std::string& displayName) { display_name_ = displayName; }
        void setDescription(const std::string& description) { description_ = description; }
        void setPermissions(const std::vector<PermissionDto> &value) { permissions_ = value; }

    private:

        std::string id_;
        std::string name_;
        std::string display_name_;
        std::string description_;
        std::vector<PermissionDto> permissions_;
    };

    inline void UpdateRoleDto::fromJson(const Json::Value& json) {

        if (json.isMember("id") && !json["id"].isNull()) {
            id_ = json["id"].asString();
        }

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
#endif //CHOPMONEYRAFFLEAPI_UPDATEROLEDTO_H