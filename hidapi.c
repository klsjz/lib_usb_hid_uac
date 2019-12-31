#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb.h>

#include "hidapi.h"

int set_control_transfer(libusb_device_handle* handle,int intf_num, const unsigned char* data,size_t length)
{
    int res;

    res = libusb_control_transfer(handle,
	    /* LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_OUT, */
	    0x21,
	    0x09/*HID set_report*/,
	    /* (HID_REPORT_TYPE_OUTPUT << 8) | report_number, */
	    (HID_REPORT_TYPE_OUTPUT << 8) | 0,
	    /* 0x0200, */
	    /* dev->interface, */
	    /* 0x0004, */
	    intf_num,
	    (unsigned char *)data, length,
	    1000/*timeout millis*/);

    return res;
}

int get_control_transfer(libusb_device_handle* handle,int intf_num, const unsigned char* data,size_t length)
{
    int res;

    res = libusb_control_transfer(handle,
	    /* LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_IN, */
	    0xa1,
	    0x01/*HID get_report*/,
	    (HID_REPORT_TYPE_INPUT << 8) | 0,
	    /* 0x0100, */
	    /* dev->interface, */
	    /* 0x0004, */
	    intf_num,
	    (unsigned char *)data, length,
	    1000/*timeout millis*/);

    return res;
}

void print_endpoint_comp(const struct libusb_ss_endpoint_companion_descriptor *ep_comp)
{
    printf("      USB 3.0 Endpoint Companion:\n");
    printf("        bMaxBurst:        %d\n", ep_comp->bMaxBurst);
    printf("        bmAttributes:     0x%02x\n", ep_comp->bmAttributes);
    printf("        wBytesPerInterval: %d\n", ep_comp->wBytesPerInterval);
}

void print_endpoint(const struct libusb_endpoint_descriptor *endpoint)
{
    int i, ret;

    printf("      Endpoint:\n");
    printf("        bEndpointAddress: %02xh\n", endpoint->bEndpointAddress);
    printf("        bmAttributes:     %02xh\n", endpoint->bmAttributes);
    printf("        wMaxPacketSize:   %d\n", endpoint->wMaxPacketSize);
    printf("        bInterval:        %d\n", endpoint->bInterval);
    printf("        bRefresh:         %d\n", endpoint->bRefresh);
    printf("        bSynchAddress:    %d\n", endpoint->bSynchAddress);

    for (i = 0; i < endpoint->extra_length;) {
	if (LIBUSB_DT_SS_ENDPOINT_COMPANION == endpoint->extra[i + 1]) {
	    struct libusb_ss_endpoint_companion_descriptor *ep_comp;

	    ret = libusb_get_ss_endpoint_companion_descriptor(NULL, endpoint, &ep_comp);
	    if (LIBUSB_SUCCESS != ret) {
		continue;
	    }

	    print_endpoint_comp(ep_comp);

	    libusb_free_ss_endpoint_companion_descriptor(ep_comp);
	}

	i += endpoint->extra[i];
    }
}

void print_altsetting(const struct libusb_interface_descriptor *interface)
{
    uint8_t i;

    printf("    Interface:\n");
    printf("      bInterfaceNumber:   %d\n", interface->bInterfaceNumber);
    printf("      bAlternateSetting:  %d\n", interface->bAlternateSetting);
    printf("      bNumEndpoints:      %d\n", interface->bNumEndpoints);
    printf("      bInterfaceClass:    %d\n", interface->bInterfaceClass);
    printf("      bInterfaceSubClass: %d\n", interface->bInterfaceSubClass);
    printf("      bInterfaceProtocol: %d\n", interface->bInterfaceProtocol);
    printf("      iInterface:         %d\n", interface->iInterface);

    for (i = 0; i < interface->bNumEndpoints; i++)
	print_endpoint(&interface->endpoint[i]);
}

void print_interface(const struct libusb_interface *interface)
{
	int i;
    printf("interface num_altsetting = %d\n", interface->num_altsetting);
    for (i = 0; i < interface->num_altsetting; ++i) {
	print_altsetting(&interface->altsetting[i]); 
    }
}

void print_config(struct libusb_config_descriptor *config)
{
    uint8_t i;
    printf("configuration:\n");
    printf("    wTotalLength:        %d\n",config->wTotalLength);
    printf("    bNumInterfaces:      %d\n",config->bNumInterfaces);
    printf("    bConfigurationValue: %d\n",config->bConfigurationValue);
    printf("    iConfiguration:      %d\n",config->iConfiguration);
    printf("    bmAttributes:        %02xh\n",config->bmAttributes);
    printf("    MaxPower:            %d\n",config->MaxPower);

    for(i = 0; i < config->bNumInterfaces;++i){
	print_interface(&config->interface[i]);
    }
}



void print_dev(libusb_device *dev)
{
    struct libusb_device_descriptor desc;
    libusb_device_handle *handle = NULL;

    char description[256];
    char string[256];

    int ret;
    int i=0;

    ret = libusb_get_device_descriptor(dev, &desc);
    if(ret < 0){
	printf("failed to get device descriptor \n");
	return;
    }

    ret = libusb_open(dev, &handle);
    if(LIBUSB_SUCCESS == ret){
	if(desc.iManufacturer){
	    ret = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, string, sizeof(string));
	    if(ret > 0){
		snprintf(description, sizeof(description),"%s - ",string);	
	    }else{
		snprintf(description, sizeof(description), "%04X - ", desc.idVendor);
	    }
	}else{
	    snprintf(description, sizeof(description), "%04X - ",desc.idVendor);
	}

	if(desc.iProduct){
	    ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct,string,sizeof(string));
	    if(ret > 0){
		snprintf(description + strlen(description), sizeof(description) - strlen(description),"%s",string);
	    }else{
		snprintf(description + strlen(description), sizeof(description) - strlen(description), "%04X",desc.idProduct);
	    }
	}else{
	    snprintf(description + strlen(description), sizeof(description) - strlen(description),"%04X", desc.idProduct);	
	}

    }else{
	snprintf(description, sizeof(description), "%04X - %04X",desc.idVendor,desc.idProduct);
    }

    snprintf(description, sizeof(description), "%04X - %04X",desc.idVendor,desc.idProduct);

    printf("%.*sDev (bus %d, device %d): %s \n",0, "................",
	    libusb_get_bus_number(dev), libusb_get_device_address(dev), description);

    if(handle){
	if(desc.iSerialNumber){
	    ret = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, string, sizeof(string));
	    if(ret > 0){
		printf("%.*s - serial number : %s \n", 0,"..........................", string);
	    }

	}
    }

    for (i = 0; i < desc.bNumConfigurations; ++i) {
	struct libusb_config_descriptor *config;
	ret = libusb_get_config_descriptor(dev, i, &config);
	if(LIBUSB_SUCCESS != ret){
	    printf("could not retrieve description \n");
	    continue;
	} 

	print_config(config);
	libusb_free_config_descriptor(config);
    }

    if(handle)
	libusb_close(handle);

}

/*
 * return interface that up to aispeech standards
 * to interrupt transfer audio data and voice key event.
 * we need the secound hid type class of interface 
 */
as_hid_dev_p init_as_standard_interface(int vid, int pid)
{
    printf("init as interface : vid = %d pid = %d\n", vid, pid);

    int i,j,k;
    int f = 0;
    int ret;

    libusb_device **devs;
    struct libusb_device_descriptor desc;
    struct libusb_config_descriptor *conf_desc = NULL;

    as_hid_dev_p as_dev = NULL;

    ret	= libusb_init(NULL);

    if(ret < 0){
	printf("init libusb err : %d\n", ret); 
	return NULL;	
    }

    printf("init libusb success\n");

    libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_INFO);

    ssize_t cnt = libusb_get_device_list(NULL, &devs);

    if(cnt <= 0){
	printf("usb device list is empty \n");
	printf("have you forgetton plugin usb device ? \n");	
	return NULL;
    }

    printf(" -----------------------------\n");
    printf("get device list cnt : %Zu\n", cnt);

    for (i = 0; devs[i]; ++i) {                                    
	ret = libusb_get_device_descriptor(devs[i], &desc);

	if(ret < 0){
	    printf("can not get device descriptor : %d\n",ret);
	    continue;
	}
	if(desc.idVendor == vid && desc.idProduct == pid){

	    /* print_dev(devs[i]); */
	    
	    as_dev = calloc(1, sizeof(as_hid_dev_t));
	    as_dev->usb_dev = devs[i];
	    break;
	}
    }                                                                  

    libusb_free_device_list(devs,1);

    if(!as_dev){
	printf("can not find target usb device or calloc as hid dev err\n");
	return NULL;
    }

    if (libusb_get_active_config_descriptor(as_dev->usb_dev, &conf_desc) < 0){
	printf("can not get config description\n"); 
	return NULL;
    }

    for (j = 0; (j < conf_desc->bNumInterfaces); j++) 
    {
		const struct libusb_interface *intf = &conf_desc->interface[j];
		for (k = 0; k < intf->num_altsetting; k++) 
		{
		    const struct libusb_interface_descriptor *intf_desc;
		    intf_desc = &intf->altsetting[k];

		    if (intf_desc->bInterfaceClass == LIBUSB_CLASS_HID) 
		    {
				printf("find hid interface: %d\n",f);
				{
				    as_dev->bintf_num = intf_desc->bInterfaceNumber;
				    /* Find the INPUT and OUTPUT endpoints. An
				     *			       OUTPUT endpoint is not required. */
				    for (i = 0; i < intf_desc->bNumEndpoints; i++)
				    {
						const struct libusb_endpoint_descriptor *ep
						    = &intf_desc->endpoint[i];

						/* Determine the type and direction of this
						 *			       endpoint. */
						int is_interrupt =
						    (ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK)
						    == LIBUSB_TRANSFER_TYPE_INTERRUPT;
						int is_output = 
						    (ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK)
						    == LIBUSB_ENDPOINT_OUT;
						int is_input = 
						    (ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK)
						    == LIBUSB_ENDPOINT_IN;
						printf("is_interrupt:0x%x,is_output:0x%x,is_input:0x%x\n",is_interrupt,is_output,is_input);
						/* Decide whether to use it for intput or output. */
						if (as_dev->input_endpoint == 0 &&
							is_interrupt && is_input) {
						    /* Use this endpoint for INPUT */
						    as_dev->input_endpoint = ep->bEndpointAddress;
						    as_dev->input_ep_max_packet_size = ep->wMaxPacketSize;
						}
						if (as_dev->output_endpoint == 0 &&
							is_interrupt && is_output) {
						    /* Use this endpoint for OUTPUT */
						    as_dev->output_endpoint = ep->bEndpointAddress;
						}
				    }
				    break;
				}
		    }
		}
    }    

    ret = libusb_open(as_dev->usb_dev, &as_dev->dev_handle);
    if(ret < 0){
	printf("can not open usb dev\n");
	return NULL;
    }

    /* Detach the kernel driver, but only if the
     * device is managed by the kernel */
    if (libusb_kernel_driver_active(as_dev->dev_handle, as_dev->bintf_num) == 1) {
		ret = libusb_detach_kernel_driver(as_dev->dev_handle, as_dev->bintf_num);
		if (ret < 0) {
		    libusb_close(as_dev->dev_handle);
		    printf("Unable to detach Kernel Driver\n");
		    return NULL;
		}
    }

    ret = libusb_claim_interface(as_dev->dev_handle, as_dev->bintf_num);
    if (ret < 0) {
		printf("can't claim interface %d: %d\n", as_dev->bintf_num, ret);
		return NULL;
    }
    
    return as_dev;
}

void destroy_as_standard_interface(as_hid_dev_p dev)
{
    printf("destroy as interface\n");
    if(dev){
	libusb_free_transfer(dev->transfer);
	libusb_release_interface(dev->dev_handle,dev->bintf_num);
	libusb_close(dev->dev_handle);

	free(dev);
    }

    libusb_exit(NULL);
}
