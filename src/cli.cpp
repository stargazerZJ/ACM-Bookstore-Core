//
// Created by zj on 11/30/2023.
//

#include "cli.h"
void BookStoreCLI::initialize(bool force_reset) {
  book_store_.initialize(force_reset);
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}
void BookStoreCLI::run() {
  std::string line;
  while (std::getline(is, line)) {
    Command command(line);
    const std::string &name = command.getName();
    const Args &args = command.getArgs();
    if (name.empty()) continue;
    if (name == "exit" || name == "quit") {
      return;
    } else if (name == "su") {
      runCommand(args, &BookStoreCLI::su);
    } else if (name == "logout") {
      runCommand(args, &BookStoreCLI::logout);
    } else if (name == "register") {
      runCommand(args, &BookStoreCLI::register_);
    } else if (name == "passwd") {
      runCommand(args, &BookStoreCLI::passwd);
    } else if (name == "useradd") {
      runCommand(args, &BookStoreCLI::useradd);
    } else if (name == "delete") {
      runCommand(args, &BookStoreCLI::delete_);
    } else if (name == "show") {
      if (!args.empty() && args[0] == "finance") runCommand(args, &BookStoreCLI::showFinance);
      else runCommand(args, &BookStoreCLI::show);
    } else if (name == "buy") {
      runCommand(args, &BookStoreCLI::buy);
    } else if (name == "select") {
      runCommand(args, &BookStoreCLI::select);
    } else if (name == "modify") {
      runCommand(args, &BookStoreCLI::modify);
    } else if (name == "import") {
      runCommand(args, &BookStoreCLI::import_);
    } else {
      runCommand(args, &BookStoreCLI::invalidCommand);
    }
  }
}
void BookStoreCLI::runCommand(const BookStoreCLI::Args &args, Func func) {
  kExceptionType ret = (this->*func)(args);
  if (ret != kExceptionType::K_SUCCESS) {
    os << "Invalid!" << endl;
  }
}
kExceptionType BookStoreCLI::su(const BookStoreCLI::Args &args) {
  if (args.empty() || args.size() > 2) return kExceptionType::K_INVALID_PARAMETER;
  if (args.size() == 1) {
    return book_store_.login(args[0], "");
  } else {
    return book_store_.login(args[0], args[1]);
  }
}
kExceptionType BookStoreCLI::logout(const BookStoreCLI::Args &args) {
  if (!args.empty()) return kExceptionType::K_INVALID_PARAMETER;
  return book_store_.logout();
}
kExceptionType BookStoreCLI::register_(const BookStoreCLI::Args &args) {
  if (args.size() != 3) return kExceptionType::K_INVALID_PARAMETER;
  return book_store_.customerUseradd(args[0], args[1], args[2]);
}
kExceptionType BookStoreCLI::passwd(const BookStoreCLI::Args &args) {
  if (args.size() != 2 && args.size() != 3) return kExceptionType::K_INVALID_PARAMETER;
  if (args.size() == 2) {
    return book_store_.passwd(args[0], args[1]);
  } else {
    return book_store_.passwd(args[0], args[2], args[1]);
  }
}
kExceptionType BookStoreCLI::useradd(const BookStoreCLI::Args &args) {
  if (args.size() != 4) return kExceptionType::K_INVALID_PARAMETER;
  unsigned int privilege;
  auto ret = Command::parseUnsignedInt(args[2]);
  if (ret.first != kExceptionType::K_SUCCESS) return ret.first;
  privilege = ret.second;
  return book_store_.useradd(args[0], args[1], args[3], privilege);
}
kExceptionType BookStoreCLI::delete_(const BookStoreCLI::Args &args) {
  if (args.size() != 1) return kExceptionType::K_INVALID_PARAMETER;
  return book_store_.deluser(args[0]);
}
kExceptionType BookStoreCLI::show(const BookStoreCLI::Args &args) {
  if (args.size() > 1) return kExceptionType::K_INVALID_PARAMETER;
  Book params;
  if (!args.empty()) {
    auto ret = Command::parseFlag(args[0], false);
    if (ret.first != kExceptionType::K_SUCCESS) return ret.first;
    Command::Flag flag = ret.second;
    if (flag.getFlag() == "ISBN") {
      params.ISBN = flag.getValue();
    } else if (flag.getFlag() == "name") {
      auto result = Command::removeQuotationMarks(flag.getValue());
      if (result.first != kExceptionType::K_SUCCESS) return result.first;
      params.title = result.second;
    } else if (flag.getFlag() == "author") {
      auto result = Command::removeQuotationMarks(flag.getValue());
      if (result.first != kExceptionType::K_SUCCESS) return result.first;
      params.author = flag.getValue();
    } else if (flag.getFlag() == "keyword") {
      auto result = Command::removeQuotationMarks(flag.getValue());
      if (result.first != kExceptionType::K_SUCCESS) return result.first;
      params.keywords = flag.getValue();
    } else {
      return kExceptionType::K_INVALID_PARAMETER;
    }
  }
  auto result = book_store_.search(params);
  if (result.first != kExceptionType::K_SUCCESS) return result.first;
  for (const auto &book : result.second) {
    os << book.ISBN << "\t" << book.title << "\t" << book.author << "\t" << book.keywords << "\t" << printMoney(book.price) << "\t"
       << book.quantity << endl;
  }
  return kExceptionType::K_SUCCESS;
}
kExceptionType BookStoreCLI::buy(const BookStoreCLI::Args &args) {
  if (args.size() != 2) return kExceptionType::K_INVALID_PARAMETER;
  auto ret = Command::parseUnsignedInt(args[1]);
  if (ret.first != kExceptionType::K_SUCCESS) return ret.first;
  auto result = book_store_.purchase(args[0], ret.second);
  if (result.first != kExceptionType::K_SUCCESS) return result.first;
  os << printMoney(result.second) << endl;
}
std::string BookStoreCLI::printMoney(unsigned long long int money) {
  std::ostringstream oss;
  oss << money / 100 << "." << std::setw(2) << std::setfill('0') << money % 100;
  return oss.str();
}
kExceptionType BookStoreCLI::select(const BookStoreCLI::Args &args) {
  if (args.size() != 1) return kExceptionType::K_INVALID_PARAMETER;
  return book_store_.select(args[0]);
}
kExceptionType BookStoreCLI::modify(const BookStoreCLI::Args &args) {
  if (args.empty()) return kExceptionType::K_INVALID_PARAMETER;
  Book new_book;
  new_book.price = -1;
  for (auto &flag_str : args) {
    auto ret = Command::parseFlag(flag_str, false);
    if (ret.first != kExceptionType::K_SUCCESS) return ret.first;
    Command::Flag flag = ret.second;
    if (flag.getFlag() == "ISBN") {
      if (!new_book.ISBN.empty()) return kExceptionType::K_INVALID_PARAMETER;
      new_book.ISBN = flag.getValue();
    } else if (flag.getFlag() == "name") {
      if (!new_book.title.empty()) return kExceptionType::K_INVALID_PARAMETER;
      auto result = Command::removeQuotationMarks(flag.getValue());
      if (result.first != kExceptionType::K_SUCCESS) return result.first;
      new_book.title = result.second;
    } else if (flag.getFlag() == "author") {
      if (!new_book.author.empty()) return kExceptionType::K_INVALID_PARAMETER;
      auto result = Command::removeQuotationMarks(flag.getValue());
      if (result.first != kExceptionType::K_SUCCESS) return result.first;
      new_book.author = flag.getValue();
    } else if (flag.getFlag() == "keyword") {
      if (!new_book.keywords.empty()) return kExceptionType::K_INVALID_PARAMETER;
      auto result = Command::removeQuotationMarks(flag.getValue());
      if (result.first != kExceptionType::K_SUCCESS) return result.first;
      new_book.keywords = flag.getValue();
    } else if (flag.getFlag() == "price") {
      if (new_book.price != -1) return kExceptionType::K_INVALID_PARAMETER;
      auto result = Command::parseMoney(flag.getValue());
      if (result.first != kExceptionType::K_SUCCESS) return result.first;
      new_book.price = result.second;
    } else {
      return kExceptionType::K_INVALID_PARAMETER;
    }
  }
  return book_store_.modify(std::move(new_book));
}
kExceptionType BookStoreCLI::import_(const BookStoreCLI::Args &args) {
  if (args.size() != 1) return kExceptionType::K_INVALID_PARAMETER;
  auto ret = Command::parseUnsignedInt(args[0]);
  if (ret.first != kExceptionType::K_SUCCESS) return ret.first;
  auto result = book_store_.import_(ret.second);
  if (result.first != kExceptionType::K_SUCCESS) return result.first;
  os << printMoney(result.second) << endl;
}
kExceptionType BookStoreCLI::showFinance(const BookStoreCLI::Args &args) {
  if (args.size() > 1) return kExceptionType::K_INVALID_PARAMETER;
  FinanceRecord record;
  if (!args.empty()) {
    auto ret = Command::parseUnsignedInt(args[0]);
    if (ret.first != kExceptionType::K_SUCCESS) return ret.first;
    auto result = book_store_.showFinance(ret.second);
    if (result.first != kExceptionType::K_SUCCESS) return result.first;
    record = result.second;
  } else {
    auto result = book_store_.showFinance();
    if (result.first != kExceptionType::K_SUCCESS) return result.first;
    record = result.second;
  }
  os << "+ " << printMoney(record.income()) << " - " << printMoney(record.expenditure()) << endl;
  return kExceptionType::K_SUCCESS;
}
kExceptionType BookStoreCLI::invalidCommand(const BookStoreCLI::Args &args) {
  return kExceptionType::K_INVALID_COMMAND;
}
