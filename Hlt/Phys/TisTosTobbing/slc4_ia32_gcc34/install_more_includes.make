#-- start of make_header -----------------

#====================================
#  Document install_more_includes
#
#   Generated Thu Aug  9 03:23:11 2007  by tskwarni
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_install_more_includes_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_install_more_includes_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_install_more_includes

TisTosTobbing_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_install_more_includes = /tmp/CMT_$(TisTosTobbing_tag)_install_more_includes.make$(cmt_lock_pid)
else
cmt_local_tagfile_install_more_includes = $(TisTosTobbing_tag)_install_more_includes.make
endif

else

tags      = $(tag),$(CMTEXTRATAGS)

TisTosTobbing_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_install_more_includes = /tmp/CMT_$(TisTosTobbing_tag).make$(cmt_lock_pid)
else
cmt_local_tagfile_install_more_includes = $(TisTosTobbing_tag).make
endif

endif

-include $(cmt_local_tagfile_install_more_includes)

ifdef cmt_install_more_includes_has_target_tag

ifdef READONLY
cmt_final_setup_install_more_includes = /tmp/CMT_TisTosTobbing_install_more_includessetup.make
cmt_local_install_more_includes_makefile = /tmp/CMT_install_more_includes$(cmt_lock_pid).make
else
cmt_final_setup_install_more_includes = $(bin)TisTosTobbing_install_more_includessetup.make
cmt_local_install_more_includes_makefile = $(bin)install_more_includes.make
endif

else

ifdef READONLY
cmt_final_setup_install_more_includes = /tmp/CMT_TisTosTobbingsetup.make
cmt_local_install_more_includes_makefile = /tmp/CMT_install_more_includes$(cmt_lock_pid).make
else
cmt_final_setup_install_more_includes = $(bin)TisTosTobbingsetup.make
cmt_local_install_more_includes_makefile = $(bin)install_more_includes.make
endif

endif

ifdef READONLY
cmt_final_setup = /tmp/CMT_TisTosTobbingsetup.make
else
cmt_final_setup = $(bin)TisTosTobbingsetup.make
endif

install_more_includes ::


ifdef READONLY
install_more_includes ::
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
	@echo 'install_more_includes'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = install_more_includes/
install_more_includes::
	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
	@echo "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

#-- end of make_header ------------------

#-- start of install_more_includes_header ------

#
#  We want to install all header files that follow the standard convention
#
#    ../<more>
#
#  This document generator needs no parameterization, since it simply expects
#  the standard convention.
#

installarea = ${CMTINSTALLAREA}
install_include_dir = $(installarea)/include

install_more_includes :: install_more_includesinstall

install :: install_more_includesinstall

install_more_includesinstall :: $(install_include_dir)
	@if test ! "$(installarea)" = ""; then\
	  echo "installation done"; \
	fi

$(install_include_dir) ::
	@if test "$(installarea)" = ""; then \
	  echo "Cannot install header files, no installation directory specified"; \
	else \
	  if test ! "" = "";then \
	    if test -d ../; then \
	       echo "Installing files from offset ../ to $(install_include_dir)"; \
	       if test ! -d $(install_include_dir) ; then mkdir -p $(install_include_dir); fi; \
	       if test ! -d $(install_include_dir)/; then mkdir -p $(install_include_dir)/; fi;\
	       (cd ../; \
	        $(copyInclude) * $(install_include_dir)//.); \
	    else \
	       echo "no offset   include directory"; \
	    fi; \
	  fi; \
	  if test ! "TisTosTobbing" = ""; then \
	    if test -d ../TisTosTobbing; then \
	      echo "Installing files from more ../TisTosTobbing to $(install_include_dir)"; \
	      if test ! -d $(install_include_dir) ; then mkdir -p $(install_include_dir); fi; \
	      if test ! -d $(install_include_dir)/TisTosTobbing; then mkdir -p $(install_include_dir)/TisTosTobbing; fi;\
	        (cd ../TisTosTobbing; \
	         $(copyInclude) * $(install_include_dir)/TisTosTobbing/.); \
	    else \
	      echo "No more TisTosTobbing include directory"; \
	    fi; \
          fi; \
	fi

install_more_includesclean :: install_more_includesuninstall

uninstall :: install_more_includesuninstall

install_more_includesuninstall ::
	@if test "$(installarea)" = ""; then \
	  echo "Cannot uninstall header files, no installation directory specified"; \
	else \
	  if test -d $(install_include_dir)/TisTosTobbing ; then \
	    echo "Uninstalling files from $(install_include_dir)/TisTosTobbing"; \
	    eval rm -Rf $(install_include_dir)/TisTosTobbing ; \
	  elif test -L $(install_include_dir)/TisTosTobbing ; then \
	    echo "Uninstalling files from $(install_include_dir)/TisTosTobbing"; \
	    eval rm -f $(install_include_dir)/TisTosTobbing ; \
	  fi ; \
	  if test -f $(install_include_dir)/TisTosTobbing.cmtref ; then \
	    echo "Uninstalling files from $(install_include_dir)/TisTosTobbing.cmtref"; \
	    eval rm -f $(install_include_dir)/TisTosTobbing.cmtref ; \
	  fi; \
	fi


#-- end of install_more_includes_header ------
#-- start of cleanup_header --------------

clean :: install_more_includesclean
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

install_more_includesclean ::
#-- end of cleanup_header ---------------
