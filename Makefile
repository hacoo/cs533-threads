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
OBJECTS   += build/thread_switch.o
CFLAGS    := -g -Wall # -O3
LIB       := -lm
INC       :=

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	$(CC) -o $(TARGET) $(LIB) $^
	@echo "---- COMPILED OK ----"

build/thread_switch.o: src/thread_switch.s
	$(CC) -c -o build/thread_switch.o src/thread_switch.s

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."
	$(RM) -r $(OBJECTS) $(TARGET)

run: $(TARGET)
	@bin/threads

.PHONY: clean



