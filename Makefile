CROSS_COMPILE 	= arm-linux-
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm

STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

CFLAGS = -Wall  -g -fPIC  
CFLAGS += -I $(shell pwd)/include
# CFLAGS += -finput-charset=GBK -fexec-charset=UTF-8
LDFLAGS = "-lm"
TOPDIR := $(shell pwd)
export TOPDIR

CFLAGS += -I ${TOPDIR}

export CFLAGS LDFLAGS
D_OUT = libes_common.so
S_OUT = libes_common.a


obj-y += src/

all : 
	make -C ./ -f $(TOPDIR)/Makefile.build
	${CC}  $(LDFLAGS)  -shared  built-in.o  -o ${D_OUT}
	${AR} -crv ${S_OUT} built-in.o

test_case: all
	make clean -C $(TOPDIR)/test_case/ 
	make all -C $(TOPDIR)/test_case/ 

clean:
	rm -f $(shell find -name "*.o")
	rm -f $(D_OUT)
	rm -f $(S_OUT)
	make clean -C $(TOPDIR)/test_case/

distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(D_OUT)
	rm -f $(S_OUT)
	make clean -C $(TOPDIR)/test_case/
	
