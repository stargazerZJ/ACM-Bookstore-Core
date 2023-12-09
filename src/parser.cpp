//
// Created by zj on 12/9/2023.
//

#include <cmath>
#include "parser.h"

Command::Command(const std::string &line) {
  /// parse a command, e.g. 'su root 123456' or 'select -ISBN=978-7-302-32998-2'
  /// an empty line is valid
  /// the only valid delimiter is ' ', i.e. no '\t' or '\n'
  /// multiple spaces are treated as one, and spaces at the beginning and end of the line are ignored
  std::istringstream iss(line);
  std::string token;
  while (std::getline(iss, token, ' ')) {
    if (command.empty()) {
      command = token;
    } else {
      args.push_back(token);
    }
  }
}
std::pair<kExceptionType, Command::Flag> Command::parseFlag(const std::string &arg, bool quoted) {
  /// parse a flag, e.g. '-ISBN=978-7-302-32998-2' (not quoted) or '-name="C++ Primer"' (quoted)
  if (arg[0] != '-') return {kExceptionType::K_INVALID_PARAMETER, Command::Flag()};
  std::string flag, value;
  auto pos = arg.find('=');
  if (pos == std::string::npos) {
    return {kExceptionType::K_INVALID_PARAMETER, Command::Flag()};
  } else {
    flag = arg.substr(1, pos - 1);
    value = arg.substr(pos + 1);
  }
  if (flag.empty()) return {kExceptionType::K_INVALID_PARAMETER, Command::Flag()};
  if (quoted) {
    if (value.empty() || value.front() != '"' || value.back() != '"')
      return {kExceptionType::K_INVALID_PARAMETER, Command::Flag()};
    value = value.substr(1, value.size() - 2);
    if (value.empty()) return {kExceptionType::K_INVALID_PARAMETER, Command::Flag()};
    if (value.find('"') != std::string::npos) return {kExceptionType::K_INVALID_PARAMETER, Command::Flag()};
  }
  return {kExceptionType::K_SUCCESS, Command::Flag(flag, value)};
}
std::pair<kExceptionType, unsigned int> Command::parseUnsignedInt(const std::string &arg) {
  /// parse an unsigned int, e.g. '123'
  /// plus sign is not allowed
  unsigned int value = 0;
  if (arg.size() > 10) return {kExceptionType::K_INVALID_PARAMETER, 0};
  if (!std::all_of(arg.begin(), arg.end(), [](char c) { return std::isdigit(c); })) {
    return {kExceptionType::K_INVALID_PARAMETER, 0};
  }
  if (arg.size() > 1 && arg[0] == '0') return {kExceptionType::K_INVALID_PARAMETER, 0};
  try {
    value = std::stoul(arg);
  } catch (std::out_of_range &) {
    return {kExceptionType::K_INVALID_PARAMETER, 0};
  }
  return {kExceptionType::K_SUCCESS, value};
}
std::pair<kExceptionType, unsigned long long int> Command::parseMoney(const std::string &arg) {
  /// parse a money, e.g. '123.45'
  /// plus sign is not allowed
  /// if the decimal part is longer than 2, it is undefined behavior
  unsigned long long int value = 0;
  if (arg.size() > 13) return {kExceptionType::K_INVALID_PARAMETER, 0};
  if (!std::all_of(arg.begin(), arg.end(), [](char c) { return std::isdigit(c) || c == '.'; })) {
    return {kExceptionType::K_INVALID_PARAMETER, 0};
  }
  try {
    value = static_cast<unsigned long long int>(std::round(std::stold(arg) * 100));
  } catch (std::out_of_range &) {
    return {kExceptionType::K_INVALID_PARAMETER, 0};
  } catch (std::invalid_argument &) {
    return {kExceptionType::K_INVALID_PARAMETER, 0};
  }
  return {kExceptionType::K_SUCCESS, value};
}
