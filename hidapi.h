#ifndef _HID_API_H
#define _HID_API_H

#define HID_REPORT_TYPE_INPUT         0x01
#define HID_REPORT_TYPE_OUTPUT        0x02
#define HID_REPORT_TYPE_FEATURE       0x03


#include "taihang_hid.h"


int set_control_transfer(libusb_device_handle* handle,int intf_num, const unsigned char* data,size_t length);

int get_control_transfer(libusb_device_handle* handle,int intf_num, const unsigned char* data,size_t length);

void print_endpoint_comp(const struct libusb_ss_endpoint_companion_descriptor *ep_comp);

void print_endpoint(const struct libusb_endpoint_descriptor *endpoint);

void print_altsetting(const struct libusb_interface_descriptor *interface);

void print_interface(const struct libusb_interface *interface);

void print_config(struct libusb_config_descriptor *config);

void print_dev(libusb_device *dev);

int print_target_dev(libusb_device *dev,uint16_t vid, uint16_t pid);

as_hid_dev_p init_as_standard_interface(int vid, int pid);

void destroy_as_standard_interface(as_hid_dev_p dev);

#endif /* ifndef _HID_API_H */
