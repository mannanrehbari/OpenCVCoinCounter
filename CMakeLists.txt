project (coincount)
cmake_minimum_required(VERSION 2.8)
find_package (OpenCV REQUIRED)

add_executable(coincount coincount.cpp)
target_link_libraries(coincount ${OpenCV_LIBS})
