//
// Created by zj on 11/30/2023.
//

#ifndef BOOKSTORE_SRC_CLI_H_
#define BOOKSTORE_SRC_CLI_H_

#include <iostream>
#include "bookstore.h"
#include "parser.h"

class BookStoreCLI {
 private:
  const std::string file_path_;
  BookStore book_store_;
  std::istream &is = std::cin;
  std::ostream &os = std::cout;
  static inline constexpr char endl[] = "\n";
  using Args = std::vector<std::string>;
  using Func = kExceptionType (BookStoreCLI::*)(const Args &args);

  static std::string printMoney(unsigned long long int money);
  /// \brief Login
  /// \details `su [UserID] ([Password])?`
  kExceptionType su(const Args &args);
  /// \brief Logout
  /// \details `logout`
  kExceptionType logout(const Args &args);
  /// \brief Register
  /// \details `register [UserID] [Password] [UserName]`
  kExceptionType register_(const Args &args);
  /// \brief Change the password of a user
  /// \details `passwd [UserID] ([OldPassword])? [NewPassword]`
  kExceptionType passwd(const Args &args);
  /// \brief Add a new user
  /// \details `useradd [UserID] [Password] [Privilege] [UserName]`
  kExceptionType useradd(const Args &args);
  /// \brief Delete a user
  /// \details `delete [UserID]`
  kExceptionType delete_(const Args &args);
  /// \brief Search for books
  /// \details `show (-ISBN=[ISBN]|-name="[BookName]"|-author="[Author]"|-keyword="[Keyword]")?`
  kExceptionType show(const Args &args);
  /// \brief Purchase books
  /// \details `buy [ISBN] [Quantity]`
  kExceptionType buy(const Args &args);
  /// \brief Select a book
  /// \details `select [ISBN]`
  kExceptionType select(const Args &args);
  /// \brief Modify the information of a book
  /// \details `modify (-ISBN=[ISBN]|-name="[BookName]"|-author="[Author]"|-keyword="[Keyword]"|-price=[Price])+`
  kExceptionType modify(const Args &args);
  /// \brief Import books
  /// \details `import [Quantity]`
  kExceptionType import_(const Args &args);
  /// \brief Show the finance log
  /// \details `show finance ([Count])?`
  kExceptionType showFinance(const Args &args);
  /// \brief Invalid command
  kExceptionType invalidCommand(const Args &args);
  void runCommand(const Args &args, Func func);
 public:
  explicit BookStoreCLI(std::string file_path = "") : file_path_(std::move(file_path)) {}

  ~BookStoreCLI() = default;

  void initialize(bool force_reset = false);

  void run();
};

#endif //BOOKSTORE_SRC_CLI_H_