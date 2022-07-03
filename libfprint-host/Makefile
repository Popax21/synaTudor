OUT ?= ../out
LIBFPRINT_HOST = $(OUT)/libfprint_host

CFLAGS = -Wall -Wno-stringop-truncation -D_GNU_SOURCE -I inc -I $(OUT)/inc $(shell pkg-config --cflags libusb-1.0)
LFLAGS = -Wall -D_GNU_SOURCE -Wl,--copy-dt-needed-entries -Wl,-z,noexecstack -Wl,-rpath='$$ORIGIN' -L $(OUT) -ltudor -lpthread -lcap -lseccomp $(shell pkg-config --libs libusb-1.0)

ifdef DEBUG
	CFLAGS := $(CFLAGS) -O0 -g3 -fstack-protector
	LFLAGS := $(LFLAGS) -O0 -g3 -fstack-protector
else
	CFLAGS := $(CFLAGS) -O2
	LFLAGS := $(LFLAGS) -O2
endif

SRCS = $(wildcard src/*.c) $(wildcard src/**/*.c) $(wildcard src/**/**/*.c) $(wildcard src/**/**/**/*.c)
OBJS = $(addprefix bin/,$(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)

all: $(LIBFPRINT_HOST)

-include $(DEPS)

$(LIBFPRINT_HOST): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(LFLAGS)

	@mkdir -p $(OUT)/inc
	cp -rf inc/. $(OUT)/inc/

bin/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) -MMD -c -o $@ $< -Isrc $(CFLAGS)
clean:
	rm -rf bin