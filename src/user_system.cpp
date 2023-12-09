//
// Created by zj on 11/29/2023.
//

#include "user_system.h"
void User::toBytes(char *dest) const {
  std::strncpy(dest, user_id.c_str(), sizeof(Username_t));
  std::strncpy(dest + sizeof(Username_t), password.c_str(), sizeof(Username_t));
  std::strncpy(dest + 2 * sizeof(Username_t), name.c_str(), sizeof(Username_t));
  dest[3 * sizeof(Username_t)] = static_cast<char>(privilege);
}
void User::fromBytes(const char *src) {
  user_id = std::string(src, sizeof(Username_t));
  password = std::string(src + sizeof(Username_t), sizeof(Username_t));
  name = std::string(src + 2 * sizeof(Username_t), sizeof(Username_t));
  privilege = static_cast<unsigned char>(src[3 * sizeof(Username_t)]);
}
unsigned int UserSystem::find(const std::string &user_id) {
  return user_id_to_id_.at(user_id);
}
User UserSystem::get(unsigned int id) {
  return user_list_.get(id);
}
const User &UserSystem::current_user() const {
  return login_stack_.back();
}
User &UserSystem::current_user() {
  return login_stack_.back();
}
bool UserSystem::isLoggedIn(const std::string &user_id) const {
  return login_count_.contains(user_id);
}
bool UserSystem::isLoggedIn() const {
  return login_stack_.size() <= 1;
}
void UserSystem::initialize(bool reset) {
  user_list_.initialize(reset);
  user_id_to_id_.initialize(reset);
  if (reset) {
    login_stack_.clear();
    login_stack_.emplace_back();
    login_count_.clear();
  }
}
kExceptionType UserSystem::login(const std::string &user_id, const std::string &password) {
  unsigned int id = find(user_id);
  if (!id) return kExceptionType::K_USER_NOT_FOUND;
  User user = get(id);
  if (password.empty()) {
    if (getPrivilege() > user.privilege) return kExceptionType::K_PERMISSION_DENIED;
  } else if (user.password != password) return kExceptionType::K_WRONG_PASSWORD;
  login_stack_.emplace_back(user);
  login_count_[user_id]++;
  return kExceptionType::K_SUCCESS;
}
kExceptionType UserSystem::logout() {
  if (!isLoggedIn()) return kExceptionType::K_NO_LOGIN_USER;
  auto &login_count = login_count_[current_user().user_id];
  if (login_count == 1) {
    login_count_.erase(current_user().user_id);
  } else {
    login_count--;
  }
  login_stack_.pop_back();
  return kExceptionType::K_SUCCESS;
}
kExceptionType UserSystem::useradd(const std::string &user_id,
                                   const std::string &password,
                                   const std::string &name,
                                   int privilege) {
  auto &id = user_id_to_id_[user_id];
  if (id) return kExceptionType::K_USER_ALREADY_EXIST;
  id = user_list_.insert(User(user_id, password, name, privilege));
  return kExceptionType::K_SUCCESS;
}
kExceptionType UserSystem::deluser(const std::string &user_id) {
  if (isLoggedIn(user_id)) return kExceptionType::K_USER_IS_LOGGED_IN;
  auto id = find(user_id);
  if (!id) return kExceptionType::K_USER_NOT_FOUND;
  user_list_.erase(id);
  user_id_to_id_.erase(user_id);
  return kExceptionType::K_SUCCESS;
}
kExceptionType UserSystem::passwd(const std::string &user_id,
                                  const std::string &new_password,
                                  const std::string &old_password) {
  auto id = find(user_id);
  if (!id) return kExceptionType::K_USER_NOT_FOUND;
  User user = get(id);
  if (!old_password.empty() && user.password != old_password) return kExceptionType::K_WRONG_PASSWORD;
  user.password = new_password;
  user_list_.set(id, user);
  return kExceptionType::K_SUCCESS;
}
unsigned int UserSystem::getPrivilege() const {
  return current_user().privilege;
}
std::string UserSystem::getUserId() const {
  return current_user().user_id;
}
std::string UserSystem::getUserName() const {
  return current_user().name;
}
unsigned int UserSystem::getSelectedId() const {
  return current_user().selected_id;
}
kExceptionType UserSystem::select(unsigned int id) {
  if (!isLoggedIn()) return kExceptionType::K_NO_LOGIN_USER;
  current_user().selected_id = id;
  return kExceptionType::K_SUCCESS;
}
