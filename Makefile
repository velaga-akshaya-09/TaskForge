CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -D_POSIX_C_SOURCE=200809L -Iinclude

LDFLAGS = -pthread

TARGET = taskforge

TEST_TARGETS = \
	test_taskforge \
	test_cancel \
	test_failure \
	test_stress

SOURCES = \
	src/main.c \
	src/task.c \
	src/queue.c \
	src/scheduler.c \
	src/stats.c \
	src/worker.c \
	src/taskforge.c \
	src/cli.c

OBJECTS = $(SOURCES:.c=.o)

COMMON_SOURCES = \
	src/task.c \
	src/queue.c \
	src/scheduler.c \
	src/stats.c \
	src/worker.c \
	src/taskforge.c

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

test_taskforge: tests/test_taskforge.c $(COMMON_SOURCES)
	$(CC) $(CFLAGS) tests/test_taskforge.c $(COMMON_SOURCES) -o test_taskforge $(LDFLAGS)

test_cancel: tests/test_cancel.c $(COMMON_SOURCES)
	$(CC) $(CFLAGS) tests/test_cancel.c $(COMMON_SOURCES) -o test_cancel $(LDFLAGS)

test_failure: tests/test_failure.c $(COMMON_SOURCES)
	$(CC) $(CFLAGS) tests/test_failure.c $(COMMON_SOURCES) -o test_failure $(LDFLAGS)

test_stress: tests/test_stress.c $(COMMON_SOURCES)
	$(CC) $(CFLAGS) tests/test_stress.c $(COMMON_SOURCES) -o test_stress $(LDFLAGS)

tests: $(TEST_TARGETS)

clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_TARGETS)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGETS)
	./test_taskforge
	./test_cancel
	./test_failure
	./test_stress

.PHONY: all clean run tests test
