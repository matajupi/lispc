CC=gcc
CFLAGS=-g -Wall
TARGET=lispc
SRCS=main.c
OBJS=$(SRCS:.c=.o)

INCDIR=-I../inc
LIBDIR=
LIBS=

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBDIR) $(LIBS)
	
$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)

all: clean $(OBJS) $(TARGET)

clean:
	-rm -f $(OBJS) $(TARGET) *.d
