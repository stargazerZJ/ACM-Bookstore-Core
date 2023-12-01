//
// Created by zj on 11/29/2023.
//

#ifndef BOOKSTORE_SRC_USER_SYSTEM_H_
#define BOOKSTORE_SRC_USER_SYSTEM_H_

#include "external_memory.h"
#include "external_hash_map.h"

using Username_t = char[30];

struct User {
  unsigned id;
  std::string user_id;
  std::string password;
  std::string name;
  unsigned privilege;
  unsigned selected_id; // not stored in file

  User();

  static constexpr unsigned int byte_size() {
    return 3 * sizeof(Username_t) + sizeof(char);
  }

  void toBytes(char *dest);

  void fromBytes(const char *src);
};

class UserSystem {
 private:
  const std::string file_prefix_;
  external_memory::List<User> user_list_;
  external_memory::ExternalHashMap<Username_t> user_id_to_id_;
  std::vector<User> login_stack_;

  unsigned int find(const std::string &user_id) const;

  User get(unsigned int id) const;

  const User &current_user() const;

 public:
  UserSystem(std::string file_prefix = "");

  ~UserSystem();

  void initialize(bool reset = false);

  //TODO: change the return type to Exception

  bool login(const std::string &user_id, const std::string &password, bool force);

  bool useradd(const std::string &user_id, const std::string &password, const std::string &name, int privilege);

  bool deluser(const std::string &user_id);

  bool passwd(const std::string &user_id,
              const std::string &new_password,
              bool force,
              const std::string &old_password = "");

  bool logout();

  unsigned getPrivilege() const;

  std::string getUserId() const;

  std::string getUserName() const;

  unsigned int getSelectedId() const;

  bool select(unsigned int id);
};

#endif //BOOKSTORE_SRC_USER_SYSTEM_H_
