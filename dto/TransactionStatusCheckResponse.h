//
// Created by Emmanuel Addo-Odame on 24/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_TRANSACTIONSTATUSCHECKRESPONSE_H
#define CHOPMONEYRAFFLEAPI_TRANSACTIONSTATUSCHECKRESPONSE_H

#include <json/json.h>
#include <string>
#include <optional>

namespace raffle::dto {


class TransactionStatusCheckResponse {
public:
    struct Data {
        std::string date;
        std::string status;
        std::string transactionId;
        std::string externalTransactionId;
        std::string paymentMethod;
        std::string clientReference;
        std::optional<std::string> currencyCode;
        double amount = 0.0;
        double charges = 0.0;
        double amountAfterCharges = 0.0;
        std::optional<bool> isFulfilled;
    };

    std::string message;
    std::string responseCode;
    Data data;

    static TransactionStatusCheckResponse fromJson(const Json::Value &json) {
        TransactionStatusCheckResponse res;
        if (json.isMember("message")) res.message = json["message"].asString();
        if (json.isMember("responseCode")) res.responseCode = json["responseCode"].asString();
        
        if (json.isMember("data") && json["data"].isObject()) {
            const auto &d = json["data"];
            if (d.isMember("date")) res.data.date = d["date"].asString();
            if (d.isMember("status")) res.data.status = d["status"].asString();
            if (d.isMember("transactionId")) res.data.transactionId = d["transactionId"].asString();
            if (d.isMember("externalTransactionId")) res.data.externalTransactionId = d["externalTransactionId"].asString();
            if (d.isMember("paymentMethod")) res.data.paymentMethod = d["paymentMethod"].asString();
            if (d.isMember("clientReference")) res.data.clientReference = d["clientReference"].asString();
            if (d.isMember("currencyCode") && !d["currencyCode"].isNull()) {
                res.data.currencyCode = d["currencyCode"].asString();
            }
            if (d.isMember("amount")) res.data.amount = d["amount"].asDouble();
            if (d.isMember("charges")) res.data.charges = d["charges"].asDouble();
            if (d.isMember("amountAfterCharges")) res.data.amountAfterCharges = d["amountAfterCharges"].asDouble();
            if (d.isMember("isFulfilled") && !d["isFulfilled"].isNull()) {
                res.data.isFulfilled = d["isFulfilled"].asBool();
            }
        }
        return res;
    }

    Json::Value toJson() const {
        Json::Value json;
        json["message"] = message;
        json["responseCode"] = responseCode;
        Json::Value d;
        d["date"] = data.date;
        d["status"] = data.status;
        d["transactionId"] = data.transactionId;
        d["externalTransactionId"] = data.externalTransactionId;
        d["paymentMethod"] = data.paymentMethod;
        d["clientReference"] = data.clientReference;
        if (data.currencyCode.has_value()) {
            d["currencyCode"] = *data.currencyCode;
        } else {
            d["currencyCode"] = Json::Value::null;
        }
        d["amount"] = data.amount;
        d["charges"] = data.charges;
        d["amountAfterCharges"] = data.amountAfterCharges;
        if (data.isFulfilled.has_value()) {
            d["isFulfilled"] = *data.isFulfilled;
        } else {
            d["isFulfilled"] = Json::Value::null;
        }
        json["data"] = d;
        return json;
    }
};

}

#endif //CHOPMONEYRAFFLEAPI_TRANSACTIONSTATUSCHECKRESPONSE_H
