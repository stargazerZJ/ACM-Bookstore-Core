//
// Created by zj on 11/29/2023.
//

#include <iostream>
#include "external_memory.h"
#include "external_vector.h"
#include "external_hash_map.h"
#include <cassert>

using namespace external_memory;
using namespace std;

#ifdef LOCAL
const string path = "../db/";
#else
const string path = "./";
#endif

class Test {
 public:

  static void print_array(Array &arr) {
    cout << "Array : ";
    for (int i = 0; i < arr.size(); ++i) {
      cout << arr.get(i) << " ";
    }
    cout << endl;
  }
  static void test_array_1(bool reset = false, bool cache = false) {
    cout << "--- Test Array ---" << endl;
    cout << "Reset: " << reset << ", Cache: " << cache << endl;
    Array arr(path + "array");
    arr.initialize(reset);
    if (cache) arr.cache();
    print_array(arr);
    cout << "push_back(1)" << endl;
    arr.push_back(1);
    print_array(arr);
    cout << "set(0, 2)" << endl;
    arr.set(0, 2);
    print_array(arr);
//  exit(0);
    cout << "double_size()" << endl;
    arr.double_size();
    print_array(arr);
  }
  static void test_array_2() {
    test_array_1(true, false);
    test_array_1(false, false);
    test_array_1(true, true);
    test_array_1(false, true);
    cout << "--- Test Array ---" << endl;
    Array arr(path + "array");
    arr.initialize(false);
    print_array(arr);
    cout << "halve_size()" << endl;
    arr.halve_size();
    print_array(arr);
  }

  static void print_vector(Vectors::Vector &vec) {
    cout << "Vector @ " << vec.getPos() << " : ";
    auto data = vec.getData();
    for (int i = 0; i < data.size(); ++i) {
      cout << data[i] << " ";
    }
    cout << endl;
  }
  static void test_vector_1(bool reset = false) {
    cout << "--- Test Vector 1 ---" << endl;
    cout << "Reset: " << reset << endl;
    Vectors vec(path + "vector");
    vec.initialize(reset);
    {
      unsigned int pos1 = 0;
      auto vec1 = vec.newVector();
      pos1 = vec1.getPos();
      cout << "vec1.push_back(1)" << endl;
      if (vec1.push_back(1)) pos1 = vec1.getPos();
      print_vector(vec1);
      cout << "vec1.push_back(2)" << endl;
      if (vec1.push_back(2)) pos1 = vec1.getPos();
      print_vector(vec1);
    }
    {
      unsigned int pos1 = 0;
      auto vec2 = vec.newVector();
      pos1 = vec2.getPos();
      cout << "vec2.push_back(1)" << endl;
      if (vec2.push_back(1)) pos1 = vec2.getPos();
      print_vector(vec2);
      cout << "vec2.push_back(2)" << endl;
      if (vec2.push_back(2)) pos1 = vec2.getPos();
      print_vector(vec2);
      vector<int> data = {3, 4, 5, 6, 7, 8, 9, 10};
      cout << "vec2.rewrite(data)" << endl;
      if (vec2.rewrite(std::move(data))) pos1 = vec2.getPos();
      print_vector(vec2);
    }
  }
  static void test_vector_2(bool reset = false) {
    cout << "--- Test Vector 2 ---" << endl;
    // insert 256 vectors whose size is 8
    Vectors vec(path + "vector");
    vec.initialize(reset);
    int n = 256, m = 8;
    unsigned int pos[n];
    cout << "Insert " << n << " vectors whose size is " << m << endl;
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.newVector();
      vector<int> data(m);
      for (int j = 0; j < m; ++j) {
        data[j] = i * m + j;
      }
      vec1.rewrite(std::move(data));
      pos[i] = vec1.getPos();
    }
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.getVector(pos[i]);
      if (i < 10 || i > n - 10) print_vector(vec1);
    }
//    n = 1;
    cout << "Delete even vectors" << endl;
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
    cout << "Insert " << n << " vectors whose size is " << m << endl;
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.newVector();
      vector<int> data(m);
      for (int j = 0; j < m; ++j) {
        data[j] = i * m + j;
      }
      vec1.rewrite(std::move(data));
      pos[i] = vec1.getPos();
    }
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.getVector(pos[i]);
      if (i < 10 || i > n - 10) print_vector(vec1);
    }
  }
  static void test_vector_3(bool reset = false) {
    cout << "--- Test Vector 3 ---" << endl;
    {
      Vectors vec(path + "vector");
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
    cout << "--- Test Vector 4 ---" << endl;
    Vectors vec(path + "vector");
    vec.initialize(reset);
    unsigned int n = 1024;
    cout << "Insert 1 vector whose size is " << n << endl;
    {
      auto vec1 = vec.newVector();
      vector<int> data(n);
      for (int j = 0; j < n; ++j) {
        data[j] = j;
      }
      vec1.rewrite(std::move(data));
      cout << "vec1.push_back(2)" << endl;
      vec1.push_back(2);
      n = 10;
      cout << "Rewrite vec1 with size " << n << endl;
      data.resize(n);
      for (int j = 0; j < n; ++j) {
        data[j] = j;
      }
      vec1.rewrite(std::move(data));
//      cout << "Delete vec1" << endl;
//      vec1.del();
      print_vector(vec1);
    }
  }
  static void test_vector_5(bool reset = false) {
    cout << "--- Test Vector 5 ---" << endl;
    // insert 256 vectors whose size is 8
    Vectors vec(path + "vector");
    vec.initialize(reset);
    int n = 256, m = 4;
    unsigned int pos[n];
    cout << "Insert " << n << " vectors whose size is " << m << endl;
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.newVector();
      vector<int> data(m);
      for (int j = 0; j < m; ++j) {
        data[j] = i * m + j;
      }
      vec1.rewrite(std::move(data));
      pos[i] = vec1.getPos();
    }
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.getVector(pos[i]);
      if (i < 10 || i > n - 10) print_vector(vec1);
    }
//    n = 1;
    cout << "Delete the latter half vectors in reverse order" << endl;
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
    cout << "Insert " << n << " vectors whose size is " << m << endl;
    for (int i = 0; i < n; ++i) {
      auto vec1 = vec.newVector();
      vector<int> data(m);
      for (int j = 0; j < m; ++j) {
        data[j] = i * m + j;
      }
      vec1.rewrite(std::move(data));
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
    MultiMap<std::string> map_;
    Vectors vectors_;
   public:
    explicit MultiMapTest(const std::string &file_name)
        : file_name_(file_name), vectors_(file_name + "_data"), map_(file_name + "_map", vectors_) {}
    void initialize(bool reset = false) {
      vectors_.initialize(reset);
      map_.initialize(reset);
    }
    void printFind(const std::string &key) {
      cout << "Find " << key << ": ";
      auto vec = map_.findAll(key);
      for (auto &x : vec) {
        cout << x << " ";
      }
      cout << endl;
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
          break;
        }
      }
      map_.rewrite(key, std::move(vec));
//      map_.insert(key, -value);
    }
    static void regularize(vector<int> &values) {
      std::sort(values.begin(), values.end());
      return;
      set<int> s;
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
        cout << "null" << endl;
      } else {
        for (auto &x : vec) {
          cout << x - 1 << " "; // avoid 0
//          cout << x << " ";
        }
        cout << endl;
      }
//      map_.rewrite(key, std::move(vec));
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
    ios::sync_with_stdio(false);
    // input n, if eof is reached, exit
    if (!(cin >> n) || n <= 0) {
      return false;
    }
    for (int i = 0; i < n; ++i) {
      string op, key;
      int value;
      cin >> op >> key;
      if (op == "insert") {
        cin >> value;
        value++; // avoid 0
        test.insert(key, value);
      } else if (op == "delete") {
        cin >> value;
        value++; // avoid 0
        test.erase(key, value);
      } else if (op == "find") {
        test.findAll(key);
      } else {
        cout << "Invalid operation" << endl;
        return false;
      }
    }
    return true;
  }
};

int main() {
  while (Test::test_multimap());
  return 0;
}