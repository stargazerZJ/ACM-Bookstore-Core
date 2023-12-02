//
// Created by zj on 11/29/2023.
//

#include "external_memory.h"

namespace external_memory {
void Array::initialize(bool reset) {
  if (reset) {
    file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
  } else {
    file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
  }
  if (!file_.is_open()) {
    //TODO: use custom exception
    throw std::runtime_error("Cannot open file " + file_name_);
  }
  file_.seekg(0, std::ios::end);
  size_ = file_.tellg() / sizeof(int);
}
unsigned int Array::size() const {
  return size_;
}
int Array::get(unsigned int n) {
  if (cached_) {
    return cache_[n];
  } else {
    file_.seekg(n * sizeof(int), std::ios::beg);
    int value;
    file_.read(reinterpret_cast<char *>(&value), sizeof(int));
    return value;
  }
}
void Array::set(unsigned int n, int value) {
  if (cached_) {
    cache_[n] = value;
  } else {
    file_.seekp(n * sizeof(int), std::ios::beg);
    file_.write(reinterpret_cast<char *>(&value), sizeof(int));
  }
}
unsigned int Array::push_back(int value) {
  if (cached_) {
    cache_.push_back(value);
    size_ = cache_.size();
    return cache_.size() - 1;
  } else {
    file_.seekp(0, std::ios::end);
    file_.write(reinterpret_cast<char *>(&value), sizeof(int));
    return size_++;
  }
}
void Array::cache() {
  if (!cached_) {
    cache_.resize(size_);
    file_.seekg(0, std::ios::beg);
    for (unsigned int i = 0; i < size_; ++i) {
      file_.read(reinterpret_cast<char *>(&cache_[i]), sizeof(int));
    }
    cached_ = true;
  }
}
void Array::uncache() {
  if (cached_) {
    file_.close();
    file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    for (unsigned int i = 0; i < size_; ++i) {
      file_.write(reinterpret_cast<char *>(&cache_[i]), sizeof(int));
    }
    cached_ = false;
  }
}
void Array::double_size() {
  if (cached_) {
    cache_.resize(size_ << 1);
    for (unsigned int i = 0; i < size_; ++i) {
      cache_[i + size_] = cache_[i];
    }
  } else {
    file_.seekg(0, std::ios::beg);
    file_.seekp(0, std::ios::end);
    for (unsigned int i = 0; i < size_; ++i) {
      int value;
      file_.read(reinterpret_cast<char *>(&value), sizeof(int));
      file_.write(reinterpret_cast<char *>(&value), sizeof(int));
    }
  }
  size_ <<= 1;
}
void Array::halve_size() {
  if (cached_) {
    cache_.resize(size_ >> 1);
  } else {
    file_.close();
    std::filesystem::resize_file(file_name_, size_ >> 1);
    file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
  }
  size_ >>= 1;
}
Array::~Array() {
  if (cached_) {
    uncache();
  }
  file_.close();
}

}