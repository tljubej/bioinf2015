# source dir and subdirs
SRC_DIR = src
SUBDIRS = sa mem
SRC_ALL = $(addprefix $(SRC_DIR)/,$(SUBDIRS))

# build dir layout is same as source dir; root dir overridable on cmdline
BUILD_ROOT ?= build
BUILD_DIRS=$(BUILD_ROOT) $(addprefix $(BUILD_ROOT)/,$(SUBDIRS))

# compilation units are files in SRC_DIR and its SUBDIRS
SRCS = $(wildcard $(SRC_DIR)/*.cc) $(foreach I,$(SRC_ALL),$(wildcard $(I)/*.cc))

# respective object files go into the build (sub)directories
OBJS = $(patsubst $(SRC_DIR)/%.cc,$(BUILD_ROOT)/%.o,$(SRCS))

# respective generated dependency files as well (for incremental builds)
DEPS = $(OBJS:.o=.d)


# targets, most trivial first

.PHONY: all clean
all: $(BUILD_DIRS) $(BUILD_ROOT)/main
clean:
	rm -fr $(BUILD_ROOT)

$(BUILD_DIRS):
	mkdir -p $(BUILD_DIRS)

# global compilation flags, overridable on cmdline
override CXXFLAGS += -std=c++11 -I$(SRC_DIR) -Wall -O3 -Wextra

# it's either c++ linker or -lstdc++
CC = $(CXX)

# target program, depends on all object files
$(BUILD_ROOT)/main: $(OBJS)

# compilation rule with substitution into build dir
$(OBJS): $(BUILD_ROOT)/%.o: $(SRC_DIR)/%.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@ -MMD

-include $(DEPS)
