//
// Created by zj on 11/30/2023.
//

#include "log.h"
FinanceRecord &FinanceRecord::log(long long int money) {
  if (money > 0) income_sum_ += money;
  else expenditure_sum_ += -money;
  return *this;
}
FinanceRecord FinanceRecord::operator-(const FinanceRecord &rhs) const {
  return {income_sum_ - rhs.income_sum_, expenditure_sum_ - rhs.expenditure_sum_};
}
void FinanceLog::initialize(bool reset) {
  log_.initialize(reset);
  if (reset) {
    current_sum_ = {0, 0};
    log_.insert(current_sum_);
  } else {
    current_sum_ = log_.get(log_.size());
  }
}
void FinanceLog::log(long long int money) {
  current_sum_.log(money);
  log_.insert(current_sum_);
}
FinanceRecord FinanceLog::sum(unsigned int count) {
  if (count > log_.size() - 1) return {static_cast<unsigned long long>(-1), static_cast<unsigned long long>(-1)};
  FinanceRecord record = log_.get(log_.size() - count);
  return current_sum_ - record;
}
FinanceRecord FinanceLog::sum() {
  return current_sum_;
}
void UserLog::initialize(bool reset) {
  if (reset) {
    log_.open(file_path_, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
  } else {
    log_.open(file_path_, std::ios::in | std::ios::out | std::ios::binary);
  }
  if (!log_.is_open()) {
    throw std::runtime_error("Cannot open file " + file_path_);
  }
}
std::string exceptionTypeToString(kExceptionType exception_type) {
  switch (exception_type) {
    case kExceptionType::K_SUCCESS: return "Success";
    case kExceptionType::K_INVALID_COMMAND: return "Invalid command";
    case kExceptionType::K_INVALID_PARAMETER: return "Invalid parameter";
    case kExceptionType::K_USER_NOT_FOUND: return "User not found";
    case kExceptionType::K_WRONG_PASSWORD: return "Wrong password";
    case kExceptionType::K_PERMISSION_DENIED: return "Permission denied";
    case kExceptionType::K_NO_LOGIN_USER: return "No login user";
    case kExceptionType::K_USER_ALREADY_EXIST: return "User already exists";
    case kExceptionType::K_USER_IS_LOGGED_IN: return "User is logged in";
    case kExceptionType::K_BOOK_NOT_FOUND: return "Book not found";
    case kExceptionType::K_NO_SELECTED_BOOK: return "No selected book";
    case kExceptionType::K_NOT_ENOUGH_INVENTORY: return "Not enough inventory";
    case kExceptionType::K_SAME_ISBN: return "Same ISBN";
    case kExceptionType::K_DUPLICATED_ISBN: return "Duplicated ISBN";
    case kExceptionType::K_DUPLICATED_KEYWORDS: return "Duplicated keywords";
    case kExceptionType::K_NOT_ENOUGH_RECORDS: return "Not enough records";
    default: return "Unknown exception type";
  }
}
