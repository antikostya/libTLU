
obj-libstring-test	=
obj-libstring-test	+= strallfrom_test.o
obj-libstring-test	+= strchr_test.o
obj-libstring-test	+= strcmp_test.o
obj-libstring-test	+= strcnt_test.o
obj-libstring-test	+= strcpy_test.o
obj-libstring-test	+= strends_test.o
obj-libstring-test	+= streq_test.o
obj-libstring-test	+= strlen_test.o
obj-libstring-test	+= strncmp_test.o
obj-libstring-test	+= strnonefrom_test.o
obj-libstring-test	+= strrchr_test.o
obj-libstring-test	+= strstarts_test.o
obj-libstring-test	+= strstr_test.o

obj-libstring-test	+= libstring_test.o

deps-libstring-test	= $(target-libstring) $(target-utest)

target-libstring-test	= build/libstring_unittest.elf

$(eval $(call build_executable,\
	$(target-libstring-test),\
	$(obj-libstring-test),\
	$(deps-libstring-test),\
	libc/test/string,\
))

