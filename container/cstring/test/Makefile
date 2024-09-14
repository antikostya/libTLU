
obj-cstring-test	=

obj-cstring-test	+= cstring_test.o

deps-cstring-test	= $(target-libcstring) $(target-utest)

target-cstring-test	= build/cstring_unittest.elf

$(eval $(call build_executable,\
	$(target-cstring-test),\
	$(obj-cstring-test),\
	$(deps-cstring-test),\
	container/cstring/test,\
))

