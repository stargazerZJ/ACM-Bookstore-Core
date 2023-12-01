//
// Created by zj on 11/30/2023.
//

#ifndef BOOKSTORE_SRC_CLI_H_
#define BOOKSTORE_SRC_CLI_H_

#include "bookstore.h"

class BookStoreCLI {
 private:
  static constexpr char file_path[] = "bookstore";
 public:
  BookStoreCLI() = default;

  ~BookStoreCLI() = default;

  bool initialize(bool force_reset = false);

  bool run();
};

#endif //BOOKSTORE_SRC_CLI_H_