//
// Created by Jai  Verma on 2019-07-28.
//

#include <stdio.h>
#include <string.h>

#include "handler.h"
#include "libusb.h"

unsigned char call_on[] = { 0x08, 0x02 };
unsigned char call_off[] = { 0x08, 0x00 };
unsigned char mute_toggle_call_on[] = { 0x08, 0x03, 0x08, 0x02, 0x1f, 0x00, 0x1f, 0x01 };
unsigned char mute_toggle_call_off[] = { 0x08, 0x01, 0x08, 0x00, 0x1f, 0x00, 0x1f, 0x01 };
unsigned char vol_up[] = { 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00 };
unsigned char vol_down[] = { 0x01, 0x02, 0x01, 0x00, 0x01, 0x02, 0x01, 0x00 };

unsigned char *ops[] = {
        call_on,
        call_off,
        mute_toggle_call_on,
        mute_toggle_call_off,
        vol_up,
        vol_down
};

int op_sizes[] = { 2, 2, 8, 8, 8, 8 };

void handle_request(struct libusb_transfer *transfer) {
    for (int i = 0; i < transfer->actual_length; i++)
        buffer[buf_idx + i] = transfer->buffer[i];
    buf_idx += transfer->actual_length;

    int r = match();

    if (r != -1) {
        buf_idx = 0;
        memset(buffer, 0, sizeof(buffer));

        if (r == CALL_ON)
            fprintf(stdout, "[+] CALL_ON\n");
        else if (r == CALL_OFF)
            fprintf(stdout, "[+] CALL_OFF\n");
        else if (r == MUTE_TOGGLE_CALL_ON)
            fprintf(stdout, "[+] MUTE_TOGGLE_CALL_ON\n");
        else if (r == MUTE_TOGGLE_CALL_OFF)
            fprintf(stdout, "[+] MUTE_TOGGLE_CALL_OFF\n");
        else if (r == VOL_UP)
            fprintf(stdout, "[+] VOL_UP\n");
        else if (r == VOL_DOWN)
            fprintf(stdout, "[+] VOL_DOWN\n");
        else
            fprintf(stderr, "[-] unrecognised command\n");
    }
//    print_transfer(transfer);
}

void print_transfer(struct libusb_transfer *transfer) {
    fprintf(stdout, "[*] dumping packet\n");
    for (int i = 0; i < transfer->actual_length; i++)
        fprintf(stdout, "%02x ", transfer->buffer[i]);
    fprintf(stdout, "\n");
}

void print_op(char *op, int sz) {
    for (int i = 0; i < sz; i++)
        fprintf(stdout, "%02x ", op[i]);
    fprintf(stdout, "\n");
}

int match() {
    for (int i = 0; i < OP_COUNT; i++) {
        char *op = ops[i];
//        fprintf(stdout, "Baseline op:\n");
//        print_op(op, sizeof(op));
//        fprintf(stdout, "Buffer:\n");
//        print_op(buffer, buf_idx);
        if (buf_idx == op_sizes[i])
            if (memcmp(buffer, op, buf_idx) == 0)
                return i;
    }
    return -1;
}
