#======== THE FOLLOWING HAVE TO BE OVERIDDEN TO CHANGE BUILD's DEFAULT ========#

K1_ERIKA_FILES    ?= .
K1_ERIKA_PKG2     ?= ${K1_ERIKA_FILES}/rtos/ee/pkg2
K1_ERIKA_APP_NAME ?= erika
K1_ERIKA_APP_DIR  ?= ${K1_ERIKA_PKG2}/examples/k1/test
K1_ERIKA_APP_SRCS ?= $(addprefix ${K1_ERIKA_APP_DIR}, /test.c /test_job.c)

K1_MULTIBIN_APP_NAME ?= mppa.mpk

export K1_OUTPTDIR ?= $(if ${O},${O},output)

#========= THE ABOVE HAVE TO BE OVERIDDEN TO CHANGE BUILD's DEFAULT ==========#

# Needed for Ubuntu/Debian since sh points to dash instead of bash...
SHELL := /bin/bash

K1_TOOLCHAIN_DIR := $(shell echo $${K1_TOOLCHAIN_DIR:-/usr/local/k1tools/})

K1_ERIKA_SRCS := $(addprefix ${K1_ERIKA_PKG2}, \
  /cpu/kalray_k1/hal/ee_k1_asm.S \
  /common/utils/ee_assert.c \
  /cpu/kalray_k1/bsp/ee_k1_irq.c \
  /cpu/kalray_k1/bsp/ee_k1_cluster_init.c \
  /cpu/kalray_k1/bsp/ee_k1_rm_task_server.c \
  /cpu/kalray_k1/bsp/ee_k1_startup_handlers.c \
  /cpu/kalray_k1/bsp/ee_k1_bsp_communication.c \
  /cpu/kalray_k1/bsp/eecfg.c \
  /cpu/kalray_k1/bsp/ee_hal_init.c \
  /hal/kernel/ee_hal_std_change_context.c \
  /kernel/bsp/oo/ee_scheduler.c \
  /kernel/bsp/oo/ee_api_dynamic.c \
  /kernel/bsp/oo/ee_api_extension.c \
  /kernel/bsp/oo/ee_api_osek.c \
  /kernel/bsp/oo/ee_scheduler_entry_points.c \
  /kernel/bsp/k1/ee_api_k1.c \
  /kernel/bsp/k1/ee_kernel_k1.c)\
  ${K1_ERIKA_FILES}/libgomp/root.c\
  ${K1_ERIKA_FILES}/libgomp/context.S\
  ${K1_ERIKA_FILES}/libgomp/offload.c

K1_ERIKA_INCLUDE_DIRS := ${K1_ERIKA_APP_DIR} \
  $(addprefix ${K1_ERIKA_PKG2}, /common/compilers /common/compilers/gcc \
    /common/utils /cpu/kalray_k1/bsp /cpu/kalray_k1/hal /hal/kernel\
    /kernel/bsp /kernel/bsp/k1 /kernel/bsp/oo) \
  ${K1_ERIKA_FILES}/libgomp ${K1_ERIKA_FILES}/libpsocoffload\
  ${K1_ERIKA_FILES}/libgomp/config ${K1_ERIKA_FILES}/libgomp/config/k1

K1_CLUSTER_INCLUDE_DIRS_OPT := $(addprefix -I, ${K1_ERIKA_INCLUDE_DIRS} ${K1_ERIKA_APP_INCLUDE_DIRS})

board := developer

# This variable is specific to the mppaipc since it specifies which flavor of
# the mppaipc you want to compile
# The possible values are:
#       - simulator: to use the simulator interface for host (ie the mppa will
#                    be emulated by the simulator k1-mppa)
#       - hardware: to use the interface with the real hardware
#       - native: to use the native implementation of mppaipc (ie: nothing will
#                 be compiled using k1-gcc)
# default if not specified is hardware
# Note that you can specify multiple platforms :

platforms := hardware simulator

# Define weither we are using OSes or not. can be os/rtems/nodeos/bare
# If not present, default to os
system-name := os
## EG: Used to plug ERIKA
cluster-system := bare

# Common cflags and lflags (for all targets)
# These flags will be propagated through the hierarchy
cflags = -std=c99 -Wall -g -O -I${BUILD_DIR}
lflags := -lm

# Common flags for k1 binaries
#k1-cflags += -DBAR_SIGWAIT -DTASKING_ENABLED -DTASK_BARRIER_BUSY_WAITING #-fopenmp
#k1-lflags += -Wl,--verbose

# The mppa-bin target is used to generate multibinary for specified target
mppa-bin := K1_MULTIBIN
K1_MULTIBIN-name := ${K1_MULTIBIN_APP_NAME}
# Define which binaries will composed multibin1
# Note that the first binary in the list defines the boot binary
K1_MULTIBIN-objs := ${K1_IO_APP_NAME} ${K1_ERIKA_APP_NAME}

# You can pass additionnal flags to "k1-create-multibinary" using the
# following command.
# for example, '-w ".*"' will active all tracepoints
#K1_MULTIBIN-flags := -w ".*"

# Cluster binaries
cluster-bin := ${K1_ERIKA_APP_NAME}
# Each *-srcs variable describe sources for the binaries
${K1_ERIKA_APP_NAME}-srcs := ${K1_ERIKA_SRCS} ${K1_ERIKA_APP_SRCS}

# Flags can be specified per binary
${K1_ERIKA_APP_NAME}-cflags :=   ${K1_CLUSTER_INCLUDE_DIRS_OPT}\
  -Wno-unused-function -save-temps=obj
${K1_ERIKA_APP_NAME}-lflags := -Wl,-Map="${K1_OUTPTDIR}/build/${K1_ERIKA_APP_NAME}.map"

ifneq (${K1_IO_APP_SRCS},)
# The io-bin var is used to build groups of var
io-bin := $(if ${K1_IO_APP_NAME},${K1_IO_APP_NAME},io-app)
# Common cflags for io sources
io-cflags :=
io-lflags :=

${K1_IO_APP_NAME}-srcs := ${K1_IO_APP_SRCS}
# Flags can be specified per binary
${K1_IO_APP_NAME}-cflags :=
${K1_IO_APP_NAME}-lflags := -Wl,-Map="${K1_OUTPTDIR}/build/${K1_IO_APP_NAME}.map"
else
ifneq (${K1_IO_APP_NAME},)
${K1_OUTPTDIR}/build/${K1_IO_APP_NAME}: ${K1_IO_APP_DIR}/${K1_IO_APP_NAME}
	@echo "Copy EXTERNAL Provided IO ELF"
	cp -r $< $(@D)
endif
endif

# Special hooks for user makefile

# Hooks specified in help-hooks are rules which will be called after displaying
# the default help
help-hooks := my_help_rule
# Pre build hooks will be called before build
pre-build-hooks =
# Post build hooks will be called after build
post-build-hooks := post_build_rule

# Finally, include the Kalray Makefile which will instantiate the rules
# and do the work
include ${K1_TOOLCHAIN_DIR}/share/make/Makefile.mppaipc

# Define your rules after the include, otherwise, the first rule will be treated
# as the default rule
my_help_rule:
	@echo Additional Targets:
	@echo "  clean-traces    - clean generated traces"
	@echo "  run-erika-sim   - run ERIKA application on cluster simulator"
	@echo "  run-erika-hw    - run ERIKA application on cluster developer board"
	@echo "  run-mppa-hw     - run MPPA (IO + ERIKA on cluster) on development board"

post_build_rule:
	@echo "Build done"

.PHONY: run-erika-sim
run-erika-sim: all
	${K1_TOOLCHAIN_DIR}/bin/k1-cluster --mboard=developer --mcluster=node --cycle-based ${RUN-FLAGS} -- ${K1_OUTPTDIR}/build/${K1_ERIKA_APP_NAME}

.PHONY: run-erika-hw
run-erika-hw: all
	${K1_TOOLCHAIN_DIR}/bin/k1-jtag-runner --exec-file=Cluster0:${K1_OUTPTDIR}/build/${K1_ERIKA_APP_NAME} ${RUN-FLAGS} --

.PHONY: run-mppa-hw
run-mppa-hw: all
	${K1_TOOLCHAIN_DIR}/bin/k1-jtag-runner --multibinary=${K1_OUTPTDIR}/bin/${K1_MULTIBIN-name} --exec-multibin=IODDR0:${K1_IO_APP_NAME} ${RUN-FLAGS} --

.PHONY: clean-traces
clean-traces:
	rm -rf MPPA.*.DSU.*
