//
// Created by zj on 11/29/2023.
//

#include <iostream>
#include <string>
//#include "test.h"
//#include <cassert>
#include "cli.h"

int main(int argc, char *argv[]) {
  std::string path = "./";
  if (argc > 1) path = argv[1];
  BookStoreCLI cli(path);
  cli.initialize(false);
  cli.run();
  return 0;
}