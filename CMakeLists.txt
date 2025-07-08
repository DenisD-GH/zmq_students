cmake_minimum_required(VERSION 3.10)
project(zmq_students)

# Настройки компилятора
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# pkg-config для поиска ZeroMQ
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZeroMQ REQUIRED libzmq)

# Настройка include директорий
include_directories(${ZeroMQ_INCLUDE_DIRS})

# Сервер
add_executable(server server/main.cpp)

# Линковка
target_link_libraries(server ${ZeroMQ_LIBRARIES})

# Клиент
add_executable(client client/main.cpp)
target_link_libraries(client ${ZeroMQ_LIBRARIES})