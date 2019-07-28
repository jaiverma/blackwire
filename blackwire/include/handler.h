//
// Created by Jai  Verma on 2019-07-28.
//

#ifndef BLACKWIRE_HANDLER_H
#define BLACKWIRE_HANDLER_H

#include "libusb.h"

void handle_request(struct libusb_transfer*);
void print_transfer(struct libusb_transfer*);

#define OP_COUNT 6

#define CALL_ON 0
#define CALL_OFF 1
#define MUTE_TOGGLE_CALL_ON 2
#define MUTE_TOGGLE_CALL_OFF 3
#define VOL_UP 4
#define VOL_DOWN 5

int buf_idx;
unsigned char buffer[8];

int match();

#endif //BLACKWIRE_HANDLER_H
