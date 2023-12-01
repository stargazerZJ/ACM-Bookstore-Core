//
// Created by zj on 11/29/2023.
//

#ifndef BOOKSTORE_SRC_EXTERNAL_MEMORY_H_
#define BOOKSTORE_SRC_EXTERNAL_MEMORY_H_

#include <string>
#include <vector>
#include <fstream>

namespace external_memory {
template<class T, unsigned info_len>
class ExternalMemory {
 private:
  std::string file_name_;
  static constexpr unsigned int size_of_T_ = sizeof(T);
  static constexpr unsigned int size_of_info_ = info_len * sizeof(unsigned int);
 public:
  std::fstream file_;
  explicit ExternalMemory(const std::string &file_name);

  ~ExternalMemory();

  void initialize(const std::string &file_name);

  void get_info(unsigned n, int &dest);

  void set_info(unsigned n, int value);

  void read(unsigned n, T &dest);

  void write(unsigned n, const T &value);
};
template<class T>
class ExternalList {
 private:
  unsigned int size_;
  static constexpr unsigned int byte_size_ = T::byte_size();
  using bytes = char[byte_size_];
  ExternalMemory<bytes, 1> list_;
 public:
  explicit ExternalList(const std::string &file_name);

  ~ExternalList();

  void initialize(const std::string &file_name);

  void read(unsigned n, T &dest);

  void update(unsigned n, const T &value);

//  void read_attr(unsigned n, unsigned offset, unsigned len, char *dest);
//
//  void update_attr(unsigned n, unsigned offset, unsigned len, const char *value);

  unsigned insert(const T &value);
};
class ExternalAllocator {
 private:
  static constexpr char file_name_[] = "external_allocator";
 public:
  ExternalAllocator();

  ~ExternalAllocator();

  void initialize();

  unsigned int allocate(int size);

  void deallocate(unsigned int n, int size);
};
class ExternalVector {
 private:
  static ExternalAllocator allocator_;
  unsigned pos_;
  std::vector<unsigned int> data_;
 public:
  ExternalVector();

  explicit ExternalVector(unsigned pos);

  std::vector<unsigned int> &get_data();

  bool commit(); // return true if pos_ is changed
};
}

//template<class T, unsigned info_len>
//class ExternalMemory;
//
//template<class T>
//class ExternalList;
//
//class ExternalAllocator; // element is unsigned int
//
//class ExternalVector; // element is unsigned int

#endif //BOOKSTORE_SRC_EXTERNAL_MEMORY_H_
