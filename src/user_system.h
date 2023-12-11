//
// Created by zj on 11/29/2023.
//

#ifndef BOOKSTORE_SRC_USER_SYSTEM_H_
#define BOOKSTORE_SRC_USER_SYSTEM_H_

#include "log.h"
#include "external_memory.h"
#include "external_hash_map.h"
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

/**
 * @brief User class
 * @details The User class is used to store the information of a user.
 * @details The information of a user includes user ID, password, name and privilege.
 * @details The user ID is the unique identifier of a user.
 * @details The password is used to login.
 * @details The privilege of a user is used to determine whether the user can perform some operations.
 * @details The information of a user is stored in external memory.
 * @details The user ID, password and name are stored in external memory as char[30].
 * @details The privilege is stored in external memory as unsigned char.
 * @details The selected ID is not stored in external memory.
 */
struct User {
  using Username_t = char[30]; // The type of user ID, password and name is char[30] in external memory. However, in memory, they are std::string.
  std::string user_id; // the user ID of the user
  std::string password; // the password of the user
  std::string name; // the name of the user
  unsigned privilege = 0; // the privilege of the user. Valid values are 1, 3 and 7.
  unsigned selected_id = 0; // the ID of the book selected by the user, not stored in external memory
  /// \brief Construct a new User object
  User() = default;
  /// \brief Construct a new User object
  User(std::string user_id,
       std::string password,
       std::string name,
       unsigned privilege)
      : user_id(std::move(user_id)), password(std::move(password)), name(std::move(name)), privilege(privilege) {}
  /// \brief Construct a new User object from bytes
  explicit User(const char *bytes) { fromBytes(bytes); }
  /// \brief Get the byte size of a User object when stored in external memory
  static constexpr unsigned int byte_size() {
    return 3 * sizeof(Username_t) + sizeof(char);
  }
  /// \brief Convert a User object to bytes
  void toBytes(char *dest) const;
  /// \brief Convert bytes to a User object
  void fromBytes(const char *src);
};
/**
 * @brief UserSystem class
 * @details The UserSystem class is used to manage users.
 * @details The UserSystem class provides the following functions:
 * @details 1. login: login with user ID and password
 * @details 2. useradd: add a new user
 * @details 3. deluser: delete a user
 * @details 4. passwd: change the password of a user
 * @details 5. logout: logout
 * @details The information of users is stored in external memory.
 * @attention Privilege check should be performed by the caller if the check only involves the privilege of the current user.
 * @attention `initialize` should be called before using any other functions.
 */
class UserSystem {
 private:
  const std::string file_prefix_; // the prefix (including path) of the files used to store the information of users
  external_memory::List<User, true> user_list_; // the list of users
  external_memory::Map<std::string> user_id_to_id_; // the map from user ID to user ID
  std::vector<User> login_stack_; // A default user is always at the bottom of the stack
  std::unordered_map<std::string, size_t> login_count_; // the number of times each user has logged in

  unsigned int find(const std::string &user_id); // return 0 if not found
  User get(unsigned int id); // no bound check
  const User &current_user() const;
  User &current_user();
  bool isLoggedIn(const std::string &user_id) const;
  bool isLoggedIn() const;

 public:
  /// \brief Construct a new UserSystem object
  explicit UserSystem(std::string file_prefix = "users")
      : file_prefix_(std::move(file_prefix)), user_list_(file_prefix_ + "_list"),
        user_id_to_id_(file_prefix_ + "_map") {}
  /// \brief Destroy the UserSystem object
  ~UserSystem() = default;
  /// \brief Initialize the UserSystem object
  /// \param reset Whether to reset the UserSystem object
  /// \attention This function should be called before using any other functions.
  /// \attention This function must not be called twice.
  /// \attention If reset is true, all the information of users will be lost.
  void initialize(bool reset = false);
  /**
   * @brief Login with user ID and password
   * @param user_id The user ID
   * @param password The password
   * @return kExceptionType K_SUCCESS if login successfully
   * @return kExceptionType K_USER_NOT_FOUND if the user is not found
   * @return kExceptionType K_WRONG_PASSWORD if the password is provided but wrong
   * @return kExceptionType K_PERMISSION_DENIED if the password is not provided but the privilege of the current user is less than the privilege of the user
   */
  kExceptionType login(const std::string &user_id, const std::string &password);
  /**
   * @brief Add a new user
   * @param user_id The user ID
   * @param password The password
   * @param name The name
   * @param privilege The privilege
   * @return kExceptionType K_SUCCESS if add successfully
   * @return kExceptionType K_USER_ALREADY_EXISTS if the user already exists
   * @attention No privilege check is performed here.
   */
  kExceptionType useradd(const std::string &user_id,
                         const std::string &password,
                         const std::string &name,
                         unsigned int privilege);
  /**
   * @brief Delete a user
   * @param user_id
   * @return kExceptionType K_SUCCESS if delete successfully
   * @return kExceptionType K_USER_NOT_FOUND if the user is not found
   * @attention No privilege check is performed here.
   */
  kExceptionType deluser(const std::string &user_id);
  /**
   * @brief Change the password of a user
   * @param user_id The user ID
   * @param new_password The new password
   * @param old_password The old password
   * @return kExceptionType K_SUCCESS if change successfully
   * @return kExceptionType K_USER_NOT_FOUND if the user is not found
   * @return kExceptionType K_WRONG_PASSWORD if the old password is provided but wrong
   * @attention No privilege check is performed here.
   */
  kExceptionType passwd(const std::string &user_id,
                        const std::string &new_password,
                        const std::string &old_password = "");
  /**
   * @brief Logout
   * @return kExceptionType K_SUCCESS if logout successfully
   * @return kExceptionType K_NO_LOGIN_USER if there is no user logged in
   */
  kExceptionType logout();
  /**
   * @brief Get the privilege of the current user
   * @details If no user is logged in, return 0.
   * @return unsigned int The privilege
   */
  unsigned int getPrivilege() const;
  /**
   * @brief Get the user ID of the current user
   * @details If no user is logged in, return an empty string.
   * @return std::string The user ID
   */
  std::string getUserId() const;
  /**
   * @brief Get the name of the current user
   * @details If no user is logged in, return an empty string.
   * @return std::string The name
   */
  std::string getUserName() const;
  /**
   * @brief Get the ID of the book selected by the current user
   * @details If no user is logged in, or the current user has not selected any book, return 0.
   * @return unsigned int The ID of the book
   */
  unsigned int getSelectedId() const;
  /**
   * @brief Select a book
   * @param id The ID of the book
   * @attention No privilege check is performed here.
   */
  kExceptionType select(unsigned int id);
};

#endif //BOOKSTORE_SRC_USER_SYSTEM_H_
