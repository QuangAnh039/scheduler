TARGET = scheduler
CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = scheduler.c file_oper.c my_menu.c task_oper.c
OBJS = $(SRCS:.c=.o)

# Phụ thuộc cho scheduler.o
scheduler.o: my_menu.h Node.h file_oper.h task_oper.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
