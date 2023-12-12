//
// Created by zj on 11/29/2023.
//

#include <numeric>
#include "book_system.h"
void Book::toBytes(char *dest) const {
  std::strncpy(dest, ISBN.c_str(), sizeof(ISBN_t));
  std::strncpy(dest + sizeof(ISBN_t), title.c_str(), sizeof(Title_t));
  std::strncpy(dest + sizeof(ISBN_t) + sizeof(Title_t), author.c_str(), sizeof(Title_t));
  std::strncpy(dest + sizeof(ISBN_t) + 2 * sizeof(Title_t), keywords.c_str(), sizeof(Title_t));
  *reinterpret_cast<unsigned long long *>(dest + sizeof(ISBN_t) + 3 * sizeof(Title_t)) = price;
  *reinterpret_cast<unsigned int *>(dest + sizeof(ISBN_t) + 3 * sizeof(Title_t) + sizeof(unsigned long long)) =
      quantity;
}
void Book::fromBytes(const char *src) {
//  ISBN = std::string(src, sizeof(ISBN_t));
//  title = std::string(src + sizeof(ISBN_t), sizeof(Title_t));
//  author = std::string(src + sizeof(ISBN_t) + sizeof(Title_t), sizeof(Title_t));
//  keywords = std::string(src + sizeof(ISBN_t) + 2 * sizeof(Title_t), sizeof(Title_t));
  ISBN = external_memory::strNRead(src, sizeof(ISBN_t));
  title = external_memory::strNRead(src + sizeof(ISBN_t), sizeof(Title_t));
  author = external_memory::strNRead(src + sizeof(ISBN_t) + sizeof(Title_t), sizeof(Title_t));
  keywords = external_memory::strNRead(src + sizeof(ISBN_t) + 2 * sizeof(Title_t), sizeof(Title_t));
  price = *reinterpret_cast<const unsigned long long *>(src + sizeof(ISBN_t) + 3 * sizeof(Title_t));
  quantity =
      *reinterpret_cast<const unsigned int *>(src + sizeof(ISBN_t) + 3 * sizeof(Title_t) + sizeof(unsigned long long));
}
Book::Book(const char *bytes) {
  fromBytes(bytes);
}
std::vector<std::string> Book::unpackKeywords(const std::string &keywords) {
  std::vector<std::string> result;
  std::string::size_type start = 0, end = 0;
  while (end != std::string::npos) {
    end = keywords.find('|', start);
    result.push_back(keywords.substr(start, end - start));
    start = end + 1;
  }
  std::sort(result.begin(), result.end());
  return result;
}
kExceptionType Book::regularizeKeywords(std::string &keywords) {
  auto keywords_vec = unpackKeywords(keywords);
  std::sort(keywords_vec.begin(), keywords_vec.end());
  if (std::unique(keywords_vec.begin(), keywords_vec.end()) != keywords_vec.end()) {
    return kExceptionType::K_DUPLICATED_KEYWORDS;
  }
  keywords = std::accumulate(keywords_vec.begin(), keywords_vec.end(), std::string(),
                             [](const std::string &a, const std::string &b) {
                               return a + '|' + b;
                             });
  return kExceptionType::K_SUCCESS;
}
bool Book::hasKeyword(const std::string &keywords, const std::string &keyword) {
  auto keywords_vec = unpackKeywords(keywords);
  return std::binary_search(keywords_vec.begin(), keywords_vec.end(), keyword);
}
template
class external_memory::List<Book, false>;
void BookSystem::initialize(bool reset) {
  book_list_.initialize(reset);
  ISBN_to_id_.initialize(reset);
  title_to_id_.initialize(reset);
  author_to_id_.initialize(reset);
  keyword_to_id_.initialize(reset);
}
unsigned int BookSystem::find(const std::string &ISBN) {
  return ISBN_to_id_.at(ISBN);
}
Book BookSystem::get(unsigned int id) {
  return book_list_.get(id);
}
unsigned int BookSystem::select(const std::string &ISBN) {
  unsigned int &id = ISBN_to_id_[ISBN];
  if (!id) id = book_list_.insert(Book(ISBN));
  return id;
}
kExceptionType BookSystem::modify(unsigned int id, const Book &old, const Book &new_book) {
  if (old.ISBN != new_book.ISBN) {
    unsigned int &new_id = ISBN_to_id_[new_book.ISBN];
    if (new_id) return kExceptionType::K_DUPLICATED_ISBN;
    ISBN_to_id_.erase(old.ISBN);
    new_id = id;
  }
  // There is no erase method in MultiMap. Erasing is done lazily.
  if (old.title != new_book.title) {
    title_to_id_.insert(new_book.title, id);
  }
  if (old.author != new_book.author) {
    author_to_id_.insert(new_book.author, id);
  }
  if (old.keywords != new_book.keywords) {
    auto old_keywords = Book::unpackKeywords(old.keywords);
    auto new_keywords = Book::unpackKeywords(new_book.keywords);
    auto old_it = old_keywords.begin();
    for (auto new_it = new_keywords.begin(); new_it != new_keywords.end(); ++new_it) {
      while (old_it != old_keywords.end() && *old_it < *new_it) {
        // There is no erase method in MultiMap. Erasing is done lazily.
        ++old_it;
      }
      if (old_it == old_keywords.end() || *old_it > *new_it) {
        keyword_to_id_.insert(*new_it, id);
      } else { // *old_it == *new_it
        ++old_it;
      }
    }
  }
  book_list_.set(id, new_book);
  return kExceptionType::K_SUCCESS;
}
BookSystem::SearchResult BookSystem::searchByISBN(const std::string &ISBN) {
  unsigned int id = find(ISBN);
  if (!id) return {};
  return {std::vector<Book>{get(id)}};
}
BookSystem::SearchResult BookSystem::searchByTitle(const std::string &title) {
  SearchResult result;
  auto ids = title_to_id_.findAll(title);
  size_t size_before = ids.size();
  std::sort(ids.begin(), ids.end());
  ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
  result.books.reserve(ids.size());
  std::erase_if(ids, [this, &result, &title](unsigned int id) {
    Book book = get(id);
    if (book.title == title) {
      result.books.push_back(book);
      return false;
    } else {
      return true;
    }
  });
  if (ids.size() < size_before) {
    title_to_id_.update(title, std::move(ids));
  }
  return result;
}
BookSystem::SearchResult BookSystem::searchByAuthor(const std::string &author) {
  SearchResult result;
  auto ids = author_to_id_.findAll(author);
  size_t size_before = ids.size();
  std::sort(ids.begin(), ids.end());
  ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
  result.books.reserve(ids.size());
  std::erase_if(ids, [this, &result, &author](unsigned int id) {
    Book book = get(id);
    if (book.author == author) {
      result.books.push_back(book);
      return false;
    } else {
      return true;
    }
  });
  if (ids.size() < size_before) {
    author_to_id_.update(author, std::move(ids));
  }
  return result;
}
BookSystem::SearchResult BookSystem::searchByKeyword(const std::string &keyword) {
  SearchResult result;
  auto ids = keyword_to_id_.findAll(keyword);
  size_t size_before = ids.size();
  std::sort(ids.begin(), ids.end());
  ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
  result.books.reserve(ids.size());
  std::erase_if(ids, [this, &result, &keyword](unsigned int id) {
    Book book = get(id);
    if (Book::hasKeyword(book.keywords, keyword)) {
      result.books.push_back(book);
      return false;
    } else {
      return true;
    }
  });
  if (ids.size() < size_before) {
    keyword_to_id_.update(keyword, std::move(ids));
  }
  return result;
}
std::vector<Book> BookSystem::search(const Book &params) {
  SearchResult result;
  if (!params.ISBN.empty()) {
    result = searchByISBN(params.ISBN);
  } else if (!params.title.empty()) {
    result = searchByTitle(params.title);
  } else if (!params.author.empty()) {
    result = searchByAuthor(params.author);
  } else if (!params.keywords.empty()) {
    result = searchByKeyword(params.keywords);
  } else {
    result = getAllBooks();
  }
  return result.sort().books;
}
BookSystem::SearchResult BookSystem::getAllBooks() {
  SearchResult result;
  book_list_.cache();
  result.books.reserve(book_list_.size());
  for (unsigned int id = 1; id <= book_list_.size(); ++id) {
    result.books.push_back(get(id));
  }
  return result;
}
BookSystem::SearchResult &BookSystem::SearchResult::filter(const Book &params) {
  std::erase_if(books, [&params](const Book &book) {
    return // !params.ISBN.empty() && book.ISBN != params.ISBN ||
        !params.title.empty() && book.title != params.title ||
            !params.author.empty() && book.author != params.author ||
            !params.keywords.empty() && !Book::hasKeyword(book.keywords, params.keywords);
//          book.price != params.price ||
//          book.quantity != params.quantity;
  });
  return *this;
}
BookSystem::SearchResult &BookSystem::SearchResult::sort() {
  std::sort(books.begin(), books.end(), [](const Book &a, const Book &b) {
    return a.ISBN < b.ISBN;
  });
  return *this;
}
