project(jedi-qbar)

cmake_minimum_required(VERSION 2.8.12)

# Add warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

set(SOURCES 
src/Main.cpp
)

find_package(PkgConfig)
pkg_check_modules(LIBDBUS REQUIRED dbus-1)

include_directories(${CMAKE_SOURCE_DIR}/src ${LIBDBUS_INCLUDE_DIRS})

add_executable(jedi-qbar ${SOURCES})
target_link_libraries(jedi-qbar ${LIBDBUS_LIBRARIES})

include(CPackConfig.cmake)

include(GNUInstallDirs)
install(TARGETS jedi-qbar DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY packaging/systemd DESTINATION /etc)
