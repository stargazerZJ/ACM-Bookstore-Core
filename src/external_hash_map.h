//
// Created by zj on 11/29/2023.
//

#ifndef BOOKSTORE_SRC_EXTERNAL_HASH_MAP_H_
#define BOOKSTORE_SRC_EXTERNAL_HASH_MAP_H_

#include <map>
#include "external_memory.h"

namespace external_memory {
using Hash_t = unsigned long long;
class Hash {
  static Hash_t splitmix64(Hash_t x);
  Hash_t operator()(const std::string &str);
};
template<class Key> // the value is unsigned int
class Map {
 private:
  const std::string file_name_;
  Pages data_;
  Array dict_;
  int &size_ = data_.getInfo(1);
  int &global_depth_ = data_.getInfo(2);
  static constexpr unsigned int kMaxGlobalDepth = 23;
  static constexpr unsigned int kSizeOfPair = sizeof(Hash_t) + sizeof(unsigned int);
  static constexpr unsigned int kPairsPerPage = (kIntegerPerPage - sizeof(unsigned short) * 2) / kSizeOfPair;

  unsigned int getBucketId(const Hash_t &key);

  struct Bucket {
   private:
    void flush() const;
   public:
    unsigned int id = 0;
    unsigned int local_depth = 0;
    std::map<Hash_t, unsigned int> data;
    Map &map;
    explicit Bucket(Map &map);
    Bucket(Map &map, unsigned int id);
    explicit Bucket(Bucket &&bucket);
    Bucket &operator=(Bucket &&bucket);
    ~Bucket();
    explicit Bucket(const Bucket &bucket) = delete;
    Bucket &operator=(const Bucket &bucket) = delete;
    unsigned int size() const;
    bool full() const;
    void insert(const Hash_t &key, unsigned int value);
    void erase(const Hash_t &key);
    auto find(const Hash_t &key) const;
    auto end() const;
    Bucket split() &&;
  };

  unsigned int cache_index_ = 0;
  Bucket cache_ = {*this};

  void flush();
  Bucket &getBucket(const Hash_t &key);
  unsigned int splitBucket(const Hash_t &key);
  void deleteBucket(const Hash_t &key);
  void expand();

 public:
  explicit Map(const std::string &file_name);

  ~Map();

  void initialize(bool reset = false);

  void insert(const Key &key, unsigned int value);

  void erase(const Key &key);

  unsigned int at(const Key &key) const;

  unsigned int size() const;

  unsigned int globalDepth() const;
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
