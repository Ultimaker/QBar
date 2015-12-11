project(jedi-qbar)

cmake_minimum_required(VERSION 2.8.12)

find_package(PkgConfig)
pkg_check_modules(LIBDBUS REQUIRED dbus-1)
if(NOT LIBDBUS_FOUND)
    message(SEND_ERROR "Coult not find libdbus-1")
endif()
find_package(CURL REQUIRED)
if(NOT CURL_FOUND)
    message(SEND_ERROR "Could not find cURL")
endif()
find_package(ZXing REQUIRED)
if(NOT ZXING_FOUND)
    message(SEND_ERROR "Could not find ZXing")
endif()

# Add warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

set(SOURCES 
src/System/Clock.cpp
src/System/Variant.cpp
src/System/UnicodeString.cpp
src/DBus/DBus.cpp
src/DBus/DBusPrinter.cpp
src/Main.cpp
src/Image/ImageReaderSource.cpp
src/Image/jpgd.cpp
src/CurlRequest.cpp
src/QRDetector.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/src ${LIBDBUS_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS} ${ZXING_INLCUDE_DIRS})

add_executable(jedi-qbar ${SOURCES})
target_link_libraries(jedi-qbar ${LIBDBUS_LIBRARIES} ${CURL_LIBRARIES} ${ZXING_LIBRARIES})

include(CPackConfig.cmake)

include(GNUInstallDirs)
install(TARGETS jedi-qbar DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY packaging/systemd DESTINATION /etc)
