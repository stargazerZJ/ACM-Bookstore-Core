//
// Created by zj on 11/30/2023.
//

#include "log.h"
FinanceRecord &FinanceRecord::log(unsigned long long int money) {
  if (money > 0) income_sum_ += money;
  else expenditure_sum_ += money;
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
