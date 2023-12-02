//
// Created by zj on 12/1/2023.
//

#ifndef BOOKSTORE_SRC_EXTERNAL_MEMORY_CPP_EXTERNAL_VECTORS_H_
#define BOOKSTORE_SRC_EXTERNAL_MEMORY_CPP_EXTERNAL_VECTORS_H_

#include <fstream>
#include <vector>
#include <string>
class Vectors {
 private:
  const std::string file_name_;
 public:
  Vectors(std::string name = "vectors");

  ~Vectors();

  void initialize();

  unsigned int allocate(int size);

  void deallocate(unsigned int n, int size);
};
#endif //BOOKSTORE_SRC_EXTERNAL_MEMORY_CPP_EXTERNAL_VECTORS_H_
