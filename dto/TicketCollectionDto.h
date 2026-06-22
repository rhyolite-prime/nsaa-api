//
// Created by Emmanuel Addo-Odame on 18/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_TICKETCOLLECTIONDTO_H
#define CHOPMONEYRAFFLEAPI_TICKETCOLLECTIONDTO_H
#include <drogon/drogon.h>
#include <string>
#include <json/json.h>

namespace raffle::dto {

    class TicketCollectionDto {

        TicketCollectionDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getSubject() const { return subject_; }


        // Setters
        void setName(const std::string& value) { name_ = value; }
        void setSubject(const std::string& value) { subject_ = value; }

    private:

        std::string name_;
        int quantity_;
        int price_per_ticket_;

    };

    inline void TicketCollectionDto::fromJson(const Json::Value& json) {

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }


    }


}
#endif //CHOPMONEYRAFFLEAPI_TICKETCOLLECTIONDTO_H