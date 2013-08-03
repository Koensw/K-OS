VPATH = src
DEPDIR = build

CXX = i586-elf-g++
CXXFLAGS = -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
df = $(DEPDIR)/$(*F)

CORE = kernel.o 
ASMCORE = boot.s

CXXSRCS = $(CORE)
ASMSRC = $(ASMCORE)

kintex : $(addprefix build/,${CXXSRCS})
	${CXX} ${CXXFLAGS} -o bin/kintex $(addprefix build/,${SRCS})

build/%.o : %.cpp
	${CXX} ${CXXFLAGS} -c -MD -o $@ $<
	
	@cp $(df).d $(df).P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(df).d >> $(df).P; \
	rm -f $(df).d

-include $(SRCS:%.o=$(DEPDIR)/%.P)

.PHONY: clean
.PHONY: test
# clean operation
clean :
	-rm -rf build/*
	find ./ -name '*~' | xargs rm -f

#execute tests
test:
	@bash test/tests.sh
