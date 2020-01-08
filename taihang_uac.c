#include "libusb.h"
#include <stdio.h>
#include "th_protocol.h"
#include "taihang_uac.h"
#include "taihang_hid.h"


extern FILE* fout_uac;
uint8_t buf[NUM_TRANSFERS][PACKET_SIZE*NUM_PACKETS];
static int bFisrt = 1;
extern int flag;
//数据传输完成后，transfer传输任务会调用此回调函数。我们在这里拿走数据，并且继续把添加新的transfer任务，循环读取static FILE* fout=NULL;static int bFisrt = 1;
static void cb_xfr(struct libusb_transfer *xfr)
{	
	if(bFisrt)	
	{		
		bFisrt = 0;		
		fout_uac = fopen("./uac_data.pcm","w+");		
		if(fout_uac == NULL)		
		{			
			printf("canok:: erro to openfile[%d%s] \n",__LINE__,__FUNCTION__);		
		}
	} 	
	int  i =0;	
	static j = 0;
	j++;
	if(fout_uac)	
	{
		//取出数据给到文件		
		for(i=0;i<xfr->num_iso_packets;i++)		
		{			
			struct libusb_iso_packet_descriptor *pack = &xfr->iso_packet_desc[i];			
			if(pack->status != LIBUSB_TRANSFER_COMPLETED)			
			{				
				printf("canok:: erro transfer status %d %s [%d%s]\n",pack->status,libusb_error_name(pack->status),__LINE__,__FUNCTION__);
				break;			
			} 			
			const uint8_t *data = libusb_get_iso_packet_buffer_simple(xfr, i);			
	//	printf("get out data %d [%d%s]\n",pack->actual_length,__LINE__,__FUNCTION__);			
			fwrite(data,1,pack->actual_length,fout_uac);		
		} 	
	}	 	
	//把transfer任务重新在提交上去	
	if(flag)
	{
		if (libusb_submit_transfer(xfr) < 0)
		{		
				printf("error re-submitting !!!!!!!exit ----------[%d%s]\n",__LINE__,__FUNCTION__);		
				exit(1);	
		}	
		else
		{		
		//	printf("re-submint ok j%d!\n",j);	
		}
	}
	else
	{
		libusb_free_transfer(xfr);
	}
	

}

int kl_uac_open(as_hid_dev_p p_as_dev)
{
	int ret,i;
	char rate[3] = {0x80,0x3e,0};
	//先做一个check，确保设备没有占用，在最小demo情况下，可以先不考虑这种复杂情况	
	ret = libusb_kernel_driver_active(p_as_dev->dev_handle,1);
	if(ret == 1)
	{		
		printf("acticve ,to deteach .\r\n");		
		ret = libusb_detach_kernel_driver(p_as_dev->dev_handle,1);		
		if(ret <0)		
		{			
			printf("canok:: erro to detach kernel!!![%d%s]\n",__LINE__,__FUNCTION__);			
			return -1;		
		}	
	}

	//------------------请求使用一个 interface 第二个参数是 interface 编号
	ret = libusb_claim_interface(p_as_dev->dev_handle,1);	
	if(ret < 0)	
	{		
		printf("can:: erro claming interface %s %d [%d%s]\n",libusb_error_name(ret),ret,__LINE__,__FUNCTION__);		
		return -1;	
	} 
	//-------------------同一个接口可以有多个接口描述符，用bAlternateSetting来识别.	//在Interface Descriptor 中的bAlternateSetting 值	
	libusb_set_interface_alt_setting(p_as_dev->dev_handle,1,1);

    ret = libusb_control_transfer(p_as_dev->dev_handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_ENDPOINT, 
                            0x01, 0x0100, 0x81, 
                            rate, sizeof(rate), 0);

	//可以一次性提交多个transfer，这样当总线上有数据在传输时，同时可以有已经从usb总线传输完成的	
	//transfer 在调用 callback, 这样可以提高 usb总线利用率，所以这里提交了 NUM_TRANSFER个 transfer
	//每一个 transfer传输 NUM_PACKETS 个包，每个包 大小为PACKET_SIZE . 可以理解为一个transfer 即一个传输任务

	struct libusb_transfer* xfr[NUM_TRANSFERS];

	for(i=0; i<NUM_TRANSFERS; i++)	
	{		
		xfr[i] = libusb_alloc_transfer(NUM_PACKETS);
		if(!xfr[i])		
		{			
			printf("can:: alloc transfer err [%d%s]o\n",__LINE__,__FUNCTION__);			
			return -1;		
		}                
		//填充transfer ,比如transfer的回调，cb_xfr, 这个transfer在执行完成后，会调用这个回调函数，我们就可以在回调里面		
		//把数据拿走。 并且重新 提交transfer任务，这样就反复循环。		
		libusb_fill_iso_transfer(xfr[i],p_as_dev->dev_handle,0x81,buf[i],PACKET_SIZE*NUM_PACKETS,NUM_PACKETS,cb_xfr,NULL,0);		
		libusb_set_iso_packet_lengths(xfr[i], PACKET_SIZE); 		
		//正式提交任务		
		ret = libusb_submit_transfer(xfr[i]);		
		if(ret ==0)		
		{			
			printf("canok:: transfer submint ok ! start capture[%d%s]\n",__LINE__,__FUNCTION__);		
		}		
		else		
		{			
			printf("canok:: transfer submint erro %d %s [%d%s]\n",ret,libusb_error_name(ret),__LINE__,__FUNCTION__);		
		}	
	}

}