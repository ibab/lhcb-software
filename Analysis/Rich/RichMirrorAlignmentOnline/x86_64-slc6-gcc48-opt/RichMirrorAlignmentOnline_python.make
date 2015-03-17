#-- start of make_header -----------------

#====================================
#  Document RichMirrorAlignmentOnline_python
#
#   Generated Tue Mar 17 22:27:58 2015  by pnaik
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_RichMirrorAlignmentOnline_python_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_RichMirrorAlignmentOnline_python_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_RichMirrorAlignmentOnline_python

RichMirrorAlignmentOnline_tag = $(tag)

#cmt_local_tagfile_RichMirrorAlignmentOnline_python = $(RichMirrorAlignmentOnline_tag)_RichMirrorAlignmentOnline_python.make
cmt_local_tagfile_RichMirrorAlignmentOnline_python = $(bin)$(RichMirrorAlignmentOnline_tag)_RichMirrorAlignmentOnline_python.make

else

tags      = $(tag),$(CMTEXTRATAGS)

RichMirrorAlignmentOnline_tag = $(tag)

#cmt_local_tagfile_RichMirrorAlignmentOnline_python = $(RichMirrorAlignmentOnline_tag).make
cmt_local_tagfile_RichMirrorAlignmentOnline_python = $(bin)$(RichMirrorAlignmentOnline_tag).make

endif

include $(cmt_local_tagfile_RichMirrorAlignmentOnline_python)
#-include $(cmt_local_tagfile_RichMirrorAlignmentOnline_python)

ifdef cmt_RichMirrorAlignmentOnline_python_has_target_tag

cmt_final_setup_RichMirrorAlignmentOnline_python = $(bin)setup_RichMirrorAlignmentOnline_python.make
#cmt_final_setup_RichMirrorAlignmentOnline_python = $(bin)RichMirrorAlignmentOnline_RichMirrorAlignmentOnline_pythonsetup.make
cmt_local_RichMirrorAlignmentOnline_python_makefile = $(bin)RichMirrorAlignmentOnline_python.make

else

cmt_final_setup_RichMirrorAlignmentOnline_python = $(bin)setup.make
#cmt_final_setup_RichMirrorAlignmentOnline_python = $(bin)RichMirrorAlignmentOnlinesetup.make
cmt_local_RichMirrorAlignmentOnline_python_makefile = $(bin)RichMirrorAlignmentOnline_python.make

endif

cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)RichMirrorAlignmentOnlinesetup.make

#RichMirrorAlignmentOnline_python :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'RichMirrorAlignmentOnline_python'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = RichMirrorAlignmentOnline_python/
#RichMirrorAlignmentOnline_python::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
#-- start of install_python_header ------


installarea = ${CMTINSTALLAREA}$(shared_install_subdir)
install_python_dir = $(installarea)

ifneq ($(strip "$(source)"),"")
src = ../$(source)
dest = $(install_python_dir)/python
else
src = ../python
dest = $(install_python_dir)
endif

ifneq ($(strip "$(offset)"),"")
dest = $(install_python_dir)/python
endif

RichMirrorAlignmentOnline_python :: RichMirrorAlignmentOnline_pythoninstall

install :: RichMirrorAlignmentOnline_pythoninstall

RichMirrorAlignmentOnline_pythoninstall :: $(install_python_dir)
	@if [ ! "$(installarea)" = "" ] ; then\
	  echo "installation done"; \
	fi

$(install_python_dir) ::
	@if [ "$(installarea)" = "" ] ; then \
	  echo "Cannot install header files, no installation source specified"; \
	else \
	  if [ -d $(src) ] ; then \
	    echo "Installing files from $(src) to $(dest)" ; \
	    if [ "$(offset)" = "" ] ; then \
	      $(install_command) --exclude="*.py?" $(src) $(dest) ; \
	    else \
	      $(install_command) --exclude="*.py?" $(src) $(dest) --destname $(offset); \
	    fi ; \
	  else \
	    echo "no source  $(src)"; \
	  fi; \
	fi

RichMirrorAlignmentOnline_pythonclean :: RichMirrorAlignmentOnline_pythonuninstall

uninstall :: RichMirrorAlignmentOnline_pythonuninstall

RichMirrorAlignmentOnline_pythonuninstall ::
	@if test "$(installarea)" = ""; then \
	  echo "Cannot uninstall header files, no installation source specified"; \
	else \
	  echo "Uninstalling files from $(dest)"; \
	  $(uninstall_command) "$(dest)" ; \
	fi


#-- end of install_python_header ------
#-- start of cleanup_header --------------

clean :: RichMirrorAlignmentOnline_pythonclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(RichMirrorAlignmentOnline_python.make) $@: No rule for such target" >&2
#	@echo "#CMT> Warning: $@: No rule for such target" >&2; exit
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnline_python.make): $@: File no longer generated" >&2; exit 0; fi
else
.DEFAULT::
	$(echo) "(RichMirrorAlignmentOnline_python.make) PEDANTIC: $@: No rule for such target" >&2
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnline_python.make): $@: File no longer generated" >&2; exit 0;\
	 elif test $@ = "$(cmt_final_setup)" -o\
	 $@ = "$(cmt_final_setup_RichMirrorAlignmentOnline_python)" ; then\
	 found=n; for s in 1 2 3 4 5; do\
	 sleep $$s; test ! -f $@ || { found=y; break; }\
	 done; if test $$found = n; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnline_python.make) PEDANTIC: $@: Seems to be missing. Ignore it for now" >&2; exit 0 ; fi;\
	 elif test `expr $@ : '.*/'` -ne 0 ; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnline_python.make) PEDANTIC: $@: Seems to be a missing file. Please check" >&2; exit 2 ; \
	 else\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(RichMirrorAlignmentOnline_python.make) PEDANTIC: $@: Seems to be a fake target due to some pattern. Just ignore it" >&2 ; exit 0; fi
endif

RichMirrorAlignmentOnline_pythonclean ::
#-- end of cleanup_header ---------------
