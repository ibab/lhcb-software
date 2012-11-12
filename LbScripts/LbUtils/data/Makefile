################################################################################
#
# Generic Makefile for CMT projects.
#
# copied to the top-level directory of a project (even user project), it can be
# used to start a build of the whole project with just a call to "make".
# It is equivalent to a "cmt broadcast make", or, if invoked with "-j" to a
# "tbroadcast make -j", but better.
#
# Targets:
#   all: build all the packages (default)
#   clean: clean all the packages
#   purge: clean and remove the files by the Makefile
#   <packagename>: build the package and all the packages it depends on
#
# Arguments:
#   PKG_ONLY : if set, build only the the package specified as target
#   no_all_groups : if set, build only the main cmt group (all) and not all the
#                   available ones
#   LOG_LOCATION : optional directory for the build log files, dependencies etc.
#                  if not specified, the current directory is used for the
#                  dependencies and build logs are put in the cmt directory
#   Package_failure_policy : can be set to stop, skip or ignore (see later
#                            comment for details)
#   logging : enabled (default on unix) or disabled (only possibility on windows)
#
# This makefile works on Windows as long as you install some Unix standard
# commands (apart from a standard LHCb environment):
#
#  - make
#    http://gnuwin32.sourceforge.net/packages/make.htm
#  - awk (gawk)
#    http://gnuwin32.sourceforge.net/packages/gawk.htm
#  - sed
#    http://gnuwin32.sourceforge.net/packages/sed.htm
#  - coreutils (for rm, uname, tee)
#    http://gnuwin32.sourceforge.net/packages/coreutils.htm
#  - bash
#    http://win-bash.sourceforge.net/
#    Note: this version is very limited, but we can leverage on the power of
#          gnu make.
#
# Note: on Windows, because of limitations of the bash interpreter, the log
#       of the build is not enabled by default.
#       Moreover, the "-j" switch of make is not supported.
#
# Author: Marco Clemencic
#
################################################################################

# --- Simplistic platform detection
platform := $(if $(findstring windows,$(shell uname)),windows,unix)

# --- List of Packages in the current directory
ifeq ($(platform),windows)
# we do not have 'find' on Windows, so we stick to at most one level of hats
packages := $(strip $(subst /cmt/requirements,,$(wildcard */cmt/requirements) $(wildcard */*/cmt/requirements)))
else
packages := $(strip $(subst /cmt/requirements,,$(shell find . -noleaf -path "*/cmt/requirements" -a -not -path "*/tests/data/*" | sed 's/.\///') ))
endif

# --- Utility functions and variables
escape_slash = $(subst /,_slash_,$(1))
unescape_slash = $(subst _slash_,/,$(1))
full_name = $(subst /,_,$(1))
COMMA := ,

#  Note: The command "cmt show groups" does not work in old versions of CMT
cmt_version := $(shell cmt version)
cmt2008 = $(findstring v1r20p2008, $(cmt_version))

# ATLAS cmt tag implies no_all_groups and the use of LOG_LOCATION
ifeq ($(filter ATLAS,$(subst $(COMMA), ,$(CMTEXTRATAGS))),ATLAS)
  no_all_groups = yes
  ifndef LOG_LOCATION
    LOG_LOCATION := LOG_$(CMTCONFIG)
  endif
  make_extra_flags += --no-print-directory
endif


ifeq ($(no_all_groups),)
  ifeq (,$(cmt2008))
    groups = all $(shell cd $(1)/cmt ; cmt show groups)
  else
    groups = all all_groups
  endif
else
  # build only the main group
  groups = all
endif

# --- Common configuration variables
# The special environment variable PIPESTATUS is a bash feature, so we have to force
# the usage of that shell.
ifeq ($(platform),windows)
  SHELL=bash.exe
else
  SHELL=/bin/bash --norc
endif
# Name of the makefile to generate
ifeq ($(platform),windows)
Makefile := NMake
else
Makefile := Makefile
endif
# Name of the mkdir command
ifneq ($(findstring winxp-vc9,$(CMTCONFIG)),)
MKDIR := gmkdir
else
MKDIR := mkdir
endif
# Banners to be printed for the targets.
define build_banner
echo "#==============================================================================="
echo "# Building package $(1) ["`ls $(dependency_dir) | grep -c ".*\.pack\.build"`"/$(words $(packages))]"
echo "#==============================================================================="
endef
define build_banner_in_file
echo "#===============================================================================" > $(2)
echo "# Building package $(1) ["`ls $(dependency_dir) | grep -c ".*\.pack\.build"`"/$(words $(packages))]" >> $(2)
echo "#===============================================================================" >> $(2)
endef
define clean_banner
echo "#==== Cleaning package $(1)"
endef

# -- Special macro to change the behavior on a failure of a package
#    (complementary to the option '-k')
#  Allowed values for Package_failure_policy:
#    stop:   do not continue (default)
#    skip:   if a group fails, continue with the next package
#    ignore: try all the groups even if one fails before going to the next package
#            (may result in duplicated errors within a package)
Package_failure_handler = test $$BUILD_RESULT -eq 0 || exit $$BUILD_RESULT ;
ifdef Package_failure_policy
ifeq ($(Package_failure_policy),stop)
Package_failure_handler = test $$BUILD_RESULT -eq 0 || exit $$BUILD_RESULT ;
else
ifeq ($(Package_failure_policy),skip)
Package_failure_handler = test $$BUILD_RESULT -eq 0 || break ;
else
ifeq ($(Package_failure_policy),ignore)
Package_failure_handler =
else
$(error Unknown Package_failure_policy "$(Package_failure_policy)". Allowed values: "stop" (default), "skip", "ignore")
endif
endif
endif
endif

# -- Macro to enable/disable logging of the build
#    (there are problems with the logging on Windows)
ifeq ($(platform),windows)
logging = disabled
else
logging = enabled
endif

ifeq ($(platform),windows)
# -- On Windows we have to unset the environment variable MAKEFLAGS because the
#    options to GNU Make are not compatible with NMake.
#strip_makeflags = export MAKEFLAGS=$(subst j,,$(subst j 1,,$(MAKEFLAGS))) ;
strip_makeflags = export MAKEFLAGS= ;
# -- On Windows, NMake ignore the environment variables, so we have to explicitly
#    pass CMTEXRATAGS and CMTUSERCONTEXT on the command line.
ifdef CMTEXTRATAGS
make_extra_flags += CMTEXTRATAGS="$(CMTEXTRATAGS)"
endif
ifdef CMTUSERCONTEXT
make_extra_flags += CMTUSERCONTEXT="$(CMTUSERCONTEXT)"
endif
endif

# Fix the separator for environment variables
# (needed if the variables are specified as lists in Eclipse)
override CMTEXTRATAGS := $(subst ;,$(COMMA),$(CMTEXTRATAGS))
ifneq ($(platform),windows)
override CMTPROJECTPATH := $(subst ;,:,$(CMTPROJECTPATH))
endif

# --- Declare the variables that have to be exported
export PATH LD_LIBRARY_PATH PYTHONPATH CMTEXTRATAGS CMTPROJECTPATH CMTUSERCONTEXT

# --- Declare directories for dependencies, stamp files and log files
dependency_dir = .$(CMTCONFIG).d
logfile = $(CURDIR)/$(1)/cmt/build.$(CMTCONFIG).log
ifdef LOG_LOCATION
  dependency_dir = $(LOG_LOCATION)/.$(CMTCONFIG).d
  logfile = $(LOG_LOCATION)/$(call full_name,$(1)).log
endif
pkg_build_flag = $(dependency_dir)/$(call full_name,$(1)).pack.build

# --- Main targets
all: $(packages:=_build)

clean: $(packages:=_clean)
	$(RM) -r InstallArea/$(CMTCONFIG)
ifdef LOG_LOCATION
	$(RM) -r $(LOG_LOCATION)
endif

# This hack is needed because the stand-alone bash implementation on Windows is very limited (short command line)
purge: clean $(foreach dep,$(wildcard $(dependency_dir)/*.pack.d),$(dep)_do_purge) $(packages:=/cmt/$(Makefile)_do_purge)
	$(RM) -r $(dependency_dir)
	find $(PWD) -name "*.pyc" -exec $(RM) \{} \;
%_do_purge: clean
	$(RM) $*

# This should be the actual implementation
#purge: clean
#	$(RM) $(dependency_dir)/*.pack.d $(packages:=/cmt/$(Makefile))
#	find $(PWD) -name "*.pyc" -exec $(RM) \{} \;

build_flags = $(wildcard $(dependency_dir)/*.pack.build)
remove_build_flags:
ifneq ($(build_flags),)
	@$(RM) -r $(build_flags)
endif

# --- Add rules to build packages (e.g. "make MyHat/MyPackage")
$(foreach pack,$(packages),$(eval $(pack): $(pack)_build))
$(foreach pack,$(packages),$(eval .PHONY: $(pack)))
# --- Prevents removal of the Makefiles generated by "cmt config"
$(foreach pack,$(packages),$(eval .PRECIOUS: $(pack)/cmt/$(Makefile)))
ifeq (,$(cmt2008))
$(foreach pack,$(packages),$(eval .PRECIOUS: $(pack)/cmt/version.cmt))
endif

# --- Actual rules
%/cmt/$(Makefile):
	@echo Configuring package $*
	@cd $*/cmt && cmt config


ifeq (,$(cmt2008))
%/cmt/version.cmt: %/cmt/requirements
	@echo Generate $@
	@awk 'BEGIN{version="v*"}/^ *version/{version = $$2}END{print version}' $< > $@
%_build: remove_build_flags %/cmt/$(Makefile) %/cmt/version.cmt
else
%_build: remove_build_flags %/cmt/$(Makefile)
endif
	@touch $(call pkg_build_flag,$*)
	@$(call build_banner,$*)
ifeq ($(logging),enabled)
	@$(call build_banner_in_file,$*,$(call logfile,$*))
	@date >> $(call logfile,$*)
	@echo `cat $*/cmt/version.cmt`" "$* >> $(call logfile,$*)
	@PWD=`pwd`
	@+cd $*/cmt ; $(strip_makeflags) \
	for group in $(call groups,$*) ; do \
	    $(if $(no_all_groups),,echo "# Building group $$group" | tee -a $(call logfile,$*) ;) \
		cmt make $(make_extra_flags) $$group 2>&1 | tee -a $(call logfile,$*) ; export BUILD_RESULT=$$PIPESTATUS ; \
		date >> $(call logfile,$*) ; \
		$(Package_failure_handler) \
	done
else
	@+cd $*/cmt ; $(strip_makeflags) \
	for group in $(call groups,$*) ; do \
	    $(if $(no_all_groups),,echo "# Building group $$group";) \
		cmt make $(make_extra_flags) $$group ; export BUILD_RESULT=$$? ; \
		$(Package_failure_handler) \
	done
endif

ifeq (,$(cmt2008))
%_clean: %/cmt/$(Makefile) %/cmt/version.cmt
else
%_clean: %/cmt/$(Makefile)
endif
	@$(call clean_banner,$*)
	@cd $*/cmt && cmt make clean binclean


## --- Dependencies between packages
# The PKG_ONLY macro can be used to build a single package disregarding the
# dependencies between packages.
ifndef PKG_ONLY
MKDEPCMD := cmt show uses | sed 's/^\# */\# /' | tr " " "\t" | awk -F"\t" '/^\#\t*use/{if($$5){print $$5"/"$$3}else{print $$3}}'
$(dependency_dir)/%.pack.d: pack=$(call unescape_slash,$*)
$(dependency_dir)/%.pack.d:
	@echo Computing dependencies for package $(pack)
	@$(MKDIR) -p $(dependency_dir)
	@echo $(pack)_build: $(patsubst %,%_build,$(filter $(packages),$(shell cd $(pack)/cmt; $(MKDEPCMD)))) > $@
	@echo $@: $(pack)/cmt/requirements >> $@

ifneq ($(MAKECMDGOALS),purge)
-include $(patsubst %,$(dependency_dir)/%.pack.d,$(call escape_slash, $(packages)))
endif
deps:
	@#fake target to force the build of the dependencies (implied by the include)
endif

# --- List of phony targets
.PHONY: all clean purge deps remove_build_flags
# This makes the package targets PHONY (.PHONY does not work with implicit rules)
$(foreach pack,$(packages),$(eval $(pack)_config $(pack)_build $(pack)_clean: FORCE))
FORCE:
