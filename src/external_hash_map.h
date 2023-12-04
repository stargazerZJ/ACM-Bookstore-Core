//
// Created by zj on 11/29/2023.
//

#ifndef BOOKSTORE_SRC_EXTERNAL_HASH_MAP_H_
#define BOOKSTORE_SRC_EXTERNAL_HASH_MAP_H_

#include "external_memory.h"

namespace external_memory {
template<class Key> // the value is unsigned int
class Map {
 private:
  unsigned int size_;
  unsigned int map_size_;
  ListHelper<Key, 2> hash_map_;
  static constexpr unsigned int size_of_Key_ = sizeof(Key);

  bool need_rehash() const;

  void rehash();

 public:
  explicit Map(const std::string &file_name);

  ~Map();

  void initialize(const std::string &file_name);

  void insert(const Key &key, unsigned int value);

  void erase(const Key &key);

  unsigned int at(const Key &key) const;

  unsigned int size() const;

  unsigned int map_size() const;
};
template<class Key>
class MultiMap {
 private:
  Map<Key> vector_pos_;
 public:
  explicit MultiMap(const std::string &file_name);

  ~MultiMap();

  void initialize(const std::string &file_name);

  void insert(const Key &key, unsigned int value);

  void erase(const Key &key, unsigned int value);

  std::vector<unsigned int> findAll(const Key &key) const;

  unsigned int size() const;

  unsigned int map_size() const;
};
}

#endif //BOOKSTORE_SRC_EXTERNAL_HASH_MAP_H_
