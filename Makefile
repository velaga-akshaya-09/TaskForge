CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -D_POSIX_C_SOURCE=200809L -Iinclude

LDFLAGS = -pthread

TARGET = taskforge

TEST_TARGETS = \
	test_taskforge \
	test_cancel \
	test_failure \
	test_stress

DEMO_TARGETS = \
	os_service_demo \
	command_execution_demo \
	process_lifecycle_demo \
	process_scheduling_demo \
	process_pitfall_demo \
	pipe_demo \
	fifo_demo \
	signal_demo \
	process_group_demo \
	memory_allocation_demo \
	address_space_demo \
	mmap_demo \
	page_fault_demo \
	cow_demo \
	file_io_demo \
	file_metadata_demo \
	buffered_io_demo \
	mmap_file_demo \
	filesystem_demo \
	semaphore_demo \
	race_mutex_demo \
	deadlock_demo \
	advanced_sync_demo

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


# ============================================================
# Main TaskForge application
# ============================================================

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)


# ============================================================
# Object files
# ============================================================

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# Existing tests
# ============================================================

test_taskforge: tests/test_taskforge.c $(COMMON_SOURCES)
	$(CC) $(CFLAGS) tests/test_taskforge.c $(COMMON_SOURCES) -o test_taskforge $(LDFLAGS)

test_cancel: tests/test_cancel.c $(COMMON_SOURCES)
	$(CC) $(CFLAGS) tests/test_cancel.c $(COMMON_SOURCES) -o test_cancel $(LDFLAGS)

test_failure: tests/test_failure.c $(COMMON_SOURCES)
	$(CC) $(CFLAGS) tests/test_failure.c $(COMMON_SOURCES) -o test_failure $(LDFLAGS)

test_stress: tests/test_stress.c $(COMMON_SOURCES)
	$(CC) $(CFLAGS) tests/test_stress.c $(COMMON_SOURCES) -o test_stress $(LDFLAGS)

tests: $(TEST_TARGETS)


# ============================================================
# CO demonstration programs
# ============================================================

os_service_demo: demos/os_service_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

command_execution_demo: demos/command_execution_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

process_lifecycle_demo: demos/process_lifecycle_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

process_scheduling_demo: demos/process_scheduling_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

process_pitfall_demo: demos/process_pitfall_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

pipe_demo: demos/pipe_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

fifo_demo: demos/fifo_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

signal_demo: demos/signal_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

process_group_demo: demos/process_group_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

memory_allocation_demo: demos/memory_allocation_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

address_space_demo: demos/address_space_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

mmap_demo: demos/mmap_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

page_fault_demo: demos/page_fault_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

cow_demo: demos/cow_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

file_io_demo: demos/file_io_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

file_metadata_demo: demos/file_metadata_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

buffered_io_demo: demos/buffered_io_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

mmap_file_demo: demos/mmap_file_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

filesystem_demo: demos/filesystem_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

semaphore_demo: demos/semaphore_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

race_mutex_demo: demos/race_mutex_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

deadlock_demo: demos/deadlock_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

advanced_sync_demo: demos/advanced_sync_demo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)


# ============================================================
# Build all CO demonstrations
# ============================================================

demos: $(DEMO_TARGETS)


# ============================================================
# AddressSanitizer memory-error demonstration
# ============================================================

memory_error_demo: demos/memory_error_demo.c
	$(CC) -Wall -Wextra -Wpedantic -std=c11 \
	-fsanitize=address \
	-g \
	$< -o $@


# ============================================================
# Run existing TaskForge tests
# ============================================================

test: $(TEST_TARGETS)
	./test_taskforge
	./test_cancel
	./test_failure
	./test_stress


# ============================================================
# Run TaskForge
# ============================================================

run: $(TARGET)
	./$(TARGET)


# ============================================================
# Clean generated files
# ============================================================

clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET)
	rm -f $(TEST_TARGETS)
	rm -f $(DEMO_TARGETS)
	rm -f memory_error_demo
	rm -f taskforge_io_demo.txt
	rm -f taskforge_buffered.txt
	rm -f taskforge_unbuffered.txt
	rm -f taskforge_mmap.txt


# ============================================================
# Phony targets
# ============================================================

.PHONY: all tests demos test run clean
