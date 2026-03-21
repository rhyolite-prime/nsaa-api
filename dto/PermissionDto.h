//
// Created by Emmanuel Addo-Odame on 10/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_PERMISSIONDTO_H
#define CHOPMONEYRAFFLEAPI_PERMISSIONDTO_H
#include <json/json.h>
#include <string>

namespace raffle::dto {

    struct PermissionDto
    {
        std::string system_name;
        std::string display_name;

        void fromJson(const Json::Value& json) {
            if (json.isMember("systemName") && !json["systemName"].isNull()) {
                system_name = json["systemName"].asString();
            }

            if (json.isMember("displayName") && !json["displayName"].isNull()) {
                display_name = json["displayName"].asString();
            }
        }
    };

}
#endif //CHOPMONEYRAFFLEAPI_PERMISSIONDTO_H