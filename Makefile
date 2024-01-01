
BUILD = build
CFLAGS = -Wall -Wextra -Werror -fdiagnostics-color=always
CFLAGS += -I include

CFLAGS += -D CONFIG_DEBUG=$(CONFIG_DEBUG)
CFLAGS += -D CONFIG_COLOR_OUTPUT=$(CONFIG_COLOR_OUTPUT)

CC = gcc
LD = gcc
AR = ar rcsT

targets =
obj-y =

all: build_all
.PHONY: all

clean: clean_all
.PHONY: clean

define build_library

$(eval target = $(strip $1))
$(eval obj = $(strip $2))
$(eval deps = $(strip $3))
$(eval dir = $(strip $4))

$(eval obj = $(addprefix $(dir)/,$(obj)))

$(eval targets = $(targets) $(target))
$(eval obj-y = $(obj-y) $(obj))

$(target): $(deps) $(obj)
	@echo AR $(basename $(target).a)
	@$(AR) $(target) $(obj) $(deps)

endef

define build_executable

$(eval target = $(strip $1))
$(eval obj = $(strip $2))
$(eval deps = $(strip $3))
$(eval dir = $(strip $4))

$(eval obj = $(addprefix $(dir)/,$(obj)))

$(eval targets = $(targets) $(target))
$(eval obj-y = $(obj-y) $(obj))

$(target): $(deps) $(obj)
	@echo LD $(basename $(target).elf)
	@$(LD) -o $(target) $(obj) $(deps)

endef


include .config

include core/Makefile
include utest/Makefile
include bench/Makefile

include libc/$(CONFIG_LIBC_TYPE)/syscall/Makefile
include libc/$(CONFIG_LIBC_TYPE)/mem/Makefile
include libc/$(CONFIG_LIBC_TYPE)/char/Makefile
include libc/$(CONFIG_LIBC_TYPE)/string/Makefile
include libc/$(CONFIG_LIBC_TYPE)/lexical/Makefile
include libc/$(CONFIG_LIBC_TYPE)/io/Makefile

include libc/test/mem/Makefile
include libc/test/string/Makefile
include libc/test/char/Makefile
include libc/test/lexical/Makefile
include libc/perf/Makefile

build_all: $(BUILD) $(targets)
.PHONY: build_all

clean_all:
	@for ob in $(obj-y) $(targets); do \
		test -f $$ob && echo "CLEAN $$(basename $$ob)"; \
		$(RM) $$ob; \
	done
.PHONY: clean_all

%.o: %.c
	@echo "CC $$(basename $@)"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	echo "ASM $@"

$(BUILD):
	@mkdir -p $(BUILD)
