//
// Created by zj on 12/7/2023.
//

#ifndef BOOKSTORE_SRC_MAIN_CPP_TEST_H_
#define BOOKSTORE_SRC_MAIN_CPP_TEST_H_

#include <cassert>
#include "external_hash_map.h"
#include "external_vector.h"
#include "external_memory.h"
#include <iostream>
using namespace std;
class Test {
 public:

#ifdef LOCAL
  static inline const string path = "../db/";
#else
  static inline const string path = "./";
#endif

  static void print_array(external_memory::Array &arr) {
    std::cout << "Array : ";
    for (int i = 0; i < arr.size(); ++i) {
      std::cout << arr.get(i) << " ";
    }
    std::cout << std::endl;
  }
  static void test_array_1(bool reset = false, bool cache = false) {
    std::cout << "--- Test Array ---" << std::endl;
    std::cout << "Reset: " << reset << ", Cache: " << cache << std::endl;
    external_memory::Array arr(path + "array");
    arr.initialize(reset);
    if (cache) arr.cache();
    print_array(arr);
    std::cout << "push_back(1)" << std::endl;
    arr.push_back(1);
    print_array(arr);
    std::cout << "set(0, 2)" << std::endl;
    arr.set(0, 2);
    print_array(arr);
//  exit(0);
    std::cout << "double_size()" << std::endl;
    arr.double_size();
    print_array(arr);
  }
  static void test_array_2() {
    test_array_1(true, false);
    test_array_1(false, false);
    test_array_1(true, true);
    test_array_1(false, true);
    std::cout << "--- Test Array ---" << std::endl;
    external_memory::Array arr(path + "array");
    arr.initialize(false);
    print_array(arr);
    std::cout << "halve_size()" << std::endl;
    arr.halve_size();
    print_array(arr);
  }

  static void print_vector(external_memory::Vectors::Vector &vec) {
    std::cout << "Vector @ " << vec.getPos() << " : ";
    auto data = vec.getData();
    for (int i = 0; i < data.size(); ++i) {
      std::cout << data[i] << " ";
    }
    std::cout << std::endl;
  }
  static void test_vector_1(bool reset = false) {
    std::cout << "--- Test Vector 1 ---" << std::endl;
    std::cout << "Reset: " << reset << std::endl;
    external_memory::Vectors vec(path + "vector");
    vec.initialize(reset);
    {
      unsigned int pos1 = 0;
      auto vec1 = vec.newVector();
      pos1 = vec1.getPos();
      std::cout << "vec1.push_back(1)" << std::endl;
      if (vec1.push_back(1)) pos1 = vec1.getPos();
      print_vector(vec1);
      std::cout << "vec1.push_back(2)" << std::endl;
      if (vec1.push_back(2)) pos1 = vec1.getPos();
      print_vector(vec1);
    }
    {
      unsigned int pos1 = 0;
      auto vec2 = vec.newVector();
      pos1 = vec2.getPos();
      std::cout << "vec2.push_back(1)" << std::endl;
      if (vec2.push_back(1)) pos1 = vec2.getPos();
      print_vector(vec2);
      std::cout << "vec2.push_back(2)" << std::endl;
      if (vec2.push_back(2)) pos1 = vec2.getPos();
      print_vector(vec2);
      std::vector<int> data = {3, 4, 5, 6, 7, 8, 9, 10};
      std::cout << "vec2.update(data)" << std::endl;
      if (vec2.update(std::move(data))) pos1 = vec2.getPos();
      print_vector(vec2);
    }
  }
  static void test_vector_2(bool reset = false) {
    std::cout << "--- Test Vector 2 ---" << std::endl;
    // insert 256 vectors whose size is 8
    external_memory::Vectors vec(path + "vector");
    vec.initialize(reset);
    int n = 256, m = 8;
    unsigned int pos[n];
    std::cout << "Insert " << n << " vectors whose size is " << m << std::endl;
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.newVector();
      std::vector<int> data(m);
      for (int j = 0; j < m; ++j) {
        data[j] = i * m + j;
      }
      vec1.update(std::move(data));
      pos[i] = vec1.getPos();
    }
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.getVector(pos[i]);
      if (i < 10 || i > n - 10) print_vector(vec1);
    }
//    n = 1;
    std::cout << "Delete even vectors" << std::endl;
    for (int i = 0; i < n; i += 2) {
      auto vec1 = vec.getVector(pos[i]);
      vec1.del();
      pos[i] = vec1.getPos();
      assert(pos[i] == 0);
    }
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.getVector(pos[i]);
      if (i < 10 || i > n - 10) print_vector(vec1);
    }
//    return;
    n = 128;
    std::cout << "Insert " << n << " vectors whose size is " << m << std::endl;
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.newVector();
      std::vector<int> data(m);
      for (int j = 0; j < m; ++j) {
        data[j] = i * m + j;
      }
      vec1.update(std::move(data));
      pos[i] = vec1.getPos();
    }
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.getVector(pos[i]);
      if (i < 10 || i > n - 10) print_vector(vec1);
    }
  }
  static void test_vector_3(bool reset = false) {
    std::cout << "--- Test Vector 3 ---" << std::endl;
    {
      external_memory::Vectors vec(path + "vector");
      vec.initialize(reset);
      {
        auto vec2 = vec.getVector(2056);
        vec2.del();
        print_vector(vec2);
      }
      {
        auto vec1 = vec.getVector(2048);
        vec1.del();
        print_vector(vec1);
      }
    }
  }
  static void test_vector_4(bool reset = false) {
    std::cout << "--- Test Vector 4 ---" << std::endl;
    external_memory::Vectors vec(path + "vector");
    vec.initialize(reset);
    unsigned int n = 1024;
    std::cout << "Insert 1 vector whose size is " << n << std::endl;
    {
      auto vec1 = vec.newVector();
      std::vector<int> data(n);
      for (int j = 0; j < n; ++j) {
        data[j] = j;
      }
      vec1.update(std::move(data));
      std::cout << "vec1.push_back(2)" << std::endl;
      vec1.push_back(2);
      n = 10;
      std::cout << "Update vec1 with size " << n << std::endl;
      data.resize(n);
      for (int j = 0; j < n; ++j) {
        data[j] = j;
      }
      vec1.update(std::move(data));
//      cout << "Delete vec1" << endl;
//      vec1.del();
      print_vector(vec1);
    }
  }
  static void test_vector_5(bool reset = false) {
    std::cout << "--- Test Vector 5 ---" << std::endl;
    // insert 256 vectors whose size is 8
    external_memory::Vectors vec(path + "vector");
    vec.initialize(reset);
    int n = 256, m = 4;
    unsigned int pos[n];
    std::cout << "Insert " << n << " vectors whose size is " << m << std::endl;
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.newVector();
      std::vector<int> data(m);
      for (int j = 0; j < m; ++j) {
        data[j] = i * m + j;
      }
      vec1.update(std::move(data));
      pos[i] = vec1.getPos();
    }
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.getVector(pos[i]);
      if (i < 10 || i > n - 10) print_vector(vec1);
    }
//    n = 1;
    std::cout << "Delete the latter half vectors in reverse order" << std::endl;
    for (int i = n - 1; i >= n / 2; --i) {
      auto vec1 = vec.getVector(pos[i]);
      vec1.del();
      pos[i] = vec1.getPos();
      assert(pos[i] == 0);
    }
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.getVector(pos[i]);
      if (i < 10 || i > n - 10) print_vector(vec1);
    }
//    return;
    n = 256;
    std::cout << "Insert " << n << " vectors whose size is " << m << std::endl;
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.newVector();
      std::vector<int> data(m);
      for (int j = 0; j < m; ++j) {
        data[j] = i * m + j;
      }
      vec1.update(std::move(data));
      pos[i] = vec1.getPos();
    }
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.getVector(pos[i]);
      if (i < 10 || i > n - 10) print_vector(vec1);
    }
  }
  static void test_vector_final() {
    test_vector_1(true);
    test_vector_1(false);
    test_vector_3(false);
    test_vector_2(false);
//    test_vector_4(true);
    test_vector_4(false);
    test_vector_4(false);
    test_vector_5(false);
  }

  class MultiMapTest {
   private:
    const std::string file_name_;
    external_memory::MultiMap<std::string> map_;
    external_memory::Vectors vectors_;
   public:
    explicit MultiMapTest(const std::string &file_name)
        : file_name_(file_name), vectors_(file_name + "_data"), map_(file_name + "_map", vectors_) {}
    void initialize(bool reset = false) {
      vectors_.initialize(reset);
      map_.initialize(reset);
    }
    void printFind(const std::string &key) {
      std::cout << "Find " << key << ": ";
      auto vec = map_.findAll(key);
      for (auto &x : vec) {
        std::cout << x << " ";
      }
      std::cout << std::endl;
    }
    void test() {
      map_.insert("a", 1);
      map_.insert("a", 2);
      map_.insert("b", 3);
      printFind("a");
      printFind("b");
    }
    void insert(const std::string &key, int value) {
      assert(value > 0);
      map_.insert(key, value);
    }
    void erase(const std::string &key, int value) {
      assert(value > 0);
      auto vec = map_.findAll(key);
      // erase the first occurrence
      for (auto it = vec.begin(); it != vec.end(); ++it) {
        if (*it == value) {
          vec.erase(it);
          map_.update(key, std::move(vec));
          return;
        }
      }
//      map_.insert(key, -value);
    }
    static void regularize(std::vector<int> &values) {
      std::sort(values.begin(), values.end());
      return;
      std::set<int> s;
      for (auto &x : values) {
        if (x > 0) {
          s.insert(x);
        } else {
          s.erase(-x);
        }
      }
      values.clear();
      for (auto &x : s) {
        values.push_back(x);
      }
    }
    void findAll(const std::string &key) {
      auto vec = map_.findAll(key);
      regularize(vec);
      if (vec.empty()) {
        std::cout << "null" << std::endl;
      } else {
        for (auto &x : vec) {
          std::cout << x - 1 << " "; // avoid 0
//          cout << x << " ";
        }
        std::cout << std::endl;
      }
//      map_.update(key, std::move(vec));
    }
  };

  static bool test_multimap(bool force_reset = false) {
    Test::MultiMapTest test(path + "multimap");
    bool reset = true;
    if (std::filesystem::exists(path + "multimap_data_data.db")) {
      reset = false;
    }
    test.initialize(reset);
    int n;
    // optimize cin and cout
    std::ios::sync_with_stdio(false);
    // input n, if eof is reached, exit
    if (!(std::cin >> n) || n <= 0) {
      return false;
    }
    for (int i = 0; i < n; ++i) {
      std::string op, key;
      int value;
      std::cin >> op >> key;
      if (op == "insert") {
        std::cin >> value;
        value++; // avoid 0
        test.insert(key, value);
      } else if (op == "delete") {
        std::cin >> value;
        value++; // avoid 0
        test.erase(key, value);
      } else if (op == "find") {
        test.findAll(key);
      } else {
        std::cout << "Invalid operation" << std::endl;
        return false;
      }
    }
    return true;
  }
};
#endif //BOOKSTORE_SRC_MAIN_CPP_TEST_H_
