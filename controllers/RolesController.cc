#include "RolesController.h"

#include "dto/BaseApiResponse.h"

Task<HttpResponsePtr> RolesController::getAll(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Roles retrieved successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;
}

Task<HttpResponsePtr> RolesController::create(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Role saved successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;
}

Task<HttpResponsePtr> RolesController::update(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Role updated successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;
}


Task<HttpResponsePtr> RolesController::deleteRole(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Role deleted successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;
}