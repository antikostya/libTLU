
obj-libstring	=
obj-libstring	+=	strallfrom.o
obj-libstring	+=	strchr.o
obj-libstring	+=	strcmp.o
obj-libstring	+=	strcnt.o
obj-libstring	+=	strcpy.o
obj-libstring	+=	strends.o
obj-libstring	+=	streq.o
obj-libstring	+=	strlen.o
obj-libstring	+=	strncmp.o
obj-libstring	+=	strnonefrom.o
obj-libstring	+=	strrchr.o
obj-libstring	+=	strstarts.o
obj-libstring	+=	strstr.o

deps-libstring	= $(target-libmem)

target-libstring	= build/libstring.a

$(eval $(call build_library,\
	$(target-libstring),\
	$(obj-libstring),\
	$(deps-libstring),\
	libc/$(CONFIG_LIBC_TYPE)/string,\
))

