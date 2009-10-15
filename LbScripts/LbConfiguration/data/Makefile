include ${LBUTILSROOT}/data/Makefile

# Set the special environment variable that enable the global lock
# needed during the parallel build (a-la tbroadcast)
export GAUDI_BUILD_LOCK=$(shell pwd)/InstallArea/gaudi_build_lock

# Try to obtain the name of the container package from the file
# project.cmt
ifneq ($(wildcard cmt/project.cmt),)
container := $(shell awk '/^ *container /{print $$2}' cmt/project.cmt)
endif

# Set some special environment variables needed to generate the
# doxygen documentation
project_homes_command := cmt show projects | sed 's/ *\([a-zA-Z0-9]\+\).*(in \([^)]*\)).*/\1 \2/' | awk '{printf("%s_HOME=%s ",$$1,$$2)}'
$(foreach var,$(shell $(project_homes_command)),$(eval export $(var)))

# Target to run the tests
tests: all
ifeq ($(container),)
	$(error Cannot run the tests because I do not know the name of the container package, specify it on the command line)
endif
	cmt -pack=$(container) TestProject

# Generate doxygen documentation
# note the "yes x |" before doxygen, it is neede to avoid stopping at LaTeX errors
docs:
ifeq ($(container),)
	$(error Cannot build the documentation because I do not know the name of the container package)
endif
	cmt -pack=$(container) run 'cd ../doc ; yes x | doxygen DoxyFile.cfg'

# Remove doxygen documentation
clean_docs:
ifeq ($(container),)
	$(warning Documentation not cleand because the container package is not known)
else
	$(RM) -r $(container)/doc/html
endif

# Add the clean_docs target to purge if we know the container package 
ifeq ($(container),)
purge: clean_docs
endif
