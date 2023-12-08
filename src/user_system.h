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

using Username_t = char[30];

struct User {
  std::string user_id;
  std::string password;
  std::string name;
  unsigned privilege = 0;
  unsigned selected_id = 0; // not stored in file

  User() = default;

  User(std::string user_id,
       std::string password,
       std::string name,
       unsigned privilege)
      : user_id(std::move(user_id)), password(std::move(password)), name(std::move(name)), privilege(privilege) {}

  explicit User(const char *bytes) { fromBytes(bytes); }

  static constexpr unsigned int byte_size() {
    return 3 * sizeof(Username_t) + sizeof(char);
  }

  void toBytes(char *dest) const;

  void fromBytes(const char *src);
};

class UserSystem {
 private:
  const std::string file_prefix_;
  external_memory::List<User, true> user_list_;
  external_memory::Map<std::string> user_id_to_id_;
  std::vector<User> login_stack_; // A default user is always at the bottom of the stack
  std::unordered_map<std::string, size_t> login_count_;

  unsigned int find(const std::string &user_id); // return 0 if not found

  User get(unsigned int id); // no bound check

  User & current_user() const;

  bool isLoggedIn(const std::string &user_id) const;

  bool isLoggedIn() const;

 public:
  explicit UserSystem(std::string file_prefix = "users")
      : file_prefix_(std::move(file_prefix)), user_list_(file_prefix_ + "_list"),
        user_id_to_id_(file_prefix_ + "_map") {}

  ~UserSystem() = default;

  void initialize(bool reset = false);

  kExceptionType login(const std::string &user_id, const std::string &password);

  kExceptionType useradd(const std::string &user_id,
                         const std::string &password,
                         const std::string &name,
                         int privilege);

  kExceptionType deluser(const std::string &user_id);

  kExceptionType passwd(const std::string &user_id,
                        const std::string &new_password,
                        const std::string &old_password = "");

  kExceptionType logout();

  unsigned int getPrivilege() const;

  std::string getUserId() const;

  std::string getUserName() const;

  unsigned int getSelectedId() const;

  void select(unsigned int id);
};

#endif //BOOKSTORE_SRC_USER_SYSTEM_H_
