

TARGET := picture
CC := arm-linux-gnueabi-gcc

#指定头文件的搜索路径
INC := -I /usr/local/arm/arm_libs/include

#指定依赖库和搜索路径
LIBS += -L . -l pthread 
LIBS += -L /usr/local/arm/arm_libs/lib  -l jpeg 

#所有的C文件的文件名
CSRCS := $(wildcard *.c)
CSRCS += $(wildcard mp3/*.c)

#所有.o文件名
OBJS := $(patsubst %.c, %.o, $(CSRCS))


$(TARGET): $(OBJS)
	$(CC) $(LIBS) $^ -o $@
	cp $(TARGET) /home/sggls/tftpboot/GEC6818/project

%.o:%.c
	$(CC) $(INC)  -c $< -o $@


clean:
	rm $(OBJS)
	rm $(TARGET)