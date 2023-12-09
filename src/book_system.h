//
// Created by zj on 11/29/2023.
//

#ifndef BOOKSTORE_SRC_BOOK_SYSTEM_H_
#define BOOKSTORE_SRC_BOOK_SYSTEM_H_

#include "external_memory.h"
#include "external_hash_map.h"
#include "log.h"

/**
 * @brief The Book class
 * @details The Book class is used to store the information of a book.
 * @details The information of a book includes ISBN, title, author, keywords, price and quantity.
 * @details ISBN is the unique identifier of a book. However, as ISBN can be modified, books are actually identified by their IDs in this system.
 * @details The keywords of a book are separated by '|'.
 * @details The price of a book is in cents.
 * @details The quantity of a book is the number of copies of the book in the store.
 * @details The information of a book is stored in external memory.
 */
struct Book {
  using ISBN_t = char[20]; // The type of ISBN is char[20] in external memory. However, in memory, it is std::string.
  using Title_t = char[60]; // The type of title, author and keywords is char[60] in external memory. However, in memory, they are std::string.
  std::string
      ISBN; // ISBN is the unique identifier of a book. However, as ISBN can be modified, books are actually identified by their IDs in this system.
  std::string title; // the title of the book
  std::string author; // the author of the book
  std::string keywords; // multiple keywords are separated by '|'
  unsigned int price = 0; // in cents TODO: price may be 0, and may be greater than INT_MAX
  unsigned int quantity = 0; // the number of copies of the book in the store
  /**
   * @brief Construct a new Book object
   */
  Book() = default;
  /**
   * @brief Construct a new Book object
   * @details Construct a new Book object with the given ISBN, title, author, keywords, price and quantity.
   * @param ISBN The ISBN of the book.
   * @param title The title of the book.
   * @param author The author of the book.
   * @param keywords The keywords of the book.
   * @param price The price of the book.
   * @param quantity The quantity of the book.
   */
  explicit Book(std::string ISBN, std::string title = "", std::string author = "", std::string keywords = "",
                unsigned int price = 0, unsigned int quantity = 0)
      : ISBN(std::move(ISBN)), title(std::move(title)), author(std::move(author)), keywords(std::move(keywords)),
        price(price), quantity(quantity) {}
  /**
   * @brief Construct a new Book object from bytes
   * @param bytes
   */
  explicit Book(const char *bytes);
  /**
   * @brief Get the byte size of a Book object when stored in external memory
   */
  static constexpr unsigned int byte_size() {
    return sizeof(ISBN_t) + 3 * sizeof(Title_t) + 2 * sizeof(unsigned int);
  }
  /**
   * @brief Convert a Book object to bytes
   * @param dest The destination of the bytes
   */
  void toBytes(char *dest) const;
  /**
   * @brief Convert bytes to a Book object
   * @param src The source of the bytes
   */
  void fromBytes(const char *src);
  /**
   * @brief Unpack the keywords of a book
   * @param keywords The keywords of the book
   * @return std::vector<std::string> The unpacked keywords
   * @attention The keywords must be regularized before unpacking.
   * @see regularizeKeywords
   */
  [[nodiscard]] static std::vector<std::string> unpackKeywords(const std::string &keywords);
  /**
   * @brief Regularize the keywords of a book
   * @param keywords The keywords of the book
   * @return K_SUCCESS if the keywords are valid
   * @return K_DUPLICATED_KEYWORDS if there are duplicated keywords.
   * @details The keywords are sorted.
   * @details The keywords are separated by '|'.
   */
  [[nodiscard]] static kExceptionType regularizeKeywords(std::string &keywords);
  /**
   * @brief Check if a book has a keyword
   * @param keywords The keywords of the book
   * @param keyword The keyword to be checked
   * @return true if the book has the keyword, false otherwise
   */
  [[nodiscard]] static bool hasKeyword(const std::string &keywords, const std::string &keyword);
};

/**
 * @brief The BookSystem class
 * @details The BookSystem class is used to manage the books in the store.
 * @details The BookSystem class provides the following functions:
 * @details 1. find: find a book by ISBN
 * @details 2. get: get a book by ID
 * @details 3. select: select a book by ISBN
 * @details 4. modify: modify a book
 * @details 5. search: search books by ISBN, title, author or keyword
 * @details The information of books is stored in external memory.
 * @details The BookSystem class uses external_memory::List, external_memory::Map, external_memory::MultiMap and external_memory::Vectors to store the information of books. The external_memory::Vectors is shared with other systems.
 * @details The external_memory::MultiMap may contain duplicated or erased items because modifying doesn't erase the items immediately. They are lazily removed by `searchByTitle`, `searchByAuthor` and `searchByKeyword`.
 * @attention The BookSystem class must be initialized before using.
 */
class BookSystem {
 private:
  const std::string file_prefix_; // the prefix (including path) of the files storing the information of books
  external_memory::List<Book, false> book_list_; // the list of books
  external_memory::Map<std::string> ISBN_to_id_; // the map from ISBN to ID
  external_memory::MultiMap<std::string> title_to_id_; // the map from title to ID
  external_memory::MultiMap<std::string> author_to_id_; // the map from author to ID
  external_memory::MultiMap<std::string> keyword_to_id_; // the map from keyword to ID
  external_memory::Vectors &vectors_; // the vectors used by external memory, shared with other systems
  struct SearchResult {
    std::vector<Book> books;

    SearchResult &filter(const Book &params); // ISBN is not considered here
    SearchResult &sort(); // sort by ISBN
  };

  SearchResult searchByISBN(const std::string &ISBN);
  SearchResult searchByTitle(const std::string &title); // Also removes the duplicated and erased items in title_to_id_
  SearchResult searchByAuthor(const std::string &author); // Also removes the duplicated and erased items in author_to_id_
  SearchResult searchByKeyword(const std::string &keyword); // There must be only one keyword, which is not checked here. Also removes the duplicated and erased items in keyword_to_id_
 public:
  /**
   * @brief Construct a new BookSystem object
   * @param file_prefix The prefix (and path) of the files storing the information of books
   * @param vectors The vectors used by external memory, shared with other systems
   */
  BookSystem(std::string file_prefix, external_memory::Vectors &vectors)
      : file_prefix_(std::move(file_prefix)), book_list_(file_prefix_ + "_list"),
        ISBN_to_id_(file_prefix_ + "_ISBN"),
        title_to_id_(file_prefix_ + "_title", vectors),
        author_to_id_(file_prefix_ + "_author", vectors),
        keyword_to_id_(file_prefix_ + "_keyword", vectors),
        vectors_(vectors) {}
  /**
   * @brief Destroy the BookSystem object
   */
  ~BookSystem() = default;
  /**
   * @brief Initialize the BookSystem object
   * @param reset Whether to reset the BookSystem object
   * @attention vectors_ must be initialized before calling this function, no matter whether reset is true or not.
   * @attention This function must not be called twice.
   * @attention If reset is true, all the information of books will be lost.
   */
  void initialize(bool reset = false);
  /**
   * @brief Find a book by ISBN
   * @param ISBN The ISBN of the book
   * @return unsigned int The ID of the book
   * @return 0 if the book is not found
   */
  [[nodiscard]] unsigned int find(const std::string &ISBN); // return ID of the book, if not found, return 0
  /**
   * @brief Get a book by ID
   * @param id The ID of the book
   * @return Book The book
   */
  [[nodiscard]] Book get(unsigned int id); // no bound checking
  /**
   * @brief Select a book by ISBN
   * @param ISBN The ISBN of the book
   * @return unsigned int The ID of the book
   * @details If the book is not found, create a new book and return its ID.
   */
  [[nodiscard]] unsigned int select(const std::string &ISBN); // return ID of the book, if not found, create a new book and return its ID
  /**
   * @brief Modify a book
   * @param id The ID of the book
   * @param old The old information of the book
   * @param new_book The new information of the book
   * @return K_SUCCESS if the book is modified successfully
   * @return K_DUPLICATED_ISBN if the ISBN of the book is duplicated
   * @details The information of the book is modified in external memory.
   * @details The information of the book is modified in external_memory::List, external_memory::Map, external_memory::MultiMap and external_memory::Vectors.
   * @details The external_memory::MultiMap may contain duplicated or erased items because modifying doesn't erase the items immediately. They are lazily removed by `searchByTitle`, `searchByAuthor` and `searchByKeyword`.
   */
  [[nodiscard]] kExceptionType modify(unsigned int id, const Book &old, const Book &new_book);
  /**
   * @brief Search books by ISBN, title, author or keyword
   * @param params The parameters of the book
   * @return std::vector<Book> The books
   * @details The result is sorted by ISBN.
   */
  [[nodiscard]] std::vector<Book> search(const Book &params);
};

#endif //BOOKSTORE_SRC_BOOK_SYSTEM_H_