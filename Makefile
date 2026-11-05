CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -I.
AR = ar
ARFLAGS = rcs

LIB = s21_matrix.a
GCOV_FLAGS = --coverage -fprofile-arcs -ftest-coverage
GCOV_REPORT_DIR = report

SRC = s21_matrix_create.c s21_matrix_arithmetic.c \
      s21_matrix_advanced.c s21_matrix_utils.c
OBJ = $(SRC:.c=.o)
TEST = s21_matrix_test.c
ALL_C = $(SRC) $(TEST)
ALL_SRC = $(SRC) $(TEST) s21_matrix.h s21_matrix_internal.h

CHECK_CFLAGS = $(shell pkg-config --cflags check)
CHECK_LIBS   = $(shell pkg-config --libs check)
LDLIBS = $(CHECK_LIBS) -lm -lpthread -lrt

CLANG_FORMAT = clang-format
CF_STYLE = -style=file

.PHONY: all test clean format check_format gcov_report valgrind

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

test:
	@mkdir -p tests
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) $(GCOV_FLAGS) $(ALL_C) -o tests/test_runner $(LDLIBS)
	./tests/test_runner

valgrind:
	@mkdir -p tests
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) $(ALL_C) -o tests/test_runner $(LDLIBS)
	valgrind --tool=memcheck --leak-check=full \
	--show-leak-kinds=all --track-origins=yes \
	./tests/test_runner

format:
	$(CLANG_FORMAT) $(CF_STYLE) -i $(ALL_SRC)
	@echo "Файлы отформатированы"

check_format:
	@failed=0; \
	for f in $(ALL_SRC); do \
	    if ! $(CLANG_FORMAT) $(CF_STYLE) --dry-run --Werror $$f >/dev/null 2>&1; then \
	        echo "  ✗ $$f не соответствует стилю"; \
	        failed=1; \
	    fi; \
	done; \
	if [ $$failed -eq 1 ]; then \
	    echo "Ошибка формата"; \
	    exit 1; \
	else \
	    echo "Все файлы соответствуют стилю"; \
	fi

gcov_report:
	$(MAKE) clean
	@mkdir -p tests $(GCOV_REPORT_DIR)
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(ALL_C) -o tests/test_runner $(LDLIBS)
	./tests/test_runner
	gcovr -r . --html --html-details \
	      --exclude '.*_test\.c' \
	      --output $(GCOV_REPORT_DIR)/index.html
	@echo "Отчёт готов: $(GCOV_REPORT_DIR)/index.html"

clean:
	rm -f *.o *.a *.gcno *.gcda *.gcov coverage.info
	rm -f test
	rm -rf tests
	rm -rf $(GCOV_REPORT_DIR)