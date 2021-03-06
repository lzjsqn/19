#!/usr/bin/make -f
#############################################################
# Copyright 1998 VMware, Inc.  All rights reserved. -- VMware Confidential
#############################################################
####
####  VMware kernel module Makefile to be distributed externally
####

####
#### SRCROOT _must_ be a relative path.
####
SRCROOT = .

VM_UNAME = $(shell uname -r)

# Header directory for the running kernel
HEADER_DIR = /lib/modules/$(VM_UNAME)/build/include

BUILD_DIR = $(HEADER_DIR)/..

DRIVER := vmxnet

# Grep program
GREP = /bin/grep

vm_check_gcc = $(shell if $(CC) $(1) -S -o /dev/null -xc /dev/null \
        > /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi)
vm_check_file = $(shell if test -f $(1); then echo "yes"; else echo "no"; fi)

ifndef VM_KBUILD
VM_KBUILD := no
ifeq ($(call vm_check_file,$(BUILD_DIR)/Makefile), yes)
ifneq ($(call vm_check_file,$(BUILD_DIR)/Rules.make), yes)
VM_KBUILD := 26
endif
endif
export VM_KBUILD
endif

ifndef VM_KBUILD_SHOWN
ifeq ($(VM_KBUILD), no)
VM_DUMMY := $(shell echo >&2 "Using standalone build system.")
else
ifeq ($(VM_KBUILD), 24)
VM_DUMMY := $(shell echo >&2 "Using 2.4.x kernel build system.")
else
VM_DUMMY := $(shell echo >&2 "Using 2.6.x kernel build system.")
endif
endif
VM_KBUILD_SHOWN := yes
export VM_KBUILD_SHOWN
endif

ifneq ($(VM_KBUILD), no)

VMCCVER := $(shell $(CC) -dumpversion)

# If there is no version defined, we are in toplevel pass, not yet in kernel makefiles...
ifeq ($(VERSION),)

ifeq ($(VM_KBUILD), 24)
DRIVER_KO := $(DRIVER).o
else
DRIVER_KO := $(DRIVER).ko
endif

.PHONY: $(DRIVER_KO)

auto-build: $(DRIVER_KO)
	cp -f $< $(SRCROOT)/../$(DRIVER).o

# $(DRIVER_KO) is a phony target, so compare file times explicitly
$(DRIVER): $(DRIVER_KO)
	if [ $< -nt $@ ]; then cp -f $< $@; fi

# Pass gcc version down the chain, so we can detect if kernel attempts to use unapproved compiler
VM_CCVER := $(VMCCVER)
export VM_CCVER
VM_CC := $(CC)
export VM_CC

MAKEOVERRIDES := $(filter-out CC=%,$(MAKEOVERRIDES))

$(DRIVER_KO):
	make -C $(BUILD_DIR) SUBDIRS=$$PWD SRCROOT=$$PWD/$(SRCROOT) modules

else

ifneq ($(VM_CCVER), $(VMCCVER))
$(warning *** Inappropriate build environment: you wanted to use gcc \
 version $(VM_CCVER) while kernel attempts to use gcc version $(VMCCVER).)
$(error For proper build you'll have to replace $(CC) with symbolic \
 link to $(VM_CC))
endif

endif

include $(SRCROOT)/Makefile.kernel

ifdef TOPDIR
ifeq ($(VM_KBUILD), 24)

O_TARGET := $(DRIVER).o

obj-y := $($(DRIVER)-y)

include $(TOPDIR)/Rules.make
endif
endif

else

include $(SRCROOT)/Makefile.normal

endif

#.SILENT:
