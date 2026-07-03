/**
 *
 *  CustomerJwtAuthFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;


class CustomerJwtAuthFilter : public HttpFilter<CustomerJwtAuthFilter>
{
  public:
    CustomerJwtAuthFilter() {}
    void doFilter(const HttpRequestPtr &req,
                  FilterCallback &&fcb,
                  FilterChainCallback &&fccb) override;
};

