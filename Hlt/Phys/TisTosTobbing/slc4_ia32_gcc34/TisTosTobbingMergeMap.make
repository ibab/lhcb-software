#-- start of make_header -----------------

#====================================
#  Document TisTosTobbingMergeMap
#
#   Generated Thu Aug  9 03:23:35 2007  by tskwarni
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_TisTosTobbingMergeMap_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_TisTosTobbingMergeMap_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_TisTosTobbingMergeMap

TisTosTobbing_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_TisTosTobbingMergeMap = /tmp/CMT_$(TisTosTobbing_tag)_TisTosTobbingMergeMap.make$(cmt_lock_pid)
else
cmt_local_tagfile_TisTosTobbingMergeMap = $(TisTosTobbing_tag)_TisTosTobbingMergeMap.make
endif

else

tags      = $(tag),$(CMTEXTRATAGS)

TisTosTobbing_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_TisTosTobbingMergeMap = /tmp/CMT_$(TisTosTobbing_tag).make$(cmt_lock_pid)
else
cmt_local_tagfile_TisTosTobbingMergeMap = $(TisTosTobbing_tag).make
endif

endif

-include $(cmt_local_tagfile_TisTosTobbingMergeMap)

ifdef cmt_TisTosTobbingMergeMap_has_target_tag

ifdef READONLY
cmt_final_setup_TisTosTobbingMergeMap = /tmp/CMT_TisTosTobbing_TisTosTobbingMergeMapsetup.make
cmt_local_TisTosTobbingMergeMap_makefile = /tmp/CMT_TisTosTobbingMergeMap$(cmt_lock_pid).make
else
cmt_final_setup_TisTosTobbingMergeMap = $(bin)TisTosTobbing_TisTosTobbingMergeMapsetup.make
cmt_local_TisTosTobbingMergeMap_makefile = $(bin)TisTosTobbingMergeMap.make
endif

else

ifdef READONLY
cmt_final_setup_TisTosTobbingMergeMap = /tmp/CMT_TisTosTobbingsetup.make
cmt_local_TisTosTobbingMergeMap_makefile = /tmp/CMT_TisTosTobbingMergeMap$(cmt_lock_pid).make
else
cmt_final_setup_TisTosTobbingMergeMap = $(bin)TisTosTobbingsetup.make
cmt_local_TisTosTobbingMergeMap_makefile = $(bin)TisTosTobbingMergeMap.make
endif

endif

ifdef READONLY
cmt_final_setup = /tmp/CMT_TisTosTobbingsetup.make
else
cmt_final_setup = $(bin)TisTosTobbingsetup.make
endif

TisTosTobbingMergeMap ::


ifdef READONLY
TisTosTobbingMergeMap ::
	@echo tags=$(tags)
	@echo cmt_local_tagfile=$(cmt_local_tagfile)
endif


dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	@echo 'TisTosTobbingMergeMap'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = TisTosTobbingMergeMap/
TisTosTobbingMergeMap::
	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
	@echo "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

#-- end of make_header ------------------

# File: cmt/fragments/merge_rootmap_header
# Author: Sebastien Binet (binet@cern.ch)

# Makefile fragment to merge a <library>.rootmap file into a single
# <project>.rootmap file in the (lib) install area
# If no InstallArea is present the fragment is dummy


.PHONY: TisTosTobbingMergeMap TisTosTobbingMergeMapclean

# default is already '#'
#genmap_comment_char := "'#'"

ifdef CMTINSTALLAREA
rootMapDir    := ${CMTINSTALLAREA}/$(tag)/lib
mergedRootMap := $(rootMapDir)/$(project).rootmap
stampRootMap  := $(rootMapDir)/TisTosTobbing.rootmap.stamp
else
rootMapDir    := ../$(tag)
mergedRootMap := 
stampRootMap  :=
endif

rootMapRef    := ../$(tag)/TisTosTobbing.rootmap

TisTosTobbingMergeMap :: $(stampRootMap) $(mergedRootMap)
	@:

$(stampRootMap) $(mergedRootMap) :: $(rootMapRef)
	@echo "Running merge_rootmap  TisTosTobbingMergeMap" 
	$(merge_rootmap_cmd) --do-merge \
         --input-file $(rootMapRef) --merged-file $(mergedRootMap)

TisTosTobbingMergeMapclean ::
	$(cleanup_silent) $(merge_rootmap_cmd) --un-merge \
         --input-file $(rootMapRef) --merged-file $(mergedRootMap) ;
	$(cleanup_silent) $(uninstall_command) $(stampRootMap)
libTisTosTobbing_so_dependencies = ../slc4_ia32_gcc34/libTisTosTobbing.so
#-- start of cleanup_header --------------

clean :: TisTosTobbingMergeMapclean
	@cd .

ifndef PEDANTIC
.DEFAULT::
	@echo "WARNING >> You should provide a target for $@"
else
.DEFAULT::
	@echo "PEDANTIC MODE >> You should provide a target for $@"
	@if test `expr index $@ '.'` != 0 ; \
	then  echo "PEDANTIC MODE >> This target seems to be a missing file, please check"; exit -1 ;\
	else echo "PEDANTIC MODE >> Just ignore it ; as it seems to be just a fake target due to some pattern" ; exit 0; fi; 		
endif

TisTosTobbingMergeMapclean ::
#-- end of cleanup_header ---------------
