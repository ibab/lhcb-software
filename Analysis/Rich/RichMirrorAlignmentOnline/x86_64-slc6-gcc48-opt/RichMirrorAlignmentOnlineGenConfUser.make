#-- start of make_header -----------------

#====================================
#  Document RichMirrorAlignmentOnlineGenConfUser
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

cmt_RichMirrorAlignmentOnlineGenConfUser_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_RichMirrorAlignmentOnlineGenConfUser_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_RichMirrorAlignmentOnlineGenConfUser

RichMirrorAlignmentOnline_tag = $(tag)

#cmt_local_tagfile_RichMirrorAlignmentOnlineGenConfUser = $(RichMirrorAlignmentOnline_tag)_RichMirrorAlignmentOnlineGenConfUser.make
cmt_local_tagfile_RichMirrorAlignmentOnlineGenConfUser = $(bin)$(RichMirrorAlignmentOnline_tag)_RichMirrorAlignmentOnlineGenConfUser.make

else

tags      = $(tag),$(CMTEXTRATAGS)

RichMirrorAlignmentOnline_tag = $(tag)

#cmt_local_tagfile_RichMirrorAlignmentOnlineGenConfUser = $(RichMirrorAlignmentOnline_tag).make
cmt_local_tagfile_RichMirrorAlignmentOnlineGenConfUser = $(bin)$(RichMirrorAlignmentOnline_tag).make

endif

include $(cmt_local_tagfile_RichMirrorAlignmentOnlineGenConfUser)
#-include $(cmt_local_tagfile_RichMirrorAlignmentOnlineGenConfUser)

ifdef cmt_RichMirrorAlignmentOnlineGenConfUser_has_target_tag

cmt_final_setup_RichMirrorAlignmentOnlineGenConfUser = $(bin)setup_RichMirrorAlignmentOnlineGenConfUser.make
#cmt_final_setup_RichMirrorAlignmentOnlineGenConfUser = $(bin)RichMirrorAlignmentOnline_RichMirrorAlignmentOnlineGenConfUsersetup.make
cmt_local_RichMirrorAlignmentOnlineGenConfUser_makefile = $(bin)RichMirrorAlignmentOnlineGenConfUser.make

else

cmt_final_setup_RichMirrorAlignmentOnlineGenConfUser = $(bin)setup.make
#cmt_final_setup_RichMirrorAlignmentOnlineGenConfUser = $(bin)RichMirrorAlignmentOnlinesetup.make
cmt_local_RichMirrorAlignmentOnlineGenConfUser_makefile = $(bin)RichMirrorAlignmentOnlineGenConfUser.make

endif

cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)RichMirrorAlignmentOnlinesetup.make

#RichMirrorAlignmentOnlineGenConfUser :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'RichMirrorAlignmentOnlineGenConfUser'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = RichMirrorAlignmentOnlineGenConfUser/
#RichMirrorAlignmentOnlineGenConfUser::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
# ============= call_command_header:begin =============
deps        = $(RichMirrorAlignmentOnlineGenConfUser_deps)
command     = $(RichMirrorAlignmentOnlineGenConfUser_command)
output      = $(RichMirrorAlignmentOnlineGenConfUser_output)

.PHONY: RichMirrorAlignmentOnlineGenConfUser RichMirrorAlignmentOnlineGenConfUserclean

RichMirrorAlignmentOnlineGenConfUser :: $(output)

RichMirrorAlignmentOnlineGenConfUserclean ::
	$(cmt_uninstallarea_command) $(output)

$(output):: $(deps)
	$(command)

FORCE:
# ============= call_command_header:end =============
#-- start of cleanup_header --------------

clean :: RichMirrorAlignmentOnlineGenConfUserclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(RichMirrorAlignmentOnlineGenConfUser.make) $@: No rule for such target" >&2
#	@echo "#CMT> Warning: $@: No rule for such target" >&2; exit
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnlineGenConfUser.make): $@: File no longer generated" >&2; exit 0; fi
else
.DEFAULT::
	$(echo) "(RichMirrorAlignmentOnlineGenConfUser.make) PEDANTIC: $@: No rule for such target" >&2
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnlineGenConfUser.make): $@: File no longer generated" >&2; exit 0;\
	 elif test $@ = "$(cmt_final_setup)" -o\
	 $@ = "$(cmt_final_setup_RichMirrorAlignmentOnlineGenConfUser)" ; then\
	 found=n; for s in 1 2 3 4 5; do\
	 sleep $$s; test ! -f $@ || { found=y; break; }\
	 done; if test $$found = n; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnlineGenConfUser.make) PEDANTIC: $@: Seems to be missing. Ignore it for now" >&2; exit 0 ; fi;\
	 elif test `expr $@ : '.*/'` -ne 0 ; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnlineGenConfUser.make) PEDANTIC: $@: Seems to be a missing file. Please check" >&2; exit 2 ; \
	 else\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnlineGenConfUser.make) PEDANTIC: $@: Seems to be a fake target due to some pattern. Just ignore it" >&2 ; exit 0; fi
endif

RichMirrorAlignmentOnlineGenConfUserclean ::
#-- end of cleanup_header ---------------
