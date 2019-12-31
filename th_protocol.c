

#include <linux/types.h>
#include "th_protocol.h"



#define HID_BUFFR_LENGH 64
char hid_buffer[HID_BUFFR_LENGH]={0};




char check_sum(char* data, char size)
{

	char check=0xff;
	while(size--)
	{
		check^=*(data++);
	}
	
	return check;
}


void th_hid_req(kl_req_type_t req_type, kl_command_type_t cmd_type)
{
	hid_buffer[0]=req_type;
	hid_buffer[1]=REQUSET_FLAG;
	hid_buffer[2]=cmd_type>>0x08;
	hid_buffer[3]=cmd_type;
	memset(hid_buffer+4,HID_BUFFR_LENGH-4-1,0);

	hid_buffer[63]=check_sum(hid_buffer,63);
}


