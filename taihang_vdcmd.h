#ifndef TAIHANG_VDCMD_H
#define TAIHANG_VDCMD_H
/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/
/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
#define VDCMD_DATA_BUFF_LEN         1024
#define VDCMD_SOF                   0x785AB496
#define VDCMD_HEADER_LEN            12
#define VDCMD_CRC_LEN               2
#define VDCMD_HEADER_CRC_LEN        (VDCMD_HEADER_LEN + VDCMD_CRC_LEN)

#define VDCMD_DATA_LEN_MAX          1024 
#define SECTOR_LEN                  4096

#define VDCMD_STD_SOF_HH_IDX        0
#define VDCMD_STD_SOF_H_IDX         1
#define VDCMD_STD_SOF_L_IDX         2
#define VDCMD_STD_SOF_LL_IDX        3
#define VDCMD_STD_CMD_TYPE_IDX      4
#define VDCMD_STD_SUB_CMD_IDX       5
#define VDCMD_STD_PARAM_IDX         6    
#define VDCMD_STD_ADDR_H_IDX        7    
#define VDCMD_STD_ADDR_L_IDX        8   
#define VDCMD_STD_ADDR_LL_IDX       9    
#define VDCMD_STD_LEN_H_IDX         10    
#define VDCMD_STD_LEN_L_IDX         11    

#define VDCMD_CRC_LEN               2
#define VDCMD_LEN_MAX               (VDCMD_CRC_LEN+VDCMD_DATA_LEN_MAX+VDCMD_HEADER_LEN)


#define SUCCESS   0
#define FAIL      1

#define DOWNLOAD_START  0
#define DOWNLOAD_END    1

#define USB_VDREQUEST_I2C_R	0x44
#define USB_VDREQUEST_I2C_W	0x45
//////////////////////////////////////
// 1.1 standard vendor cmd 
//////////////////////////////////////

// vendor request info
#define VD_REQUEST_TYPE_SET		(LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_VENDOR|LIBUSB_RECIPIENT_INTERFACE)
#define VD_REQUEST_TYPE_GET		(LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_VENDOR|LIBUSB_RECIPIENT_INTERFACE)
#define USB_CTL_TRANSFER_TIMEROUT  1000 // unit is 1ms

//cmd data transfer declaration
#define VCMD_DATA_VLD_MASK	0x80
#define VCMD_DATA_DIR_MASK	0x40
#define VCMD_DATA_VLD	    0x80
#define VCMD_DATA_INVLD     0x00    // no data transfer stage
#define VCMD_DIR_IN		    0x00	//DATA from device to host
#define VCMD_DIR_OUT 	    0x40	//DATA from host to device


//vendor cmd standard types
#define VCMD_STANDARD_TYPE_SPI           0x01
#define VCMD_STANDARD_TYPE_SYS           0x02

//spi vendor cmd list
#define VCMD_SPI_READ_ID            (0x01|VCMD_DATA_VLD|VCMD_DIR_IN)
#define VCMD_SPI_READ               (0x02|VCMD_DATA_VLD|VCMD_DIR_IN)
#define VCMD_SPI_WRITE              (0x02|VCMD_DATA_VLD|VCMD_DIR_OUT)
#define VCMD_SPI_RDSR               (0x03|VCMD_DATA_VLD|VCMD_DIR_IN)
#define VCMD_SPI_WRSR               (0x03|VCMD_DATA_VLD|VCMD_DIR_OUT)
#define VCMD_SPI_ERASE              (0x04|VCMD_DATA_INVLD)

//sys vendor cmd list
#define VCMD_DEV_INFO_GET           (0x01|VCMD_DATA_VLD|VCMD_DIR_IN)
#define VCMD_RST_TO_NEW_FW          (0x02|VCMD_DATA_INVLD)  
#define VCMD_DOWLOAD_MODE_SWITCH    (0x03|VCMD_DATA_INVLD)  
#define VCMD_UART_PARAM_GET         (0x04|VCMD_DATA_VLD|VCMD_DIR_IN) 
#define VCMD_UART_PARAM_SET         (0x04|VCMD_DATA_VLD|VCMD_DIR_OUT) 
#define VCMD_SN_PARAM_GET           (0x05|VCMD_DATA_VLD|VCMD_DIR_IN) 
#define VCMD_SN_PARAM_SET           (0x05|VCMD_DATA_VLD|VCMD_DIR_OUT) 


//vendor cmd process result status        
#define	VCMD_STS_SUCCESS		    0
#define	VCMD_STS_UNKNOWN_CMD        0x01
#define	VCMD_STS_UNKNOWN_SCMD       0x02
#define	VCMD_STS_ERR_LEN		    0x03
#define	VCMD_STS_ERR_PARA	        0x04
#define	VCMD_STS_ERR_CRC	        0x05
#define	VCMD_STS_HW_ERR             0x06


typedef struct __attribute__((packed))
{
    uint32_t sof;

    uint8_t cmd_type;
    uint8_t sub_cmd;
    uint8_t param;
    uint16_t addr_l_ll;
    uint8_t addr_h;
    uint16_t len;
   
    uint16_t crc_value;

    uint8_t *p_data_buf;
    
}vdcmd_t;

typedef struct
{
	uint8_t		bmRequestType;
	uint8_t		bRequest;
	uint16_t	wValue;
	uint16_t	wIndex;
	uint16_t	wLength;
}usb_ctl_info_t;

#endif