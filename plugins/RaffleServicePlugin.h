/**
 *
 *  RaffleServicePlugin.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>

#include "services/colors/ColorService.h"
#include "services/customers/CustomerService.h"
#include "services/dashboard/DashboardService.h"
#include "services/draws/DrawService.h"
#include "services/price_tiers/PriceTierService.h"
#include "services/raffles/RaffleService.h"
#include "services/rng_logs/RngLogService.h"
#include "services/roles/RoleService.h"
#include "services/settings/SettingService.h"
#include "services/tickets/TicketService.h"
#include "services/transaction_audit/TransactionAuditService.h"
#include "services/users/UserService.h"

namespace raffle::plugins {

    class RaffleServicePlugin : public drogon::Plugin<RaffleServicePlugin>
    {
    public:
        RaffleServicePlugin() = default;
        ~RaffleServicePlugin() override = default;
        /// This method must be called by drogon to initialize and start the plugin.
        /// It must be implemented by the user.
        void initAndStart(const Json::Value &config) override;

        /// This method must be called by drogon to shutdown the plugin.
        /// It must be implemented by the user.
        void shutdown() override;

        services::ColorService &getColorService() { return colorService_; }
        services::CustomerService &getCustomerService() { return customerService_; }
        services::DashboardService &getDashboardService() { return dashboardService_; }
        services::PriceTierService &getPriceTierService() { return priceTierService_; }
        services::RngLogService &getRngLogService() { return rngLogService_; }
        services::RoleService &getRoleService() { return roleService_; }
        services::SettingService &getSettingService() { return settingService_; }
        services::TicketService &getTicketService() { return ticketService_; }
        services::TransactionAuditService &getTransactionAuditService() { return transactionAuditService_; }
        services::UserService &getUserService() { return userService_; }
        services::DrawService &getDrawService() { return drawService_; }
        services::RaffleService &getRaffleService() { return raffleService_; }

    private:
        services::DrawService drawService_;
        services::RaffleService raffleService_;
        services::ColorService colorService_;
        services::CustomerService customerService_;
        services::DashboardService dashboardService_;
        services::PriceTierService priceTierService_;
        services::RngLogService rngLogService_;
        services::RoleService roleService_;
        services::SettingService settingService_;
        services::TicketService ticketService_;
        services::TransactionAuditService transactionAuditService_;
        services::UserService userService_;
    };

}
