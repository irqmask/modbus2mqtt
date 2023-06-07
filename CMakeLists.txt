cmake_minimum_required(VERSION 3.12)
project(sun2000mqtt VERSION 0.1)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Modbus IMPORTED_TARGET libmodbus REQUIRED)
pkg_check_modules(Mosquitto IMPORTED_TARGET libmosquitto REQUIRED)
pkg_check_modules(LibConfig IMPORTED_TARGET libconfig++ REQUIRED)

set (SRCS
    appsettings.h
    appsettings.cpp
    main.cpp
    mqtt.h
    mqtt.cpp
)

include_directories(${Modbus_INCLUDE_DIRS} ${LibConfig_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} ${SRCS})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
target_link_libraries(${PROJECT_NAME} PkgConfig::Mosquitto PkgConfig::Modbus PkgConfig::LibConfig)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME})
