/**
 *
 *  RaffleServicePlugin.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>

#include "services/affiliate_partners/AffiliatePartnerService.h"
#include "services/banners/BannerService.h"
#include "services/colors/ColorService.h"
#include "services/customers/CustomerService.h"
#include "services/dashboard/DashboardService.h"
#include "services/draws/DrawService.h"
#include "services/faqs/FaqService.h"
#include "services/hero_sliders/HeroSliderService.h"
#include "services/hubtel_api/HubtelApi.h"
#include "services/media_library/MediaLibraryService.h"
#include "services/pages/PageService.h"
#include "services/prize_config/PrizeConfigService.h"
#include "services/quartz/QuartzApi.h"
#include "services/raffles/RaffleService.h"
#include "services/redis/RedisCacheManager.h"
#include "services/rng_logs/RngLogService.h"
#include "services/roles/RoleService.h"
#include "services/settings/SettingService.h"
#include "services/support_ticket/SupportTicketService.h"
#include "services/tickets/TicketService.h"
#include "services/ticket_collection/TicketCollectionService.h"
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

        services::RedisCacheManager &getRedisCacheManager() { return redisCacheService_; }
        services::HubtelApi &getHubtelApi() { return hubtelApi_; }
        services::ColorService &getColorService() { return colorService_; }
        services::CustomerService &getCustomerService() { return customerService_; }
        services::DashboardService &getDashboardService() { return dashboardService_; }
        services::RngLogService &getRngLogService() { return rngLogService_; }
        services::RoleService &getRoleService() { return roleService_; }
        services::SettingService &getSettingService() { return settingService_; }
        services::TicketService &getTicketService() { return ticketService_; }
        services::TransactionAuditService &getTransactionAuditService() { return transactionAuditService_; }
        services::UserService &getUserService() { return userService_; }
        services::DrawService &getDrawService() { return drawService_; }
        services::RaffleService &getRaffleService() { return raffleService_; }
        services::QuartzApi &getQuartzApi() { return quartzApi_; }
        services::AffiliatePartnerService &getAffiliatePartnerService() { return affiliatePartnerService_; }
        services::BannerService &getBannerService() { return bannerService_; }
        services::FaqService &getFaqService() { return faqService_; }
        services::HeroSliderService &getHeroSliderService() { return heroSliderService_; }
        services::MediaLibraryService &getMediaLibraryService() { return mediaLibraryService_; }
        services::PageService &getPageService() { return pageService_; }
        services::SupportTicketService &getSupportTicketService() { return supportTicketService_; }
        services::PrizeConfigService &getPrizeConfigService() { return prizeConfigService_; }
        services::TicketCollectionService &getTicketCollectionService() { return ticketCollectionService_; }

    private:
        services::RedisCacheManager redisCacheService_;
        services::HubtelApi hubtelApi_;
        services::DrawService drawService_;
        services::RaffleService raffleService_;
        services::ColorService colorService_;
        services::CustomerService customerService_;
        services::DashboardService dashboardService_;
        services::RngLogService rngLogService_;
        services::RoleService roleService_;
        services::SettingService settingService_;
        services::TicketService ticketService_;
        services::TransactionAuditService transactionAuditService_;
        services::UserService userService_;
        services::QuartzApi quartzApi_;
        services::AffiliatePartnerService affiliatePartnerService_;
        services::BannerService bannerService_;
        services::FaqService faqService_;
        services::HeroSliderService heroSliderService_;
        services::MediaLibraryService mediaLibraryService_;
        services::PageService pageService_;
        services::SupportTicketService supportTicketService_;
        services::PrizeConfigService prizeConfigService_;
        services::TicketCollectionService ticketCollectionService_;
    };

}
