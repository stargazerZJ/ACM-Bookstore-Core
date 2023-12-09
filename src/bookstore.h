//
// Created by zj on 11/29/2023.
//

#ifndef BOOKSTORE_SRC_BOOKSTORE_H_
#define BOOKSTORE_SRC_BOOKSTORE_H_

#include "book_system.h"
#include "user_system.h"
#include "log.h"

class BookStore {
 private:
  const std::string file_path_;
  external_memory::Vectors vectors_;
  BookSystem book_system_;
  UserSystem user_system_;
  FinanceLog finance_log_;
 public:
  explicit BookStore(std::string file_path = "bookstore") : file_path_(std::move(file_path)),
                                                            vectors_(file_path_ + "_data"),
                                                            book_system_(file_path_ + "book", vectors_),
                                                            user_system_(file_path_ + "_user"),
                                                            finance_log_(file_path_ + "_finance_log") {}

  ~BookStore() = default;

  void initialize(bool force_reset = false);

  kExceptionType login(const std::string &user_id, const std::string &password = "");

  kExceptionType logout();

  kExceptionType useradd(const std::string &user_id,
                         const std::string &password,
                         const std::string &name,
                         int privilege);
  kExceptionType touristUseradd(const std::string &user_id, const std::string &password, const std::string &name);

  kExceptionType passwd(const std::string &user_id,
                        const std::string &new_password,
                        const std::string &old_password = "");

  kExceptionType deluser(const std::string &user_id);

  std::pair<kExceptionType, std::vector<Book>> search(const Book &params);

  kExceptionType select(const std::string &ISBN);

  kExceptionType modify(Book &&new_book);

  kExceptionType import_(int quantity, int cost);

  std::pair<kExceptionType, unsigned long long> purchase(const std::string &ISBN, int quantity);

  std::pair<kExceptionType, FinanceRecord> showFinance(int count);
  std::pair<kExceptionType, const FinanceRecord &> showFinance();
};

#endif //BOOKSTORE_SRC_BOOKSTORE_H_
