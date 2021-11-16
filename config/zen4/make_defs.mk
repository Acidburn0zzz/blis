#
#
#  BLIS
#  An object-based framework for developing high-performance BLAS-like
#  libraries.
#
<<<<<<< HEAD
#  Copyright (C) 2022, Advanced Micro Devices, Inc. All rights reserved.
=======
#  Copyright (C) 2021, Advanced Micro Devices, Inc. All rights reserved.
>>>>>>> 06113811... Added support for zen4 architecture
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#   - Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#   - Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#   - Neither the name(s) of the copyright holder(s) nor the names of its
#     contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#

# FLAGS that are specific to the 'zen3' architecture are added here.
# FLAGS that are common for all the AMD architectures are present in
# config/zen/amd_config.mk.

# Declare the name of the current configuration and add it to the
# running list of configurations included by common.mk.
THIS_CONFIG    := zen4
#CONFIGS_INCL   += $(THIS_CONFIG)

#
# --- Determine the C compiler and related flags ---
#

# NOTE: The build system will append these variables with various
# general-purpose/configuration-agnostic flags in common.mk. You
# may specify additional flags here as needed.

<<<<<<< HEAD
CPPROCFLAGS    :=
=======
# Since we removed BLIS_CONFIG_EPYC from header file, we need to
# add it here at two places,
#     CPPROCFLAGS = This will enable it for framework code
#                   This flag is used when configure is invoked with specific architecture
#     CKOPTFLAGS  = This will enable it for architecture specific kernels
#                   This flag is used for kernels assocaited with this architecture
#                   irrespective of the configuration it is built for.

CPPROCFLAGS    := -DBLIS_CONFIG_EPYC
>>>>>>> 06113811... Added support for zen4 architecture
CMISCFLAGS     :=
CPICFLAGS      :=
CWARNFLAGS     :=

ifneq ($(DEBUG_TYPE),off)
CDBGFLAGS      := -g
endif

ifeq ($(DEBUG_TYPE),noopt)
COPTFLAGS      := -O0
else
COPTFLAGS      := -O3
endif

# Flags specific to optimized kernels.
# NOTE: The -fomit-frame-pointer option is needed for some kernels because
# they make explicit use of the rbp register.
CKOPTFLAGS     := $(COPTFLAGS) -fomit-frame-pointer
ifeq ($(CC_VENDOR),gcc)
GCC_VERSION := $(strip $(shell $(CC) -dumpversion | cut -d. -f1))
# gcc or clang version must be atleast 4.0
# gcc 9.0 or later:
ifeq ($(shell test $(GCC_VERSION) -ge 11; echo $$?),0)
CKVECFLAGS     += -march=znver3
else
ifeq ($(shell test $(GCC_VERSION) -ge 9; echo $$?),0)
CKVECFLAGS     += -march=znver2
else
# If gcc is older than 9.1.0 but at least 6.1.0, then we can use -march=znver1
# as the fallback option.
CRVECFLAGS += -march=znver1 -mno-avx256-split-unaligned-store
CKVECFLAGS += -march=znver1 -mno-avx256-split-unaligned-store
endif # GCC 9
endif # GCC 11
else
ifeq ($(CC_VENDOR),clang)

# AOCC clang has various formats for the version line

# AOCC.LLVM.2.0.0.B191.2019_07_19 clang version 8.0.0 (CLANG: Jenkins AOCC_2_0_0-Build#191) (based on LLVM AOCC.LLVM.2.0.0.B191.2019_07_19)
# AOCC.LLVM.2.1.0.B1030.2019_11_12 clang version 9.0.0 (CLANG: Build#1030) (based on LLVM AOCC.LLVM.2.1.0.B1030.2019_11_12)
# AMD clang version 10.0.0 (CLANG: AOCC_2.2.0-Build#93 2020_06_25) (based on LLVM Mirror.Version.10.0.0)
# AMD clang version 11.0.0 (CLANG: AOCC_2.3.0-Build#85 2020_11_10) (based on LLVM Mirror.Version.11.0.0)
# AMD clang version 12.0.0 (CLANG: AOCC_3.0.0-Build#2 2020_11_05) (based on LLVM Mirror.Version.12.0.0)

# For our prupose we just want to know if it version 2x or 3x

# for version 3x we will enable znver3
ifeq ($(strip $(shell $(CC) -v |&head -1 |grep -c 'AOCC_3')),1)
CKVECFLAGS += -march=znver3
else
# for version 2x we will enable znver2
ifeq ($(strip $(shell $(CC) -v |&head -1 |grep -c 'AOCC.LLVM.2\|AOCC_2')),1)
CKVECFLAGS += -march=znver2
else
#if compiling with clang
VENDOR_STRING := $(strip $(shell ${CC_VENDOR} --version | egrep -o '[0-9]+\.[0-9]+\.?[0-9]*'))
CC_MAJOR := $(shell (echo ${VENDOR_STRING} | cut -d. -f1))
#clang 9.0 or later:
ifeq ($(shell test $(CC_MAJOR) -ge 9; echo $$?),0)
CKVECFLAGS += -march=znver2
else
CKVECFLAGS += -march=znver1
endif # ge 9
endif # aocc 2
endif # aocc 3
endif # clang
endif # gcc

# Flags specific to reference kernels.
CROPTFLAGS     := $(CKOPTFLAGS)
CRVECFLAGS     := $(CKVECFLAGS)

<<<<<<< HEAD
=======
# Add this after updating variables for reference kernels
# we don't want this defined for them
CKOPTFLAGS += -DBLIS_CONFIG_EPYC

>>>>>>> 06113811... Added support for zen4 architecture
# Store all of the variables here to new variables containing the
# configuration name.
$(eval $(call store-make-defs,$(THIS_CONFIG)))

