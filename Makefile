CC ?= gcc
CFLAGS ?= -Wall -Wextra -g -O0

TARGET := compiler
SRCDIR := src
OBJDIR := build

SRC := $(SRCDIR)/main.c $(SRCDIR)/lexer.c $(SRCDIR)/parser.c $(SRCDIR)/debug.c $(SRCDIR)/ir.c
OBJS := $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

.PHONY: all clean

all: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)



clean: 
	rm -rf $(OBJDIR) $(TARGET)	
