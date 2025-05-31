# Compiler và cờ biên dịch
CC=gcc
CFLAGS=-Wall -g

# Danh sách các file thực thi
TARGETS=task1 task2 task3 test

# Mục mặc định: build tất cả
all: $(TARGETS)

# Luật chung để build từng file thực thi
%: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Clean
clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean
