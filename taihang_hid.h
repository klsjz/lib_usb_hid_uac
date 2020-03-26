
#ifndef TAIHANG_HID_H
#define TAIHANG_HID_H

#include "libusb.h"



typedef struct as_hid_device_ {
    libusb_device* usb_dev;
    libusb_device_handle *dev_handle;
    int input_endpoint;
    int output_endpoint;
    int input_ep_max_packet_size;
    /* interface bInterfaceNumber */
    int bintf_num;

    struct libusb_transfer *transfer;

}as_hid_dev_t,*as_hid_dev_p; 


#if 1
typedef struct
{
	uint32_t audio_frame_index;
	uint32_t audio_data_len;
	int8_t wakeup_info[128];
	int8_t major_status;
	int8_t wakeup_status;
	int8_t vad;
	int8_t rollback;
	uint8_t local_asr_result;
	int8_t nwakeup;
	int16_t doa;
	uint8_t reserved2[92];
} __attribute__((packed)) kl_audio_info_t;

#else
typedef struct
{
  int8_t WakeUpInfo[128];//0

  int8_t MajorStatus;    //128

  int8_t WakeupStatus;   //129

  int8_t Vad;            //130   

  int8_t rollback;       //131

  int16_t Doa;           //132   

  uint32_t AudioDataLength;  //134

  uint8_t reserved[98];  //

}__attribute__((packed)) kl_audio_info_t; 
#endif

extern int kl_device_close(as_hid_dev_p p_as_dev);
extern as_hid_dev_p kl_device_open(void); 

extern int kl_start_record(as_hid_dev_p p_as_dev);
extern int kl_audio_read(as_hid_dev_p p_as_dev, char *pAudio_buffer, kl_audio_info_t *pAudio_info);
extern int kl_stop_record(as_hid_dev_p p_as_dev);

extern int kl_stop_play(as_hid_dev_p p_as_dev);
extern int kl_start_play(as_hid_dev_p p_as_dev);

extern int kl_audio_play(as_hid_dev_p p_as_dev,char *pAudio_buffer, unsigned int length);
extern int kl_get_fw_version(as_hid_dev_p p_as_dev,unsigned char* version);

#endif