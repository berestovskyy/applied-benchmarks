##
## SPDX-License-Identifier: MIT
## Copyright (c) 2019 Andriy Berestovskyy <berestovskyy@gmail.com>
##

BUILDDIR  ?= ../build
BENCHMARK ?= ../benchmark

CXXFLAGS  += -I$(BENCHMARK)/include \
             -Wall -Wextra -Wpedantic -Wshadow -Wpointer-arith \
             -Wcast-qual -Werror -std=c++17 -O3 -g \
             -Wno-uninitialized
LDFLAGS   += -L$(BUILDDIR)/src -lbenchmark -pthread

PROG       = $(basename $(word 1, $(wildcard *.cpp)))
OBJS       = ${PROG}.o

${PROG}: ${OBJS}
	${CXX} -o ${PROG} ${OBJS} ${LDFLAGS}

clean:
	${RM} ${PROG} ${OBJS} ${PROG}.dSYM
