.DEFAULT_GOAL = app

WORK_DIR  = $(shell pwd)
BUILD_DIR = $(WORK_DIR)/build

SRCS  = $(shell find -name "*.cpp")

OBJ_DIR  = $(BUILD_DIR)/obj
BINARY   = $(BUILD_DIR)/emu

CXX := g++

LD := $(CXX)
# CFLAGS  := -O2 -MMD -Wall -Werror
CFLAGS := -O2 -MMD
LDFLAGS := -O2 $(LDFLAGS) -lreadline

OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

# Compilation patterns
$(OBJ_DIR)/%.o: %.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<
	$(call call_fixdep, $(@:.o=.d), $@)

$(OBJ_DIR)/%.o: %.cpp
	@echo + CXX $<
	@mkdir -p $(dir $@)
	@$(CXX) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<
	$(call call_fixdep, $(@:.o=.d), $@)

# Depencies
-include $(OBJS:.o=.d)

# Some convenient rules

.PHONY: app clean

app: $(BINARY)

$(OBJ_DIR)/main.o: main.cpp
	@echo + CXX $<
	@mkdir -p $(dir $@)
	@$(CXX) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<
	$(call call_fixdep, $(@:.o=.d), $@)


$(BINARY): $(OBJS)
	@echo + LD $@
	@$(LD) -o $@ $(OBJS) $(LDFLAGS)

run: $(BINARY)
#   @echo $(OBJS)
	@$(BINARY)
clean:
	-rm -rf $(BUILD_DIR)
