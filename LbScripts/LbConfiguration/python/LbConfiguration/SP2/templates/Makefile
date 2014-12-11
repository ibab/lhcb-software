USE_CMAKE=${use_cmake}
ifneq ($$(USE_CMAKE),)
# CMake case
include $$(LBUTILSROOT)/data/Makefile-cmake.mk
else
# CMT case
all:
%:
	@. ./build_env.sh && $$(MAKE) -f $$(LBCONFIGURATIONROOT)/data/Makefile-cmt.mk $$*
endif
