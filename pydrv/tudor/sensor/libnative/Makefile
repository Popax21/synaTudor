ASM_SRC := synaWudfBioUsb104.dll.tudorIplOpen.asm synaWudfBioUsb104.dll.tudorIplClose.asm synaWudfBioUsb104.dll.tudorIplProcessFrame.asm
C_SRC := extracted.c image.c

CFLAGS ?= -O0 -m64

OBJ := $(addprefix obj/, $(ASM_SRC:.asm=.o) $(C_SRC:.c=.o))

libnative.so: $(OBJ)
	gcc -fPIC -shared -o $@ $^ $(CFLAGS)

clean:
	rm -rf obj

-include $(OBJ:.o=.d)

obj/%.o: %.asm
	@mkdir -p obj
	nasm -felf64 -o $@ $<

obj/%.o: %.c
	@mkdir -p obj
	gcc -MMD -fPIC -c -o $@ $< $(CFLAGS)