cmake_minimum_required(VERSION 3.10)
message(STATUS "cmake version ${CMAKE_VERSION}")

project(warehouse_system)

set(TOOLCHAIN_DIR /opt/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-g++)
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-gcc)
# set(CMAKE_SYSROOT /opt/sysroot)
set(SQLITE_HEADER ./include/3rdparty/)
set(SQLITE_LIB ./lib/libSqlite3/)
# set(SQLITE_PATH rk3566/warehouse_system/include/3rdparty)
set(MQTT_LIB ./lib/libpahoMQTT/)
set(MQTT_HEADER ./include/3rdparty/)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")


include_directories(.)
include_directories(./include)
include_directories(./include/3rdparty)
# include_directories(/usr/include)
# 指定SQLite库的头文件路径
include_directories(${SQLITE_HEADER})
# 指定SQLite库文件的路径
include_directories(${SQLITE_LIB})
# 指定MQTT库文件的路径
include_directories(${MQTT_LIB})
# 指定MQTT库的头文件路径
include_directories(${MQTT_HEADER})
# include_directories(${SQLITE_PATH})

link_directories(include lib ${SQLITE_HEADER} ${SQLITE_LIB} ${MQTT_LIB} ${MQTT_HEADER})
# link_directories(include lib ${SQLITE_PATH})


aux_source_directory(./src DIR_SRCS)

add_executable(${PROJECT_NAME} ${DIR_SRCS})
target_link_libraries(${PROJECT_NAME} pthread sqlite3 paho-mqtt3c dl)
