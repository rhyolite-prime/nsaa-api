cmake_minimum_required(VERSION 3.10)
project(ChopMoneyRaffleApi CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fcoroutines)
endif()

include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)
if (NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
    # Do nothing
elseif (HAS_ANY AND HAS_STRING_VIEW AND HAS_COROUTINE)
    set(CMAKE_CXX_STANDARD 20)
elseif (HAS_ANY AND HAS_STRING_VIEW)
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 14)
endif ()



add_executable(${PROJECT_NAME} main.cc
        plugins/RaffleServicePlugin.h
        filters/CustomerJwtAuthFilter.h
        filters/CustomerJwtAuthFilter.cc
        controllers/AuthController.h
        services/colors/ColorService.h
        dto/BaseApiResponse.h
        dto/CreateColorDto.h
        dto/UpdateColorDto.h
        services/price_tiers/PriceTierService.h
        services/roles/RoleService.h
        services/settings/SettingService.h
        services/users/UserService.h
        services/tickets/TicketService.h
        services/dashboard/DashboardService.h
        services/customers/CustomerService.h
        services/transaction_audit/TransactionAuditService.h
        services/winners/WinnerService.h
        services/rng_logs/RngLogService.h
        constants/ErrorCodes.h
        services/colors/ColorService.cc
        services/customers/CustomerService.cc
        services/price_tiers/PriceTierService.cc
        services/draws/DrawService.h
        services/draws/DrawService.cc
        services/tickets/TicketService.cc
        services/raffles/RaffleService.h
        services/raffles/RaffleService.cc
        services/hubtel_api/HubtelApi.h
        dto/CreateRaffleDto.h
        dto/UpdateRaffleDto.h
        services/quartz/QuartzApi.h
        dto/QuartzJobDto.h
        services/quartz/QuartzApi.cc
        dto/CreateTicketDto.h
        utils/IdGeneratorUtils.h
        utils/IdGeneratorUtils.cc
        utils/CronUtils.h
        utils/CronUtils.cc
        utils/DateUtils.h
        utils/DateUtils.cc
        utils/StringUtils.h
        utils/StringUtils.cc
        controllers/PaymentCallbackController.h
        services/settings/SettingService.cc
        dto/CreateUserDto.h
        dto/UpdateUserDto.h
        services/users/UserService.cc
        dto/CreateRoleDto.h
        dto/UpdateRoleDto.h
        services/roles/RoleService.cc
        dto/PermissionDto.h
        dto/CreateSettingsDto.h
        dto/WebCheckoutDto.h
        dto/DirectReceiveMoneyDto.h
        services/hubtel_api/HubtelApi.cc
        dto/CheckoutResponse.h
        dto/DirectReceiveMoneyResponse.h
        controllers/HubtelCallbackProcessorController.h
        services/email/EmailService.h
        dto/SendEmailDto.h
        services/email/EmailService.cc
        services/affiliate_partners/AffiliatePartnerService.h
        services/affiliate_partners/AffiliatePartnerService.cc
        dto/AffiliatePartnerDto.h
        utils/DatabaseUtils.cc
        services/infobip/InfobipApi.h
        services/infobip/InfobipApi.cc
        services/faqs/FaqService.h
        services/hero_sliders/HeroSliderService.h
        services/media_library/MediaLibraryService.h
        services/pages/PageService.h
        services/banners/BannerService.h
        dto/AffiliatePartnerDraftDto.h
        services/faqs/FaqService.cc
        services/banners/BannerService.cc
        services/hero_sliders/HeroSliderService.cc
        services/media_library/MediaLibraryService.cc
        services/pages/PageService.cc
        utils/codecvt_helper.h
        dto/HeroSliderDto.h
        dto/SupportTicketDto.h
        services/support_ticket/SupportTicketService.h
        services/support_ticket/SupportTicketService.cc
        constants/RewardTypes.h
        dto/CustomerAuthDto.h
        utils/PasswordUtils.h
        utils/PasswordUtils.cc
        services/redis/RedisCacheManager.h
        services/redis/RedisCacheManager.cc
        dto/CustomerOtpDto.h
        dto/CustomerVerifyOtpDto.h
        services/ticket_collection/TicketCollectionService.h
        dto/TicketCollectionDto.h
        dto/InitializeTicketPurchaseDto.h
        dto/ChangeCustomerPinDto.h
        services/ticket_collection/TicketCollectionService.cc

)

# ##############################################################################
# If you include the drogon source code locally in your project, use this method
# to add drogon 
# add_subdirectory(drogon) 
# target_link_libraries(${PROJECT_NAME} PRIVATE drogon)
#
# and comment out the following lines
find_package(Drogon CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Drogon::Drogon
        bcrypt
        jwt-cpp
        OpenSSL::SSL
        OpenSSL::Crypto
)

# ##############################################################################

if (CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "c++17 or higher is required")
elseif (CMAKE_CXX_STANDARD LESS 20)
    message(STATUS "use c++17")
else ()
    message(STATUS "use c++20")
endif ()

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)

drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                    ${CMAKE_CURRENT_BINARY_DIR})
# use the following line to create views with namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE)
# use the following line to create views with namespace CHANGE_ME prefixed
# and path namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE CHANGE_ME)



target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external_deps/jwt-cpp/include
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/models
)
target_sources(${PROJECT_NAME}
               PRIVATE
               ${SRC_DIR}
               ${CTL_SRC}
               ${FILTER_SRC}
               ${PLUGIN_SRC}
               ${SERVICE_SRC}
               ${MODEL_SRC})
# ##############################################################################
# uncomment the following line for dynamically loading views 
# set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)

# ##############################################################################

target_compile_options(${PROJECT_NAME} PRIVATE "-include" "${CMAKE_CURRENT_SOURCE_DIR}/utils/codecvt_helper.h" "-Wno-deprecated-declarations")

set(JWT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(JWT_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(test)
add_subdirectory(external_deps/Bcrypt)
add_subdirectory(external_deps/jwt-cpp)
