//
// Created by zj on 11/30/2023.
//

#ifndef BOOKSTORE_SRC_VALIDATOR_H_
#define BOOKSTORE_SRC_VALIDATOR_H_

#include <string>
#include <algorithm>
#include "user_system.h"
#include "book_system.h"

namespace validator {

/**
 * @brief Check whether the user ID is valid
 * @details The user ID should be a non-empty string consisting of alphanumeric characters and underscores.
 * @details The maximum length of the user ID is 30.
 * @param id
 * @return Whether the user ID is valid
 */
bool isValidUserID(const std::string &id);
/**
 * @brief Check whether the password is valid
 * @details The password should be a non-empty string consisting of alphanumeric characters and underscores.
 * @details The maximum length of the password is 30.
 * @param password
 * @return Whether the password is valid
 */
bool isValidPassword(const std::string &password);
/**
 * @brief Check whether the user name is valid
 * @details The user name should be a non-empty string consisting of visible characters.
 * @details The maximum length of the user name is 30.
 * @param name
 * @return Whether the user name is valid
 */
bool isValidUserName(const std::string &name);
/**
 * @brief Check whether the privilege is valid
 * @details The privilege should be 1, 3 or 7.
 * @param privilege
 * @return Whether the privilege is valid
 */
bool isValidPrivilege(unsigned int privilege);
/**
 * @brief Check whether the ISBN is valid
 * @details The ISBN should be a non-empty string consisting of visible characters.
 * @details The maximum length of the ISBN is 20.
 * @param ISBN
 * @return Whether the ISBN is valid
 */
bool isValidISBN(const std::string &ISBN);
/**
 * @brief Check whether the book name is valid
 * @details The book name should be a non-empty string consisting of visible characters except '"'.
 * @details The maximum length of the book name is 60.
 * @param name
 * @return Whether the book name is valid
 */
bool isValidBookName(const std::string &name);
/**
 * @brief Check whether the author is valid
 * @details The author should be a non-empty string consisting of visible characters except '"'.
 * @details The maximum length of the author is 60.
 * @param author
 * @return Whether the author is valid
 */
bool isValidAuthor(const std::string &author);
/**
 * @brief Check whether a list of keywords is valid
 * @details The keyword should be a non-empty string consisting of visible characters except '"'.
 * @details Keywords should not be duplicated.
 * @details The maximum length of the keyword is 60.
 * @note Multiple keywords are separated by '|'.
 * @param keyword
 * @return Whether the keyword is valid
 */
bool isValidKeyword(const std::string &keyword);
/**
 * @brief Check whether the keyword is valid
 * @param keyword
 * @return Whether the keyword is valid
 */
bool isValidSingleKeyword(const std::string &keyword);

} // namespace validator

#endif //BOOKSTORE_SRC_VALIDATOR_H_
