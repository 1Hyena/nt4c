# SPDX-License-Identifier: MIT
CC      = gcc
PROF    = -O0 -g -rdynamic
C_FLAGS = -Wall -Werror -Wextra -pedantic-errors -Wconversion \
          -Wno-unused-parameter -fmax-errors=5 -std=gnu23
L_FLAGS = -lm
SRC_DIR = src
OUT_DIR = bin

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OUT_FILES := $(patsubst $(SRC_DIR)/%.c,%,$(SRC_FILES))

all:
	@$(MAKE) verbose -s

verbose: $(OUT_FILES)
	@printf "\033[1;32mAll done! Examples are in the '%s' directory.\033[0m\n" \
	$(OUT_DIR)

PRINT_FMT1 = "\033[1m\033[31mCompiling \033[37m....\033[34m %-20s"
PRINT_FMT2 = "\t\033[33m%6s\033[31m lines\033[0m \n"
PRINT_FMT  = $(PRINT_FMT1)$(PRINT_FMT2)

$(OUT_FILES): %: $(SRC_DIR)/%.c
	@printf $(PRINT_FMT) $*.c "`wc -l $(SRC_DIR)/$*.c | cut -f1 -d' '`"
	$(CC) $< $(C_FLAGS) $(PROF) -o $(OUT_DIR)/$@ $(L_FLAGS)

clean:
	@printf "\033[1;36mCleaning \033[37m ...."
	@rm -f $(OUT_FILES)
	@printf "\033[1;37m Binaries cleaned!\033[0m\n"
