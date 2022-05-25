TUDOR = out/tudor

CFLAGS = -O0 -g3 -Wall -D_GNU_SOURCE -lpthread -lcrypto $(shell pkg-config --cflags libusb-1.0)
LFLAGS = -O0 -g3 -Wall -D_GNU_SOURCE -Wl,--copy-dt-needed-entries -lpthread -lcrypto $(shell pkg-config --libs libusb-1.0)

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

all: $(TUDOR)

-include $(DEPS)

$(TUDOR): $(OBJS) $(addprefix bin/windrv/,$(DRVDLLS:.dll=.o))
	@mkdir -p $(dir $@)
	$(CC) $(LFLAGS) -o $@ $^

bin/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) -MMD -c -Isrc $(CFLAGS) -o $@ $<

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
	rm -rf bin out