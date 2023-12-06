//
// Created by zj on 12/2/2023.
//

#include "external_vector.h"
namespace external_memory {
void Vectors::initialize(bool reset) {
  info_.initialize(reset);
  data_.initialize(reset);
  info_.cache();
  for (unsigned int i = 1; i <= info_.size() / kInfoPerPage; ++i) {
    int capacity = getPageInfo(kPageInfo::kCapacity, i);
    if (capacity > 0 && capacity < kIntegerPerPage) {
      int unoccupied_beg = getPageInfo(kPageInfo::kUnoccupiedBeg, i);
      if (unoccupied_beg != kIntegerPerPage) {
        unsigned int capacity_log = __builtin_ctz(capacity);
        auto &tmp = free_pages_of_capacity_[capacity_log];
        tmp.insert(tmp.end(), i);
      }
    }
  }
}
int Vectors::getPageInfo(Vectors::kPageInfo type, unsigned int n) {
  return info_.get((n - 1) * kInfoPerPage + static_cast<unsigned int>(type));
}
void Vectors::setPageInfo(Vectors::kPageInfo type, unsigned int n, int value) {
  info_.set((n - 1) * kInfoPerPage + static_cast<unsigned int>(type), value);
}
void Vectors::deletePage(unsigned int n) {
  setPageInfo(kPageInfo::kCapacity, n, 0);
  setPageInfo(kPageInfo::kFreeHead, n, 0);
  setPageInfo(kPageInfo::kUnoccupiedBeg, n, 0);
  data_.deletePage(n);
}
Vectors::Vector Vectors::getVector(unsigned int pos) {
  if (!pos) {
    return Vector(*this);
  }
  auto ret = Vector(*this, pos);
  if (ret.capacity() <= kIntegerPerPage) {
    data_.fetchPage(ret.page_id_);
  }
  return ret;
}
Vectors::Vector Vectors::newVector() {
  return Vector(*this);
}
unsigned int Vectors::getPageOfCapacity(unsigned int capacity) {
  unsigned capacity_log = __builtin_ctz(capacity);
  auto &tmp = free_pages_of_capacity_[capacity_log];
  if (tmp.empty()) {
    auto new_page = newPage();
    setPageInfo(kPageInfo::kCapacity, new_page, capacity);
    setPageInfo(kPageInfo::kFreeHead, new_page, -1);
    setPageInfo(kPageInfo::kUnoccupiedBeg, new_page, 0);
    tmp.insert(new_page);
    return new_page;
  } else {
    return *tmp.begin();
  }
}
unsigned int Vectors::newPage() {
  unsigned new_page = data_.newPage();
  if (new_page > info_.size() / kInfoPerPage) {
    for (unsigned int i = 0; i < kInfoPerPage; ++i) {
      info_.push_back(0);
    }
  }
  return new_page;
}
unsigned int Vectors::allocate(unsigned int capacity) {
  if (capacity < kIntegerPerPage) {
    unsigned int page = getPageOfCapacity(capacity);
    unsigned int free_head = getPageInfo(kPageInfo::kFreeHead, page);
    unsigned int unoccupied_beg = getPageInfo(kPageInfo::kUnoccupiedBeg, page);
    data_.fetchPage(page);
    if (free_head != -1) {
      int new_free_head;
      data_.getPart(page, free_head, 1, &new_free_head);
      setPageInfo(kPageInfo::kFreeHead, page, new_free_head);
      clearSpace(capacity, page, free_head);
      if (new_free_head == -1 && unoccupied_beg == kIntegerPerPage) {
        free_pages_of_capacity_[__builtin_ctz(capacity)].erase(page);
      }
      return external_memory::Pages::toPosition(page, free_head);
    } else {
      setPageInfo(kPageInfo::kUnoccupiedBeg, page, unoccupied_beg + capacity);
      if (unoccupied_beg + capacity == kIntegerPerPage) {
        free_pages_of_capacity_[__builtin_ctz(capacity)].erase(page);
      }
      clearSpace(capacity, page, unoccupied_beg);
      return external_memory::Pages::toPosition(page, unoccupied_beg);
    }
  } else { // capacity == kIntegerPerPage
    unsigned int page = newPage();
    data_.fetchPage(page);
    setPageInfo(kPageInfo::kCapacity, page, capacity);
    setPageInfo(kPageInfo::kNextPage, page, 0);
    setPageInfo(kPageInfo::kLastPage, page, page);
    return external_memory::Pages::toPosition(page, 0);
  }
}
void Vectors::clearSpace(const unsigned int capacity, unsigned int n, unsigned int offset) {
  int tmp[capacity];
  memset(tmp, 0, sizeof(tmp));
  data_.setPart(n, offset, capacity, tmp);
}
void Vectors::deallocate(unsigned int page, unsigned int offset, unsigned int capacity) {
  if (page == 0 || capacity == 0) {
    return;
  }
  if (capacity < kIntegerPerPage) {
    int free_head = getPageInfo(kPageInfo::kFreeHead, page);
    int unoccupied_beg = getPageInfo(kPageInfo::kUnoccupiedBeg, page);
    data_.fetchPage(page);
    if (offset + capacity == unoccupied_beg) {
      setPageInfo(kPageInfo::kUnoccupiedBeg, page, offset);
      if (offset == 0) {
        deletePage(page);
        free_pages_of_capacity_[__builtin_ctz(capacity)].erase(page);
        return;
      }
    } else {
      setPageInfo(kPageInfo::kFreeHead, page, offset);
      data_.setPart(page, offset, 1, &free_head);
    }
    free_pages_of_capacity_[__builtin_ctz(capacity)].insert(page);
  } else {
    deletePage(page);
  }
}
unsigned int Vectors::Vector::getPos() const {
  return pos_;
}
std::vector<int> Vectors::Vector::getData() {
  if (!pos_) {
    return {};
  }
  unsigned capacity = this->capacity();
  std::vector<int> ret(capacity);
  if (capacity <= kIntegerPerPage) {
    vectors_.data_.getPart(page_id_, offset_, capacity, ret.data());
  } else {
    unsigned page = page_id_;
    int *data = ret.data();
    while (page) {
      vectors_.data_.getPage(page, data);
      data += kIntegerPerPage;
      page = vectors_.getPageInfo(kPageInfo::kNextPage, page);
    }
  }
  while (!ret.empty() && ret.back() == 0) {
    ret.pop_back();
  }
  return ret;
}
unsigned int Vectors::Vector::capacity() const {
  return pos_ ? vectors_.getPageInfo(kPageInfo::kCapacity, page_id_) : 0;
}
bool Vectors::Vector::push_back(int value) {
  unsigned capacity = this->capacity();
  if (capacity < kIntegerPerPage) {
    auto data = getData();
    data.push_back(value);
    if (data.size() <= capacity) {
      vectors_.data_.setPart(page_id_, offset_, data.size(), data.data());
      return false;
    } else {
      if (pos_) {
        vectors_.deallocate(page_id_, offset_, capacity);
        updatePos(vectors_.allocate(capacity * 2));
      } else {
        updatePos(vectors_.allocate(1));
      }
      vectors_.data_.setPart(page_id_, offset_, data.size(), data.data());
      return true;
    }
  } else {
    unsigned int last_page = vectors_.getPageInfo(kPageInfo::kLastPage, page_id_);
    Page tmp;
    int first_unoccupied = kIntegerPerPage;
    vectors_.data_.getPage(last_page, tmp);
    for (; first_unoccupied > 0 && tmp[first_unoccupied - 1] == 0; --first_unoccupied);
    if (first_unoccupied < kIntegerPerPage) {
      tmp[first_unoccupied] = value;
      vectors_.data_.setPage(last_page, tmp);
      return false;
    } else {
      unsigned int new_page = appendPage();
      vectors_.data_.setPart(new_page, 0, 1, &value);
      return false;
    }
  }
}
unsigned int Vectors::Vector::appendPage() {
  unsigned new_page = vectors_.newPage();
  unsigned last_page = vectors_.getPageInfo(kPageInfo::kLastPage, page_id_);
  unsigned capacity = vectors_.getPageInfo(kPageInfo::kCapacity, page_id_);
  vectors_.setPageInfo(kPageInfo::kCapacity, new_page, -1);
  vectors_.setPageInfo(kPageInfo::kNextPage, new_page, 0);
  vectors_.setPageInfo(kPageInfo::kLastPage, new_page, -1);
  vectors_.setPageInfo(kPageInfo::kNextPage, last_page, new_page);
  vectors_.setPageInfo(kPageInfo::kLastPage, page_id_, new_page);
  vectors_.setPageInfo(kPageInfo::kCapacity, page_id_, capacity + kIntegerPerPage);
  return new_page;
}
bool Vectors::Vector::rewrite(std::vector<int> &&data) {
  if (data.empty()) {
    return del();
  }
  unsigned capacity = this->capacity();
  bool ret = false;
  if (capacity < kIntegerPerPage) {
    if (data.size() <= capacity) {
      data.resize(capacity);
      vectors_.data_.setPart(page_id_, offset_, data.size(), data.data());
      return false;
    } else {
      if (data.size() <= kIntegerPerPage) {
        unsigned new_capacity = 1 << (32 - __builtin_clz(data.size() - 1));
        data.resize(new_capacity);
        vectors_.deallocate(page_id_, offset_, capacity);
        updatePos(vectors_.allocate(new_capacity));
        vectors_.data_.setPart(page_id_, offset_, data.size(), data.data());
        return true;
      } else {
        vectors_.deallocate(page_id_, offset_, capacity);
        updatePos(vectors_.allocate(kIntegerPerPage));
        vectors_.data_.setPart(page_id_, offset_, data.size(), data.data());
        ret = true;
      }
    }
  }
  data.resize((data.size() + kIntegerPerPage - 1) / kIntegerPerPage * kIntegerPerPage);
  int *data_ptr = data.data();
  unsigned page = page_id_;
  unsigned last_page = 0;
  for (unsigned filled = 0; filled < data.size(); filled += kIntegerPerPage) {
    last_page = page;
    vectors_.data_.setPage(page, data_ptr);
    data_ptr += kIntegerPerPage;
    page = vectors_.getPageInfo(kPageInfo::kNextPage, page);
    if (!page) {
      page = appendPage();
    }
  }
  if (page) {
    discardAfter(last_page);
  }
  return ret;
}
void Vectors::Vector::updatePos(unsigned int new_pos) {
  pos_ = new_pos;
  std::tie(page_id_, offset_) = external_memory::Pages::toPageOffset(pos_);
}
void Vectors::Vector::discardAfter(unsigned int last_page) {
  if (!last_page) {
    return;
  }
  unsigned page = vectors_.getPageInfo(kPageInfo::kNextPage, last_page);
  vectors_.setPageInfo(kPageInfo::kNextPage, last_page, 0);
  vectors_.setPageInfo(kPageInfo::kLastPage, page_id_, last_page);
  while (page) {
    unsigned next_page = vectors_.getPageInfo(kPageInfo::kNextPage, page);
    vectors_.deletePage(page);
    page = next_page;
  }
}
bool Vectors::Vector::del() {
  if (!pos_) {
    return false;
  }
  unsigned capacity = this->capacity();
  if (capacity < kIntegerPerPage) {
    vectors_.deallocate(page_id_, offset_, capacity);
  } else {
    discardAfter(page_id_);
    vectors_.deletePage(page_id_);
  }
  updatePos(0);
  return true;
}
}
