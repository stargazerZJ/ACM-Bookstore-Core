//
// Created by zj on 11/29/2023.
//

#ifndef BOOKSTORE_SRC_EXTERNAL_HASH_MAP_H_
#define BOOKSTORE_SRC_EXTERNAL_HASH_MAP_H_

#include <map>
#include "external_memory.h"
#include "external_vector.h"

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
  Array dict_;
  Pages data_;
  int &size_ = data_.getInfo(1);
  int &global_depth_ = data_.getInfo(2);
  static constexpr unsigned int kMaxGlobalDepth = 23;
  static constexpr unsigned int kSizeOfPair = sizeof(Hash_t) + sizeof(unsigned int);
  static constexpr unsigned int kPairsPerPage = (kIntegerPerPage - sizeof(unsigned short) * 2) / kSizeOfPair;

  inline unsigned int getBucketId(const Hash_t &key);

  struct Bucket {
   private:
    void flush() const;
   public:
    Map<Key> &map;
    unsigned int id = 0;
    unsigned int local_depth = 0;
    std::map<Hash_t, unsigned int> data;
    Bucket(Map &map) : map(map) {};
    Bucket(Map &map, unsigned int id);
    Bucket(Bucket &&bucket) noexcept;
    Bucket &operator=(Bucket &&bucket) noexcept;
    ~Bucket();
    explicit Bucket(const Bucket &bucket) = delete;
    Bucket &operator=(const Bucket &bucket) = delete;
    [[nodiscard]] inline Hash_t getLocalHighBit(const Hash_t &key) const;
    [[nodiscard]] unsigned int size() const;
    [[nodiscard]] bool full() const; // remember that inserting to a full bucket is possible when the key is already in the bucket!
    [[nodiscard]] bool empty() const;
    [[nodiscard]] bool contains(const Hash_t &key) const;
    [[nodiscard]] unsigned int &operator[](const Hash_t &key); // using this method to insert into a full bucket is undefined!
    [[nodiscard]] bool insert(const Hash_t &key, unsigned int value);
    [[nodiscard]] bool erase(const Hash_t &key);
    [[nodiscard]] auto find(const Hash_t &key) const;
    [[nodiscard]] auto end() const;
    Bucket split() &&; // the id of the new bucket is not set!
  };

  Bucket cache_ = {*this};

  void flush();
  Bucket &getBucket(const Hash_t &key);
  unsigned int splitBucket(const Hash_t &key);
  void deleteBucket(const Hash_t &key);
  void expand();
  void insertByHash(const Hash_t &key, unsigned int value);
  void eraseByHash(const Hash_t &key);

 public:
  explicit Map(std::string file_name)
      : file_name_(std::move(file_name)), dict_(file_name_ + "_dict"), data_(file_name_ + "_data") {};

  ~Map();

  void initialize(bool reset = false);

  void insert(const Key &key, unsigned int value);

  unsigned int &operator[](const Key &key);

  void erase(const Key &key);

  unsigned int at(const Key &key) const;

  unsigned int size() const;

  unsigned int globalDepth() const;
};
template<class Key>
unsigned int Map<Key>::globalDepth() const {
  return global_depth_;
}
template<class Key>
unsigned int Map<Key>::size() const {
  return size_;
}
template<class Key>
unsigned int Map<Key>::at(const Key &key) const {
  Hash_t hash = Hash()(key);
  Bucket &bucket = getBucket(hash);
  auto it = bucket.find(hash);
  if (it == bucket.end()) {
    return 0;
  } else {
    return it->second;
  }
}
template<class Key>
void Map<Key>::erase(const Key &key) {
  Hash_t hash = Hash()(key);
  eraseByHash(hash);
}
template<class Key>
void Map<Key>::eraseByHash(const Hash_t &key) {
  Bucket &bucket = getBucket(key);
  if (bucket.erase(key)) {
    size_--;
  }
  if (bucket.empty() && bucket.local_depth > 0) {
    deleteBucket(key);
  }
}
template<class Key>
unsigned int &Map<Key>::operator[](const Key &key) {
  Hash_t hash = Hash()(key);
  insertByHash(hash, 0);
  return getBucket(hash)[hash];
}
template<class Key>
void Map<Key>::insert(const Key &key, unsigned int value) {
  Hash_t hash = Hash()(key);
  insertByHash(hash, value);
}
template<class Key>
void Map<Key>::insertByHash(const Hash_t &key, unsigned int value) {
  Bucket &bucket = getBucket(key);
  if (bucket.full() && !bucket.contains(key)) {
    if (bucket.local_depth == global_depth_) {
      expand();
    }
    splitBucket(key);
    insertByHash(key, value);
  } else {
    size_ += bucket.insert(key, value);
  }
}
template<class Key>
Map<Key>::~Map() {
  flush();
}
template<class Key>
void Map<Key>::expand() {
  if (global_depth_ == kMaxGlobalDepth) {
    throw std::runtime_error("The global depth has reached the maximum!");
  }
  dict_.double_size();
  global_depth_++;
}
template<class Key>
void Map<Key>::deleteBucket(const Hash_t &key) {
  Bucket old(std::move(getBucket(key)));
  unsigned int bucket_id = old.id;
  unsigned int local_depth = old.local_depth;
  unsigned int sibling_id = getBucketId(key ^ (1 << (local_depth - 1)));
  for (unsigned int i = (key & ((1 << local_depth) - 1)); i < (1 << global_depth_); i += (1 << local_depth)) {
    dict_.set(i, sibling_id);
  }
  data_.deletePage(bucket_id);
  old.id = 0;
}
template<class Key>
unsigned int Map<Key>::splitBucket(const Hash_t &key) {
  Bucket old(std::move(getBucket(key)));
  bool flag = old.getLocalHighBit(key);
  unsigned int new_id = data_.newPage();
  Bucket new_bucket = std::move(old.split());
  new_bucket.id = new_id;
  unsigned int new_local_depth = new_bucket.local_depth;
  for (unsigned int i =
      (key & ((1 << new_local_depth) - 1))
          | (1 << (new_local_depth - 1));
       i < (1 << global_depth_); i += (1 << new_local_depth)) {
    dict_.set(i, new_id);
  }
  cache_ = std::move(flag ? new_bucket : old);
  return new_id;
}
template<class Key>
Map<Key>::Bucket &Map<Key>::getBucket(const Hash_t &key) {
  unsigned int bucket_id = getBucketId(key);
  if (bucket_id == cache_.id) {
    return cache_;
  } else {
    cache_ = {*this, bucket_id};
    return cache_;
  }
}
template<class Key>
void Map<Key>::flush() {
  cache_ = {*this};
}
template<class Key>
void Map<Key>::initialize(bool reset) {
  data_.initialize(reset);
  dict_.initialize(reset);
  dict_.cache();
  if (reset) {
    unsigned int id = data_.newPage();
    Bucket bucket(*this, id);
    dict_.push_back(id);
  }
}
template<class Key>
unsigned int Map<Key>::getBucketId(const Hash_t &key) {
  return dict_.get(key & ((1 << global_depth_) - 1));
}
template<class Key> // the value is int
class MultiMap {
 private:
  const std::string file_name_;
  Map<Key> vector_pos_;
  Vectors &vectors_;
 public:
  MultiMap(std::string file_name, Vectors &vectors) : file_name_(std::move(file_name)), vectors_(vectors) {};

  ~MultiMap();

  void initialize(bool reset = false);

  void insert(const Key &key, int value);

  void rewrite(const Key &key, std::vector<int> &&values);

  std::vector<int> findAll(const Key &key) const &&; // the result may contain duplicated values as well as deleted values (as there is no erase method). It's the user's responsibility to remove them. It's recommended to call `rewrite` to remove them in this multimap.
};
template<class Key>
std::vector<int> MultiMap<Key>::findAll(const Key &key) const &&{
  unsigned int pos = vector_pos_.at(key);
  return vectors_.getVector(pos).getData();
}
template<class Key>
void MultiMap<Key>::rewrite(const Key &key, std::vector<int> &&values) {
  unsigned int pos = vector_pos_.at(key);
  auto vector = vectors_.getVector(pos);
  if (vector.rewrite(std::move(values))) {
    pos = vector.getPos();
    vector_pos_[key] = pos;
  }
}
template<class Key>
void MultiMap<Key>::insert(const Key &key, int value) {
  unsigned int pos = vector_pos_.at(key);
  auto vector = vectors_.getVector(pos);
  if (vector.push_back(value)) {
    pos = vector.getPos();
    vector_pos_[key] = pos;
  }
}
template<class Key>
void MultiMap<Key>::initialize(bool reset) {
  vector_pos_.initialize(reset);
}
}

#endif //BOOKSTORE_SRC_EXTERNAL_HASH_MAP_H_
