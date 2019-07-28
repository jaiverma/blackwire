cmake_minimum_required(VERSION 3.13)
project(plantronics_usb)

set(CMAKE_CXX_STANDARD 14)

find_library(LIBUSB
        NAMES libusb-1.0.dylib
        HINTS "/usr/local/Cellar/libusb/1.0.22/lib")

add_executable(plantronics_usb main.cpp)
target_link_libraries(plantronics_usb ${LIBUSB})
target_include_directories(plantronics_usb
        PUBLIC "/usr/local/Cellar/libusb/1.0.22/include")
