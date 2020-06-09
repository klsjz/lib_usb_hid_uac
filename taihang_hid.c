#include "libusb.h"
#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"
#include "th_protocol.h"
#include <linux/types.h>
#include <string.h>


#define AUDIO_INFO_LENGTH 236
#define AUDIO_DATA_LENTH 1024
#define AUDIO_BUFFER_LENGTH (1024+20+AUDIO_INFO_LENGTH)  //20 is totol type length, total 20 packet

extern char hid_buffer[]; 
char g_audio_buffer[AUDIO_BUFFER_LENGTH-20];
char g_play_buffer[AUDIO_BUFFER_LENGTH-20];

int kl_audio_read(as_hid_dev_p p_as_dev, char *pAudio_buffer, kl_audio_info_t *pAudio_info)
{	
	int ret;
	int size;
	int read_length=0;
	int target_size=AUDIO_BUFFER_LENGTH-20;
	char hid_buf[64];
    const size_t max_len = p_as_dev->input_ep_max_packet_size;

   	if (!pAudio_buffer)
	{
		printf("audio read buf is null\n");
		goto err;
	}

    while (read_length<target_size)
    {
		ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->input_endpoint,
			hid_buf,
			max_len,
			&size,
			0//no time out
		);	
		if (ret!=0)
			goto err;

		memcpy(g_audio_buffer+read_length,hid_buf+1,size-1);
		read_length+=size-1;
    }	

    //extract audio info and audio buffer
    memcpy(pAudio_info,g_audio_buffer,sizeof(kl_audio_info_t));
   // memcpy(pAudio_buffer,g_audio_buffer+sizeof(kl_audio_info_t),AUDIO_DATA_LENTH);

    return 0;
err:
	printf("audio read stun, failed\n");
	return -1;
}




as_hid_dev_p kl_device_open(void) 
{
	
	as_hid_dev_p p_as_dev;
	int vid=0x5448;
	int pid=0x152e;

	p_as_dev=init_as_standard_interface(vid,pid);

	if (!p_as_dev)
	{
		printf("init interface failed\n");
		goto err;
	}

    print_dev(p_as_dev->usb_dev);

	printf("\n>>>>>>>>>device open sucess<<<<<<<<<\n");
	return p_as_dev;
err:
	return NULL;
}

int kl_get_fw_version(as_hid_dev_p p_as_dev,unsigned char* version)
{
	int ret,size;
	unsigned char buf[64]={0};
	size_t max_len;
	max_len = p_as_dev->input_ep_max_packet_size;

    //fill in the hid buffer
	th_hid_req(COMMAND,GET_FW_VERSION);
    //p_as_dev->transfer = libusb_alloc_transfer(0);
    size=64;
	ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->output_endpoint,
			hid_buffer,
			max_len,
			&size,
			0//no time out
		); 

	if (ret==0)
	{
		ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->input_endpoint,
			buf,
			max_len,
			&size,
			0//no time out
		); 
		if (0==ret)
		{
			*version = buf[4];
		}
		else
		{	
			printf("cannot get fwversion\n");
			return -1;
		}
	}	
	else
	{
		printf("cannot get fwversion\n");
		return -1;
	}
	return ret;
}

int kl_get_wakeup_words(as_hid_dev_p p_as_dev,char *words)
{
	int ret,size;
	unsigned char buf[64]={0};
	size_t max_len;
	max_len = p_as_dev->input_ep_max_packet_size;

    //fill in the hid buffer
	th_hid_req(COMMAND,GET_WAKEUP_WORD);
    //p_as_dev->transfer = libusb_alloc_transfer(0);
    size=64;
	ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->output_endpoint,
			hid_buffer,
			max_len,
			&size,
			0//no time out
		); 
	if (ret==0)
	{
		ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->input_endpoint,
			buf,
			max_len,
			&size,
			0//no time out
		); 
		if (0==ret)
		{
			memcpy(words,buf+4,strlen(buf+4));
		//	printf("wakeup words%s\n",buf+4);
		}
		else
		{	
			printf("cannot get wake up words\n");
			return -1;
		}
	}	
	else
	{
		printf("cannot get wake up wordsn\n");
		return -1;
	}
	return ret;
}


int kl_get_alg_info(as_hid_dev_p p_as_dev, char *majorstatus, char*doa, char*nwakeup, char*wakeup_status, char*beamflag)
{
	int ret,size;
	unsigned char buf[64]={0};
	size_t max_len;
	max_len = p_as_dev->input_ep_max_packet_size;

    //fill in the hid buffer
	th_hid_req(COMMAND,GET_ALG_INFO);
    //p_as_dev->transfer = libusb_alloc_transfer(0);
    size=64;
	ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->output_endpoint,
			hid_buffer,
			max_len,
			&size,
			0//no time out
		); 

	if (ret==0)
	{
		ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->input_endpoint,
			buf,
			max_len,
			&size,
			0//no time out
		); 
		if (0==ret)
		{
//			printf("alg:%02x %02x  %d\n",buf[2],GET_ALG_INFO>>8,(buf[2]==(GET_ALG_INFO>>8)));
			if(buf[2]!=(GET_ALG_INFO>>8))
			{
				ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
                        		p_as_dev->input_endpoint,
                        		buf,
                         		max_len,
                         		&size,
                         		0//no time out
                			);
			}
			if(ret!=0)
			{
				printf("cannot get alg info\n");	
			}
			*majorstatus = buf[4];
			*doa = buf[5];
			*nwakeup = buf[6];
			*wakeup_status = buf[7];
			*beamflag = buf[8];
		//	printf("111MajorStatus:%d \n",buf[4]);
		//	printf("111Doa:%d\n",buf[5]);
		//	printf("111nwakeup:%d\n",buf[6]);
		}
		else
		{	
			printf("cannot get alg info\n");
			return -1;
		}
	}	
	else
	{
		printf("cannot get alg info\n");
		return -1;
	}
	return ret;
}

int kl_start_record(as_hid_dev_p p_as_dev)
{
	int ret,size;
	unsigned char buf[64]={0};
	size_t max_len;
	max_len = p_as_dev->input_ep_max_packet_size;

	printf("\n=========1. check record state=======\n");
    //fill in the hid buffer
	th_hid_req(COMMAND,GET_RECORD_STATUS);
    //p_as_dev->transfer = libusb_alloc_transfer(0);
    size=64;
	ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->output_endpoint,
			hid_buffer,
			max_len,
			&size,
			0//no time out
		); 

	if (ret==0)
	{
		ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->input_endpoint,
			buf,
			max_len,
			&size,
			0//no time out
		); 
		if (0==ret)
		{
			printf("rsp length:%d\n", size);
			printf("rsp type:0x%x\n", buf[0]);
			printf("rsp status:0x%x\n",buf[1]);
			printf("rsp command:0x%x\n",buf[2]);
			printf("rsp sub-command:0x%x\n",buf[3]);
			printf("rsp data:0x%x\n",buf[4]);
		}
		else
		{	
			printf("cannot read record state\n");
			goto err;
		}
	}	
	else
	{
		printf("cannot request record state \n");
		goto err;
	}

   	printf("\n=========2. open record =======\n");
   	//fill in the hid buffer
	th_hid_req(COMMAND,SET_RECORD_OPEN);
    //p_as_dev->transfer = libusb_alloc_transfer(0);
    size=64;
	ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->output_endpoint,
			hid_buffer,
			max_len,
			&size,
			0//no time out
		); 

	if (ret==0)
	{
		ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->input_endpoint,
			buf,
			max_len,
			&size,
			0//no time out
		); 
		if (0==ret)
		{
			printf("rsp length:%d\n", size);
			printf("rsp type:0x%x\n", buf[0]);
			printf("rsp status:0x%x\n",buf[1]);
			printf("rsp command:0x%x\n",buf[2]);
			printf("rsp sub-command:0x%x\n",buf[3]);
			printf("rsp data:0x%x\n",buf[4]);
		}
		else
		{	
			printf("req open record return false\n");
			goto err;
		}
	}	                            
	else
	{
		printf("cannot open record \n");
		goto err;
	}
	return 0;
err:
	return -1;
}


int kl_stop_record(as_hid_dev_p p_as_dev)
{
	int size;
	unsigned char buf[64]={0};
	size_t max_len=64;
	int ret;
	int count=0;
	//resetUSB(p_as_dev->dev_handle);
	printf("\n=========close record device =======\n");
    //fill in the hid buffer
	th_hid_req(COMMAND,SET_RECORD_CLOSE);
    //p_as_dev->transfer = libusb_alloc_transfer(0);
    size=64;
	ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->output_endpoint,
			hid_buffer,
			max_len,
			&size,
			0//no time out
		); 
	printf("close ret:%d\n",ret );
	if (ret==0)
	{
		while(count<=40)  //clear taihang device buffer
		{
			ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
				p_as_dev->input_endpoint,
				buf,
				max_len,
				&size,
				0//no time out
			); 

			if (0==ret)
			{
				if (buf[0]==0x01 && buf[1]==0x01)
				{
					printf("rsp length:%d\n", size);
					printf("rsp type:0x%x\n", buf[0]);
					printf("rsp status:0x%x\n",buf[1]);
					printf("rsp command:0x%x\n",buf[2]);
					printf("rsp sub-command:0x%x\n",buf[3]);
					printf("rsp data:0x%x\n",buf[4]);
					printf("\n>>>>>>>>>device close success<<<<<<<<<\n");
					break;					
				}							
			}
			else
			{	
				printf("stop record return failed,ret:%d\n",ret);
				goto err;
			}
			count++;			
		}	
		if (count>40)
		{
			printf("rsp length:%d\n", size);
			printf("rsp type:0x%x\n", buf[0]);
			printf("rsp status:0x%x\n",buf[1]);
			printf("rsp command:0x%x\n",buf[2]);
			printf("rsp sub-command:0x%x\n",buf[3]);
			printf("rsp data:0x%x\n",buf[4]);
			printf("\n>>>>>>>>>device close failed<<<<<<<<<\n");
		}
	}	
	else
	{
		printf("cannot request stop record,ret:%d \n",ret);
		goto err;
	}

	return 0;
err:
	return -1;
}

int kl_start_play(as_hid_dev_p p_as_dev)
{
	int ret,size;
	unsigned char buf[64]={0};
	size_t max_len;
	max_len = p_as_dev->input_ep_max_packet_size;    

   	printf("\n=========1. open play =======\n");
   	//fill in the hid buffer
	th_hid_req(COMMAND,SET_PLAYER_OPEN);
    //p_as_dev->transfer = libusb_alloc_transfer(0);
    size=64;
	ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->output_endpoint,
			hid_buffer,
			max_len,
			&size,
			0//no time out
		); 

	if (ret==0)
	{
		ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->input_endpoint,
			buf,
			max_len,
			&size,
			0//no time out
		); 
		if (0==ret)
		{
			printf("rsp length:%d\n", size);
			printf("rsp type:0x%x\n", buf[0]);
			printf("rsp status:0x%x\n",buf[1]);
			printf("rsp command:0x%x\n",buf[2]);
			printf("rsp sub-command:0x%x\n",buf[3]);
			printf("rsp data:0x%x\n",buf[4]);
		}
		else
		{	
			printf("req open play return false\n");
			goto err;
		}
	}	                            
	else
	{
		printf("cannot open play \n");
		goto err;
	}
	return 0;
err:
	return -1;
}

unsigned int g_send_index=0;

int kl_audio_play(as_hid_dev_p p_as_dev,char *pAudio_buffer, unsigned int length)
{
	int ret;
	int size;
	int write_length=0;
	kl_audio_info_t kl_header;
	char hid_buf[64];
	int target_size=AUDIO_BUFFER_LENGTH-20;
	unsigned int send_len=0;
	int i=0;
	unsigned int left=0;
	unsigned int index=0;
    const size_t max_len = p_as_dev->input_ep_max_packet_size;

   	if (!pAudio_buffer)
	{
		printf("audio play buf is null\n");
		goto err;
	}

	index=length/AUDIO_DATA_LENTH;
	left=length%AUDIO_DATA_LENTH;
	if (left)
		index=index+1;

	//mark the type
    hid_buf[0]=0x02;

    for (i=0;i<index;i++)
    {
    	//pack a new packet
		kl_header.audio_frame_index=g_send_index++;
  		kl_header.audio_data_len=AUDIO_DATA_LENTH;
  		//printf("send_len:%d\n",send_len);
  		//fill audio info and audio buffer
    	memcpy(g_play_buffer,&kl_header,sizeof(kl_audio_info_t));
    	if (i==index-1 && left)
    	{
    		memcpy(g_play_buffer+sizeof(kl_audio_info_t),pAudio_buffer+send_len,left);
    		memset(g_play_buffer+sizeof(kl_audio_info_t)+left,0,AUDIO_DATA_LENTH-left);
    	}	
    	else
    		memcpy(g_play_buffer+sizeof(kl_audio_info_t),pAudio_buffer+send_len,AUDIO_DATA_LENTH);

    	write_length=0;
    	while (write_length<target_size)
    	{
	  		//printf("write_length:%d\n",write_length);

    		memcpy(hid_buf+1,g_play_buffer+write_length,max_len-1);
    		ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
				p_as_dev->output_endpoint,
				hid_buf,
				max_len,
				&size,
				0//no time out
			);

			if (ret!=0)
				goto err;	
			write_length+=size-1;
			
    	}
		
		send_len+=AUDIO_DATA_LENTH;
    }	

    return 0;
err:
	printf("audio write stun, failed\n");
	return -1;
}


int kl_stop_play(as_hid_dev_p p_as_dev)
{
	int size;
	unsigned char buf[64]={0};
	size_t max_len=64;
	int ret;
	int count=0;
	//resetUSB(p_as_dev->dev_handle);
	printf("\n=========set play close =======\n");
    //fill in the hid buffer
	th_hid_req(COMMAND,SET_PLAYER_CLOSE);
    //p_as_dev->transfer = libusb_alloc_transfer(0);
    size=64;
	ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
			p_as_dev->output_endpoint,
			hid_buffer,
			max_len,
			&size,
			0//no time out
		); 
	printf("close ret:%d\n",ret );
	if (ret==0)
	{
		while(count<=40)  //clear taihang device buffer
		{
			ret = libusb_interrupt_transfer(p_as_dev->dev_handle,
				p_as_dev->input_endpoint,
				buf,
				max_len,
				&size,
				0//no time out
			); 

			if (0==ret)
			{
				if (buf[0]==0x01 && buf[1]==0x01)
				{
					printf("rsp length:%d\n", size);
					printf("rsp type:0x%x\n", buf[0]);
					printf("rsp status:0x%x\n",buf[1]);
					printf("rsp command:0x%x\n",buf[2]);
					printf("rsp sub-command:0x%x\n",buf[3]);
					printf("rsp data:0x%x\n",buf[4]);
					printf("\n>>>>>>>>>device close success<<<<<<<<<\n");
					break;					
				}							
			}
			else
			{	
				printf("stop play return failed,ret:%d\n",ret);
				goto err;
			}
			count++;			
		}	
		if (count>40)
		{
			printf("rsp length:%d\n", size);
			printf("rsp type:0x%x\n", buf[0]);
			printf("rsp status:0x%x\n",buf[1]);
			printf("rsp command:0x%x\n",buf[2]);
			printf("rsp sub-command:0x%x\n",buf[3]);
			printf("rsp data:0x%x\n",buf[4]);
			printf("\n>>>>>>>>>device close failed<<<<<<<<<\n");
		}
	}	
	else
	{
		printf("cannot request stop play,ret:%d \n",ret);
		goto err;
	}

	return 0;
err:
	return -1;
}

int kl_device_close(as_hid_dev_p p_as_dev)
{
	destroy_as_standard_interface(p_as_dev);

	return 0;
}


void resetUSB(libusb_device_handle *dev_handle) 
{
    int success;
    int bpoint = 0;
    do {
        success = libusb_reset_device(dev_handle);
        if ((bpoint % 10) == 0) {
            printf(".");
        }
        ++bpoint;
        if (bpoint > 100) {
            success = 1;
        }
    } while (success < 0);
    if (success) {
        printf("\nreset usb device failed:%d\n", success);
    } else {
        printf("\nreset usb device ok\n");
    }
}




