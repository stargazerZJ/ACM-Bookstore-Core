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
  BookSystem book_system_;
  UserSystem user_system_;
  FinanceLog finance_log_;
  UserLog user_log_;
 public:
  BookStore(std::string file_path = "");

  ~BookStore();

  void initialize(bool force_reset = false);

  bool login(const std::string &user_id, const std::string &password = "") const;

  bool logout();

  bool useradd(const std::string &user_id, const std::string &password, const std::string &name, int privilege);

  bool passwd(const std::string &user_id,
              const std::string &new_password,
              const std::string &old_password = "");

  bool deluser(const std::string &user_id);

  std::vector<Book> search(const Book &params) const;

  bool select(const std::string &ISBN);

  bool modify(const Book &new_book);

  bool import(
  int quantity,
  int cost
  );

  const FinanceLog *GetFinanceLog() const;

  const UserLog *GetUserLog() const;
};

#endif //BOOKSTORE_SRC_BOOKSTORE_H_
