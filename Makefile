# Henry Cooney <hacoo36@gmail.com> <Github: hacoo>
# 6 Apr. 2016
#
# Top-level makefile for 533-threads

CC        := gcc
C_CC      := gcc
SRCDIR    := src
BUILDDIR  := build
TARGET    := bin/threads

SRCEXT    := c
SOURCES   := $(wildcard $(SRCDIR)/*.$(SRCEXT))


OBJECTS   := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SOURCES:.$(SRCEXT)=.o))
OBJECTS   += build/thread_asm.o
CFLAGS    := -g -Wall # -O3
LIB       := -lm -lrt
INC       :=

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	$(CC) -o $(TARGET) $^ $(LIB) 
	@echo "---- COMPILED OK ----"

build/thread_asm.o: src/thread_asm.s
	$(CC) -c -o build/thread_asm.o src/thread_asm.s

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."
	$(RM) -r $(OBJECTS) $(TARGET)

run: $(TARGET)
	@bin/threads

.PHONY: clean



