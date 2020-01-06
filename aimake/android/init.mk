#
# android ndk r11b configuration
#

ANDROID_NDK_HOME = /home/sunjinzhou/android-ndk-r11b
CXX_STL = $(ANDROID_NDK_HOME)/sources/cxx-stl/gnu-libstdc++/4.9

#ANDABI = armv6-vfp
ANDABI = armv7a-neon
$(warning build for ANDABI: $(ANDABI))
$(shell sleep 1)

ifeq ($(ANDABI), armv5te)
    CFLAGS :=  -march=armv5te -mthumb -mfloat-abi=soft
else ifeq ($(ANDABI), armv6-vfp)
    CFLAGS := -march=armv6 -marm -mfloat-abi=softfp -mfpu=vfp
else ifeq ($(ANDABI), armv7a-vfpv3)
    CFLAGS := -march=armv7-a -marm -mfloat-abi=softfp -mfpu=vfpv3
else ifeq ($(ANDABI), armv7a-neon)
    CFLAGS := -march=armv7-a -marm -mfloat-abi=softfp -mfpu=neon -ftree-vectorize -D USE_NEON
else ifeq ($(ANDABI), armeabi-v7a)
    CFLAGS := -march=armv7-a -marm -mfloat-abi=softfp -mfpu=neon -ftree-vectorize -D USE_NEON
else ifeq ($(ANDABI), aarch64)
    CFLAGS := -march=armv8-a -D__ANDROID__ -D__ARM_ARCH_5__ -D__ARM_ARCH_7A__ -D__ARM_NEON__ -D__VFP_FP__
else ifeq ($(ANDABI), x86_64)
    CFLAGS :=  -march=atom -mtune=atom -msse3 
else ifeq ($(ANDABI), mips)
else ifeq ($(ANDABI), x86)
    CFLAGS :=  -march=atom -mtune=atom -msse3 
else
    $(error only support ANDABI: armv5te, armv6-vfp, armv7a-vfpv3, armv7a-neon, mips, x86, the default is armv6-vfp)
endif

ifeq (arm, $(findstring arm, $(ANDABI)))
    TOOLCHAINS = $(ANDROID_NDK_HOME)/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64
    PLATFORM = $(ANDROID_NDK_HOME)/platforms/android-14/arch-arm

    CC  = $(TOOLCHAINS)/bin/arm-linux-androideabi-gcc
    LD  = $(TOOLCHAINS)/bin/arm-linux-androideabi-ld
    CPP = $(TOOLCHAINS)/bin/arm-linux-androideabi-cpp
    CXX = $(TOOLCHAINS)/bin/arm-linux-androideabi-g++
    AR  = $(TOOLCHAINS)/bin/arm-linux-androideabi-ar
    AS  = $(TOOLCHAINS)/bin/arm-linux-androideabi-as
    NM  = $(TOOLCHAINS)/bin/arm-linux-androideabi-nm
    STRIP = $(TOOLCHAINS)/bin/arm-linux-androideabi-strip
    CFLAGS += -fsigned-char -I$(PLATFORM)/usr/include -I $(CXX_STL)/include 
    CXXFLAGS := $(CFLAGS) -I $(CXX_STL)/include -I $(CXX_STL)/libs/armeabi/include -frtti
    LDFLAGS := --sysroot=$(PLATFORM) -L$(PLATFORM)/usr/lib -llog -lc $(CXX_STL)/libs/armeabi/libgnustl_static.a
else ifeq (x86_64, $(findstring x86_64, $(ANDABI)))
    TOOLCHAINS = $(ANDROID_NDK_HOME)/toolchains/x86_64-4.9/prebuilt/linux-x86_64
    PLATFORM = $(ANDROID_NDK_HOME)/platforms/android-21/arch-x86_64

    CC  = $(TOOLCHAINS)/bin/x86_64-linux-android-gcc
    LD  = $(TOOLCHAINS)/bin/x86_64-linux-android-ld
    CPP = $(TOOLCHAINS)/bin/x86_64-linux-android-cpp
    CXX = $(TOOLCHAINS)/bin/x86_64-linux-android-g++
    AR  = $(TOOLCHAINS)/bin/x86_64-linux-android-ar
    AS  = $(TOOLCHAINS)/bin/x86_64-linux-android-as
    NM  = $(TOOLCHAINS)/bin/x86_64-linux-android-nm
    STRIP = $(TOOLCHAINS)/bin/x86_64-linux-android-strip
    CFLAGS += -fsigned-char -I$(PLATFORM)/usr/include -I $(CXX_STL)/include 
    CXXFLAGS := $(CFLAGS)  -I $(CXX_STL)/libs/x86/include  -frtti
    LDFLAGS := --sysroot=$(PLATFORM) -L$(PLATFORM)/usr/lib64 -llog -lc $(CXX_STL)/libs/x86_64/libgnustl_static.a
else ifeq (x86, $(findstring x86, $(ANDABI)))
    TOOLCHAINS = $(ANDROID_NDK_HOME)/toolchains/x86-4.9/prebuilt/linux-x86_64
    PLATFORM = $(ANDROID_NDK_HOME)/platforms/android-14/arch-x86

    CC  = $(TOOLCHAINS)/bin/i686-linux-android-gcc
    LD  = $(TOOLCHAINS)/bin/i686-linux-android-ld
    CPP = $(TOOLCHAINS)/bin/i686-linux-android-cpp
    CXX = $(TOOLCHAINS)/bin/i686-linux-android-g++
    AR  = $(TOOLCHAINS)/bin/i686-linux-android-ar
    AS  = $(TOOLCHAINS)/bin/i686-linux-android-as
    NM  = $(TOOLCHAINS)/bin/i686-linux-android-nm
    STRIP = $(TOOLCHAINS)/bin/i686-linux-android-strip
    CFLAGS += -fsigned-char -I$(PLATFORM)/usr/include -I $(CXX_STL)/include 
    CXXFLAGS := $(CFLAGS)  -I $(CXX_STL)/libs/x86/include  -frtti
    LDFLAGS := --sysroot=$(PLATFORM) -L$(PLATFORM)/usr/lib -llog -lc $(CXX_STL)/libs/x86/libgnustl_static.a
else ifeq (aarch64, $(findstring aarch64, $(ANDABI)))
    TOOLCHAINS = $(ANDROID_NDK_HOME)/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64
    PLATFORM = $(ANDROID_NDK_HOME)/platforms/android-21/arch-arm64

    CC  = $(TOOLCHAINS)/bin/aarch64-linux-android-gcc
    LD  = $(TOOLCHAINS)/bin/aarch64-linux-android-ld
    CPP = $(TOOLCHAINS)/bin/aarch64-linux-android-cpp
    CXX = $(TOOLCHAINS)/bin/aarch64-linux-android-g++
    AR  = $(TOOLCHAINS)/bin/aarch64-linux-android-ar
    AS  = $(TOOLCHAINS)/bin/aarch64-linux-android-as
    NM  = $(TOOLCHAINS)/bin/aarch64-linux-android-nm
    STRIP = $(TOOLCHAINS)/bin/aarch64-linux-android-strip
    CFLAGS += -fsigned-char -I$(PLATFORM)/usr/include -I $(CXX_STL)/include 
    CXXFLAGS := $(CFLAGS)  -I $(CXX_STL)/libs/arm64-v8a/include  -frtti
    LDFLAGS := --sysroot=$(PLATFORM) -L$(PLATFORM)/usr/lib -llog -lc $(CXX_STL)/libs/arm64-v8a/libgnustl_static.a
endif
