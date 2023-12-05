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
/**
 * @brief A class to manage a set of vectors.
 * @details A vector is a sequence of non-zero integers.
 * @details A vector is identified by its position in the file.
 * @details When making changes to a vector, its position may change. If so, the return value of the modifying function will be true.
 * @details Internally, there are two types of vectors: small vectors and large vectors.
 * @details - Small vectors, whose capacity is strictly less than kIntegerPerPage, and is a power of 2, share a page with other small vectors of the same capacity.
 * @details - Large vectors, whose capacity is greater than or equal to kIntegerPerPage, have their own page(s).
 * @details The info file stores the information of all pages. It is cached in memory.
 *
 * @attention The position of a vector is not stored in the file. It is the user's responsibility to keep track of the position of a vector.
 * @attention The position of a vector may change after modifying it.
 * @attention Vector can only store non-zero integers. Storing zero is undefined behavior.
 *
 * @note Caching mechanism : If the vector is stored in a single page, the page is cached in memory. Otherwise, the vector is not cached.
 * @note When a vector is created, it is empty.
 * @note If the capacity of a vector is >= kIntegerPerPage, the vector will no longer change its position, because new data will be stored in a new page.
 */
class Vectors {
 private:
  const std::string file_name_; // file_name_ + "_info" and file_name_ + "_data"
  static constexpr unsigned int kInfoPerPage = 3; // number of information integers per page
  static constexpr unsigned int
      kCapacityLogMax = __builtin_ctz(kIntegerPerPage / 2); // log2 of the maximum capacity of a small vector
  Array info_; // the info file, storing the information of all pages, cached in memory
  Pages data_; // the data file, storing all vectors
  std::set<int> free_pages_of_capacity_[kCapacityLogMax + 1]; // free pages of capacity 2^i, only for small vectors
  /**
   * @brief The information of a page.
   */
  enum class kPageInfo : unsigned int {
    kCapacity =
    0, // capacity of the page. Specially, 0 means the page is not used, -1 means the page is used for large vectors but is not the first page of the vector
    kFreeHead = 1, // for small vectors, the head of the free space in the page.
    kUnoccupiedBeg =
    2, // for small vectors, the beginning of the known unoccupied space at the end of the page. May not be the actual beginning of the unoccupied space.
    kNextPage = 1, // for large vectors, the next page of the vector.
    kLastPage = 2 // for large vectors, the last page of the vector. Only valid for the first page of the vector.
  };
  /**
   * @brief Get the information of a page.
   *
   * @param type The type of the information.
   * @param n The page number.
   * @return The information.
   *
   * @attention No bound checking is performed.
   */
  [[nodiscard]] int getPageInfo(kPageInfo type, unsigned int n);
  /**
   * @brief Set the information of a page.
   *
   * @param type The type of the information.
   * @param n The page number.
   * @param value The information.
   *
   * @attention No bound checking is performed.
   */
  void setPageInfo(kPageInfo type, unsigned int n, int value);
  /**
   * @brief Get a page storing small vectors of the given capacity.
   *
   * @details
   * This function is called by `allocate`.
   * It will find a page storing small vectors of the given capacity that has free space.
   * If there is no such page, a new page will be allocated.
   *
   * @param capacity The capacity of the vector, must be a power of 2 and strictly less than kIntegerPerPage.
   * @return The page number.
   *
   * @attention `capacity` must be a power of 2 and strictly less than kIntegerPerPage. Otherwise, the behavior is undefined.
   */
  [[nodiscard]] unsigned int getPageOfCapacity(unsigned int capacity); // capacity < kIntegerPerPage
  /**
   * @brief Allocate space for a small vector, or (implicitly) a large vector of capacity kIntegerPerPage.
   *
   * @details
   * When `capacity` is kIntegerPerPage, this function will allocate a new page and convert the vector to a large vector.
   *
   * @param capacity The capacity of the vector, must be a power of 2 and <= kIntegerPerPage.
   * @return The offset of the allocated space.
   *
   * @note It's guaranteed that the allocated space is cleared.
   *
   * @attention `capacity` must be a power of 2 and <= kIntegerPerPage. Otherwise, the behavior is undefined.
   */
  [[nodiscard]] unsigned int allocate(unsigned int capacity); // and clear, and deal with capacity of kIntegerPerPage; capacity <= kIntegerPerPage
  /**
   * @brief Deallocate space for a small vector, or (implicitly) a large vector of capacity kIntegerPerPage.
   *
   * @details
   * When `capacity` is kIntegerPerPage, this function will delete the page entirely.
   *
   * @param capacity The capacity of the vector, must be a power of 2 and <= kIntegerPerPage.
   * @param n The page number.
   * @param offset The offset of the allocated space.
   *
   * @attention `capacity` must be a power of 2 and <= kIntegerPerPage. Otherwise, the behavior is undefined.
   */
  void deallocate(unsigned int n, unsigned int offset, unsigned int capacity);
  /**
   * @brief Allocate a new page.
   * @return unsigned int The index of the new page, 1-based.
   * @note The info of the page is cleared.
   */
  [[nodiscard]] unsigned int newPage();
  /**
   * @brief Delete a page.
   * @param n The index of the page, 1-based.
   * @note The info of the page is cleared, making it a free page.
   */
  void deletePage(unsigned int n);
  /**
   * @brief Clear the space for a vector.
   * @param capacity The capacity of the vector, must be a power of 2 and <= kIntegerPerPage.
   * @param n The index of the page, 1-based.
   * @param offset The offset of the allocated space.
   * @attention It's not checked whether the space is actually occupied by a single vector.
   */
  void clearSpace(unsigned int capacity, unsigned int n, unsigned int offset);
 public:
  /**
   * @brief Construct a new Vectors object.
   */
  explicit Vectors(std::string name = "vectors")
      : file_name_(std::move(name)), info_(file_name_ + "_info"), data_(file_name_ + "_data") {}
  Vectors(const Vectors &) = delete;
  Vectors &operator=(const Vectors &) = delete;
  /**
   * @brief Destroy the Vectors object.
   *
   * @details
   * The destructor closes the files.
   * If the cache is not empty, the cache is written back to the files.
   */
  ~Vectors() = default;
  /**
   * @brief Initialize the vectors.
   *
   * @details
   * When `reset` is `true`, the files are truncated.
   * Otherwise, the files are opened.
   *
   * @param reset Whether to reset the files.
   *
   * @attention `initialize` must be called before using the vectors.
   * @attention `initialize` must not be called twice.
   */
  void initialize(bool reset = false);
  /**
   * @brief a class to manage a vector.
   *
   * @details
   * When calling a member function of this class, the vector may change its position.
   * If so, the return value of the member function will be true.
   *
   * @attention The position of a vector is not stored in the file. It is the user's responsibility to keep track of the position of a vector.
   */
  class Vector {
    friend class Vectors;
   private:
    Vectors &vectors_; // the vectors class where the vector belongs to
    unsigned int pos_; // the position of the vector. Specially, 0 means the vector is empty.
    unsigned int page_id_ = 0; // the page number of the vector, 1-based
    unsigned int offset_ = 0; // the offset of the vector in the page
    explicit Vector(Vectors &vectors, unsigned int pos = 0) :
        vectors_(vectors), pos_(pos) { updatePos(pos); }
    /**
     * @brief Only for large vectors. Append a new page to the vector.
     * @return the index of the new page, 1-based.
     */
    unsigned int appendPage();
    /**
     * @brief Only for large vectors. Discard all pages after the given page.
     * @param last_page The index of the last page to be kept, 1-based.
     */
    void discardAfter(unsigned int last_page);
    /**
     * @brief Update the position of the vector.
     * @param new_pos The new position.
     */
    void updatePos(unsigned int new_pos);
   public:
    ~Vector() = default;
    /**
     * @brief Get the position of the vector.
     * @return The position of the vector.
     */
    [[nodiscard]] unsigned int getPos() const;
    /**
     * @brief Get the data of the vector.
     * @return The data of the vector.
     * @attention For better performance, save the returned vector and reuse it.
     * @attention The returned vector is read from the cache or even the file every time this function is called.
     */
    [[nodiscard]] std::vector<int> getData();
    /**
     * @brief Get the capacity of the vector.
     * @return The capacity of the vector.
     */
    [[nodiscard]] unsigned int capacity() const;
    /**
     * @brief Append a new element to the vector.
     * @param value The value to be appended.
     * @return Whether the position of the vector has changed.
     * @attention The change is committed to cache or file immediately.
     */
    [[nodiscard]] bool push_back(int value);
    /**
     * @brief Modify the vector.
     * @param data The new data of the vector.
     * @return Whether the position of the vector has changed.
     * @attention The vector will be cleared before modifying.
     * @attention The change is committed to cache or file immediately.
     */
    [[nodiscard]] bool rewrite(std::vector<int> &&data);
    /**
     * @brief Delete the vector.
     * @return Whether the position of the vector has changed.
     */
    [[nodiscard]] bool del();
  };
  /**
   * @brief Get a vector.
   * @param pos The position of the vector.
   * @return The vector.
   */
  [[nodiscard]] Vector getVector(unsigned int pos = 0);
  /**
   * @brief Create an empty vector.
   * @return The new vector.
   */
  [[nodiscard]] Vector newVector();
};
}
#endif //BOOKSTORE_SRC_EXTERNAL_MEMORY_CPP_EXTERNAL_VECTORS_H_
