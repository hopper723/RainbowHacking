cmake_minimum_required(VERSION 3.17)
project(RainbowHacking)

set(CMAKE_CXX_STANDARD 14)

# link_directories(${OPENMP_LIBRARIES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

add_executable(RainbowHacking HashMethod.hpp TableBuilder.hpp TableBuilder.cpp RainbowTable.h RainbowTable.cpp RainbowHacking.h RainbowHacking.cpp)
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)