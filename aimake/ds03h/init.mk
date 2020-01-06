#
# DS03H linux configuration
#

#export SDKTARGETSYSROOT=/opt/ds03h/sv-ds03h-system-sdk/sysroots/cortexa7hf-neon-vfpv4-poky-linux-gnueabi
#export PKG_CONFIG_PATH=$SDKTARGETSYSROOT/usr/lib/pkgconfig
#export CONFIG_SITE=/opt/ds03h/sv-ds03h-system-sdk/site-config-cortexa7hf-neon-vfpv4-poky-linux-gnueabi
#export OECORE_NATIVE_SYSROOT="/opt/ds03h/sv-ds03h-system-sdk/sysroots/x86_64-oesdk-linux"
#export OECORE_TARGET_SYSROOT="$SDKTARGETSYSROOT"
#export OECORE_ACLOCAL_OPTS="-I /opt/ds03h/sv-ds03h-system-sdk/sysroots/x86_64-oesdk-linux/usr/share/aclocal"


DS03H_LINUX_NDK_HOME = /work/sdk/sv-ds03h-system-sdk/sysroots/x86_64-oesdk-linux/usr/bin/arm-poky-linux-gnueabi
DS03H_LINUX_SYSROOT = /work/sdk/sv-ds03h-system-sdk/sysroots/cortexa7hf-neon-vfpv4-poky-linux-gnueabi

CC  = $(DS03H_LINUX_NDK_HOME)/arm-poky-linux-gnueabi-gcc
LD  = $(DS03H_LINUX_NDK_HOME)/arm-poky-linux-gnueabi-ld
CPP = $(DS03H_LINUX_NDK_HOME)/arm-poky-linux-gnueabi-gcc -E
CXX = $(DS03H_LINUX_NDK_HOME)/arm-poky-linux-gnueabi-g++
AR  = $(DS03H_LINUX_NDK_HOME)/arm-poky-linux-gnueabi-ar
AS  = $(DS03H_LINUX_NDK_HOME)/arm-poky-linux-gnueabi-as
NM  = $(DS03H_LINUX_NDK_HOME)/arm-poky-linux-gnueabi-nm
STRIP = $(DS03H_LINUX_NDK_HOME)/arm-poky-linux-gnueabi-strip


CFLAGS := -fsigned-char -march=armv7ve -mfpu=neon-vfpv4  -mfloat-abi=hard -mcpu=cortex-a7 -I $(DS03H_LINUX_SYSROOT)/usr/include --sysroot=$(DS03H_LINUX_SYSROOT)
CXXFLAGS := $(CFLAGS)# -I $(CXX_STL)

LDFLAGS := -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed -L$(DS03H_LINUX_SYSROOT)/lib -lpthread -ldl -lrt --sysroot=$(DS03H_LINUX_SYSROOT)

