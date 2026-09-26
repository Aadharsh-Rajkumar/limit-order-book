cmake_minimum_required(VERSION 4.3)

project(TGT_C___Exchange LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Drogon REQUIRED)
find_package(Catch2 3 REQUIRED)

add_executable(TGT_C___Exchange
        main.cpp
        src/order_book.cpp
        src/matching_engine.cpp
        api/exchange_service.cpp
        api/order_controller.cpp
        api/book_reader.cpp
        api/book_data_streamer.cpp
)

add_executable(matching_engine_tests
        tests/matching_engine_tests.cpp
        src/order_book.cpp
        src/matching_engine.cpp
)

target_sources(TGT_C___Exchange
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        src/order.cppm
        src/order_book.cppm
        src/matching_engine.cppm
)

target_sources(matching_engine_tests
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        src/order.cppm
        src/order_book.cppm
        src/matching_engine.cppm
)

target_link_libraries(TGT_C___Exchange PRIVATE Drogon::Drogon)

target_link_libraries(matching_engine_tests PRIVATE Catch2::Catch2WithMain)

target_compile_options(TGT_C___Exchange PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_compile_options(matching_engine_tests PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)