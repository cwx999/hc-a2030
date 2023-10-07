
#设定生成的目标名称(可执行文件名称)
TARGET := hc-a2030-main

#配置文件
# include mk-rules.mk


#头文件相对路径，以MakeFile所在文件为根目录
#这个变量留给用户去配置，后面会将INC_DIRS转换成绝对路径，因为编译器只能识别到绝对路径，
#之所以填入相对路径，是因为这样对用户来说更加简洁明了。
INC_DIRS := \
main \
hal \
components/protocol \
components/socketcan \
components/socketudp \
components/utils

#源文件相对路径，以MakeFile所在文件为根目录
#同上
SRC_DIRS := \
main \
hal \
components/protocol \
components/socketcan \
components/socketudp \
components/utils

#静态/动态库路径
LIB_DIRS := 

#.o文件输出目录，以MakeFile所在文件为根目录
#同上，以根目录下的output为.o文件输出目录
OUTPUT_COBJS := objects

CFLAGS += -Wall -Os -std=c++11
CLIBS += -lstdc++ -lpthread

#将INC_DIRS、SRC_DIRS、OUTPUT_COBJS 转换为绝对路径
CUR_DIRS = $(shell pwd)
INC_DIRS := $(patsubst %,$(CUR_DIRS)/%,$(INC_DIRS))
SRC_DIRS := $(patsubst %,$(CUR_DIRS)/%,$(SRC_DIRS))
LIB_DIRS := $(patsubst %,$(CUR_DIRS)/%,$(LIB_DIRS))

OUTPUT_COBJS := $(patsubst %,$(CUR_DIRS)/%,$(OUTPUT_COBJS))

#VPATH是Makefile的内置变量，依赖项如果在根目录找不到的话就从VPATH目录中查找.
VPATH := $(SRC_DIRS)

#指定编译器工具名称前缀
# CROSS_COMPILE ?= arm-linux-gnueabihf-
CROSS_COMPILE ?= arm-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AS = $(CROSS_COMPILE)as
NM = $(CROSS_COMPILE)nm
STRIP = strip

#CFLAGS用于C编译器的编译选项。
CFLAGS += $(patsubst %,-I%,$(INC_DIRS))
CFLAGS += $(patsubst %,-L%,$(LIB_DIRS))

#将SRC_DIRS内全部目录下的.c文件展开并返回到CFILES(包含绝对路径)
#SRC_DIRS在mk-rules.mk文件中由用户指定
CFILES := $(foreach dirs,$(SRC_DIRS),$(wildcard $(dirs)/*.cpp) )

#CFILES_NO_DIR包含了所有指定目录下的.c文件(去除掉了绝对路径)
CFILES_NO_DIR := $(notdir $(CFILES))

#COBJS包含了所有.c文件对应的.o文件
COBJS := $(patsubst %.cpp,%.o,$(CFILES_NO_DIR))
COBJS := $(patsubst %,$(OUTPUT_COBJS)/%,$(COBJS))

#声明伪目标
.PHONY : all clean $(TARGET)

#声明可忽略错误的目标。
#在命令行执行 make all ，如果没有这一行，当不存在*.o文件或者TARGET文件的时候
#会导致目标 clean 会出错导致make停止运行。
.IGNORE : clean

all: $(OUTPUT_COBJS) $(TARGET)

#TARGET生成依赖于COBJS中的所有.o
$(TARGET) : $(COBJS)
	$(CC) -o $@ $^ $(CLIBS) $(CFLAGS)

#COBJS内所有.o文件的生成依赖于对应的.c.
#以下是MakeFile的一种循环目标匹配规则：
#每次从COBJS中拿出一项进行 $(OUTPUT_COBJS)/%.o:%.c 这样的字符串匹配。
#$(OUTPUT_COBJS)/%.o为目标，%.c为依赖
$(COBJS) : $(OUTPUT_COBJS)/%.o:%.cpp
	$(CC) -o $@ -c $^ $(CFLAGS)

$(OUTPUT_COBJS) : 
	mkdir -p $@

#4 clean执行流程
clean:
	rm $(OUTPUT_COBJS)/*.o
	rm $(TARGET)

