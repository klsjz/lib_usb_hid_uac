

#include "taihang_hid.h"
#include <signal.h>
#include "stdio.h"

FILE *file;
int flag=1;
as_hid_dev_p p_as_dev;

void exit_all_process(void)
{
	flag=0;
	//sleep(1);
	//if (p_as_dev)
	//	kl_device_close(p_as_dev);
	printf("\noops, exit\n");
	//exit(0);
}

#include "audio.h"

int main(int argc, char *argv[])
{
	char buffer[64]={0};
	int size;
	int period_size=1024; //fixed size

	kl_audio_info_t audio_info;
	char *pAudio_buffer;

	//init audio buffer
	pAudio_buffer=(char *)malloc(period_size);
	memset(&audio_info,sizeof(kl_audio_info_t),0);
    
    signal(SIGINT, exit_all_process); 
    file = fopen("./hidpcm.pcm", "wb");
	
	p_as_dev=kl_device_open();

	if (!p_as_dev)
	{
		printf("open hid device failed\n");
		goto err;
	}

	kl_start_play(p_as_dev);

	//while (flag==1)
	{
		kl_audio_play(p_as_dev,__1_wav,__1_wav_len);
	}

	//kl_stop_play(p_as_dev);
#if 1
	kl_start_record(p_as_dev);
	while(flag==1 && !kl_audio_read(p_as_dev,pAudio_buffer,&audio_info))
	{
		//kl_audio_play(p_as_dev,xxx_00001_wav,xxx_00001_wav_len);
		//kl_audio_play(p_as_dev,__1_wav,__1_wav_len);

	    if (fwrite(pAudio_buffer, 1, period_size, file) != period_size) 
	    {
            fprintf(stderr,"write file failed\n");
            fclose(file);
            goto out;    
        }
        printf("wakeup words:%s\n", audio_info.WakeUpInfo);
        printf("MajorStatus:%d\n", audio_info.MajorStatus);
        printf("Doa:%d\n", audio_info.Doa);
        printf("WakeupStatus:%d\n",audio_info.WakeupStatus);
	}
	kl_stop_record(p_as_dev);
	kl_stop_play(p_as_dev);
#endif

	kl_device_close(p_as_dev);

out:
	return 0;

err:
	return -1;
}
