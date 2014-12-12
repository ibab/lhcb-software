USE_CMAKE=${use_cmake}
-include build.conf

ifneq ($$(USE_CMAKE),)
######## CMake case ########

include $$(LBUTILSROOT)/data/Makefile-cmake.mk

else
######### CMT case #########

CMTPROJECTPATH := ${search_path_env}
ifneq ($$(nightly_slot),)
  CMTPROJECTPATH := $${LHCBNIGHTLIES}/$$(nightly_slot)/$$(nightly_day):$$(CMTPROJECTPATH)
endif

all:
%:
	@env CMTPROJECTPATH="$$(CMTPROJECTPATH)" $$(MAKE) -f $$(LBCONFIGURATIONROOT)/data/Makefile-cmt.mk $$*

endif
