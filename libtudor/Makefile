OUT ?= ../out
LIBTUDOR = $(OUT)/libtudor.so

CFLAGS = -shared -fPIC -Wall -Wno-strict-aliasing -D_GNU_SOURCE -I inc -lpthread -lcrypto $(shell pkg-config --cflags libusb-1.0)
LFLAGS = -shared -fPIC -Wall -D_GNU_SOURCE -Wl,--copy-dt-needed-entries -lpthread -lcrypto $(shell pkg-config --libs libusb-1.0)

ifdef DEBUG
	CFLAGS := $(CFLAGS) -O0 -g3 -fstack-protector
	LFLAGS := $(LFLAGS) -O0 -g3 -fstack-protector
else
	CFLAGS := $(CFLAGS) -O2
	LFLAGS := $(LFLAGS) -O2
endif

ifdef DBGIMPORT
	CFLAGS := $(CFLAGS) -DDBGIMPORT
endif

ifdef DBGWDF
	CFLAGS := $(CFLAGS) -DDBGWDF
endif

SRCS = $(wildcard src/*.c) $(wildcard src/**/*.c) $(wildcard src/**/**/*.c) $(wildcard src/**/**/**/*.c)
OBJS = $(addprefix bin/,$(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)

DRVDLLS = synaFpAdapter104.dll synaWudfBioUsb104.dll

all: $(LIBTUDOR)

-include $(DEPS)

$(LIBTUDOR): $(OBJS) $(addprefix bin/windrv/,$(DRVDLLS:.dll=.o))
	@mkdir -p $(dir $@)

	$(CC) -o $@ $^ $(LFLAGS)

	rm -rf $(OUT)/inc
	cp -r inc $(OUT)/inc

bin/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) -MMD -c -o $@ $< -Isrc $(CFLAGS)

bin/windrv/%.o: windrv/%.dll
	@mkdir -p $(dir $@)
	$(LD) -r -b binary -o $@ $<

extract: windrv/installer.exe
	innoextract -d windrv windrv/installer.exe
	mv windrv/**/* windrv/
	find windrv/* -type d -delete

windrv/installer.exe:
	@rm -rf windrv
	@mkdir -p windrv
	wget https://download.lenovo.com/pccbbs/mobiles/r19fp02w.exe -O $@
	shasum -c installer.sha

clean:
	rm -rf bin