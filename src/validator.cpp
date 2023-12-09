//
// Created by zj on 11/30/2023.
//

#include "validator.h"
namespace validator {
bool isValidUserID(const std::string &id) {
  if (id.length() < 1 || id.length() > 30) return false;
  return std::all_of(id.begin(), id.end(), [](char c) { return std::isalnum(c) || c == '_'; });
}
bool isValidPassword(const std::string &password) {
  return isValidUserID(password);
}
bool isValidUserName(const std::string &name) {
  if (name.length() < 1 || name.length() > 30) return false;
  return std::all_of(name.begin(), name.end(), [](char c) { return std::isgraph(c); });
}
bool isValidPrivilege(unsigned int privilege) {
  return privilege == 1 || privilege == 3 || privilege == 7;
}
bool isValidISBN(const std::string &ISBN) {
  if (ISBN.length() < 1 || ISBN.length() > 20) return false;
  return std::all_of(ISBN.begin(), ISBN.end(), [](char c) { return std::isgraph(c); });
}
bool isValidBookName(const std::string &name) {
  if (name.length() < 1 || name.length() > 60) return false;
  return std::all_of(name.begin(), name.end(), [](char c) { return std::isgraph(c) && c != '"'; });
}
bool isValidAuthor(const std::string &author) {
  return isValidBookName(author);
}
bool isValidKeyword(const std::string &keyword) {
  if (!isValidBookName(keyword)) return false;
  auto keywords_vec = Book::unpackKeywords(keyword);
  if (std::unique(keywords_vec.begin(), keywords_vec.end()) != keywords_vec.end()) return false;
  return true;
}
bool isValidSingleKeyword(const std::string &keyword) {
  if (!isValidKeyword(keyword)) return false;
  if (keyword.find('|') != std::string::npos) return false;
  return true;
}
bool isValidQuantity(unsigned int quantity) {
  return quantity > 0;
}
} // namespace validator