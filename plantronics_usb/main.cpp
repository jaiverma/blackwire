#include <iostream>
#include "libusb-1.0/libusb.h"

extern "C" void LIBUSB_CALL callback(libusb_transfer *transfer) {
    printf("Callback called\n");
    for (int i = 0; i < transfer->actual_length; i++)
        printf("%02x ", transfer->buffer[i]);
    printf("\n");
}

int main() {
    int r = libusb_init(NULL);
    if (r < 0)
        return r;

    ssize_t cnt;
    libusb_device **devs;
    libusb_device *plantronics = nullptr;
    libusb_context *ctx = nullptr;

    cnt = libusb_get_device_list(NULL, &devs);
    if (cnt < 0) {
        libusb_exit(NULL);
        return (int)cnt;
    }

    libusb_device *dev = nullptr;
    uint8_t path[8];
    int i = 0;

    while ((dev = devs[i++]) != NULL) {
        libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(dev, &desc);
        if (r < 0) {
            std::cerr << "Failed to get device descriptor\n";
            return r;
        }

        if (desc.idVendor == 0x47f && desc.idProduct == 0xc056) {
            plantronics = dev;
        }
    }

    libusb_free_device_list(devs, 1);

    if (plantronics) {
        std::cout << "Found device!\n";
        libusb_device_descriptor desc;
        libusb_get_device_descriptor(plantronics, &desc);
        printf("0x%04x:0x%04x\n", desc.idVendor, desc.idProduct);

        libusb_config_descriptor *config;
        libusb_get_config_descriptor(plantronics, 0, &config);

        printf("Num interface: %d\n", config->bNumInterfaces);
        const libusb_interface *inter;
        const libusb_interface_descriptor *interdesc;
        const libusb_endpoint_descriptor *epdesc;

        for (auto i = 0; i < (int)config->bNumInterfaces; i++) {
            inter = &config->interface[i];
            printf("\tNum alternate settings: %d\n", inter->num_altsetting);

            for (auto j = 0; j < inter->num_altsetting; j++) {
                interdesc = &inter->altsetting[j];
                printf("\t\tInterface number: %d\n", (int)interdesc->bInterfaceNumber);
                printf("\t\tNum endpoints: %d\n", (int)interdesc->bNumEndpoints);

                for (auto k = 0; k < (int)interdesc->bNumEndpoints; k++) {
                    epdesc = &interdesc->endpoint[k];
                    printf("\t\t\tDescriptor type: %d\n", (int)epdesc->bDescriptorType);
                    printf("\t\t\tEP address: 0x%x\n", (int)epdesc->bEndpointAddress);
                    printf("\t\t\tData length: %d\n", epdesc->bLength);
                }
            }
        }

        libusb_free_config_descriptor(config);

        libusb_device_handle *handle = nullptr;

        r = libusb_open(plantronics, &handle);
        if (r < 0) {
            std::cerr << "Failed to get handle\n";
            return r;
        }

        r = libusb_kernel_driver_active(handle, 3);
        if (r == 1) {
            printf("Kernel driver is active\n");
            r = libusb_detach_kernel_driver(handle, 3);
            if (r < 0) {
                std::cerr << "Failed to detach kernel driver\n";
                return r;
            }
        }

        r = libusb_claim_interface(handle, 3);
        if (r < 0) {
            std::cerr << "Cannot claim interface\n";
            printf("%d\n", r);
            return r;
        }

        libusb_transfer *transfer = nullptr;
        unsigned char data[8];
        transfer = libusb_alloc_transfer(0);
        libusb_fill_interrupt_transfer(transfer, handle, 0x84, data, sizeof(data), callback, NULL, 0);
        libusb_submit_transfer(transfer);

        while(1)
            libusb_handle_events(ctx);
//        return 0;
        while (1) {
            unsigned char data[8];
            int len = 0;
            r = libusb_interrupt_transfer(handle, 0x84, data, sizeof(data), &len, 0);
            if (r < 0) {
                printf("Interrupt transfer failed\n");
                return r;
            }

            printf("Bytes read: %d\n", len);
            for (int i = 0; i < len; i++) {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
    }

    libusb_exit(NULL);
    return 0;
}
