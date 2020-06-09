#include "libusb.h"
#include "taihang_download.h"
#include "taihang_vdcmd.h"
#include "th_protocol.h"
#include "taihang_hid.h"
#include "stdio.h"

extern as_hid_dev_p p_as_dev;

uint8_t cmd_buf[VDCMD_DATA_LEN_MAX + VDCMD_HEADER_CRC_LEN];
vdcmd_t vdcmd_header={0};
unsigned short const crc_ccitt_table[256] ={ 0x0000, 0x1189, 0x2312, 0x329b, 0x4624,
		0x57ad, 0x6536, 0x74bf, 0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5,
		0xe97e, 0xf8f7, 0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7,
		0x643e, 0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
		0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd, 0xad4a,
		0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5, 0x3183, 0x200a,
		0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c, 0xbdcb, 0xac42, 0x9ed9,
		0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974, 0x4204, 0x538d, 0x6116, 0x709f,
		0x0420, 0x15a9, 0x2732, 0x36bb, 0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868,
		0x99e1, 0xab7a, 0xbaf3, 0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528,
		0x37b3, 0x263a, 0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb,
		0xaa72, 0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
		0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1, 0x7387,
		0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738, 0xffcf, 0xee46,
		0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70, 0x8408, 0x9581, 0xa71a,
		0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7, 0x0840, 0x19c9, 0x2b52, 0x3adb,
		0x4e64, 0x5fed, 0x6d76, 0x7cff, 0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad,
		0xc324, 0xf1bf, 0xe036, 0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c,
		0x7df7, 0x6c7e, 0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c,
		0xd1b5, 0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
		0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134, 0x39c3,
		0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c, 0xc60c, 0xd785,
		0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3, 0x4a44, 0x5bcd, 0x6956,
		0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb, 0xd68d, 0xc704, 0xf59f, 0xe416,
		0x90a9, 0x8120, 0xb3bb, 0xa232, 0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1,
		0x0d68, 0x3ff3, 0x2e7a, 0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3,
		0x8238, 0x93b1, 0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70,
		0x1ff9, 0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
		0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78 };

static inline uint16_t crc_ccitt_byte(uint16_t crc, const uint8_t c)
{
	return (crc >> 8) ^ crc_ccitt_table[(crc ^ c) & 0xff];
}

uint16_t crc16_ccitt_sw(uint16_t crc, uint8_t *buffer, uint16_t len)
{
	uint16_t uiCnt = 0;
	while (uiCnt < len)
	{
		crc = crc_ccitt_byte(crc, *(buffer + uiCnt));
		++uiCnt;
	}
	return crc;
}

int usb_get_data(uint16_t wLen, uint8_t data[])
{
	usb_ctl_info_t tUsbCtlTransferInfo = { 0 };
    //uint8_t data1[100];
	int usb_transfer_result = LIBUSB_ERROR_OTHER;
	tUsbCtlTransferInfo.bmRequestType = VD_REQUEST_TYPE_GET;
	tUsbCtlTransferInfo.bRequest = USB_VDREQUEST_I2C_R;
	tUsbCtlTransferInfo.wLength = wLen;

	if (p_as_dev->dev_handle == NULL)
	{
		printf("ERR:no usb device opened\n");
		return FAIL;
	}

	usb_transfer_result = libusb_control_transfer(p_as_dev->dev_handle, tUsbCtlTransferInfo.bmRequestType, tUsbCtlTransferInfo.bRequest, \
			tUsbCtlTransferInfo.wValue, tUsbCtlTransferInfo.wIndex, data, wLen, 0);//
	if (usb_transfer_result < LIBUSB_SUCCESS)
	{
		printf("ERR:usb transfer error:%d\n", usb_transfer_result);
		return FAIL;
	}

	return SUCCESS;
}

int usb_send_data(uint16_t wLen, uint8_t data[])
{
	usb_ctl_info_t tUsbCtlTransferInfo = {0};
	int usb_transfer_result = LIBUSB_ERROR_OTHER;

	tUsbCtlTransferInfo.bmRequestType = VD_REQUEST_TYPE_SET;
	tUsbCtlTransferInfo.bRequest = USB_VDREQUEST_I2C_W;
	tUsbCtlTransferInfo.wLength = wLen;

	if (p_as_dev->dev_handle == NULL)
	{
		printf("ERR:no usb device opened\n");
		return FAIL;
	}

	usb_transfer_result = libusb_control_transfer(p_as_dev->dev_handle, tUsbCtlTransferInfo.bmRequestType, tUsbCtlTransferInfo.bRequest, \
		tUsbCtlTransferInfo.wValue, tUsbCtlTransferInfo.wIndex, data, tUsbCtlTransferInfo.wLength, 0);//

	if (usb_transfer_result < LIBUSB_SUCCESS)  
	{
		printf("ERR:usb transfer error:%d\n", usb_transfer_result);
		return FAIL;
	}

	return SUCCESS;
}

int vdcmd_write(uint16_t wLen)
{
	uint8_t ret = FAIL;
    uint16_t crc;
    int total_len  = wLen + VDCMD_HEADER_CRC_LEN;
    cmd_buf[0] = VDCMD_SOF;
    cmd_buf[1] = VDCMD_SOF >> 8;
    cmd_buf[2] = VDCMD_SOF >> 16;
    cmd_buf[3] = VDCMD_SOF >> 24;
    cmd_buf[4] = vdcmd_header.cmd_type;
    cmd_buf[5] = vdcmd_header.sub_cmd;
    cmd_buf[6] = vdcmd_header.param;
    cmd_buf[7] = vdcmd_header.addr_l_ll;
    cmd_buf[8] = vdcmd_header.addr_l_ll>>8;
    cmd_buf[9] = vdcmd_header.addr_h;
    cmd_buf[10] = vdcmd_header.len;
    cmd_buf[11] = vdcmd_header.len>>8;


    crc = crc16_ccitt_sw(0, cmd_buf, (total_len-VDCMD_CRC_LEN));

    cmd_buf[wLen + VDCMD_HEADER_LEN] = crc & 0xff;
    cmd_buf[wLen + VDCMD_HEADER_LEN + 1] = crc >> 8;

	ret = usb_send_data(total_len, cmd_buf);
     if(vdcmd_header.sub_cmd == 0xc2)
    {
   //     return FAIL;
    }   
	if (ret == FAIL)
	{
		printf("ERR:vdcmd cmd write fail\n");
		return FAIL;
	}

	return SUCCESS;
}

int spi_write(uint32_t addr,int len,uint8_t buf[])
{
    int offset = 0;
    int temp_len,ret;
   // vdcmd_t vdcmd_header={0};
    unsigned short crc;

    vdcmd_header.cmd_type = 1;
    vdcmd_header.sub_cmd = 0xc2;
    while(len>0)
    {
        if(len > VDCMD_DATA_BUFF_LEN)
        {
            temp_len = VDCMD_DATA_BUFF_LEN;
        }   
        else
        {
            temp_len = len;
        }
        vdcmd_header.param = addr>>24;
        vdcmd_header.addr_l_ll = addr&0xFFFF;
        vdcmd_header.addr_h = (addr&0xFFFFFF)>>16;
        vdcmd_header.len = temp_len;
        memcpy(cmd_buf + VDCMD_HEADER_LEN,buf + offset, temp_len);
        ret = vdcmd_write(temp_len);
        if(ret!=SUCCESS)
        {
            printf("spi_write error\r\n");
            return FAIL;
        }    
        offset +=temp_len;
        addr +=temp_len;
        len -=temp_len;
    }
    return SUCCESS;
}

int write_data_to_flash(uint32_t addr,int len,uint8_t buf[])
{
    int write_len,ret;
    printf("write_data_to_flash\r\n");
    while(len > 0)
    {
        if (len > SECTOR_LEN)
        {
            write_len = SECTOR_LEN;
        }
        else
        {
            write_len = len;
        }
     //   printf("…");
        ret = spi_write(addr, write_len, buf);
        if(ret != SUCCESS)
        {
            printf("write_data_to_flash fail\r\n");
            return FAIL;
        }   
        addr +=write_len;
        len -=write_len;
        buf +=write_len;       
    }
    printf("\r\n");
    return SUCCESS;

}

int switch_download_mode(int mode)
{
    int ret;
    unsigned short crc;

    vdcmd_header.cmd_type = 2;
    vdcmd_header.sub_cmd = 0x03;
    vdcmd_header.param = mode;
    //vdcmd_header.sof = VDCMD_SOF;

   // memcpy(cmd_buf,&vdcmd_header,sizeof(vdcmd_t));
   // crc = crc16_ccitt_sw(0, cmd_buf, VDCMD_HEADER_LEN);
   // cmd_buf[VDCMD_HEADER_LEN] = crc & 0xff;
   // md_buf[VDCMD_HEADER_LEN + 1] = crc >> 8;
    ret = vdcmd_write(0);
    if(ret!=SUCCESS)
    {
        printf("spi_write error\r\n");
        return FAIL;
    }
    return SUCCESS;

}

int spi_erase(uint32_t addr, int sector_num)
{
    int ret;
    //vdcmd_t vdcmd_header={0};
    vdcmd_header.cmd_type = 1;
    vdcmd_header.sub_cmd = 0x4;
    vdcmd_header.param = addr>>24;
    vdcmd_header.len = sector_num;
    vdcmd_header.addr_l_ll = addr;
    vdcmd_header.addr_h = addr>>16;
    ret = vdcmd_write(0);
    if(ret!=SUCCESS)
    {
        printf("spi_write error\r\n");
        return FAIL;
    }

}

int erase_flash(uint32_t addr, int len)
{
    int erase_sector,sector_num,ret;
    printf("erase flash\r\n");
    if ((SECTOR_LEN - addr & 0xFFF) == 0)
        erase_sector = (len + SECTOR_LEN-1)/SECTOR_LEN;
    else if( len > (SECTOR_LEN - addr & 0xFFF))
        erase_sector = (len + SECTOR_LEN-1)/SECTOR_LEN+1;
    else
        erase_sector = 1;
    while (erase_sector > 0)
    {
        if(erase_sector >= 32)
            sector_num = 32;
        else
            sector_num = erase_sector;
        ret = spi_erase(addr, sector_num);
        if(ret!=SUCCESS)
        {
            printf("erase_flash fail\r\n");
        }   
        addr += sector_num*SECTOR_LEN;
        erase_sector -= sector_num;
    }  
    return SUCCESS; 
} 

int erase_result_check(uint8_t* addr, int len)
{
    return SUCCESS;
}

int vdcmd_read(int rd_len,uint8_t* pdata)
{
	uint8_t ret = FAIL;
    uint16_t crc,crc_cal;
    int total_len  = rd_len + VDCMD_HEADER_CRC_LEN;
    uint8_t rd_data[VDCMD_DATA_BUFF_LEN+VDCMD_HEADER_CRC_LEN];
    cmd_buf[0] = VDCMD_SOF;
    cmd_buf[1] = VDCMD_SOF >> 8;
    cmd_buf[2] = VDCMD_SOF >> 16;
    cmd_buf[3] = VDCMD_SOF >> 24;
    cmd_buf[4] = vdcmd_header.cmd_type;
    cmd_buf[5] = vdcmd_header.sub_cmd;
    cmd_buf[6] = vdcmd_header.param;
    cmd_buf[7] = vdcmd_header.addr_l_ll;
    cmd_buf[8] = vdcmd_header.addr_l_ll>>8;
    cmd_buf[9] = vdcmd_header.addr_h;
    cmd_buf[10] = vdcmd_header.len;
    cmd_buf[11] = vdcmd_header.len>>8;
    crc = crc16_ccitt_sw(0, cmd_buf, VDCMD_HEADER_LEN);
    cmd_buf[VDCMD_HEADER_LEN] = crc & 0xff;
    cmd_buf[VDCMD_HEADER_LEN + 1] = crc >> 8;

    ret = usb_send_data(VDCMD_HEADER_CRC_LEN, cmd_buf);
    if (ret == FAIL)
    {
        printf("vcmd_read fail\n");
        return FAIL;
    }

    //read data from usb
    ret = usb_get_data(total_len, rd_data);


    if (ret == FAIL)
    {
        printf("vcmd_read fail\n");
        return FAIL;
    }
    //check reservi data CRC
    crc = rd_data[total_len-2] + (rd_data[total_len-1]<<8);
    crc_cal = crc16_ccitt_sw(0, rd_data, (total_len-VDCMD_CRC_LEN));
    if(crc != crc_cal)
    {
        printf("vdcmd read crc check fail crc:%x crc_cal:%x \n",crc,crc_cal);
        return FAIL;        
    }  

  
    memcpy(pdata,rd_data+VDCMD_HEADER_LEN,rd_len);

    return SUCCESS;
}

int spi_read(uint32_t addr,int len,uint8_t buf[])
{
    int temp_len,ret;
    int offset = 0;
    uint8_t temp_data[VDCMD_DATA_BUFF_LEN];
    vdcmd_header.cmd_type = 1;
    vdcmd_header.sub_cmd = 0x82;
    while(len>0) 
    {
        if(len > VDCMD_DATA_BUFF_LEN)
            temp_len = VDCMD_DATA_BUFF_LEN;
        else
            temp_len = len;
        vdcmd_header.param = addr>>24;
        vdcmd_header.addr_l_ll = addr&0xFFFF;
        vdcmd_header.addr_h = (addr&0xFFFFFF)>>16;
        vdcmd_header.len = temp_len;
        ret = vdcmd_read(temp_len, temp_data);
        if(ret!=SUCCESS)
        {
            printf("spi_read fail\r\n");
            return FAIL;
        } 
        memcpy(buf+offset,temp_data,temp_len); 
        offset += temp_len;
        addr += temp_len;
        len -= temp_len;
    }
    return SUCCESS;
}

int read_and_compara(uint32_t addr, int len, uint8_t image_file[])
{
    int offset = 0;
    int step_len = len; // STEP_NUM
    int read_len,ret;
    uint8_t *read_data_list;
    read_data_list = (uint8_t*)malloc(SECTOR_LEN);
    printf("read and compare\r\n");
    while(len > 0)
    {
      //  printf("…");
        if (len > SECTOR_LEN)
            read_len = SECTOR_LEN;
        else
        {
           read_len = len;
        }
        ret = spi_read(addr, read_len, read_data_list);
        if(ret != SUCCESS)
        {
           printf("read_and_compara fail,line%d\r\n",__LINE__);
           return FAIL;  
        }   
        
        for(int i=0;i<read_len;i++)
        {
            if(read_data_list[i]!=image_file[i+offset])
            {
                 printf("compare data fail %d %02x,%02x\r\n",i+offset,read_data_list[i],image_file[i+offset]);
                 return FAIL;
            }   
        }
        offset += read_len;
        addr += read_len;
        len -= read_len;
    }
        free(read_data_list);
        return SUCCESS;

}

int reset_device()
{
    int ret;
    unsigned short crc;

    vdcmd_header.cmd_type = 2;
    vdcmd_header.sub_cmd = 0x02;
    //vdcmd_header.sof = VDCMD_SOF;

   // memcpy(cmd_buf,&vdcmd_header,sizeof(vdcmd_t));
   // crc = crc16_ccitt_sw(0, cmd_buf, VDCMD_HEADER_LEN);
   // cmd_buf[VDCMD_HEADER_LEN] = crc & 0xff;
   // md_buf[VDCMD_HEADER_LEN + 1] = crc >> 8;
    ret = vdcmd_write(0);
    if(ret!=SUCCESS)
    {
        printf("spi_write error\r\n");
        return FAIL;
    }
    return SUCCESS;

}

int dev_info_get(uint8_t * version)
{
    int ret;
    unsigned short crc;

    vdcmd_header.cmd_type = 2;
    vdcmd_header.sub_cmd = 0x81;
    vdcmd_header.len = 1;
    //vdcmd_header.sof = VDCMD_SOF;

   // memcpy(cmd_buf,&vdcmd_header,sizeof(vdcmd_t));
   // crc = crc16_ccitt_sw(0, cmd_buf, VDCMD_HEADER_LEN);
   // cmd_buf[VDCMD_HEADER_LEN] = crc & 0xff;
   // md_buf[VDCMD_HEADER_LEN + 1] = crc >> 8;
    ret = vdcmd_read(1,version);
   // printf("version:%02x\r\n",*version);
    if(ret!=SUCCESS)
    {
        printf("spi_write error\r\n");
        return FAIL;
    }
    return SUCCESS;

}

int taihang_download(uint32_t addr, int len, uint8_t image_file[])
{
    int ret;
    ret = switch_download_mode(DOWNLOAD_START);
    if(ret!=SUCCESS)
    {
         printf("taihang_download fail line:%d\r\n",__LINE__);
         return FAIL;
    }

    // erase and check erase area
    ret = erase_flash(addr, len); 
    if(ret!=SUCCESS)
    {
         printf("taihang_download fail line:%d\r\n",__LINE__);
         return FAIL;
    } 
  //  ret = erase_result_check(addr, len);
    if(ret!=SUCCESS)
    {
        printf("taihang_download fail line:%d\r\n",__LINE__);
        return FAIL;
    } 
  
    ret = write_data_to_flash(addr, len, image_file);
    if(ret!=SUCCESS)
    {
         printf("taihang_download fail line:%d\r\n",__LINE__);
         return FAIL;
    }
    else
    {
        printf("write data to flash success \r\n");
    }
    ret = read_and_compara(addr, len, image_file);
    if(ret!=SUCCESS)
    {
         printf("taihang_download fail line:%d\r\n",__LINE__);
         return FAIL;
    }
    switch_download_mode(DOWNLOAD_END);   
    printf("download success\r\n"); 
    return SUCCESS;

}

int work_mode_set(int mode)
{
    int ret;
    unsigned short crc;

    vdcmd_header.cmd_type = 2;
    vdcmd_header.sub_cmd = 0x08;
    vdcmd_header.param = mode;
    //vdcmd_header.sof = VDCMD_SOF;

   // memcpy(cmd_buf,&vdcmd_header,sizeof(vdcmd_t));
   // crc = crc16_ccitt_sw(0, cmd_buf, VDCMD_HEADER_LEN);
   // cmd_buf[VDCMD_HEADER_LEN] = crc & 0xff;
   // md_buf[VDCMD_HEADER_LEN + 1] = crc >> 8;
    ret = vdcmd_write(0);
    if(ret!=SUCCESS)
    {
        printf("spi_write error\r\n");
        return FAIL;
    }
    return SUCCESS;

}
   #if 0 
    def download_progress(self, addr, length, file_path):
        self.msg_log.emit('')
        self.work_status.emit('Busy')
        start_time = time.clock()
        file = open(file_path, "rb") 
        file_data = file.read(length)
        file.close()
        # 1)if in uart download mode.switch uart baud rate
        if self.vdcmd_handle.port_type == UART_DOWNLOAD:
            self._uart_baud_rate_get()
            self._uart_baud_rate_set(DOWNLOAD_START)
        # 2)switch spi mode to download mode
        if self._switch_download_mode(DOWNLOAD_START) == FAIL:
            return   
        # 3)check flash id
        if self.__check_flash_id(length)== FAIL:
            return  
        # 4)erase and check erase area
        if self._erase(addr, length) == FAIL:
            return            
        if self.__erase_result_check(addr, length) == FAIL:
            return
        # 5) write fw
        write_data = (c_ubyte*length)()
        if self.__write_data_to_flash(addr, length, file_data, write_data) == FAIL:
            return 
        # 6) read and compare
        if self.__read_and_compara(addr, length, write_data) == FAIL:
            return 
        # 7)switch spi mode to orign mode
        if self._switch_download_mode(DOWNLOAD_END) == FAIL:
            return 
        self.progress_bar.emit(5)
        # 8)if download fw, reset to new fw;if download config, switch uart baud rate to orign
        self._uart_baud_rate_set(DOWNLOAD_END) 
        if self.reset is True:
            self.msg_log.emit('Reset to new Firmware') 
            self.vdcmd_handle.reset_to_new_fw()
        end_time = time.clock()
        delate_time = int((end_time - start_time)*1000)            
        self.success_result.emit('Download succeed , use : ' + '%d' % delate_time+'ms')
        return 
        #endif 
