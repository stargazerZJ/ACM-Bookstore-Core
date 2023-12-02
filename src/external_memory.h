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
/**
 * @brief A class for storing a list of integers in external memory.
 *
 * @details
 * The list is stored in a file, whose name (and path) is specified in the constructor.
 *
 * `initialize` must be called before using the list.
 *
 * @attention The list is 0-indexed.
 * @attention No bounds checking is performed.
 */
class Array {
 private:
  unsigned int size_; // number of elements
  const std::string file_name_; // name (and path) of the file
  std::fstream file_; // the file
  bool cached_; // whether the whole list is cached
  std::vector<int> cache_; // the cache
 public:
  /**
   * @brief Construct a new Array object.
   *
   * @param name The name (and path) of the file.
   */
  explicit Array(std::string name = "") : size_(0), file_name_(std::move(name) + ".db"), cached_() {}
  /**
   * @brief Destroy the Array object.
   *
   * @details
   * The destructor closes the file.
   * If the list is cached, the cache is written back to the file.
   */
  ~Array();
  /**
   * @brief Initialize the list.
   *
   * @details
   * When `reset` is `true`, the file is truncated.
   * Otherwise, the file is opened.
   *
   * @param reset Whether to reset the file.
   */
  void initialize(bool reset = false);
  /**
   * @brief Get the size of the list.
   *
   * @return unsigned int The size of the list.
   */
  unsigned int size() const;
  /**
   * @brief Get the value of the n-th element (0-based).
   *
   * @param n The index of the element, 0-based.
   * @return int The value of the n-th element (0-based).
   */
  int get(unsigned int n);
  /**
   * @brief Set the value of the n-th element (0-based).
   *
   * @param n The index of the element, 0-based.
   * @param value The value to be set.
   */
  void set(unsigned int n, int value);
  /**
   * @brief Append a new element to the list.
   *
   * @param value The value to be appended.
   * @return unsigned int The index of the new element.
   */
  unsigned int push_back(int value);
  /**
   * @brief Cache the whole list.
   *
   * @details
   * The whole list is read into the cache.
   * Subsequent operations will be performed on the cache.
   */
  void cache();
  /**
   * @brief Write the cache back to the file.
   *
   * @details
   * The whole list is written back to the file.
   * Subsequent operations will be performed on the file.
   */
  void flush();
  /**
   * @brief Append a copy of the list to the end of the file, doubling the size of the file.
   *
   */
  void double_size();
  /**
   * @brief Halve the size of the file.
   *
   */
  void halve_size();
};

}

#endif //BOOKSTORE_SRC_EXTERNAL_MEMORY_H_
