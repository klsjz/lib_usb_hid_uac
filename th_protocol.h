

#include <linux/types.h>
#include <stdint.h>


typedef enum reqtype
{	
	COMMAND=1,
	AUDIO=2
}kl_req_type_t;


typedef enum status
{	
	REQUSET_FLAG=0,
	REQUSET_EXE_PASS=1,
	UNSUPPORTED_REQUEST=2,
	REQUSET_EXE_FAIL=3,
}kl_status_t;

typedef enum command_type
{	
	GET_PROJECT_NAME=0x8100,
	SET_RECORD_OPEN=0x0200,
	SET_RECORD_CLOSE=0x0201,
	GET_RECORD_STATUS=0x8202,
	GET_FW_VERSION=0x8300,
	GET_ALG_INFO=0x8600,
	GET_WAKEUP_WORD=0x8700,
	GET_SERIAL_NUMBER=0x8400,
	SET_PLAYER_OPEN=0x0500,
	SET_PLAYER_CLOSE=0x0501,

}kl_command_type_t;


typedef enum kl_workmode{
    KL_IDLE_MODE=0x01,
    KL_FULLSPEED_MODE=0x02,
    KL_ONEWAY_MODE=0x03,
    KL_BYPASS_MODE=0x04,
    KL_DEBUG_MODE =0x05,
}kl_work_mode_t;


typedef struct keywords
{
    char *pKey_word;
    uint32_t words_length;
}keywords_t;

typedef struct kl_wake_up_info{
    uint16_t doa;
    uint8_t is_main_keyword;
    keywords_t keywords;
    uint8_t is_wakeup_now;
    uint8_t is_doa_update;
}kl_wake_up_info_t;


