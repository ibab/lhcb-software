ifeq ($(wildcard CMakeLists.txt),) 
# we cannot use the CMake version if we do not have a CMakeLists.txt
override USE_CMAKE =
endif

ifeq ($(USE_CMAKE),)
include $(LBCONFIGURATIONROOT)/data/Makefile-cmt.mk
else
include $(LBUTILSROOT)/data/Makefile-cmake.mk
endif
