//
// Created by Emmanuel Addo-Odame on 10/06/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_REWARDTYPES_H
#define CHOPMONEYRAFFLEAPI_REWARDTYPES_H
namespace raffle::constants {

    enum RewardType {
        CASH = 0,
        POINTS = 1,
        GIFT_CARD = 2,
        VOUCHER = 3,
        PHYSICAL_ITEM = 4,
        DIGITAL_ITEM = 5,
        SUBSCRIPTION = 6,
        SERVICE = 7,
        EXPERIENCE = 8,
        DONATION = 9,
        COUPON = 10,
        DISCOUNT = 11,
        AIRTIME = 12,
        DATA_BUNDLE = 13,
        MEMBERSHIP = 14,
        CONSOLATION = 15,
        HYBRID = 16
    };
}
#endif //CHOPMONEYRAFFLEAPI_REWARDTYPES_H