//
// Created by Emmanuel Addo-Odame on 23/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CHECKOUTRESPONSE_H
#define CHOPMONEYRAFFLEAPI_CHECKOUTRESPONSE_H
#include <json/json.h>
#include <string>

namespace raffle::dto {

class CheckoutResponse {
public:
    struct Data {
        std::string checkoutUrl;
        std::string checkoutId;
        std::string clientReference;
        std::string message;
        std::string checkoutDirectUrl;
    };

    std::string responseCode;
    std::string status;
    Data data;

    static CheckoutResponse fromJson(const Json::Value &json) {
        CheckoutResponse res;
        if (json.isMember("responseCode")) res.responseCode = json["responseCode"].asString();
        if (json.isMember("status")) res.status = json["status"].asString();
        
        if (json.isMember("data") && json["data"].isObject()) {
            const auto &d = json["data"];
            if (d.isMember("checkoutUrl")) res.data.checkoutUrl = d["checkoutUrl"].asString();
            if (d.isMember("checkoutId")) res.data.checkoutId = d["checkoutId"].asString();
            if (d.isMember("clientReference")) res.data.clientReference = d["clientReference"].asString();
            if (d.isMember("message")) res.data.message = d["message"].asString();
            if (d.isMember("checkoutDirectUrl")) res.data.checkoutDirectUrl = d["checkoutDirectUrl"].asString();
        }
        return res;
    }

    Json::Value toJson() const {
        Json::Value json;
        json["responseCode"] = responseCode;
        json["status"] = status;
        Json::Value d;
        d["checkoutUrl"] = data.checkoutUrl;
        d["checkoutId"] = data.checkoutId;
        d["clientReference"] = data.clientReference;
        d["message"] = data.message;
        d["checkoutDirectUrl"] = data.checkoutDirectUrl;
        json["data"] = d;
        return json;
    }
};

}
#endif //CHOPMONEYRAFFLEAPI_CHECKOUTRESPONSE_H