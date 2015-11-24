ifeq ($(wildcard cmt/project.cmt),) 
# we cannot use the CMT version if we do not have a cmt/project.cmt
override USE_CMT =
endif

ifeq ($(USE_CMT),)
include $(LBUTILSROOT)/data/Makefile-cmake.mk
else
include $(LBCONFIGURATIONROOT)/data/Makefile-cmt.mk
endif
