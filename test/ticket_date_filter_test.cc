#include "../utils/TicketDateFilter.h"
#include <iostream>

#define CHECK(condition) do { \
  if (!(condition)) { \
    std::cerr << "Failed: " #condition << "\n"; \
    return 1; \
  } \
} while (false)

int main() {
  using namespace raffle::utils;
  CHECK(isValidTicketDate(""));
  CHECK(isValidTicketDate("2024-02-29"));
  CHECK(isValidTicketDate("2026-12-31"));
  CHECK(!isValidTicketDate("2025-02-29"));
  CHECK(!isValidTicketDate("2026-04-31"));
  CHECK(!isValidTicketDate("0000-01-01"));
  CHECK(!isValidTicketDate("2026-13-01"));
  CHECK(!isValidTicketDate("2026-01-01T12:00:00"));
  CHECK(!isValidTicketDate("2026-1-01"));
  CHECK(!isValidTicketDate("not-a-date"));
  CHECK(ticketDateStart("2026-11-03") == "2026-11-03 00:00:00");
  CHECK(ticketDateEnd("2026-11-03") == "2026-11-03 23:59:59.999999");
}
