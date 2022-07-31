CC=gcc
CFLAGS=-g -Wall
LDFLAGS = -lm
TARGET=lispc
SRCS=main.c lexer.c error.c preprocessor.c parser.c generator.c cgenerator.c
OBJS=$(SRCS:.c=.o)

INCDIR=-I../inc
LIBDIR=
LIBS=

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBDIR) $(LIBS)
	
$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)

all: clean $(OBJS) $(TARGET)

clean:
	-rm -f $(OBJS) $(TARGET) *.d
