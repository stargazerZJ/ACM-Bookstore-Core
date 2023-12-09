//
// Created by zj on 11/29/2023.
//

#ifndef BOOKSTORE_SRC_BOOKSTORE_H_
#define BOOKSTORE_SRC_BOOKSTORE_H_

#include "book_system.h"
#include "user_system.h"
#include "validator.h"
#include "log.h"

/**
 * @brief BookStore class
 * @details The BookStore class is used to manage the bookstore.
 * @details The BookStore class provides the following functions:
 * @details 1. login: login with user ID and password
 * @details 2. logout: logout
 * @details 3. useradd: add a new user
 * @details 4. passwd: change the password of a user
 * @details 5. deluser: delete a user
 * @details 6. search: search for books
 * @details 7. select: select a book
 * @details 8. modify: modify the information of a book
 * @details 9. import: import books
 * @details 10. purchase: purchase books
 * @details 11. showFinance: show the finance log
 * @details This class checks privileges if the check only involves the current user.
 * @details For example, if the current user is a tourist, the privilege check of `search` will fail.
 * @details However, if the check involves two users, the privilege check will not be performed.
 * @details For example, `useradd` requires the privilege of the current user to be greater than the privilege of the user to be added.
 * @details This check is done by the `UserSystem` class.
 * @details This class also performs parameter checks.
 * @attention `initialize` should be called before using this class.
 * @attention Parameter checks should be performed by the caller if the check does not involve the information stored in the database.
 * @attention For example, `BookName` should not contain '"', which should be checked by the caller.
 */
class BookStore {
 private:
  const std::string file_prefix_; // the prefix (including path) of the files storing the information of books
  external_memory::Vectors vectors_; // the vectors used by external memory, shared with other systems
  BookSystem book_system_; // the book system
  UserSystem user_system_; // the user system
  FinanceLog finance_log_; // the finance log
 public:
  /// \brief Construct a new BookStore object
  explicit BookStore(std::string file_prefix = "bookstore") : file_prefix_(std::move(file_prefix)),
                                                              vectors_(file_prefix_ + "_data"),
                                                              book_system_(file_prefix_ + "_book", vectors_),
                                                              user_system_(file_prefix_ + "_user"),
                                                              finance_log_(file_prefix_ + "_finance_log") {}
  /// \brief Destroy the BookStore object
  ~BookStore() = default;
  /**
   * @brief Initialize the BookStore object
   * @details This function initializes the BookStore object and the external memory.
   * @details If `force_reset` is false, the database will not be reset if it exists.
   * @param force_reset Whether to reset the BookStore object
   * @attention This function must be called before using any other functions.
   * @attention This function must not be called twice.
   * @attention If force_reset is true, all the information of the bookstore will be lost.
   * @attention If force_reset is false but the database is corrupted, the behavior is undefined.
   */
  void initialize(bool force_reset = false);
  /**
   * @brief Login with user ID and password
   * @param user_id
   * @param password
   * @return kExceptionType
   * @return K_SUCCESS if login successfully
   * @return K_USER_NOT_FOUND if the user is not found
   * @return K_WRONG_PASSWORD if the password is provided but wrong
   * @return K_PERMISSION_DENIED if the password is not provided but the privilege of the current user is less than the privilege of the user
   * @return K_INVALID_PARAMETER if the user ID or password is invalid
   */
  kExceptionType login(const std::string &user_id, const std::string &password = "");
  /**
   * @brief Logout
   * @return kExceptionType
   * @return K_SUCCESS if logout successfully
   * @return K_NO_LOGIN_USER if no user is logged in
   */
  kExceptionType logout();
  /**
   * @brief Add a new user
   * @param user_id
   * @param password
   * @param name
   * @param privilege
   * @return kExceptionType
   * @return K_SUCCESS if add successfully
   * @return K_USER_ALREADY_EXISTS if the user already exists
   * @return K_PERMISSION_DENIED if the privilege of the current user is less than the privilege of the user to be added
   * @return K_INVALID_PARAMETER if the user ID, password, name or privilege is invalid
   */
  kExceptionType useradd(const std::string &user_id,
                         const std::string &password,
                         const std::string &name,
                         unsigned int privilege);
  /**
   * @brief Add a new customer user
   * @details A customer user is a user with privilege 1.
   * @details This command does not require privilege check.
   * @param user_id
   * @param password
   * @param name
   * @return kExceptionType
   * @return K_SUCCESS if add successfully
   * @return K_USER_ALREADY_EXISTS if the user already exists
   * @return K_INVALID_PARAMETER if the user ID, password or name is invalid
   */
  kExceptionType customerUseradd(const std::string &user_id, const std::string &password, const std::string &name);
  /**
   * @brief Change the password of a user
   * @param user_id
   * @param new_password
   * @param old_password
   * @return kExceptionType
   * @return K_SUCCESS if change successfully
   * @return K_USER_NOT_FOUND if the user is not found
   * @return K_WRONG_PASSWORD if the old password is provided but wrong
   * @return K_PERMISSION_DENIED if the old password is not provided but the privilege of the current user is less than 7
   * @return K_INVALID_PARAMETER if the user ID, new password or old password is invalid
   */
  kExceptionType passwd(const std::string &user_id,
                        const std::string &new_password,
                        const std::string &old_password = "");
  /**
   * @brief Delete a user
   * @param user_id
   * @return K_SUCCESS if delete successfully
   * @return K_USER_NOT_FOUND if the user is not found
   * @return K_PERMISSION_DENIED if the privilege of the current user is less than 7
   * @return K_INVALID_PARAMETER if the user ID is invalid
   */
  kExceptionType deluser(const std::string &user_id);
  /**
   * @brief Search for books
   * @param params The parameters
   * @return kExceptionType
   * @return K_SUCCESS if search successfully
   * @return K_PERMISSION_DENIED if the privilege of the current user is less than 1
   * @return K_INVALID_PARAMETER if the parameters are invalid
   */
  std::pair<kExceptionType, std::vector<Book>> search(const Book &params);
  /**
   * @brief Select a book
   * @param ISBN The ISBN of the book
   * @return kExceptionType
   * @return K_SUCCESS if select successfully
   * @return K_BOOK_NOT_FOUND if the book is not found
   * @return K_PERMISSION_DENIED if the privilege of the current user is less than 3
   * @return K_INVALID_PARAMETER if the ISBN is invalid
   */
  kExceptionType select(const std::string &ISBN);
  /**
   * @brief Modify the information of a book
   * @param new_book The new information of the book
   * @return kExceptionType
   * @return K_SUCCESS if modify successfully
   * @return K_NO_SELECTED_BOOK if no book is selected
   * @return K_SAME_ISBN if the ISBN of the book is the same as the old one
   * @return K_PERMISSION_DENIED if the privilege of the current user is less than 3
   * @return K_INVALID_PARAMETER if the ISBN, title, author or keyword is invalid
   */
  kExceptionType modify(Book &&new_book);
  /**
   * @brief Import books
   * @param quantity The quantity of the books
   * @param cost The total cost of the books
   * @return kExceptionType
   * @return K_SUCCESS if import successfully
   * @return K_PERMISSION_DENIED if the privilege of the current user is less than 3
   * @return K_INVALID_PARAMETER if the quantity or cost is invalid
   */
  kExceptionType import_(unsigned int quantity, unsigned long long int cost);
  /**
   * @brief Purchase books
   * @param ISBN The ISBN of the book
   * @param quantity The quantity of the books
   * @return std::pair<kExceptionType, unsigned long long>
   * @return K_SUCCESS if purchase successfully. The second element is the total cost of the books.
   * @return K_BOOK_NOT_FOUND if the book is not found
   * @return K_NOT_ENOUGH_INVENTORY if the inventory of the book is not enough
   * @return K_PERMISSION_DENIED if the privilege of the current user is less than 1
   * @return K_INVALID_PARAMETER if the ISBN or quantity is invalid
   */
  std::pair<kExceptionType, unsigned long long> purchase(const std::string &ISBN, unsigned int quantity);
  /**
   * @brief Show the finance log
   * @param count The number of records to show
   * @return std::pair<kExceptionType, FinanceRecord>
   * @return K_SUCCESS if show successfully. The second element is the finance record.
   * @return K_NOT_ENOUGH_RECORDS if the number of records is less than count
   * @return K_PERMISSION_DENIED if the privilege of the current user is less than 7
   */
  std::pair<kExceptionType, FinanceRecord> showFinance(unsigned int count);
  /**
   * @brief Show all the finance log
   * @return std::pair<kExceptionType, FinanceRecord>
   * @return K_SUCCESS if show successfully. The second element is the finance record.
   * @return K_PERMISSION_DENIED if the privilege of the current user is less than 7
   */
  std::pair<kExceptionType, const FinanceRecord &> showFinance();
};

#endif //BOOKSTORE_SRC_BOOKSTORE_H_
