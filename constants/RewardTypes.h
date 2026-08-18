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

    enum RaffleStatus {
        ACTIVE = 0,
        SCHEDULED = 1,
        DRAW_COMPLETED = 2,
        CLOSED = 3,
    };

    enum DrawStatus {
        IN_PROGRESS = 0,
        DRAW_SCHEDULED = 1,
        COMPLETED = 2,
        CANCELLED = 3,
    };

    enum TicketStatus {
        PENDING = 0,
        PAID = 1,
        FAILED = 2,
        REFUNDED = 3,
    };

    enum PaymentStatus {
        PAYMENT_INITIATED = 0,
        SUCCESS = 1,
        PAYMENT_FAILED = 2,
    };

}
#endif //CHOPMONEYRAFFLEAPI_REWARDTYPES_H