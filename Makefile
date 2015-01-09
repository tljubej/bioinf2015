# source dir and subdirs
SRC_DIR = src
SUBDIRS = sa mem
SRC_DIRS = $(addprefix $(SRC_DIR)/,$(SUBDIRS))

# build dir layout is same as source dir; root dir overridable on cmdline
BUILD_ROOT ?= build
BUILD_DIRS=$(BUILD_ROOT) $(addprefix $(BUILD_ROOT)/,$(SUBDIRS) programs)

# compilation units are files in SRC_DIR and its SUBDIRS
SRCS = $(wildcard $(SRC_DIR)/*.cc) $(foreach I,$(SRC_DIRS),$(wildcard $(I)/*.cc))
PROGRAM_SRCS = $(wildcard $(SRC_DIR)/programs/*.cc)

# respective object files go into the build (sub)directories
OBJS = $(patsubst $(SRC_DIR)/%.cc,$(BUILD_ROOT)/%.o,$(SRCS))
PROGRAM_OBJS = $(patsubst $(SRC_DIR)/%.cc,$(BUILD_ROOT)/%.o,$(PROGRAM_SRCS))

PROGRAM_EXES = $(PROGRAM_OBJS:.o=)

# respective generated dependency files as well (for incremental builds)
DEPS = $(OBJS:.o=.d) $(PROGRAM_OBJS:.o=.d)

# targets, most trivial first

.PHONY: all clean
all: $(BUILD_DIRS) $(PROGRAM_EXES)
clean:
	rm -fr $(BUILD_ROOT)

$(BUILD_DIRS):
	mkdir -p $(BUILD_DIRS)

# global compilation flags, overridable on cmdline
override CXXFLAGS += -std=c++11 -I$(SRC_DIR) -Wall -O3 -Wextra

# it's either c++ linker or -lstdc++
CC = $(CXX)

# target programs, depends on all object files
$(BUILD_ROOT)/programs/memer: $(OBJS) $(BUILD_ROOT)/programs/memer.o
$(BUILD_ROOT)/programs/suffix_array: $(OBJS) $(BUILD_ROOT)/programs/suffix_array.o

# compilation rule with substitution into build dir
$(OBJS) $(PROGRAM_OBJS): $(BUILD_ROOT)/%.o: $(SRC_DIR)/%.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@ -MMD

$(PROGRAM_EXES): $(BUILD_ROOT)/programs/%: $(BUILD_ROOT)/programs/%.o $(OBJS)

-include $(DEPS)
