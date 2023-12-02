//
// Created by zj on 11/29/2023.
//

#ifndef BOOKSTORE_SRC_EXTERNAL_MEMORY_H_
#define BOOKSTORE_SRC_EXTERNAL_MEMORY_H_

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace external_memory {
template<class T, unsigned info_len>
class ListHelper {
 private:
  std::string file_name_;
  static constexpr unsigned int size_of_T_ = sizeof(T);
  static constexpr unsigned int size_of_info_ = info_len * sizeof(unsigned int);
 public:
  std::fstream file_;
  explicit ListHelper(const std::string &file_name);

  ~ListHelper();

  void initialize(const std::string &file_name);

  void get_info(unsigned n, int &dest);

  void set_info(unsigned n, int value);

  void read(unsigned n, T &dest);

  void write(unsigned n, const T &value);
};
template<class T>
class List {
 private:
  unsigned int size_;
  static constexpr unsigned int byte_size_ = T::byte_size();
  using bytes = char[byte_size_];
  ListHelper<bytes, 1> list_;
 public:
  explicit List(const std::string &file_name);

  ~List();

  void initialize(const std::string &file_name);

  void read(unsigned n, T &dest);

  void update(unsigned n, const T &value);

//  void read_attr(unsigned n, unsigned offset, unsigned len, char *dest);
//
//  void update_attr(unsigned n, unsigned offset, unsigned len, const char *value);

  unsigned insert(const T &value);
};
class Array {
 private:
  unsigned int size_;
  const std::string file_name_;
  std::fstream file_;
  bool cached_;
  std::vector<int> cache_;
 public:
  explicit Array(std::string name = "") : size_(0), file_name_(std::move(name) + ".db"), cached_() {}
  ~Array();
  void initialize(bool reset = false);
  unsigned int size() const;
  int get(unsigned int n);
  void set(unsigned int n, int value);
  unsigned int push_back(int value);
  void cache();
  void uncache();
  void double_size();
  void halve_size();
};

}

#endif //BOOKSTORE_SRC_EXTERNAL_MEMORY_H_
