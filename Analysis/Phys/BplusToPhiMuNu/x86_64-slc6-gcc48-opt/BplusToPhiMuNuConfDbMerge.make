#-- start of make_header -----------------

#====================================
#  Document BplusToPhiMuNuConfDbMerge
#
#   Generated Wed Feb 24 12:25:02 2016  by ismith
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_BplusToPhiMuNuConfDbMerge_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_BplusToPhiMuNuConfDbMerge_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_BplusToPhiMuNuConfDbMerge

BplusToPhiMuNu_tag = $(tag)

#cmt_local_tagfile_BplusToPhiMuNuConfDbMerge = $(BplusToPhiMuNu_tag)_BplusToPhiMuNuConfDbMerge.make
cmt_local_tagfile_BplusToPhiMuNuConfDbMerge = $(bin)$(BplusToPhiMuNu_tag)_BplusToPhiMuNuConfDbMerge.make

else

tags      = $(tag),$(CMTEXTRATAGS)

BplusToPhiMuNu_tag = $(tag)

#cmt_local_tagfile_BplusToPhiMuNuConfDbMerge = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_BplusToPhiMuNuConfDbMerge = $(bin)$(BplusToPhiMuNu_tag).make

endif

include $(cmt_local_tagfile_BplusToPhiMuNuConfDbMerge)
#-include $(cmt_local_tagfile_BplusToPhiMuNuConfDbMerge)

ifdef cmt_BplusToPhiMuNuConfDbMerge_has_target_tag

cmt_final_setup_BplusToPhiMuNuConfDbMerge = $(bin)setup_BplusToPhiMuNuConfDbMerge.make
#cmt_final_setup_BplusToPhiMuNuConfDbMerge = $(bin)BplusToPhiMuNu_BplusToPhiMuNuConfDbMergesetup.make
cmt_local_BplusToPhiMuNuConfDbMerge_makefile = $(bin)BplusToPhiMuNuConfDbMerge.make

else

cmt_final_setup_BplusToPhiMuNuConfDbMerge = $(bin)setup.make
#cmt_final_setup_BplusToPhiMuNuConfDbMerge = $(bin)BplusToPhiMuNusetup.make
cmt_local_BplusToPhiMuNuConfDbMerge_makefile = $(bin)BplusToPhiMuNuConfDbMerge.make

endif

cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)BplusToPhiMuNusetup.make

#BplusToPhiMuNuConfDbMerge :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'BplusToPhiMuNuConfDbMerge'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = BplusToPhiMuNuConfDbMerge/
#BplusToPhiMuNuConfDbMerge::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
# File: cmt/fragments/merge_genconfDb_header
# Author: Sebastien Binet (binet@cern.ch)

# Makefile fragment to merge a <library>.confdb file into a single
# <project>.confdb file in the (lib) install area

.PHONY: BplusToPhiMuNuConfDbMerge BplusToPhiMuNuConfDbMergeclean

# default is already '#'
#genconfDb_comment_char := "'#'"

instdir      := ${CMTINSTALLAREA}/$(tag)
confDbRef    := /afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/Phys/BplusToPhiMuNu/x86_64-slc6-gcc48-opt/genConf/BplusToPhiMuNu/BplusToPhiMuNu.confdb
stampConfDb  := $(confDbRef).stamp
mergedConfDb := $(instdir)/lib/$(project).confdb

BplusToPhiMuNuConfDbMerge :: $(stampConfDb) $(mergedConfDb)
	@:

.NOTPARALLEL : $(stampConfDb) $(mergedConfDb)

$(stampConfDb) $(mergedConfDb) :: $(confDbRef)
	@echo "Running merge_genconfDb  BplusToPhiMuNuConfDbMerge"
	$(merge_genconfDb_cmd) \
          --do-merge \
          --input-file $(confDbRef) \
          --merged-file $(mergedConfDb)

BplusToPhiMuNuConfDbMergeclean ::
	$(cleanup_silent) $(merge_genconfDb_cmd) \
          --un-merge \
          --input-file $(confDbRef) \
          --merged-file $(mergedConfDb)	;
	$(cleanup_silent) $(remove_command) $(stampConfDb)
libBplusToPhiMuNu_so_dependencies = ../x86_64-slc6-gcc48-opt/libBplusToPhiMuNu.so
#-- start of cleanup_header --------------

clean :: BplusToPhiMuNuConfDbMergeclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(BplusToPhiMuNuConfDbMerge.make) $@: No rule for such target" >&2
#	@echo "#CMT> Warning: $@: No rule for such target" >&2; exit
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuConfDbMerge.make): $@: File no longer generated" >&2; exit 0; fi
else
.DEFAULT::
	$(echo) "(BplusToPhiMuNuConfDbMerge.make) PEDANTIC: $@: No rule for such target" >&2
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuConfDbMerge.make): $@: File no longer generated" >&2; exit 0;\
	 elif test $@ = "$(cmt_final_setup)" -o\
	 $@ = "$(cmt_final_setup_BplusToPhiMuNuConfDbMerge)" ; then\
	 found=n; for s in 1 2 3 4 5; do\
	 sleep $$s; test ! -f $@ || { found=y; break; }\
	 done; if test $$found = n; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuConfDbMerge.make) PEDANTIC: $@: Seems to be missing. Ignore it for now" >&2; exit 0 ; fi;\
	 elif test `expr $@ : '.*/'` -ne 0 ; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuConfDbMerge.make) PEDANTIC: $@: Seems to be a missing file. Please check" >&2; exit 2 ; \
	 else\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuConfDbMerge.make) PEDANTIC: $@: Seems to be a fake target due to some pattern. Just ignore it" >&2 ; exit 0; fi
endif

BplusToPhiMuNuConfDbMergeclean ::
#-- end of cleanup_header ---------------
