#include "DisbursementsController.h"

#include "dto/BaseApiResponse.h"


Task<HttpResponsePtr> DisbursementsController::getAll(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Disbursement deleted successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}

Task<HttpResponsePtr> DisbursementsController::issueBatchPayout(HttpRequestPtr req)
{
    raffle::dto::BaseApiResponse response;
    response.success = true;

    response.message = "Disbursement deleted successfully";

    auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
    co_return resp;

}
