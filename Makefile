
include .config

BUILD = build
CFLAGS =
LDFLAGS =
CFLAGS += -I include

include cflags.mk

CFLAGS += -D CONFIG_DEBUG=$(CONFIG_DEBUG)

CFLAGS += -D CONFIG_UTEST_FAIL_HALT=$(CONFIG_UTEST_FAIL_HALT)
CFLAGS += -D CONFIG_UTEST_FIRST_FAIL=$(CONFIG_UTEST_FIRST_FAIL)
CFLAGS += -D CONFIG_UTEST_COLOR=$(CONFIG_UTEST_COLOR)

CFLAGS += -D CONFIG_CVECTOR_VALIDATION_LAYER=$(CONFIG_CVECTOR_VALIDATION_LAYER)

ifeq ($(CONFIG_SANITIZE),1)
CFLAGS += -fsanitize=address
CFLAGS += -fsanitize=pointer-compare
CFLAGS += -fsanitize=pointer-subtract
CFLAGS += -fsanitize=undefined
CFLAGS += -fsanitize-address-use-after-scope
CFLAGS += -static-libasan
endif

ifeq ($(CONFIG_COVERAGE),1)
CFLAGS += -fprofile-arcs
CFLAGS += -ftest-coverage
LDFLAGS += -lgcov --coverage
endif

ifeq ($(CONFIG_DEBUG),1)
CFLAGS += -O0 -g3 -fno-inline
MESSAGE = "build in debug mode"
else
CFLAGS += -O3
MESSAGE = "build in release mode"
endif

ifeq ($(CONFIG_SILENT),1)
ECHO = :
else
ECHO = echo
endif

CC = gcc
LD = gcc
AR = ar rcsT
RM = rm -f

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
$(eval name = $(notdir $(target)))

$(eval targets = $(targets) $(target))
$(eval obj-y = $(obj-y) $(obj))

$(name): $(target)

$(target): $(deps) $(obj)
	@$(ECHO) AR $(name)
	@$(AR) $(target) $(obj) $(deps)

endef

define build_executable

$(eval target = $(strip $1))
$(eval obj = $(strip $2))
$(eval deps = $(strip $3))
$(eval dir = $(strip $4))

$(eval obj = $(addprefix $(dir)/,$(obj)))
$(eval name = $(notdir $(target)))

$(eval targets = $(targets) $(target))
$(eval obj-y = $(obj-y) $(obj))

$(name): $(target)

$(target): $(deps) $(obj)
	@$(ECHO) LD $(name)
	@$(LD) -o $(target) $(obj) $(deps) $(LDFLAGS) $(CFLAGS)

endef


include utest/Makefile
include libc/io/Makefile

include libc/native/mem/Makefile
include libc/native/string/Makefile
include libc/native/char/Makefile

include libc/test/mem/Makefile
include libc/test/string/Makefile
include libc/test/char/Makefile

include container/cvector/Makefile
include container/cvector/test/Makefile

include container/cstring/Makefile
include container/cstring/test/Makefile

build_all: $(BUILD) $(targets)
	$(ECHO) $(MESSAGE)
.PHONY: build_all

clean_all:
	@$(RM) *.gcov
	@for ob in $(obj-y); do \
		f=$${ob%.o}; \
		if test -f $$f.o; then \
			$(ECHO) "CLEAN $$(basename $$ob)"; \
			$(RM) $$f.o; \
			$(RM) $$f.gcno; \
			$(RM) $$f.gcda; \
			$(RM) $$f.gcov; \
		fi \
	done
	@for ob in $(targets); do \
		test -f $$ob && $(ECHO) "CLEAN $$(basename $$ob)"; \
		$(RM) $$ob; \
	done
.PHONY: clean_all

%.o: %.c
	@$(ECHO) "CC $$(basename $@)"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	@$(ECHO) "ASM $$(basename $@)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	@$(ECHO) "MK $(BUILD)"
	@mkdir -p $(BUILD)

