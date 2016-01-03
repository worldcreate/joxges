CC = g++
CFLAGS = -O2 
LDFLAGS =
INCLUDES = 
LIBS =
DEBUG =
TARGET = main
SRC=$(wildcard *.cpp) 
OBJS = $(SRC:.cpp=.o)
.SUFFIXES:	.cpp
# 生成規則 
all: $(TARGET)

debug:
	make $(TARGET) DEBUG='-g -DDEBUG' CFLAGS='-O0'

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(DEBUG) -o $@ $(OBJS) $(LIBS) 

run:
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS) .nfs* *~ *.core *.stackdump \#* core 
.cpp.o:
	$(CC) $(CFLAGS) $(DEBUG) $(INCLUDES) -c $< 
