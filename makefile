NAME= amd_temp
TYPE= DRIVER

SRCS= amd_temp.cpp
RSRCS= 

LIBS=
LIBPATHS= 
SYSTEM_INCLUDE_PATHS = 
LOCAL_INCLUDE_PATHS = 
OPTIMIZE= FULL
DEFINES= 
WARNINGS = ALL
SYMBOLS = 
DEBUGGER = 
COMPILER_FLAGS = -Wno-unused
LINKER_FLAGS =
APP_VERSION = 
DRIVER_PATH = sensors

## include the makefile-engine
include $(BUILDHOME)/etc/makefile-engine
