#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "libusb.h"
#include "handler.h"
#include "blackwire.h"

struct libusb_context *ctx = NULL;
struct libusb_device_handle *blackwire = NULL;
struct libusb_transfer *transfer_in = NULL;

int flag = 1;

void event_loop();
void sighandler(int);

int main() {
    buf_idx = 0;
    memset(buffer, 0, sizeof(buffer));
    struct sigaction sigact;

    int r = libusb_init(&ctx);
    if (r < 0) {
        fprintf(stderr, "[-] libusb_init failed\n");
        return r;
    }
    fprintf(stdout, "[+] libusb_init success\n");

    blackwire = libusb_open_device_with_vid_pid(
            ctx,
            VENDOR_ID,
            PRODUCT_ID
            );

    if (!blackwire) {
        fprintf(stderr, "[-] libusb_open_device failed\n");
        libusb_exit(ctx);
        return 1;
    }
    fprintf(stdout, "[+] libusb_open_device success\n");

    r = libusb_kernel_driver_active(blackwire, INTERFACE_NUM);
    if (r == 1) {
        fprintf(stdout, "[*] kernel driver is active\n");
        r = libusb_detach_kernel_driver(blackwire, INTERFACE_NUM);
        if (r < 0) {
            fprintf(stderr, "[-] failed to detach kernel driver\n");
            libusb_close(blackwire);
            libusb_exit(ctx);
            return r;
        }
    }
    fprintf(stdout, "[+] detached kernel driver\n");

    r = libusb_claim_interface(blackwire, INTERFACE_NUM);
    if (r < 0) {
        fprintf(stderr, "[-] failed to claim interface\n");
        libusb_close(blackwire);
        libusb_exit(ctx);
        return r;
    }
    fprintf(stdout, "[+] claim interface success\n");

    fprintf(stdout, "[*] registering signal handler\n");
    sigact.sa_handler = sighandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGINT, &sigact, NULL);
    sigaction(SIGTERM, &sigact, NULL);
    sigaction(SIGQUIT, &sigact, NULL);

    fprintf(stdout, "[*] starting event loop\n\n");
    event_loop();
    libusb_handle_events_completed(ctx, NULL);
    libusb_free_transfer(transfer_in);


    r = libusb_release_interface(blackwire, INTERFACE_NUM);
    if (r < 0)
        fprintf(stderr, "[-] release interface failed\n");
    else
        fprintf(stdout, "[+] release interface success\n");

    r = libusb_attach_kernel_driver(blackwire, INTERFACE_NUM);
    if (r < 0)
        fprintf(stderr, "[-] kernel driver attach failed\n");
    else
        fprintf(stdout, "[+] kernel driver attach success\n");

    libusb_close(blackwire);
    libusb_exit(ctx);
    return 0;
}

void event_loop() {
    transfer_in = libusb_alloc_transfer(0);
    unsigned char data[8];
    memset(data, 0, sizeof(data));

    libusb_fill_interrupt_transfer(
                transfer_in,
                blackwire,
                ENDPOINT_ADDR,
                data,
                sizeof(data),
                handle_request,
                (void*)transfer_in,
                0
                );

    libusb_submit_transfer(transfer_in);

    while (flag) {
        int r = libusb_handle_events_completed(ctx, NULL);
        if (r < 0) {
            fprintf(stderr, "[-] handle events failed\n");
            break;
        }
    }

    if (transfer_in) {
        int r = libusb_cancel_transfer(transfer_in);
        if (r == 0)
            fprintf(stdout, "[*] transfer cancel success\n");
    }
}


void sighandler(int signum) {
    fprintf(stdout, "[*] signal handler called (%d)\n", signum);
    flag = 0;
}
