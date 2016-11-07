
#
# Detect operation system and kernel version
#

ifeq "$(findstring QNX, $(shell uname -a))" "QNX"
    OS := GIPCY_QNX
    DEF_TYPES := __QNX__
else
    OS := GIPCY_LINUX
    DEF_TYPES := __LINUX__
endif

ifeq "$(findstring c6x-uclinux-gcc, $(CC))" "c6x-uclinux-gcc"
    TARGET := -march=c674x -D_c6x_
    DEF_TYPES := __LINUX__
endif

ifneq   "$(findstring 2.4.,$(shell uname -a))" ""
    KERNEL_VER := -DGIPCY_2_4_X
endif

#
# Select apropriate IPC methods and type
#
#IPC := __IPC_QNX__
IPC := __IPC_LINUX__

#IPC_TYPE := _SYSTEMV_IPC_
#IPC_TYPE := _POSIX_IPC_
IPC_TYPE := _INSYS_IPC_

