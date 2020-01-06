#
# SINXI linux configuration
#
SINXI_LINUX_NDK_HOME = /usr/local/aimake/R16/gcc/linux-x86/arm/toolchain-sunxi/toolchain
TOOLCHAINS = $(SINXI_LINUX_NDK_HOME)
PLATFORM = $(SINXI_LINUX_NDK_HOME)
LIBEXEC  = $(SINXI_LINUX_NDK_HOME)/libexec/gcc/arm-openwrt-linux-muslgnueabi/5.2.0
CXX_STL = $(SINXI_LINUX_NDK_HOME)/arm-openwrt-linux-muslgnueabi/include/c++/5.2.0

CC  = $(TOOLCHAINS)/bin/arm-openwrt-linux-gcc
LD  = $(TOOLCHAINS)/bin/arm-openwrt-linux-ld
CPP = $(TOOLCHAINS)/bin/arm-openwrt-linux-cpp
CXX = $(TOOLCHAINS)/bin/arm-openwrt-linux-c++
AR  = $(TOOLCHAINS)/bin/arm-openwrt-linux-ar
AS  = $(TOOLCHAINS)/bin/arm-openwrt-linux-as
NM  = $(TOOLCHAINS)/bin/arm-openwrt-linux-nm
STRIP = $(TOOLCHAINS)/bin/arm-openwrt-linux-strip


CFLAGS := -fsigned-char -I$(PLATFORM)/include

CXXFLAGS := $(CFLAGS) -I $(CXX_STL)

LDFLAGS := -L$(PLATFORM)/lib -L$(LIBEXEC) -lpthread -ldl -lrt --sysroot=$(SINXI_LINUX_NDK_HOME)

