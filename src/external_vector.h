//
// Created by zj on 12/1/2023.
//

#ifndef BOOKSTORE_SRC_EXTERNAL_MEMORY_CPP_EXTERNAL_VECTORS_H_
#define BOOKSTORE_SRC_EXTERNAL_MEMORY_CPP_EXTERNAL_VECTORS_H_

#include <fstream>
#include <vector>
#include <string>
#include <set>
#include "external_memory.h"

namespace external_memory {
class Vectors {
 private:
  const std::string file_name_;
  static constexpr unsigned int kInfoPerPage = 3;
  static constexpr unsigned int kCapacityLogMax = __builtin_ctz(kIntegerPerPage / 2);
  Array info_;
  Pages data_;
  std::set<int> free_pages_of_capacity_[kCapacityLogMax + 1];
  enum class kPageInfo : unsigned int {
    kCapacity = 0,
    kFreeHead = 1,
    kUnoccupiedBeg = 2,
    kNextPage = 1,
    kLastPage = 2
  };
  [[nodiscard]] int getPageInfo(kPageInfo type, unsigned int n);
  void setPageInfo(kPageInfo type, unsigned int n, int value);
  [[nodiscard]] unsigned int getPageOfCapacity(unsigned int capacity); // capacity < kIntegerPerPage
  [[nodiscard]] unsigned int allocate(unsigned int capacity); // and clear, and deal with capacity of kIntegerPerPage; capacity <= kIntegerPerPage
  void deallocate(unsigned int n, unsigned int offset, unsigned int capacity);
  [[nodiscard]] unsigned int newPage();
  void deletePage(unsigned int n);
  void clearSpace(unsigned int capacity, unsigned int n, unsigned int offset);
 public:
  explicit Vectors(std::string name = "vectors")
      : file_name_(std::move(name)), info_(file_name_ + "_info"), data_(file_name_ + "_data") {}
  ~Vectors() = default;
  void initialize(bool reset = false);
  class Vector {
    friend class Vectors;
   private:
    Vectors &vectors_;
    unsigned int pos_;
    unsigned int page_id_ = 0;
    unsigned int offset_ = 0;
    explicit Vector(Vectors &vectors, unsigned int pos = 0) :
        vectors_(vectors), pos_(pos) { updatePos(pos); }
    unsigned int appendPage();
    void discardAfter(unsigned int last_page);
   public:
    ~Vector() = default;
    [[nodiscard]] unsigned int getPos() const;
    std::vector<int> getData();
    [[nodiscard]] unsigned int capacity() const;
    [[nodiscard]] bool push_back(int value);
    [[nodiscard]] bool rewrite(std::vector<int> &data);
    void updatePos(unsigned int new_pos);
  };
  Vector getVector(unsigned int pos = 0);
  Vector newVector();
};
}
#endif //BOOKSTORE_SRC_EXTERNAL_MEMORY_CPP_EXTERNAL_VECTORS_H_
