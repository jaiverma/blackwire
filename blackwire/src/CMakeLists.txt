add_executable(blackwire main.c handler.c mq.c)

find_library(LIBUSB
        NAMES libusb-1.0.so
        HINTS /usr/lib/x86_64-linux-gnu)

include_directories(blackwire
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
        PUBLIC /usr/include/libusb-1.0)

find_library(ZMQ
        NAMES libzmq.so
        HINTS /usr/lib/x86_64-linux-gnu)

target_link_libraries(blackwire ${LIBUSB} ${ZMQ})
