VPATH = $(shell find src -type d -printf '%p:')

DEPDIR = build
df = $(DEPDIR)/$(*F)

CXX = cross/bin/i586-elf-g++
AS = nasm

CXXFLAGS = -ffreestanding -Wall -Wextra -fno-exceptions -fno-rtti -nostdlib 

ASFLAGS = -felf

LDFLAGS = -ffreestanding -nostdlib -lgcc

CXXSRCS = $(shell find src -name '*.cpp')
ASMSRCS = $(shell find src -name '*.s')

CXXOBJS = $(addprefix build/,$(notdir $(CXXSRCS:.cpp=.o)))
ASOBJS =  $(addprefix build/,$(notdir $(ASMSRCS:.s=.so)))

CRTI_OBJ=build/crti.spo
CRTBEGIN_OBJ:=$(shell $(CXX) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CXX) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ=build/crtn.spo

OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ) $(CXXOBJS) $(ASOBJS) $(CRTEND_OBJ) $(CRTN_OBJ)

INCLUDE_DIR = $(notdir $(CXXOBJS))

kos : $(OBJ_LINK_LIST)
	${CXX} -T linker.ld ${LDFLAGS} -o bin/kos.bin ${OBJ_LINK_LIST}

build/%.so : %.s
	${AS} ${ASFLAGS} -o $@ $<
	
build/%.spo : %.sp
	${AS} ${ASFLAGS} -o $@ $<	
	
build/%.o : %.cpp
	${CXX} ${CXXFLAGS} -c -MD -o $@ $<
	
	@cp $(df).d $(df).P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(df).d >> $(df).P; \
	rm -f $(df).d

-include $(INCLUDE_DIR:%.o=$(DEPDIR)/%.P)

.PHONY: clean
.PHONY: qemu
.PHONY: bochs

# clean operation
clean :
	-rm -rf build/*
	find ./ -name '*~' | xargs rm -f

#execute tests
qemu: kos
	qemu-system-i386 -kernel bin/kos.bin

bochs: kos
	cp bin/kos.bin iso/boot/kos.bin
	grub-mkrescue -o bin/kos.iso iso
	bochs -log build/bochsout -f external/bochsrc -q