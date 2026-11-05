#pragma once

#include <chrono>
#include <string>

namespace raffle::utils {

// Ticket list date filters are calendar dates, not timestamps.
inline bool isValidTicketDate(const std::string &date) {
  if (date.empty())
    return true;
  if (date.size() != 10 || date[4] != '-' || date[7] != '-')
    return false;
  for (size_t i = 0; i < date.size(); ++i) {
    if (i != 4 && i != 7 && (date[i] < '0' || date[i] > '9'))
      return false;
  }
  int year = std::stoi(date.substr(0, 4));
  unsigned month = std::stoi(date.substr(5, 2));
  unsigned day = std::stoi(date.substr(8, 2));
  return year > 0 && std::chrono::year_month_day{
                         std::chrono::year{year}, std::chrono::month{month},
                         std::chrono::day{day}}.ok();
}

inline std::string ticketDateStart(const std::string &date) {
  return date + " 00:00:00";
}

inline std::string ticketDateEnd(const std::string &date) {
  // PostgreSQL timestamps have microsecond precision.
  return date + " 23:59:59.999999";
}

} // namespace raffle::utils
