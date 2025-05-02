PROJECT_NAME     := d20_smartwatch
TARGETS          := d20_smartwatch
OUTPUT_DIRECTORY := build

SDK_ROOT := ../../..
PROJ_DIR := ../code
LIBS_DIR := ../libs

LINKER_SCRIPT  := ldscript.ld

# Collect source files automatically from project and libs directories
PROJ_C := $(shell find $(PROJ_DIR) -type f -name "*.c")
LIBS_C := $(shell find $(LIBS_DIR) -type f -name "*.c" 2>/dev/null || echo "")

# Add paths to VPATH for automatic resolution
VPATH := $(PROJ_DIR) $(LIBS_DIR) $(SDK_ROOT)

# Find all subdirectories for header files in project and libs
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
ALL_PROJ_DIRS := $(sort $(dir $(call rwildcard,$(PROJ_DIR),*)))
ALL_LIBS_DIRS := $(sort $(dir $(call rwildcard,$(LIBS_DIR),* 2>/dev/null || echo "")))

# SDK source files remain explicitly listed
SDK_SRC_FILES += \
    $(SDK_ROOT)/components/driver/driver_ssp.c \
    $(SDK_ROOT)/components/driver/driver_keyscan.c \
    $(SDK_ROOT)/components/driver/driver_pmu_qdec.c \
    $(SDK_ROOT)/components/driver/driver_pmu.c \
    $(SDK_ROOT)/components/driver/driver_rtc.c \
    $(SDK_ROOT)/components/driver/driver_uart.c \
    $(SDK_ROOT)/components/driver/driver_wdt.c \
    $(SDK_ROOT)/components/driver/driver_efuse.c \
    $(SDK_ROOT)/components/modules/platform/source/exception_handlers.c \
    $(SDK_ROOT)/components/modules/platform/source/app_boot_vectors.c \
    $(SDK_ROOT)/components/modules/patch/patch.c

# Combine all source files
SRC_FILES := $(PROJ_C) $(LIBS_C) $(SDK_SRC_FILES)

# Include folders for SDK remain explicitly listed
SDK_INC_FOLDERS += \
  $(SDK_ROOT)/components/ble/include \
  $(SDK_ROOT)/components/driver/include \
  $(SDK_ROOT)/components/modules/os/include \
  $(SDK_ROOT)/components/modules/sys/include \
  $(SDK_ROOT)/components/modules/platform/include \
  $(SDK_ROOT)/components/modules/common/include \
  $(SDK_ROOT)/components/modules/lowpow/include \

# Include base directories for project and libs
PROJECT_INCLUDES = -I$(PROJ_DIR) -I$(LIBS_DIR)

# Add all discovered subdirectories in project and libs for includes
PROJECT_INCLUDES += $(patsubst %,-I%,$(filter-out $(PROJ_DIR)/,$(ALL_PROJ_DIRS)))
PROJECT_INCLUDES += $(patsubst %,-I%,$(filter-out $(LIBS_DIR)/,$(ALL_LIBS_DIRS)))

# Combine all include paths
INC_FOLDERS := $(SDK_INC_FOLDERS) $(PROJ_DIR) $(LIBS_DIR) $(filter-out $(PROJ_DIR)/ $(LIBS_DIR)/,$(ALL_PROJ_DIRS) $(ALL_LIBS_DIRS))

# Libraries common to all targets
LIB_FILES += -lfr8010h_stack

# Optimization flags
OPT = -Os -g

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -mcpu=cortex-m3
CFLAGS += -mthumb
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fmessage-length=0 -fsigned-char
CFLAGS += -std=gnu11

# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m3
ASMFLAGS += -mthumb

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb
LDFLAGS += -mcpu=cortex-m3
LDFLAGS +=  -T$(LINKER_SCRIPT) -L$(SDK_ROOT)/components/ble/library
LDFLAGS += $(SDK_ROOT)/components/ble/library/syscall_gcc.txt
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

# Debug print for checking collected files and directories
print-debug:
	@echo "Project C files:"
	@echo $(PROJ_C)
	@echo "Libs C files:"
	@echo $(LIBS_C)
	@echo "All include directories:"
	@echo $(INC_FOLDERS)