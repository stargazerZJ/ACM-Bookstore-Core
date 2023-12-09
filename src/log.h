//
// Created by zj on 11/30/2023.
//

#ifndef BOOKSTORE_SRC_LOG_H_
#define BOOKSTORE_SRC_LOG_H_

#include "external_memory.h"

enum class kExceptionType {
  K_SUCCESS,
  K_INVALID_COMMAND,
  K_INVALID_PARAMETER,
  K_USER_NOT_FOUND,
  K_WRONG_PASSWORD,
  K_PERMISSION_DENIED,
  K_NO_LOGIN_USER,
  K_USER_ALREADY_EXIST,
  K_USER_IS_LOGGED_IN,
  K_BOOK_NOT_FOUND,
  K_NO_SELECTED_BOOK,
  K_NOT_ENOUGH_INVENTORY,
  K_SAME_ISBN,
  K_DUPLICATED_ISBN,
  K_DUPLICATED_KEYWORDS,
  K_NOT_ENOUGH_RECORDS
};

std::string exceptionTypeToString(kExceptionType exception_type);

/// @brief The finance record
class FinanceRecord {
  unsigned long long int income_sum_ = 0; // in cents
  unsigned long long int expenditure_sum_ = 0; // in cents
 public:
  FinanceRecord() = default;
  static constexpr unsigned int byte_size() { return sizeof(unsigned long long int) * 2; }
  void toBytes(char *dest) const { std::memcpy(dest, this, byte_size()); }
  void fromBytes(const char *src) { std::memcpy(this, src, byte_size()); }
  explicit FinanceRecord(const char *bytes) { fromBytes(bytes); }
  FinanceRecord(unsigned long long int income_sum, unsigned long long int expenditure_sum)
      : income_sum_(income_sum), expenditure_sum_(expenditure_sum) {}
  [[nodiscard]] unsigned long long int income() const { return income_sum_; }
  [[nodiscard]] unsigned long long int expenditure() const { return expenditure_sum_; }
  [[nodiscard]] unsigned long long int balance() const { return income_sum_ - expenditure_sum_; }
  FinanceRecord operator-(const FinanceRecord &rhs) const;
  [[nodiscard]] bool valid() const { return income_sum_ != -1ull && expenditure_sum_ != -1ull; }
  /// @brief Log a transaction
  /// @param money The amount of money
  /// @details If `money` is positive, it is regarded as income, otherwise it is regarded as expenditure.
  /// @return FinanceRecord& The reference of the current FinanceRecord
  FinanceRecord &log(long long int money);
};

/**
 * @brief The finance log
 * @details The finance log is used to record the income and expenditure of the store.
 * @details The finance log is stored in external memory.
 * @attention `initialize` must be called before using the finance log.
 */
class FinanceLog {
 private:
  const std::string file_path_;
  external_memory::List<FinanceRecord, false> log_;
  FinanceRecord current_sum_ = {0, 0};
 public:
  /// \brief Construct a new FinanceLog object
  explicit FinanceLog(std::string file_path = "finance") : file_path_(std::move(file_path)), log_(file_path_) {};
  /// \brief Initialize the FinanceLog object
  /// \param reset Whether to reset the FinanceLog object
  /// \attention This function must be called before using any other functions.
  /// \attention This function must not be called twice.
  /// \attention If reset is true, all the information of the finance log will be lost.
  void initialize(bool reset = false);
  /// \brief Log a transaction
  /// \param money The amount of money
  /// @details If `money` is positive, it is regarded as income, otherwise it is regarded as expenditure.
  void log(long long int money);
  /// \brief Get the sum of the last `count` logs
  /// \param count The number of logs
  /// \return FinanceRecord The sum of the last `count` logs
  /// \attention If `count` is larger than the number of logs, return FinanceRecord(-1, -1)
  FinanceRecord sum(unsigned int count); // if `count` is larger than the number of logs, return FinanceRecord(-1, -1)
  /// \brief Get the sum of all the logs
  FinanceRecord sum();
};

class UserLog {
 private:
  const std::string file_path_;
  std::fstream log_;
 public:
  explicit UserLog(std::string file_path = "log") : file_path_(std::move(file_path) + ".csv") {};
  void initialize(bool reset = false);
};

#endif //BOOKSTORE_SRC_LOG_H_
