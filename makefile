.RECIPEPREFIX = >

CC := gcc

BUILD := build

CFLAGS := -O2 -std=gnu99 -MMD -MP $(shell pkg-config --cflags sdl2) -DSI_VEC
#CFLAGS := -Og -ggdb -std=gnu99 -MMD -MP $(shell pkg-config --cflags sdl2) -DSI_VEC

LIBS := -lpthread -lm -lSDL2

CLSTL_SOURCES := $(wildcard *.c)
CLSTL_OBJECTS := $(patsubst %, $(BUILD)/%.o, $(CLSTL_SOURCES))

DEPS := $(patsubst %, $(BUILD)/%.d, $(CLSTL_SOURCES))

CWARNINGS := -Wall -Wextra -Werror=vla -Werror=shadow -Wswitch-enum \
             -Wno-strict-prototypes -Wno-sign-compare

.PHONY: default
default: $(BUILD)/celestial

-include $(DEPS)

$(BUILD)/celestial: $(CLSTL_OBJECTS) makefile
> @mkdir -p $(dir $@)
> $(CC) $(CFLAGS) -o $@ $(CLSTL_OBJECTS) $(LIBS)

$(BUILD)/%.c.o: %.c makefile
> @mkdir -p $(dir $@)
> $(CC) $(CFLAGS) $(CWARNINGS) -c -o $@ $<

.PHONY: clean
clean:
> rm -rf build/

