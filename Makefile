

PRE=

CC  = $(PRE)gcc
LD  = $(PRE)ld
CPP = $(PRE)cpp
CXX = $(PRE)g++
STRIP = $(PRE)strip


WORK_PATH =$(shell pwd)
#DFU_SRC_FILES := $(patsubst %.c, %,$(wildcard ${WORK_PATH}/dfu-util/src/*.c))
#LIB_USB_SRC_FILES := $(patsubst %.c, %,$(wildcard ${WORK_PATH}/libusb/libusb/*.c))
#LIB_USB_SRC_FILES += $(patsubst %.c, %,$(wildcard ${WORK_PATH}/libusb/libusb/os/*.c))


#LIB_USB_SRC_FILES 			  := $(wildcard ${WORK_PATH}/libusb/libusb/*.c)
#LIB_USB_SRC_FILES 			  += $(wildcard ${WORK_PATH}/libusb/libusb/os/*.c)
#LIB_USB_SRC_FILES			  += ${WORK_PATH}/libusb/libusb/os/linux_netlink.c
#LIB_USB_SRC_FILES			  += ${WORK_PATH}/libusb/libusb/os/linux_udev.c
#LIB_USB_SRC_FILES			  += ${WORK_PATH}/libusb/libusb/os/linux_usbfs.c
#LIB_USB_SRC_FILES			  += ${WORK_PATH}/libusb/libusb/os/thread_posix.c

#LIB_USB_OBJ 				  := $(patsubst %.c,%.o, $(LIB_USB_SRC_FILES))




DFU_SRC_FILES                 := ${WORK_PATH}/hidapi.c
DFU_SRC_FILES                 += ${WORK_PATH}/taihang_hid.c
DFU_SRC_FILES                 += ${WORK_PATH}/th_protocol.c
DFU_SRC_FILES                 += ${WORK_PATH}/taihang_uac.c
DFU_SRC_FILES                 += ${WORK_PATH}/taihang_download.c

DFU_SRC_FILES                 += ${WORK_PATH}/main.c

DFU_OBJ 					  := $(patsubst %.c, %.o, ${DFU_SRC_FILES})
#DFU_OBJ 					  := $(patsubst %.c, %.o, )



EXECUTABLE  				  = taihang_hid 
#SLIB						  = libusb_arm.a
#SLIB						  = libusb_fpic.a
SLIB						  = libusb-1.0.a
#SLIB						  = libusb_nvida.a
DLIB 						  = libtaihang_hid.so

#LOCAL_MODULE 				  := libusb
#LOCAL_SRC_FILES				  += ${WORK_PATH}/dfu-util/src/libusb1.0.a
#LOCAL_STATIC_LIBRARIES        += libusb

INCLUDE_FLAGS  				  += -I$(WORK_PATH)/

#used for nvdia
LOCAL_CFLAGS 				  := -Wall -g -O0 -DHAVE_CONFIG_H  $(INCLUDE_FLAGS)

#LOCAL_CFLAGS 				  := -Wall -g -O0 -DHAVE_CONFIG_H  $(INCLUDE_FLAGS) 
LOCAL_LDFLAGS 				  += -pie -fPIE -pthread  -fPIC -ludev  


.PHONY: clean all help

all: $(EXECUTABLE)
#$(DLIB) 
%.o : %.c
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS)  -fPIC -c $< -o $@

$(EXECUTABLE) : $(DFU_OBJ) $(SLIB)
#$(EXECUTABLE) : $(DFU_OBJ)
	echo $(DFU_OBJ) $(SLIB) 
	$(CXX) $^  $(LOCAL_LDFLAGS) $(LOCAL_CFLAGS)  -fPIC -pthread -o $@
	
dtest: ${WORK_PATH}/main.o $(DLIB)
#	$(CXX) -Wl,-rpath=/data/ -L. -ltaihang_hid spiaudiotest.o $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@
	$(CXX) $^ $(LOCAL_CFLAGS) -Wl,-rpath=./ -L. -ltaihang_hid  $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@


#stest: spiaudiotest.o $(SLIB)
#	$(CXX) -o $@ $^ $(SLIB)	


$(DLIB): $(DFU_OBJ) $(SLIB)
	$(CXX) $(LOCAL_CFLAGS) -fPIC -shared $^ -o $@ 


#$(SLIB):$(LIB_USB_OBJ)
#	$(AR) -r $@ $^ 	

clean:
	rm -rf $(EXECUTABLE) $(DFU_OBJ) $(LIB_USB_OBJ) dtest stest

help:
	echo $(LIB_USB_SRC_FILES)
