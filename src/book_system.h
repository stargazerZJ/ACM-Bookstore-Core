//
// Created by zj on 11/29/2023.
//

#ifndef BOOKSTORE_SRC_BOOK_SYSTEM_H_
#define BOOKSTORE_SRC_BOOK_SYSTEM_H_

#include "external_memory.h"
#include "external_hash_map.h"
#include "log.h"

using ISBN_t = char[20];
using Title_t = char[60];

struct Book {
  std::string ISBN;
  std::string title;
  std::string author;
  std::string keywords; // multiple keywords are separated by '|'
  unsigned int price = 0; // in cents
  unsigned int quantity = 0;

  Book() = default;

  explicit Book(std::string ISBN, std::string title = "", std::string author = "", std::string keywords = "",
                unsigned int price = 0, unsigned int quantity = 0)
      : ISBN(std::move(ISBN)), title(std::move(title)), author(std::move(author)), keywords(std::move(keywords)),
        price(price), quantity(quantity) {}

  explicit Book(const char *bytes);

  static constexpr unsigned int byte_size() {
    return sizeof(ISBN_t) + 3 * sizeof(Title_t) + 2 * sizeof(unsigned int);
  }

  void toBytes(char *dest) const;

  void fromBytes(const char *src);

  static std::vector<std::string> unpackKeywords(const std::string &keywords);

  static kExceptionType regularizeKeywords(std::string &keywords);

  static bool hasKeyword(const std::string &keywords, const std::string &keyword);
};

class BookSystem {
 private:
  const std::string file_prefix_;
  external_memory::List<Book, false> book_list_;
  external_memory::Map<std::string> ISBN_to_id_;
  external_memory::MultiMap<std::string> title_to_id_;
  external_memory::MultiMap<std::string> author_to_id_;
  external_memory::MultiMap<std::string> keyword_to_id_;
  external_memory::Vectors &vectors_;
  struct SearchResult {
    std::vector<Book> books;

    SearchResult &filter(const Book &params); // ISBN is not considered here
  };

  SearchResult searchByISBN(const std::string &ISBN);

  SearchResult searchByTitle(const std::string &title);

  SearchResult searchByAuthor(const std::string &author);

  SearchResult searchByKeyword(const std::string &keyword); // there must be only one keyword, which is not checked here
 public:
  BookSystem(std::string file_prefix, external_memory::Vectors &vectors)
      : file_prefix_(std::move(file_prefix)), book_list_(file_prefix_ + "_list"),
        ISBN_to_id_(file_prefix_ + "_ISBN"),
        title_to_id_(file_prefix_ + "_title", vectors),
        author_to_id_(file_prefix_ + "_author", vectors),
        keyword_to_id_(file_prefix_ + "_keyword", vectors),
        vectors_(vectors) {}

  ~BookSystem() = default;

  void initialize(bool reset = false);

  unsigned int find(const std::string &ISBN); // return ID of the book, if not found, return 0

  Book get(unsigned int id); // no bounds check

  unsigned int select(const std::string &ISBN); // return ID of the book, if not found, create a new book and return its ID

  kExceptionType modify(unsigned int id, const Book &old, const Book &new_book);

  std::vector<Book> search(const Book &params);
};

#endif //BOOKSTORE_SRC_BOOK_SYSTEM_H_