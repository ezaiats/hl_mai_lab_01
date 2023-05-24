cmake_minimum_required(VERSION 3.2)

project(hl_server C CXX)

SET (USER_SERVICE "user_server")
SET (POST_SERVICE "post_service")
SET (DELIVERY_SERVICE "delivery_service")

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")
link_directories("/usr/local/lib")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${USER_SERVICE} user_main.cpp 
                                 config/config.cpp 
                                 database/database.cpp
                                 database/user.cpp)

target_include_directories(${USER_SERVICE} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${USER_SERVICE} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${USER_SERVICE} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${USER_SERVICE} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${USER_SERVICE} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             ZLIB::ZLIB)

add_executable(${POST_SERVICE} post_main.cpp 
                                 config/config.cpp 
                                 database/database.cpp
                                 database/post.cpp)

target_include_directories(${POST_SERVICE} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${POST_SERVICE} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${POST_SERVICE} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${POST_SERVICE} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${POST_SERVICE} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             ZLIB::ZLIB)

add_executable(${DELIVERY_SERVICE} delivery_main.cpp 
                                 config/config.cpp 
                                 database/database.cpp
                                 database/delivery.cpp)

target_include_directories(${DELIVERY_SERVICE} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${DELIVERY_SERVICE} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${DELIVERY_SERVICE} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${DELIVERY_SERVICE} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${DELIVERY_SERVICE} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             ZLIB::ZLIB)
