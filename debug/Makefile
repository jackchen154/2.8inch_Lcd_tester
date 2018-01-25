#自定义头文件路径
DIR_INC = ./

#生成的目标文件存放的路径
DIR_BIN = ./

#自己的动态库的路径
DIR_LIB = ./

#生成目标的文件名称
TARGET = test

BIN_TARGET = $(DIR_BIN)/$(TARGET)

CROSS_COMPILE = 

CFLAGS =  -Wall -I$(DIR_INC)

#可根据实际需要修改，选择所需平台SDK，默认为x64或x86平台。
LDFLAGS := -L$(DIR_LIB)

#生成目标文件所需要的标准库
LDFLAGS += -lpthread 

OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))

$(BIN_TARGET) : $(OBJECTS)

	$(CROSS_COMPILE)gcc $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o : %.c
	$(CROSS_COMPILE)gcc -c $(CFLAGS) $< -o $@

clean:
	@rm -f *.o $(BIN_TARGET)

.PHONY:clean

