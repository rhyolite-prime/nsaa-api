#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class DisbursementsController : public drogon::HttpController<DisbursementsController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/disbursements";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(DisbursementsController::getAll, std::string(PREFIX) + "/get-all", Get, Options);
    ADD_METHOD_TO(DisbursementsController::issueBatchPayout, std::string(PREFIX) + "/issue-batch-payout", Post, Options);

    METHOD_LIST_END

    drogon::Task<HttpResponsePtr> getAll(HttpRequestPtr req);
    drogon::Task<HttpResponsePtr> issueBatchPayout(HttpRequestPtr req);

};
