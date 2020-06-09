#include "taihang_hid.h"
#include <signal.h>
#include "stdio.h"
#include<time.h>
#include<unistd.h>
FILE *file;
FILE* fout_uac;
FILE* image_file;
char *image_buff;
char *pAudio_buffer;
int flag=1;
as_hid_dev_p p_as_dev;

void exit_all_process(void)
{
	flag=0;
	//sleep(1);
	//if (p_as_dev)
	//	kl_device_close(p_as_dev);
	free(image_buff);
	free(pAudio_buffer);
	printf("\noops, exit\n");
	//exit(0);
}

#include "audio.h"

int main(int argc, char *argv[])
{
	char buffer[64]={0};
	char words[64] = {0};
	char majorstatus;
	char doa;
	char nwakeup;
	char wakeup_status;
	char beamflag;
	int size,image_len;
	int period_size=1024; //fixed size
	int ret;
	int i,cnt=0;
	uint8_t version = 0;
	kl_audio_info_t audio_info;
	clock_t start, end;
	//init audio buffer
	pAudio_buffer=(char *)malloc(period_size);
	memset(&audio_info,sizeof(kl_audio_info_t),0);
    
	signal(SIGINT, exit_all_process); 
	file = fopen("./hidpcm.pcm", "wb");
	image_file = fopen("./taihang.fw","r");
	fseek(image_file, 0L, SEEK_END);
	image_len =ftell(image_file);
	fseek(image_file, 0, SEEK_SET);
    printf("the image len is %d\r\n",image_len);
	image_buff = (char *)malloc(image_len);
	fread(image_buff,1,image_len,image_file);
	fclose(image_file);
	p_as_dev=kl_device_open();
	if (!p_as_dev)
	{
		printf("open hid device failed\n");
		goto err;
	};

	ret = work_mode_set(0);
	return 0;
/*下载版本*/
	ret = taihang_download(0,image_len,image_buff);
	return 0;
/*重启设备*/		
//	ret = reset_device();
//	if(ret != LIBUSB_SUCCESS)		
//	{			
//		printf("can not reset device\r\n");				
//	}


#if 0
//	kl_start_play(p_as_dev);

/*uac 录音*/	
//	kl_uac_open(p_as_dev);
    ret  = libusb_set_option(NULL,LIBUSB_OPTION_LOG_LEVEL);

	while(1)        
    {           
		cnt++;
		if(cnt==10000)
			cnt = 0;
		start = clock(); 

		ret = kl_get_alg_info(p_as_dev,&majorstatus,&doa,&nwakeup,&wakeup_status,&beamflag);
		{
			printf("Doa:%d\n",doa);

			end = clock();
			double seconds  =(double)(end - start)/CLOCKS_PER_SEC;
			printf("Use time11 is: %.8f\n", seconds);			

			printf("MajorStatus:%d \n",majorstatus);
			printf("Doa:%d\n",doa);
			printf("nwakeup:%d\n",nwakeup);
			printf("wakeup_status:%d\n",wakeup_status);
			printf("beamflag:%d\n",beamflag);

		}

		start = clock(); 

		//if(cnt%100==0)
		{
			ret = kl_get_wakeup_words(p_as_dev,words);
			if(0==ret)
			{
				end = clock();
				double seconds  =(double)(end - start)/CLOCKS_PER_SEC;
				printf("Use time222 is: %.8f\n", seconds);
				printf("words:%s\n",words);
			}
		}

		start = clock();

		ret = libusb_handle_events(NULL);      
		if(ret == LIBUSB_SUCCESS)               
		{ 
			end = clock();
			double seconds  =(double)(end - start)/CLOCKS_PER_SEC;
			printf("Use time333 is:\n");
		}         
		if(ret != LIBUSB_SUCCESS)               
		{                 
				printf("can:: handle event erro ,exit! [%d%s]\n",__LINE__,__FUNCTION__);                        
				break;          
		}   
 
    }


	while (flag==1)
	{
	
		kl_audio_play(p_as_dev,__1_wav,__1_wav_len);
	}
	ret  = libusb_set_option(NULL,LIBUSB_OPTION_LOG_LEVEL);
	kl_stop_play(p_as_dev);



	kl_start_record(p_as_dev);
	while(flag==1 && !kl_audio_read(p_as_dev,pAudio_buffer,&audio_info))
	{
		//kl_audio_play(p_as_dev,xxx_00001_wav,xxx_00001_wav_len);
		//kl_audio_play(p_as_dev,__1_wav,__1_wav_len);
		ret = libusb_handle_events(NULL);		
		if(ret != LIBUSB_SUCCESS)		
		{			
			printf("can:: handle event erro ,exit! [%d%s]\n",__LINE__,__FUNCTION__);			
			break;		
		}	

	    if (fwrite(pAudio_buffer, 1, period_size, file) != period_size) 
	    {
            fprintf(stderr,"write file failed\n");
            fclose(file);
            goto out;    
        }
        printf("wakeup words:%s\n", audio_info.wakeup_info);
        printf("MajorStatus:%d\n", audio_info.major_status);
        printf("Doa:%d\n", audio_info.doa);
        printf("Vad:%d\n", audio_info.vad);
        printf("nwakeup:%d\n",audio_info.nwakeup);
        printf("WakeupStatus:%d\n",audio_info.wakeup_status);
	}
	kl_stop_record(p_as_dev);
	kl_uac_close(p_as_dev);
//	kl_stop_play(p_as_dev);
	kl_uac_close(p_as_dev);
	fclose(file);
	fclose(fout_uac);
#endif
	kl_device_close(p_as_dev);

out:
	return 0;

err:
	return -1;
}
