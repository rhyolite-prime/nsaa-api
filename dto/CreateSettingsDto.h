//
// Created by Emmanuel Addo-Odame on 12/03/2026.
//

#ifndef CHOPMONEYRAFFLEAPI_CREATESETTINGSDTO_H
#define CHOPMONEYRAFFLEAPI_CREATESETTINGSDTO_H

#include <json/json.h>

namespace raffle::dto {

    class CreateSettingsDto {

    public:

        CreateSettingsDto() = default;

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getBrandName() const { return brand_name_; }
        [[nodiscard]] const std::string& getDefaultCurrency() const { return default_currency_; }
        [[nodiscard]] const std::string& getSupportEmail() const { return support_email_; }
        [[nodiscard]] const std::string& getSupportPhone() const { return support_phone_; }
        [[nodiscard]] const std::string& getLogoUrl() const { return logo_url_; }
        [[nodiscard]] const std::string& getDrawBufferTimeInMins() const { return draw_buffer_time_in_mins_; }
        [[nodiscard]] const std::string& getSessionPersistenceInMins() const { return session_persistence_in_mins_; }
        [[nodiscard]] const std::string& getPricePoolPercentage() const { return price_pool_percentage_; }
        [[nodiscard]] const std::string& getSmtpHost() const { return smtp_host_; }
        [[nodiscard]] const std::string& getSmtpPort() const { return smtp_port_; }
        [[nodiscard]] const std::string& getMailUsername() const { return mail_username_; }
        [[nodiscard]] const std::string& getMailPassword() const { return mail_password_; }
        [[nodiscard]] const std::string& getMailSenderName() const { return mail_sender_name_; }
        [[nodiscard]] bool getAutomatedDrawInitiation() const { return automated_draw_initiation_; }

        // Setters
        void setBrandName(const std::string& value) { brand_name_ = value; }
        void setDefaultCurrency(const std::string& value) { default_currency_ = value; }
        void setSupportEmail(const std::string& value) { support_email_ = value; }
        void setSupportPhone(const std::string& value) { support_phone_ = value; }
        void setLogoUrl(const std::string& value) { logo_url_ = value; }
        void setDrawBufferTimeInMins(const std::string& value) { draw_buffer_time_in_mins_ = value; }
        void setSessionPersistenceInMins(const std::string& value) { session_persistence_in_mins_ = value; }
        void setPricePoolPercentage(const std::string& value) { price_pool_percentage_ = value; }
        void setSmtpHost(const std::string& value) { smtp_host_ = value; }
        void setSmtpPort(const std::string& value) { smtp_port_ = value; }
        void setMailUsername(const std::string& value) { mail_username_ = value; }
        void setMailPassword(const std::string& value) { mail_password_ = value; }
        void setMailSenderName(const std::string& value) { mail_sender_name_ = value; }
        void setAutomatedDrawInitiation(bool value) { automated_draw_initiation_ = value; }



    private:

        std::string brand_name_;
        std::string default_currency_;
        std::string support_email_;
        std::string support_phone_;
        std::string logo_url_;
        std::string draw_buffer_time_in_mins_;
        std::string session_persistence_in_mins_;
        std::string price_pool_percentage_;
        //smtp credentials
        std::string smtp_host_;
        std::string smtp_port_;
        std::string mail_username_;
        std::string mail_password_;
        std::string mail_sender_name_;
        bool automated_draw_initiation_ = false;

    };

   inline void CreateSettingsDto::fromJson(const Json::Value& json) {

        if (json.isMember("brandName") && !json["brandName"].isNull()) {
            brand_name_ = json["brandName"].asString();
        }

        if (json.isMember("defaultCurrency") && !json["defaultCurrency"].isNull()) {
            default_currency_ = json["defaultCurrency"].asString();
        }

        if (json.isMember("supportEmail") && !json["supportEmail"].isNull()) {
            support_email_ = json["supportEmail"].asString();
        }

        if (json.isMember("supportPhone") && !json["supportPhone"].isNull()) {
            support_phone_ = json["supportPhone"].asString();
        }

        if (json.isMember("logoUrl") && !json["logoUrl"].isNull()) {
            logo_url_ = json["logoUrl"].asString();
        }

        if (json.isMember("drawBufferTimeInMins") && !json["drawBufferTimeInMins"].isNull()) {
            draw_buffer_time_in_mins_ = json["drawBufferTimeInMins"].asString();
        }

        if (json.isMember("sessionPersistenceInMins") && !json["sessionPersistenceInMins"].isNull()) {
            session_persistence_in_mins_ = json["sessionPersistenceInMins"].asString();
        }

        if (json.isMember("pricePoolPercentage") && !json["pricePoolPercentage"].isNull()) {
            price_pool_percentage_ = json["pricePoolPercentage"].asString();
        }

        if (json.isMember("smtpHost") && !json["smtpHost"].isNull()) {
            smtp_host_ = json["smtpHost"].asString();
        }

        if (json.isMember("smtpPort") && !json["smtpPort"].isNull()) {
            smtp_port_ = json["smtpPort"].asString();
        }

        if (json.isMember("mailUsername") && !json["mailUsername"].isNull()) {
            mail_username_ = json["mailUsername"].asString();
        }

        if (json.isMember("mailPassword") && !json["mailPassword"].isNull()) {
            mail_password_ = json["mailPassword"].asString();
        }

        if (json.isMember("mailSenderName") && !json["mailSenderName"].isNull()) {
            mail_sender_name_ = json["mailSenderName"].asString();
        }

        if (json.isMember("automatedDrawInitiation") && !json["automatedDrawInitiation"].isNull()) {
            automated_draw_initiation_ = json["automatedDrawInitiation"].asBool();
        }
    }

}
#endif //CHOPMONEYRAFFLEAPI_CREATESETTINGSDTO_H