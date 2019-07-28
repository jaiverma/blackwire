add_executable(blackwire main.c handler.c)

find_library(LIBUSB
        NAMES libusb-1.0.so
        HINTS /usr/lib/x86_64-linux-gnu)

include_directories(blackwire
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
        PUBLIC /usr/include/libusb-1.0)

target_link_libraries(blackwire ${LIBUSB})
