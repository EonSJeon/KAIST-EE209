# CORRECTED: Assignment 5, Jeon Sangeon. Override CC=gcc209 on lab PCs.
CC = cc
CPPFLAGS = -D_DEFAULT_SOURCE -D_GNU_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -g
BUILD = .build
SOURCES = ish.c token.c lexer.c parser.c execute.c signals.c dynarray.c
OBJECTS = $(SOURCES:%.c=$(BUILD)/%.o)
DEPS = $(OBJECTS:.o=.d)

all: $(BUILD)/ish

$(BUILD)/ish: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

$(BUILD)/%.o: %.c
	@mkdir -p $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(BUILD)

.PHONY: all clean
