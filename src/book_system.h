//
// Created by zj on 11/29/2023.
//

#ifndef BOOKSTORE_SRC_BOOK_SYSTEM_H_
#define BOOKSTORE_SRC_BOOK_SYSTEM_H_

#include "external_memory.h"
#include "external_hash_map.h"

using ISBN_t = char[20];
using Title_t = char[60];

struct Book {
  unsigned int id;
  std::string ISBN;
  std::string title;
  std::string author;
  std::string keywords;
  unsigned int price; // in cents
  unsigned int quantity;

  Book();

  static constexpr unsigned int byte_size() {
    return sizeof(ISBN_t) + 3 * sizeof(Title_t) + 2 * sizeof(unsigned int);
  }

  void toBytes(char *dest);

  void fromBytes(const char *src);

  static std::vector<std::string> unpack_keywords(const std::string &keywords);
};

class BookSystem {
 private:
  const std::string file_prefix_;
  external_memory::ExternalList<Book> book_list_;
  external_memory::ExternalHashMap<ISBN_t> ISBN_to_id_;
  external_memory::ExternalHashMultiMap<Title_t> title_to_id_;
  external_memory::ExternalHashMultiMap<Title_t> author_to_id_;
  external_memory::ExternalHashMultiMap<Title_t> keyword_to_id_;
 public:
  BookSystem(std::string file_prefix = "");

  ~BookSystem();

  void initialize(bool reset = false);

  unsigned int find(const std::string &ISBN) const;

  Book get(unsigned int id) const;

  unsigned int select(const std::string &ISBN); // return ID of the book, if not found, create a new book and return its ID

  void modify(const Book &old, const Book &new_book);

  struct SearchResult {
    std::vector<Book> books;

    SearchResult &filter(const Book &params);
  };

  SearchResult searchByISBN(const std::string &ISBN) const;

  SearchResult searchByTitle(const std::string &name) const;

  SearchResult searchByAuthor(const std::string &author) const;

  SearchResult searchByKeyword(const std::string &keyword) const;

  SearchResult search(const Book &params) const;
};

#endif //BOOKSTORE_SRC_BOOK_SYSTEM_H_
