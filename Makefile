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

prep-test-data: res/dna.fa
	mkdir -p test/
	head -n 201 res/dna.fa > test/dna-short.fa
	python3 scripts/subseqs.py test/dna-short.fa /dev/stdout 200 100 | pv >res/test_200x100.fq
	python3 scripts/subseqs.py test/dna-short.fa /dev/stdout 1000 10 | pv >res/tets_1000x10.fq

run-tests:
	ref=test/dna-short.fa
	# Make sure suffix array file exists and check it is correct.
	if [[ ! -e "$ref.sa" ]]; then
	  echo "Computing suffix array file."
	  time bin/suffix_array $ref $ref.sa
	else 
	  echo "Suffix array file found."
	fi
	echo "Checking suffix array file integrity."
	bin/sa_checker $ref $ref.sa
	if [ $? -eq 0 ]; then
	  echo "Valid."
	else
	  echo "Invalid! Delete it and run again."
	  exit 1
	fi
	# Look for query files and run tests.
	tmp=test/tmp.out
	err=test/err.out
	L=20
	for query in test/*.fq
	do
	  echo "Testing query file $query."
	  time bin/memer $ref $ref.sa $query 1 $L > $tmp
	  echo "Checking correctness."
	  rm $err
	  python3 scripts/bruteforce.py $ref $tmp $err $L > /dev/null 
	  if [[ ! -s "$err" ]]; then
	    echo "Errors found."
	    cat $err
	  else
	    echo "All good."
	  fi
	done
	rm $tmp $err
	

