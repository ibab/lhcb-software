################################################################################
#
# Generic Makefile to simplify the use of CMake projects.
#
# This simple Makefile is meant to provide a simplified entry point for the
# configuration and build of CMake-based projects that use a default toolchain
# (as it is the case for Gaudi-based projects).
#
# Only a few targets are actually provided: all the main targets are directly
# delegated to the CMake Makefile.
#
# Main targets:
#
#     all
#         (default) build everything
#  
#     test
#         run the declared tests
#
#     install
#         populate the InstallArea with the products of the build
#
#     clean
#         remove build products from the build directory
#
#     purge [*]
#         deep clean of the build, including InstallArea
#         (requires re-configuration)
#
#     help
#         print the list of available targets
#
#     configure [*]
#         alias to CMake 'rebuild_cache' target
#
# .. [*]: targets defined by this Makefile. 
################################################################################

# settings
PROJDIR := $(shell pwd)
ifneq ($(wildcard $(PROJDIR)/toolchain.cmake),)
override CMAKEFLAGS += -DCMAKE_TOOLCHAIN_FILE=$(PROJDIR)/toolchain.cmake
endif
BUILDDIR := $(PROJDIR)/build.$(CMTCONFIG)

# default target
all:

# deep clean
purge:
	$(RM) -r $(BUILDDIR) $(PROJDIR)/InstallArea/$(CMTCONFIG)
	find $(PROJDIR) -name "*.pyc" -exec $(RM) -v \{} \;

# delegate any target to the build directory (except 'purge')
ifneq ($(MAKECMDGOALS),purge)
%: $(BUILDDIR)/Makefile
	$(MAKE) -C build.$(CMTCONFIG) $*
endif

# aliases
ifneq ($(wildcard $(BUILDDIR)/Makefile),)
configure: rebuild_cache
else
configure:
endif
	@ # do not delegate further

# trigger CMake configuration
$(BUILDDIR)/Makefile:
	mkdir -p $(BUILDDIR)
	cd $(BUILDDIR) && cmake $(CMAKEFLAGS) $(PROJDIR)
