//
// Created by zj on 11/29/2023.
//

#ifndef BOOKSTORE_SRC_EXTERNAL_MEMORY_H_
#define BOOKSTORE_SRC_EXTERNAL_MEMORY_H_

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cstring>

namespace external_memory {
constexpr char kFileExtension[] = ".db";
constexpr unsigned int kPageSize = 4096;
static constexpr unsigned int kIntegerPerPage = kPageSize / sizeof(int); // number of integers in a page
using Page = int[kIntegerPerPage]; // a page
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
 * @attention No bound checking is performed.
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
  explicit Array(std::string name = "array") : size_(0), file_name_(std::move(name) + kFileExtension), cached_() {}
  Array(const Array &) = delete;
  Array &operator=(const Array &) = delete;
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
   *
   * @attention `initialize` must be called before using the list.
   * @attention `initialize` must not be called twice.
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
/**
 * @brief A class for storing pages of integers in external memory.
 *
 * @details
 * The pages are stored in a file, whose name (and path) is specified in the constructor.
 *
 * The first page is used to store information about the pages. This page is referred to as the info page.
 *
 * `initialize` must be called before using the pages.
 *
 * @attention The pages are 1-indexed.
 * @attention The info page is 1-indexed.
 * @attention The first integer of the info page is reserved for storing the head of the free pages.
 * @attention No bound checking is performed.
 *
 * @note The info page is implicitly cached.
 */
class Pages {
 private:
  unsigned int size_; // number of pages
  const std::string file_name_; // name (and path) of the file
  std::fstream file_; // the file
  unsigned int cache_index_; // the index of the cached page
  Page cache_; // the cache
  Page info_; // the info page
  int &free_head_ = info_[0]; // the head of the free pages
 public:
  /**
   * @brief Construct a new Pages object.
   *
   * @param name The name (and path) of the file.
   */
  explicit Pages(std::string name = "pages")
      : size_(), file_name_(std::move(name) + kFileExtension), cache_index_(), cache_(), info_() {}
  Pages(const Pages &) = delete;
  Pages &operator=(const Pages &) = delete;
  /**
   * @brief Destroy the Pages object.
   *
   * @details
   * The destructor closes the file.
   * If the cache is not empty, the cache is written back to the file.
   */
  ~Pages();
  /**
   * @brief Initialize the pages.
   *
   * @details
   * When `reset` is `true`, the file is truncated.
   * Otherwise, the file is opened.
   *
   * @param reset Whether to reset the file.
   *
   * @attention `initialize` must be called before using the pages.
   * @attention `initialize` must not be called twice.
   */
  void initialize(bool reset = false);
  /**
   * @brief Flush the info page cache.
   */
  void flushInfo();
  /**
   * @brief Get the size of the pages, not including the info page.
   *
   * @return unsigned int The size of the pages, not including the info page.
   */
  unsigned int size() const;
  /**
   * @brief Get the i-th info.
   *
   * @param i The index of the info, 1-based.
   * @return int The i-th info.
   * @attention No bound checking is performed.
   */
  int &getInfo(unsigned int n);
  /**
   * @brief Get the i-th info.
   *
   * @param i The index of the info, 1-based.
   * @param dest The destination.
   * @attention No bound checking is performed.
   */
  void getInfo(unsigned int n, int &dest);
  /**
   * @brief Set the i-th info.
   *
   * @param i The index of the info, 1-based.
   * @param value The value to be set.
   *
   * @attention No bound checking is performed.
   */
  void setInfo(unsigned int n, int value);
  /**
   * @brief Fetch the n-th page into the cache.
   *
   * @param n The index of the page, 1-based.
   * @param reset Whether to reset page content into 0.
   *
   * @attention No bound checking is performed.
   */
  void fetchPage(unsigned int n, bool reset = false);
  /**
   * @brief Flush the page cache back to the file.
   */
  void flush();
  /**
   * @brief Get the n-th page, 1-based.
   *
   * @param n The index of the page, 1-based.
   * @param dest The destination.
   *
   * @attention No bound checking is performed.
   */
  void getPage(unsigned n, int *dest);
  /**
   * @brief Set the n-th page, 1-based.
   *
   * @param n The index of the page, 1-based.
   * @param value The value to be set.
   *
   * @attention No bound checking is performed.
   */
  void setPage(unsigned n, const int *value);
  /**
   * @brief Get a part of the n-th page, 1-based.
   *
   * @param n The index of the page, 1-based.
   * @param offset The offset of the part, 0-based, in integers.
   * @param len The length of the part, the number of integers.
   * @param dest The destination.
   *
   * @attention Must read within the n-th page.
   * @attention No bound checking is performed.
   */
  void getPart(unsigned n, unsigned offset, unsigned len, int *dest);
  /**
   * @brief Set a part of the n-th page, 1-based.
   *
   * @param n The index of the page, 1-based.
   * @param offset The offset of the part, 0-based, in integers.
   * @param len The length of the part, the number of integers.
   * @param value The value to be set.
   *
   * @attention Must write within the n-th page.
   * @attention No bound checking is performed.
   */
  void setPart(unsigned n, unsigned offset, unsigned len, const int *value);
  /**
   * @brief Convert the page number and the offset to an absolute position.
   *
   * @param n The index of the page, 1-based.
   * @param offset The offset of the integer, 0-based, in integers, must be smaller than `kIntegerPerPage`.
   * @return unsigned int The absolute position.
   */
   static unsigned int toPosition(unsigned n, unsigned offset);
  /**
   * @brief Convert the absolute position to the page number and the offset.
   *
   * @param position The absolute position.
   * @return std::pair<unsigned, unsigned> The page number and the offset.
   */
  static std::pair<unsigned, unsigned> toPageOffset(unsigned position);
  /**
   * @brief Allocate a new page.
   *
   * @param value The value to be set.
   * @return unsigned int The index of the new page, 1-based.
   *
   * @note The new page is cleared and fetched into the cache.
   */
  unsigned int newPage(const int *value = nullptr);
  /**
   * @brief Deallocate a page.
   *
   * @param n The index of the page, 1-based.
   *
   * @note The page is not actually erased.
   * @note An integer is written to the beginning of the page to indicate the next free page.
   * @note The rest of the page is not modified.
   */
  void deletePage(unsigned int n);
};

}

#endif //BOOKSTORE_SRC_EXTERNAL_MEMORY_H_
