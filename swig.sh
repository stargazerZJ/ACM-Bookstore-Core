cd src
swig -c++ -python bookstore.swx
g++ --std=c++20 -fPIC -shared -I/usr/include/python3.10 -o _bookstore.so bookstore_wrap.cxx book_system.cpp bookstore.cpp cli.cpp external_hash_map.cpp external_memory.cpp external_vector.cpp log.cpp parser.cpp user_system.cpp validator.cpp
cd ..