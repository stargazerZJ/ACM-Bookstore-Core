//
// Created by zj on 11/29/2023.
//

#include "external_hash_map.h"
namespace external_memory {
Hash::Hash_t Hash::splitmix64(external_memory::Hash::Hash_t x) {
  // Reference: http://xorshift.di.unimi.it/splitmix64.c
  x += 0x9e3779b97f4a7c15;
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
  x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
  return x ^ (x >> 31);
}
Hash::Hash_t Hash::operator()(const std::string &str) {
  // compute the hash every 8 characters to increase the speed
  Hash_t hash = 0;
  unsigned int i = 0;
  for (; i + 8 <= str.size(); i += 8) {
    hash = splitmix64(hash ^ *reinterpret_cast<const Hash_t *>(str.data() + i));
  }
  for (; i < str.size(); ++i) {
    hash = splitmix64(hash ^ str[i]);
  }
  return hash;
}
}
