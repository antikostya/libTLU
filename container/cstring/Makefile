
obj-libcstring	= cstring.o

deps-libcstring	= $(target-libmem) $(target-libconsole)

target-libcstring	= build/libcstring.a

$(eval $(call build_library,\
	$(target-libcstring),\
	$(obj-libcstring),\
	$(deps-libcstring),\
	container/cstring,\
))

