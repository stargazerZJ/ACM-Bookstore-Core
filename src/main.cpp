//
// Created by zj on 11/29/2023.
//

#include <iostream>
#include <string>
//#include "test.h"
//#include <cassert>
#include "cli.h"

int main() {
#ifdef LOCAL
  static const std::string path = "../db/";
#else
  static inline const string path = "./";
#endif
  BookStoreCLI cli(path);
  cli.initialize();
  cli.run();
  return 0;
}