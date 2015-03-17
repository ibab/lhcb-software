#-- start of make_header -----------------

#====================================
#  Document RichMirrorAlignmentOnlineConfUserDbMerge
#
#   Generated Tue Mar 17 22:27:59 2015  by pnaik
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_RichMirrorAlignmentOnlineConfUserDbMerge_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_RichMirrorAlignmentOnlineConfUserDbMerge_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_RichMirrorAlignmentOnlineConfUserDbMerge

RichMirrorAlignmentOnline_tag = $(tag)

#cmt_local_tagfile_RichMirrorAlignmentOnlineConfUserDbMerge = $(RichMirrorAlignmentOnline_tag)_RichMirrorAlignmentOnlineConfUserDbMerge.make
cmt_local_tagfile_RichMirrorAlignmentOnlineConfUserDbMerge = $(bin)$(RichMirrorAlignmentOnline_tag)_RichMirrorAlignmentOnlineConfUserDbMerge.make

else

tags      = $(tag),$(CMTEXTRATAGS)

RichMirrorAlignmentOnline_tag = $(tag)

#cmt_local_tagfile_RichMirrorAlignmentOnlineConfUserDbMerge = $(RichMirrorAlignmentOnline_tag).make
cmt_local_tagfile_RichMirrorAlignmentOnlineConfUserDbMerge = $(bin)$(RichMirrorAlignmentOnline_tag).make

endif

include $(cmt_local_tagfile_RichMirrorAlignmentOnlineConfUserDbMerge)
#-include $(cmt_local_tagfile_RichMirrorAlignmentOnlineConfUserDbMerge)

ifdef cmt_RichMirrorAlignmentOnlineConfUserDbMerge_has_target_tag

cmt_final_setup_RichMirrorAlignmentOnlineConfUserDbMerge = $(bin)setup_RichMirrorAlignmentOnlineConfUserDbMerge.make
#cmt_final_setup_RichMirrorAlignmentOnlineConfUserDbMerge = $(bin)RichMirrorAlignmentOnline_RichMirrorAlignmentOnlineConfUserDbMergesetup.make
cmt_local_RichMirrorAlignmentOnlineConfUserDbMerge_makefile = $(bin)RichMirrorAlignmentOnlineConfUserDbMerge.make

else

cmt_final_setup_RichMirrorAlignmentOnlineConfUserDbMerge = $(bin)setup.make
#cmt_final_setup_RichMirrorAlignmentOnlineConfUserDbMerge = $(bin)RichMirrorAlignmentOnlinesetup.make
cmt_local_RichMirrorAlignmentOnlineConfUserDbMerge_makefile = $(bin)RichMirrorAlignmentOnlineConfUserDbMerge.make

endif

cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)RichMirrorAlignmentOnlinesetup.make

#RichMirrorAlignmentOnlineConfUserDbMerge :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'RichMirrorAlignmentOnlineConfUserDbMerge'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = RichMirrorAlignmentOnlineConfUserDbMerge/
#RichMirrorAlignmentOnlineConfUserDbMerge::
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

.PHONY: RichMirrorAlignmentOnlineConfUserDbMerge RichMirrorAlignmentOnlineConfUserDbMergeclean

# default is already '#'
#genconfDb_comment_char := "'#'"

instdir      := ${CMTINSTALLAREA}/$(tag)
confDbRef    := /afs/cern.ch/user/p/pnaik/cmtuser/PANOPTES/PANOPTES_HEAD/Rich/RichMirrorAlignmentOnline/x86_64-slc6-gcc48-opt/genConf/RichMirrorAlignmentOnline/RichMirrorAlignmentOnline_user.confdb
stampConfDb  := $(confDbRef).stamp
mergedConfDb := $(instdir)/lib/$(project).confdb

RichMirrorAlignmentOnlineConfUserDbMerge :: $(stampConfDb) $(mergedConfDb)
	@:

.NOTPARALLEL : $(stampConfDb) $(mergedConfDb)

$(stampConfDb) $(mergedConfDb) :: $(confDbRef)
	@echo "Running merge_genconfDb  RichMirrorAlignmentOnlineConfUserDbMerge"
	$(merge_genconfDb_cmd) \
          --do-merge \
          --input-file $(confDbRef) \
          --merged-file $(mergedConfDb)

RichMirrorAlignmentOnlineConfUserDbMergeclean ::
	$(cleanup_silent) $(merge_genconfDb_cmd) \
          --un-merge \
          --input-file $(confDbRef) \
          --merged-file $(mergedConfDb)	;
	$(cleanup_silent) $(remove_command) $(stampConfDb)
#-- start of cleanup_header --------------

clean :: RichMirrorAlignmentOnlineConfUserDbMergeclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(RichMirrorAlignmentOnlineConfUserDbMerge.make) $@: No rule for such target" >&2
#	@echo "#CMT> Warning: $@: No rule for such target" >&2; exit
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnlineConfUserDbMerge.make): $@: File no longer generated" >&2; exit 0; fi
else
.DEFAULT::
	$(echo) "(RichMirrorAlignmentOnlineConfUserDbMerge.make) PEDANTIC: $@: No rule for such target" >&2
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnlineConfUserDbMerge.make): $@: File no longer generated" >&2; exit 0;\
	 elif test $@ = "$(cmt_final_setup)" -o\
	 $@ = "$(cmt_final_setup_RichMirrorAlignmentOnlineConfUserDbMerge)" ; then\
	 found=n; for s in 1 2 3 4 5; do\
	 sleep $$s; test ! -f $@ || { found=y; break; }\
	 done; if test $$found = n; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnlineConfUserDbMerge.make) PEDANTIC: $@: Seems to be missing. Ignore it for now" >&2; exit 0 ; fi;\
	 elif test `expr $@ : '.*/'` -ne 0 ; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnlineConfUserDbMerge.make) PEDANTIC: $@: Seems to be a missing file. Please check" >&2; exit 2 ; \
	 else\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnlineConfUserDbMerge.make) PEDANTIC: $@: Seems to be a fake target due to some pattern. Just ignore it" >&2 ; exit 0; fi
endif

RichMirrorAlignmentOnlineConfUserDbMergeclean ::
#-- end of cleanup_header ---------------
