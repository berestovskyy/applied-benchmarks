##
## SPDX-License-Identifier: MIT
## Copyright (c) 2019 Andriy Berestovskyy <berestovskyy@gmail.com>
##

# List of benchmarks
SUBDIRS += memory-loads

# Default directories
BUILDDIR  ?= build
BENCHMARK ?= benchmark

# Default target
all: $(BENCHMARK) subdirs
.PHONY: all

# Compile Benchmark library
$(BENCHMARK):
	@echo "==> $@"
	mkdir -p $(BUILDDIR) \
	&& cd $(BUILDDIR) \
	&& cmake $(abspath $@) \
		-DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_GTEST_TESTS=OFF \
	&& make
.PHONY: $(BENCHMARK)

# Make subdirs
subdirs: $(SUBDIRS)
.PHONY: subdirs
$(SUBDIRS):
	@echo "==> $@"
	$(MAKE) -C $@ BENCHMARK=$(abspath $(BENCHMARK)) \
		BUILDDIR=$(abspath $(BUILDDIR))
.PHONY: $(SUBDIRS)

# Clean target
clean: $(BENCHMARK)-clean subdirs-clean
.PHONY: clean

# Clean Benchmark library
$(BENCHMARK)-clean:
	@echo "==> $@"
	rm -rdf $(BUILDDIR)
.PHONY: $(BENCHMARK)-clean

# Clean subdirs
SUBDIRS_CLEAN = $(addsuffix .clean,$(SUBDIRS))
subdirs-clean: $(SUBDIRS_CLEAN)
.PHONY: subdirs-clean
$(SUBDIRS_CLEAN):
	@echo "==> $(basename $@)"
	$(MAKE) -C $(basename $@) clean
.PHONY: $(SUBDIRS_CLEAN)
