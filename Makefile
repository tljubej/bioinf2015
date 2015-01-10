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
override CXXFLAGS += -std=c++11 -I$(SRC_DIR) -pthread \
	-O3 -flto \
	-Wall -Wextra 
override LDFLAGS += -pthread \
	-flto

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

DNA_URL = 'ftp://ftp.ensembl.org/pub/release-78/fasta/caenorhabditis_elegans/dna/Caenorhabditis_elegans.WBcel235.dna.toplevel.fa.gz'
res/dna.fa.gz:
	mkdir -p res/
	wget ftp://ftp.ensembl.org/pub/release-78/fasta/caenorhabditis_elegans/dna/Caenorhabditis_elegans.WBcel235.dna.toplevel.fa.gz -O res/dna.fa.gz
res/dna.fa: res/dna.fa.gz
	gzip -d $< -k

TEST_DNA = test/dna-short.fa
prep-test-data: res/dna.fa
	@mkdir -p test/
	@head -n 201 res/dna.fa > $(TEST_DNA)
	@python3 scripts/subseqs.py $(TEST_DNA) test/test_200x500.fq 200 500
	@python3 scripts/subseqs.py $(TEST_DNA) test/test_1000x10.fq 1000 10 
	@echo "Test data prepared."
run-tests:
	@# Make sure suffix array file exists and check it is correct.
	@if [ ! -e $(TEST_DNA).sa ]; then \
	  echo "Computing suffix array file $(TEST_DNA).sa."; \
	  time $(BUILD_ROOT)/programs/suffix_array $(TEST_DNA) $(TEST_DNA).sa; \
	else \
	  echo "Suffix array file found."; \
	fi
	@echo "Checking suffix array file integrity."
	@err=test/err.out; \
	$(BUILD_ROOT)/programs/sa_checker $(TEST_DNA) $(TEST_DNA).sa > $$err ;\
	cat $$err ; \
	if [ ! -s $$err ]; then \
	  echo "Valid."; \
	else \
	  echo "Invalid! Delete it and run again."; \
	  exit 1; \
	fi
	@# Look for query files and run tests.
	@tmp=test/tmp.out; \
	err=test/err.out; \
	L=20; \
	for query in test/*.fq; do \
	  echo "\n==========================="; \
	  echo "Testing query file $$query."; \
	  time $(BUILD_ROOT)/programs/memer $(TEST_DNA) $(TEST_DNA).sa $$query 1 $$L > $$tmp; \
	  echo "\nChecking correctness."; \
	  : > $$err; \
	  time python3 scripts/bruteforce.py $(TEST_DNA) $$tmp $$err $$L > /dev/null; \
	  if [ -s $$err ]; then \
	    echo "Errors found."; \
	    cat $$err; \
	  else \
	    echo "All good."; \
	  fi \
	done \
	rm $$tmp; \
	rm $$err; \

