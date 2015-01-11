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

.PHONY: all clean clean-all clean-benchmark clean-test
all: $(PROGRAM_EXES)
clean:
	rm -fr $(BUILD_ROOT)
clean-test:
	rm -fr test/
clean-benchmark:
	rm -fr benchmark/
clean-all: clean-benchmark clean-test clean

$(BUILD_DIRS):
	mkdir -p $(BUILD_DIRS)

$(OBJS) $(PROGRAM_OBJS): | $(BUILD_DIRS)

# global compilation flags, overridable on cmdline
override CXXFLAGS += -std=c++11 -I$(SRC_DIR) -pthread \
	-O3 -flto \
	-Wall -Wextra
override LDFLAGS += -pthread \
	-flto

# it's either c++ linker or -lstdc++
CC = $(CXX)

# target programs, depends on all object files
$(PROGRAM_EXES): $(BUILD_ROOT)/programs/%: $(BUILD_ROOT)/programs/%.o $(OBJS)

# compilation rule with substitution into build dir
$(OBJS) $(PROGRAM_OBJS): $(BUILD_ROOT)/%.o: $(SRC_DIR)/%.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@ -MMD

-include $(DEPS)

DNA_URL = 'ftp://ftp.ensembl.org/pub/release-78/fasta/caenorhabditis_elegans/dna/Caenorhabditis_elegans.WBcel235.dna.toplevel.fa.gz'
res/dna.fa.gz:
	mkdir -p res/
	wget '$(DNA_URL)' -O res/dna.fa.gz
res/dna.fa: res/dna.fa.gz
	gzip -d $< -k

TEST_DNA = test/dna-short.fa
.INTERMEDIATE: prep-test-data
test/test_1000x10.fq test/test_200x500.fq $(TEST_DNA): prep-test-data
prep-test-data: res/dna.fa
	@mkdir -p test/
	@head -n 201 res/dna.fa > $(TEST_DNA)
	@python3 scripts/subseqs.py $(TEST_DNA) test/test_200x500.fq 200 500 seeeed
	@python3 scripts/subseqs.py $(TEST_DNA) test/test_1000x10.fq 1000 10 seeeed
	@echo "Test data prepared."
TIME = command time -f "real %e\nuser %U\nsys %s\n\nmem %MK\n"
run-tests: all test/test_1000x10.fq test/test_200x500.fq $(TEST_DNA)
	@# Make sure suffix array file exists and check it is correct.
	@if [ ! -e $(TEST_DNA).sa ]; then \
	  echo "Computing suffix array file $(TEST_DNA).sa."; \
	  $(TIME) $(BUILD_ROOT)/programs/suffix_array $(TEST_DNA) $(TEST_DNA).sa; \
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
	  echo "==========================="; \
	  echo "Testing query file $$query."; \
	  $(TIME) $(BUILD_ROOT)/programs/memer \
	      $(TEST_DNA) $(TEST_DNA).sa $$query 1 $$L > $$tmp; \
	  echo "Checking correctness."; \
	  : > $$err; \
	  $(TIME) python3 scripts/bruteforce.py \
	      $(TEST_DNA) $$tmp $$err $$L > /dev/null && \
	  if [ -s $$err ]; then \
	    echo "Errors found."; \
	    cat $$err; \
	  else \
	    echo "All good."; \
	  fi || echo "Checker crashed.";  \
	done; \
	rm $$tmp; \
	rm $$err;

benchmark/%.sa: res/% $(BUILD_ROOT)/programs/suffix_array
	@mkdir -p benchmark/
	@echo 'Making suffix array for $<'
	$(TIME) $(BUILD_ROOT)/programs/suffix_array $< $@
.INTERMEDIATE: prep-benchmark-data
benchmark/test_2Mx1000.fq benchmark/test_20Mx200.fq: prep-benchmark-data
prep-benchmark-data: res/dna.fa
	@mkdir -p benchmark/
	@echo "Preparing benchmark queries... (will take minutes and GBs)"
	python3 scripts/subseqs.py \
	    $< benchmark/test_2Mx1000.fq 2000000 1000 seeeed
	python3 scripts/subseqs.py \
	    $< benchmark/test_20Mx200.fq 20000000 200 seeeed
	@echo "Benchmark data prepared."
run-benchmark: $(BUILD_ROOT)/programs/memer \
		$(BUILD_ROOT)/programs/suffix_array \
		benchmark/dna.fa.sa \
		benchmark/test_2Mx1000.fq benchmark/test_20Mx200.fq
	@for k in 1 2 3; do \
	  echo "Running benchmark with sparse factor K=$$k."; \
	  for i in benchmark/*.fq; do \
	    echo "Testing with $$i query file"; \
	    pv $$i | $(TIME) $(BUILD_ROOT)/programs/memer \
	        res/dna.fa benchmark/dna.fa.sa /dev/stdin $$k 20 \
			> $${i##.fq}.out; \
	    echo "==============="; \
	  done ;\
	done;
