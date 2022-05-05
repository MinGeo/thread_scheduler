TARGET	= pa2
CFLAGS	= -g -c -D_POSIX_C_SOURCE -D_GNU_SOURCE -D_XOPEN_SOURCE=700
CFLAGS += -std=c99 -Wextra -Wno-unused-parameter -Werror
LDFLAGS	=

all: pa2

pa2: pa2.o uthread.o
	gcc $(LDFLAGS) $^ -o $@

%.o: %.c
	gcc $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(TARGET) *.o *.dSYM *.txt

test: $(TARGET) test-fifo test-rr test-prio test-sjf

test-fifo: $(TARGET)
	@echo "Test Fisrt In Fisrt Out SCHEDULER"; \
	./$(TARGET) < testcases/fifo.txt 2> fifo.txt; \
	diff -Naur fifo.txt outputs/fifo.txt; \
	if [ $$? -eq 0 ]; then echo "\tTest seems correct\n";\
	else echo "\tResults not identical, check the diff output\n";\
	fi

test-rr:
	@echo "Test Round Robin SCHEDULER"; \
	./$(TARGET) < testcases/rr.txt 2> rr.txt; \
    diff -Naur rr.txt outputs/rr.txt; \
	if [ $$? -eq 0 ]; then echo "\tTest seems correct\n";\
	else echo "\tResults not identical, check the diff output\n";\
	fi

test-prio:
	@echo "Test Priority SCHEDULER"; \
	./$(TARGET) < testcases/prio.txt 2> prio.txt; \
	diff -Naur prio.txt outputs/prio.txt; \
	if [ $$? -eq 0 ]; then echo "\tTest seems correct\n";\
	else echo "\tResults not identical, check the diff output\n";\
	fi

test-sjf:
	@echo "Test Shortest Job First SCHEDULER"; \
	./$(TARGET) < testcases/sjf.txt 2> sjf.txt; \
	diff -Naur sjf.txt outputs/sjf.txt; \
	if [ $$? -eq 0 ]; then echo "\tTest seems correct\n";\
	else echo "\tResults not identical, check the diff output\n";\
	fi
