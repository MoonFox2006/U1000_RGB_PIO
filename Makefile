# See Tools/build/Makefile_template for more information
SDK_DIR = C:/WORK/Baikal/SDK

BOARD = EVU_LI_2_1

MEM_REG_ROM = TCMA

CSRC += $(SDK_DIR)/Drivers/HAL/Src/bmcu_eflash.c

# Include the main makefile
include $(SDK_DIR)/Tools/build/common.mk
