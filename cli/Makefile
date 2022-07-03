OUT ?= ../out
TUDOR_CLI = $(OUT)/tudor_cli

CFLAGS = -Wall -D_GNU_SOURCE -I $(OUT)/inc $(shell pkg-config --cflags libusb-1.0)
LFLAGS = -Wall -D_GNU_SOURCE -Wl,--copy-dt-needed-entries -Wl,-z,noexecstack -Wl,-rpath='$$ORIGIN' -L $(OUT) -ltudor $(shell pkg-config --libs libusb-1.0)

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

all: $(TUDOR_CLI)

-include $(DEPS)

$(TUDOR_CLI): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(LFLAGS)

bin/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) -MMD -c -o $@ $< -Isrc $(CFLAGS)
clean:
	rm -rf bin