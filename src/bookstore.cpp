//
// Created by zj on 11/29/2023.
//

#include "bookstore.h"
void BookStore::initialize(bool force_reset) {
  bool reset = force_reset;
  if (!force_reset) {
    std::ifstream file(file_path_ + "_data_data.db"); // a file that belongs to the `Vectors` class
    reset = !file.is_open(); // if the file does not exist or cannot be opened, reset the database
  }
  vectors_.initialize(reset);
  book_system_.initialize(reset);
  user_system_.initialize(reset);
  finance_log_.initialize(reset);
}
kExceptionType BookStore::login(const std::string &user_id, const std::string &password) {
  return user_system_.login(user_id, password);
}
kExceptionType BookStore::logout() {
  return user_system_.logout();
}
kExceptionType BookStore::useradd(const std::string &user_id,
                                  const std::string &password,
                                  const std::string &name,
                                  int privilege) {
  if (user_system_.getPrivilege() <= privilege)
    return kExceptionType::K_PERMISSION_DENIED; // privilege check: the privilege of the current user must be greater than the privilege of the user to be added
  return user_system_.useradd(user_id, password, name, privilege);
}
kExceptionType BookStore::customerUseradd(const std::string &user_id,
                                          const std::string &password,
                                          const std::string &name) {
  // this command does not require privilege check
  return useradd(user_id, password, name, 1);
}
kExceptionType BookStore::passwd(const std::string &user_id,
                                 const std::string &new_password,
                                 const std::string &old_password) {
  if (old_password.empty() && user_system_.getPrivilege() < 7)
    return kExceptionType::K_PERMISSION_DENIED; // privilege check: if the old password is not provided, the privilege of the current user must be 7
  return user_system_.passwd(user_id, new_password, old_password);
}
kExceptionType BookStore::deluser(const std::string &user_id) {
  if (user_system_.getPrivilege() < 7)
    return kExceptionType::K_PERMISSION_DENIED; // privilege check: the privilege of the current user must be 7
  return user_system_.deluser(user_id);
}
std::pair<kExceptionType, std::vector<Book>> BookStore::search(const Book &params) {
  if (user_system_.getPrivilege() < 1)
    return {kExceptionType::K_PERMISSION_DENIED,
            std::vector<Book>()}; // privilege check: the privilege of the current user must be greater than 1
  return {kExceptionType::K_SUCCESS, book_system_.search(params)};
}
kExceptionType BookStore::select(const std::string &ISBN) {
  if (user_system_.getPrivilege() < 3)
    return kExceptionType::K_PERMISSION_DENIED; // privilege check: the privilege of the current user must be greater than 3
  auto id = book_system_.find(ISBN);
  if (!id) return kExceptionType::K_BOOK_NOT_FOUND;
  return user_system_.select(id);
}
kExceptionType BookStore::modify(Book &&new_book) {
  if (user_system_.getPrivilege() < 3)
    return kExceptionType::K_PERMISSION_DENIED; // privilege check: the privilege of the current user must be greater than 3
  auto selected_id = user_system_.getSelectedId();
  if (!selected_id) return kExceptionType::K_NO_SELECTED_BOOK;
  Book old_book = book_system_.get(selected_id);
  if (new_book.ISBN.empty()) {
    new_book.ISBN = old_book.ISBN;
  } else {
    if (new_book.ISBN == old_book.ISBN) return kExceptionType::K_SAME_ISBN;
  }
  if (new_book.title.empty()) {
    new_book.title = old_book.title;
  }
  if (new_book.author.empty()) {
    new_book.author = old_book.author;
  }
  if (new_book.keywords.empty()) {
    new_book.keywords = old_book.keywords;
  }
  if (new_book.price == 0) {
    new_book.price = old_book.price;
  }
  new_book.quantity = old_book.quantity; // the quantity cannot be modified by this command
  return book_system_.modify(selected_id, old_book, new_book);
}
kExceptionType BookStore::import_(int quantity, int cost) {
  if (user_system_.getPrivilege() < 3)
    return kExceptionType::K_PERMISSION_DENIED; // privilege check: the privilege of the current user must be greater than 3
  auto selected_id = user_system_.getSelectedId();
  if (!selected_id) return kExceptionType::K_NO_SELECTED_BOOK;
  Book book = book_system_.get(selected_id);
  Book new_book = book;
  new_book.quantity += quantity;
  finance_log_.log(-cost);
  return book_system_.modify(selected_id, book, new_book);
}
std::pair<kExceptionType, unsigned long long> BookStore::purchase(const std::string &ISBN, int quantity) {
  if (user_system_.getPrivilege() < 1)
    return {kExceptionType::K_PERMISSION_DENIED,
            0}; // privilege check: the privilege of the current user must be greater than 1
  auto id = book_system_.find(ISBN);
  if (!id) return {kExceptionType::K_BOOK_NOT_FOUND, 0};
  Book book = book_system_.get(id);
  if (book.quantity < quantity) return {kExceptionType::K_NOT_ENOUGH_INVENTORY, 0};
  Book new_book = book;
  new_book.quantity -= quantity;
  finance_log_.log(static_cast<long long>(book.price) * quantity);
  return {book_system_.modify(id, book, new_book), static_cast<unsigned long long>(book.price) * quantity};
}
std::pair<kExceptionType, FinanceRecord> BookStore::showFinance(int count) {
  if (user_system_.getPrivilege() < 7)
    return {kExceptionType::K_PERMISSION_DENIED,
            FinanceRecord()}; // privilege check: the privilege of the current user must be greater than 7
  auto finance_log = finance_log_.sum(count);
  if (!finance_log.valid()) return {kExceptionType::K_NOT_ENOUGH_RECORDS, FinanceRecord()};
  return {kExceptionType::K_SUCCESS, finance_log};
}
std::pair<kExceptionType, const FinanceRecord &> BookStore::showFinance() {
  if (user_system_.getPrivilege() < 7)
    return {kExceptionType::K_PERMISSION_DENIED,
            FinanceRecord()}; // privilege check: the privilege of the current user must be greater than 7
  auto finance_log = finance_log_.sum();
  if (!finance_log.valid()) return {kExceptionType::K_NOT_ENOUGH_RECORDS, FinanceRecord()};
  return {kExceptionType::K_SUCCESS, finance_log};
}
