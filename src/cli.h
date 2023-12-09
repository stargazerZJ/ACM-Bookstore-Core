//
// Created by zj on 11/30/2023.
//

#ifndef BOOKSTORE_SRC_CLI_H_
#define BOOKSTORE_SRC_CLI_H_

#include <iostream>
#include "bookstore.h"
#include "parser.h"

/**
 * @brief The BookStoreCLI class
 * @details The BookStoreCLI class is used to interact with users.
 * @details The BookStoreCLI class is used to parse commands and call the corresponding functions.
 * @details The BookStoreCLI class is used to print the results.
 * @attention `initialize` must be called before using the BookStoreCLI.
 */
class BookStoreCLI {
 private:
  const std::string file_path_; // the path of the database files
  BookStore book_store_; // the bookstore
  std::istream &is = std::cin;
  std::ostream &os = std::cout;
  static inline constexpr char endl[] = "\n"; // use "\n" instead of std::endl to increase the speed
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
  /// \details `import [Quantity] [TotalCost]`
  kExceptionType import_(const Args &args);
  /// \brief Show the finance log
  /// \details `show finance ([Count])?`
  kExceptionType showFinance(const Args &args);
  /// \brief Invalid command
  kExceptionType invalidCommand(const Args &args);
  void runCommand(const Args &args, Func func);
 public:
  /// \brief Construct a new BookStoreCLI object
  explicit BookStoreCLI(std::string file_path = "") : file_path_(std::move(file_path)), book_store_(file_path_ + "bookstore") {}
  /// \brief Destroy the BookStoreCLI object
  ~BookStoreCLI() = default;
  /// \brief Initialize the BookStoreCLI object
  /// \param force_reset Whether to reset the BookStoreCLI object
  /// \attention This function must be called before using any other functions.
  /// \attention This function must not be called twice.
  void initialize(bool force_reset = false);
  /// \brief Run the BookStoreCLI object
  /// \details This function will keep reading commands from `is` and print the results to `os` until `exit`, `quit` or EOF is read.
  void run();
};

#endif //BOOKSTORE_SRC_CLI_H_