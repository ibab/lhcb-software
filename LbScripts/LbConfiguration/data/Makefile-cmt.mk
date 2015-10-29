include ${LBUTILSROOT}/data/Makefile

# Set the special environment variable that enable the global lock
# needed during the parallel build (a-la tbroadcast)
export GAUDI_BUILD_LOCK=$(shell pwd)/InstallArea/gaudi_build_lock

# Try to obtain the name of the container package from the file
# project.cmt
ifneq ($(wildcard cmt/project.cmt),)
container := $(shell awk '/^ *container /{print $$2}' cmt/project.cmt)
endif
ifndef container
container := $(word 1,$(filter %Sys,$(shell echo $(packages) | sed 's_/[^ ]*__g')))
ifdef container
$(warning No container keyword in cmt/project.cmt, so I try $(container).)
endif
endif

# Special hack to generate the python.zip file only once
# - if we have the container in the local directory and we are going to build it
#   we delay the generation of the python.zip file
ifneq ($(wildcard $(container)/cmt/requirements),) # we do have the container
_building_container = false
ifndef MAKECMDGOALS # MAKECMDGOALS empty...
_building_container = true
else
ifneq ($(findstring $(container),$(MAKECMDGOALS)),) # ... or including 'container' ...
_building_container = true
else
ifneq ($(findstring all,$(MAKECMDGOALS)),) # ... or including 'all'
_building_container = true
endif
endif
endif
ifeq ($(_building_container),true)
$(info Adding "no-pyzip" to the CMTEXTRATAGS)
override CMTEXTRATAGS := no-pyzip,$(CMTEXTRATAGS)
ifeq ($(platform),windows)
ifeq ($(find CMTEXTRATAGS,$(make_extra_flags)),)
# This is to ensure that when we add something to CMTEXTRATAGS it is passed
# to nmake explicitely
make_extra_flags += CMTEXTRATAGS="$(CMTEXTRATAGS)"
endif
endif
endif
endif

# Get the name of the project as known by cmt
cmt_project = $(shell cmt show projects | awk '/current/{print $$1}')

# Set some special environment variables needed to generate the
# doxygen documentation (home directories and versions of the projects)
project_homes_command := cmt show projects | sed 's/ *\([a-zA-Z0-9]\+\).*(in \([^)]*\)).*/\1 \2/;s/^\(.*\)_\([^_]*\)/\1_\2 \2/' | awk '{printf("%s_HOME=%s %sVERS=%s ",$$1,$$2,$$1,$$3)}'
$(foreach var,$(shell $(project_homes_command)),$(eval export $(var)))

# Target to run the tests
test:
ifeq ($(container),)
	$(error Cannot run the tests because I do not know the name of the container package, specify it on the command line with "container=Package")
endif
	cmt -pack=$(container) TestProject

# dummy targets to comply with CMake Makefile interface
install: all
	true
unsafe-install:
	true
post-install:
	true

# backward compatibiliy
tests: all
	$(MAKE) test

################################################################################
# Generate doxygen documentation
################################################################################
# The documentation is build only for the current project, the output are generated
# in a temporary directory and copied to InstallArea/doc.
# The tagfile is put in InstallArea/doc as <PROJECT>_doxygen.tag and all the tagfiles
# of the used projects are included (without location).
# LaTeX errors are skipped by the use of the macro ignore_latex_errors_cmd, which
# still show them.
DOXYFILE = DoxyFile.cfg
ifneq ($(ignore_latex_errors),no)
ignore_latex_errors_cmd = yes s |
else
ignore_latex_errors_cmd =
endif
# Location for the teporary directory (use the first found among the alternatives)
doxygen_temp_dir := $(word 1,$(wildcard /build/tmp /build/$(USER)/tmp /build/$(USER) $(TMPDIR) /tmp/$(USER) /tmp .))
DOXYGEN_INPUT=$$PWD
doxygen_tagfiles_command = cmt show projects | grep -v current | sed 's/ *\([a-zA-Z0-9]\+\).*(in \([^)]*\)).*/\2\/InstallArea\/doc\/\1_doxygen.tag/'
DOXYGEN_TAGFILES=$(wildcard $(shell $(doxygen_tagfiles_command)))
# Actual target
docs:
ifeq ($(container),)
	$(error Cannot build the documentation because I do not know the name of the container package)
endif
	@export TMPDOYXGENDIR=`mktemp -d $(doxygen_temp_dir)/doxygen.XXXXXX` ; \
	export TMPDOXYFILE=`mktemp $(doxygen_temp_dir)/doxygen.XXXXXX` ; \
	cat $(container)/doc/$(DOXYFILE) >> $$TMPDOXYFILE ; \
	echo "OUTPUT_DIRECTORY = $$TMPDOYXGENDIR" >> $$TMPDOXYFILE ; \
	echo "GENERATE_TAGFILE = $$TMPDOYXGENDIR/$(cmt_project)_doxygen.tag" >> $$TMPDOXYFILE ; \
	echo "TAGFILES += $(DOXYGEN_TAGFILES)" >> $$TMPDOXYFILE ; \
	echo "INPUT = $(DOXYGEN_INPUT)" >> $$TMPDOXYFILE ; \
	echo "Building doxygen documentation in $$TMPDOYXGENDIR" ; \
	cmt -pack=$(container) run 'cd ../doc ; $(ignore_latex_errors_cmd) doxygen $$TMPDOXYFILE' && \
	( echo "Installing documentation in InstallArea/doc" ; \
	  $(RM) -r InstallArea/doc ; $(MKDIR) -p InstallArea/doc ; \
	  cp -fa $$TMPDOYXGENDIR/* InstallArea/doc ) ; \
	echo "Removing temporary files" ; \
	$(RM) -r $$TMPDOYXGENDIR $$TMPDOXYFILE

# Remove doxygen documentation
clean_docs:
	$(RM) -r InstallArea/doc

# Add the clean_docs target to purge if we know the container package
purge: clean_docs

# Include optional custom makefile from the project
ifneq ($(container),)
-include $(container)/cmt/Custom.mk
endif
