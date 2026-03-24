//
// Created by Emmanuel Addo-Odame on 24/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_DIRECTRECEIVEMONEYRESPONSE_H
#define CHOPMONEYRAFFLEAPI_DIRECTRECEIVEMONEYRESPONSE_H
#include <json/json.h>
#include <string>

namespace raffle {
namespace dto {

    class DirectReceiveMoneyResponse {
    public:
        struct Data {
            std::string transactionId;
            std::string description;
            std::string clientReference;
            double amount = 0.0;
            double charges = 0.0;
            double amountAfterCharges = 0.0;
            double amountCharged = 0.0;
            double deliveryFee = 0.0;
        };

        std::string message;
        std::string responseCode;
        Data data;

        static DirectReceiveMoneyResponse fromJson(const Json::Value &json) {
            DirectReceiveMoneyResponse res;
            if (json.isMember("Message")) res.message = json["Message"].asString();
            if (json.isMember("ResponseCode")) res.responseCode = json["ResponseCode"].asString();

            if (json.isMember("Data") && json["Data"].isObject()) {
                const auto &d = json["Data"];
                if (d.isMember("TransactionId")) res.data.transactionId = d["TransactionId"].asString();
                if (d.isMember("Description")) res.data.description = d["Description"].asString();
                if (d.isMember("ClientReference")) res.data.clientReference = d["ClientReference"].asString();
                if (d.isMember("Amount")) res.data.amount = d["Amount"].asDouble();
                if (d.isMember("Charges")) res.data.charges = d["Charges"].asDouble();
                if (d.isMember("AmountAfterCharges")) res.data.amountAfterCharges = d["AmountAfterCharges"].asDouble();
                if (d.isMember("AmountCharged")) res.data.amountCharged = d["AmountCharged"].asDouble();
                if (d.isMember("DeliveryFee")) res.data.deliveryFee = d["DeliveryFee"].asDouble();
            }
            return res;
        }

        Json::Value toJson() const {
            Json::Value json;
            json["Message"] = message;
            json["ResponseCode"] = responseCode;
            Json::Value d;
            d["TransactionId"] = data.transactionId;
            d["Description"] = data.description;
            d["ClientReference"] = data.clientReference;
            d["Amount"] = data.amount;
            d["Charges"] = data.charges;
            d["AmountAfterCharges"] = data.amountAfterCharges;
            d["AmountCharged"] = data.amountCharged;
            d["DeliveryFee"] = data.deliveryFee;
            json["Data"] = d;
            return json;
        }
    };

}
}
#endif //CHOPMONEYRAFFLEAPI_DIRECTRECEIVEMONEYRESPONSE_H