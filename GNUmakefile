#!/usr/bin/make -f
.SUFFIXES:

# dirname.out
TARGET := $(shell basename $$PWD).out

## make.mk
# commented assignments are overrides and are otherwise reasonable defaults.

# NOTE: remove lto requirement or at least put in in a "Emil's dent" if group with PREFER_GCC=1
#       I will try to fix my gentoo

OPTIMIZE       := 2
DEBUG_OPTIMIZE := 0

WARNING.filter := -Wno-unused-parameter -Wno-switch -Wno-c99-extensions -Wno-c99-designator

CFLAGS   := -std=c23 -pthread ${WARNING.filter}
CXXFLAGS := -std=c++20 -pthread ${WARNING.filter}
CPPFLAGS := -Ilibrary -Ilibrary -D_GNU_SOURCE # -DVERSION=$(shell cat VERSION) -DCOMMIT=$(shell cat .git/refs/heads/master)
LDFLAGS  := -lm -lraylib -lX11 -lglfw -lchad -larchive -lvterm -lenet

SOURCE.dir := source
OBJECT.dir := object

MAKE.dir := tool/make
MAKE.filter := 11-lib.mk 05-peru.mk

SEARCH.dir := ${SOURCE.dir} ${SOURCE.dir}/level-2

## compiler

# Externally overridable with CC=.. CXX=..
PREFER_GCC := 0
PREFER_GDB := 1

## verbose

# Megabroken, define here only if you want the `1' behavior,
# or inline or on command line.
# VERBOSE := 1

## debug

DEBUG          ?= 0
SANITIZE       ?= 0
VECTORIZED_ALL ?= 0
VECTORIZED     ?= 0
DO_LTO         ?= 1 		# always nulled if library

CPPFLAGS += -DDEBUG=${DEBUG}

## peru

PERU_MUST_WORK := 0             # make peru nonoptional

## lib

NOT_APART_OF_LIBRARY :=
#LIBTARGET :=

## bison

LFLAGS += --debug --trace
YFLAGS += --debug

## gperf

# GPERF := gperf

## pch

HEADER.pch.filter :=

## dependency

#DEPEND := $(OBJECT.dir)/.depend

-include ${MAKE.dir}/make.mk
