//
// Created by zj on 12/9/2023.
//

#ifndef BOOKSTORE_SRC_PARSER_H_
#define BOOKSTORE_SRC_PARSER_H_

#include "log.h"
#include <string>
#include <vector>

/**
 * @brief The Command class
 * @details The Command class is used to parse commands.
 * @details A command consists of a name and some arguments.
 * @details A command must not occupy multiple lines.
 * @details The only delimiter of a command is space.
 * @details Multiple spaces are treated as one delimiter.
 * @details Spaces at the beginning and end of the line are ignored.
 * @details A command may have flags.
 * @details There are two types of flags: quoted and not quoted.
 * @details A quoted flag is in the form of '-flag="value"'.
 * @details A not quoted flag is in the form of '-flag=value'.
 */
class Command {
 private:
  std::string name_; // the name of the command
  std::vector<std::string> args_; // the arguments
  explicit Command(std::string name, std::vector<std::string> args)
      : name_(std::move(name)), args_(std::move(args)) {}
 public:
  class Flag {
    friend class Command;
   private:
    const std::string flag; // the flag
    const std::string value; // the value of the flag
    explicit Flag(std::string flag, std::string value = "")
        : flag(std::move(flag)), value(std::move(value)) {}
   public:
    Flag() = default;
    [[nodiscard]] const std::string &getFlag() const { return flag; }
    [[nodiscard]] const std::string &getValue() const { return value; }
  };
  Command() = default;
  /// \brief Parse a command
  explicit Command(const std::string &line);
  [[nodiscard]] const std::string &getName() const { return name_; }
  [[nodiscard]] const std::vector<std::string> &getArgs() const { return args_; }
  /**
   * @brief Parse a flag
   * @details Parse a flag, e.g. '-ISBN=978-7-302-32998-2' (not quoted) or '-name="C++ Primer"' (quoted)
   * @param arg
   * @param quoted
   * @return A pair of kExceptionType and Flag
   * @return If the flag is not valid, the kExceptionType will be K_INVALID_PARAMETER.
   * @return If the flag is valid, the kExceptionType will be K_SUCCESS.
   */
  static std::pair<kExceptionType, Flag> parseFlag(const std::string &arg, bool quoted = false);
  static std::pair<kExceptionType, std::string> removeQuotationMarks(const std::string &arg);
  /**
   * @brief Parse an unsigned int
   * @details Parse an unsigned int, e.g. '123'
   * @param arg
   * @return A pair of kExceptionType and unsigned int
   * @return If the unsigned int is not valid, the kExceptionType will be K_INVALID_PARAMETER.
   * @return If the unsigned int is valid, the kExceptionType will be K_SUCCESS.
   */
  static std::pair<kExceptionType, unsigned int> parseUnsignedInt(const std::string &arg);
  /**
   * @brief Parse a money
   * @details Parse a money, e.g. '123.45'
   * @param arg
   * @return A pair of kExceptionType and unsigned long long int
   * @return If the money is not valid, the kExceptionType will be K_INVALID_PARAMETER.
   * @return If the money is valid, the kExceptionType will be K_SUCCESS.
   */
  static std::pair<kExceptionType, unsigned long long int> parseMoney(const std::string &arg); // in cents
};

#endif //BOOKSTORE_SRC_PARSER_H_
