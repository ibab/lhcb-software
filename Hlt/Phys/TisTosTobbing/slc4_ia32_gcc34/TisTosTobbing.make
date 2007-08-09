#-- start of make_header -----------------

#====================================
#  Library TisTosTobbing
#
#   Generated Thu Aug  9 03:23:12 2007  by tskwarni
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_TisTosTobbing_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_TisTosTobbing_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_TisTosTobbing

TisTosTobbing_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_TisTosTobbing = /tmp/CMT_$(TisTosTobbing_tag)_TisTosTobbing.make$(cmt_lock_pid)
else
cmt_local_tagfile_TisTosTobbing = $(TisTosTobbing_tag)_TisTosTobbing.make
endif

else

tags      = $(tag),$(CMTEXTRATAGS)

TisTosTobbing_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_TisTosTobbing = /tmp/CMT_$(TisTosTobbing_tag).make$(cmt_lock_pid)
else
cmt_local_tagfile_TisTosTobbing = $(TisTosTobbing_tag).make
endif

endif

-include $(cmt_local_tagfile_TisTosTobbing)

ifdef cmt_TisTosTobbing_has_target_tag

ifdef READONLY
cmt_final_setup_TisTosTobbing = /tmp/CMT_TisTosTobbing_TisTosTobbingsetup.make
cmt_local_TisTosTobbing_makefile = /tmp/CMT_TisTosTobbing$(cmt_lock_pid).make
else
cmt_final_setup_TisTosTobbing = $(bin)TisTosTobbing_TisTosTobbingsetup.make
cmt_local_TisTosTobbing_makefile = $(bin)TisTosTobbing.make
endif

else

ifdef READONLY
cmt_final_setup_TisTosTobbing = /tmp/CMT_TisTosTobbingsetup.make
cmt_local_TisTosTobbing_makefile = /tmp/CMT_TisTosTobbing$(cmt_lock_pid).make
else
cmt_final_setup_TisTosTobbing = $(bin)TisTosTobbingsetup.make
cmt_local_TisTosTobbing_makefile = $(bin)TisTosTobbing.make
endif

endif

ifdef READONLY
cmt_final_setup = /tmp/CMT_TisTosTobbingsetup.make
else
cmt_final_setup = $(bin)TisTosTobbingsetup.make
endif

TisTosTobbing ::


ifdef READONLY
TisTosTobbing ::
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
	@echo 'TisTosTobbing'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = TisTosTobbing/
TisTosTobbing::
	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
	@echo "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

#-- end of make_header ------------------

#-- start of libary_header ---------------

TisTosTobbinglibname   = $(bin)$(library_prefix)TisTosTobbing$(library_suffix)
TisTosTobbinglib       = $(TisTosTobbinglibname).a
TisTosTobbingstamp     = $(bin)TisTosTobbing.stamp
TisTosTobbingshstamp   = $(bin)TisTosTobbing.shstamp

TisTosTobbing :: dirs  TisTosTobbingLIB
	@/bin/echo "------> TisTosTobbing ok"

#-- end of libary_header ----------------
#-- start of libary ----------------------

TisTosTobbingLIB :: $(TisTosTobbinglib) $(TisTosTobbingshstamp)
	@/bin/echo "------> TisTosTobbing : library ok"

$(TisTosTobbinglib) :: $(bin)TisTosTobbing_dll.o $(bin)TisTosTobbing_load.o $(bin)TriggerSelectionTisTos.o
	$(lib_echo) library
	$(lib_silent) cd $(bin); \
	  $(ar) $(TisTosTobbinglib) $?
	$(lib_silent) $(ranlib) $(TisTosTobbinglib)
	$(lib_silent) cat /dev/null >$(TisTosTobbingstamp)

#------------------------------------------------------------------
#  Future improvement? to empty the object files after
#  storing in the library
#
##	  for f in $?; do \
##	    rm $${f}; touch $${f}; \
##	  done
#------------------------------------------------------------------

#
# We add one level of dependency upon the true shared library 
# (rather than simply upon the stamp file)
# this is for cases where the shared library has not been built
# while the stamp was created (error??) 
#

$(TisTosTobbinglibname).$(shlibsuffix) :: $(TisTosTobbinglib) requirements $(use_requirements) $(TisTosTobbingstamps)
	$(lib_silent) cd $(bin); QUIET=$(QUIET); $(make_shlib) "$(tags)" TisTosTobbing $(TisTosTobbing_shlibflags)

$(TisTosTobbingshstamp) :: $(TisTosTobbinglibname).$(shlibsuffix)
	@if test -f $(TisTosTobbinglibname).$(shlibsuffix) ; then cat /dev/null >$(TisTosTobbingshstamp) ; fi

TisTosTobbingclean ::
	$(cleanup_echo) objects
	$(cleanup_silent) cd $(bin); /bin/rm -f $(bin)TisTosTobbing_dll.o $(bin)TisTosTobbing_load.o $(bin)TriggerSelectionTisTos.o
	$(cleanup_silent) cd $(bin); /bin/rm -rf TisTosTobbing_deps TisTosTobbing_dependencies.make

#-----------------------------------------------------------------
#
#  New section for automatic installation
#
#-----------------------------------------------------------------

install_dir = ${CMTINSTALLAREA}/$(tag)/lib
TisTosTobbinginstallname = $(library_prefix)TisTosTobbing$(library_suffix).$(shlibsuffix)

TisTosTobbing :: TisTosTobbinginstall

install :: TisTosTobbinginstall

TisTosTobbinginstall :: $(install_dir)/$(TisTosTobbinginstallname)
	@if test ! "${CMTINSTALLAREA}" = ""; then\
	  echo "installation done"; \
	fi

$(install_dir)/$(TisTosTobbinginstallname) :: $(bin)$(TisTosTobbinginstallname)
	@if test ! "${CMTINSTALLAREA}" = ""; then \
	  $(cmt_install_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(TisTosTobbinginstallname)" \
	    -out "$(install_dir)" \
	    -cmd "$(cmt_installarea_command)" \
	    -cmtpath "$($(package)_cmtpath)" ; \
	fi

##TisTosTobbingclean :: TisTosTobbinguninstall

uninstall :: TisTosTobbinguninstall

TisTosTobbinguninstall ::
	@if test ! "${CMTINSTALLAREA}" = ""; then \
	  $(cmt_uninstall_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(TisTosTobbinginstallname)" \
	    -out "$(install_dir)" \
	    -cmtpath "$($(package)_cmtpath)" ; \
	fi


#-- end of libary -----------------------
#-- start of dependency ------------------

$(bin)TisTosTobbing_dependencies.make :: dirs

ifndef QUICK
$(bin)TisTosTobbing_dependencies.make :: $(src)*.cpp  requirements $(use_requirements) $(cmt_final_setup_TisTosTobbing)
	@echo "------> (TisTosTobbing.make) Rebuilding $@"; \
	  args=`echo $? | sed -e "s#requirements.*##"`; \
	  $(build_dependencies) TisTosTobbing -all_sources $${args}
endif

#$(TisTosTobbing_dependencies)

-include $(bin)TisTosTobbing_dependencies.make

#-- end of dependency -------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)TisTosTobbing_dll.o : $(TisTosTobbing_dll_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)TisTosTobbing_dll.o $(use_pp_cppflags) $(TisTosTobbing_pp_cppflags) $(lib_TisTosTobbing_pp_cppflags) $(TisTosTobbing_dll_pp_cppflags) $(use_cppflags) $(TisTosTobbing_cppflags) $(lib_TisTosTobbing_cppflags) $(TisTosTobbing_dll_cppflags) $(TisTosTobbing_dll_cpp_cppflags)  $(src)TisTosTobbing_dll.cpp

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)TisTosTobbing_load.o : $(TisTosTobbing_load_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)TisTosTobbing_load.o $(use_pp_cppflags) $(TisTosTobbing_pp_cppflags) $(lib_TisTosTobbing_pp_cppflags) $(TisTosTobbing_load_pp_cppflags) $(use_cppflags) $(TisTosTobbing_cppflags) $(lib_TisTosTobbing_cppflags) $(TisTosTobbing_load_cppflags) $(TisTosTobbing_load_cpp_cppflags)  $(src)TisTosTobbing_load.cpp

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)TriggerSelectionTisTos.o : $(TriggerSelectionTisTos_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)TriggerSelectionTisTos.o $(use_pp_cppflags) $(TisTosTobbing_pp_cppflags) $(lib_TisTosTobbing_pp_cppflags) $(TriggerSelectionTisTos_pp_cppflags) $(use_cppflags) $(TisTosTobbing_cppflags) $(lib_TisTosTobbing_cppflags) $(TriggerSelectionTisTos_cppflags) $(TriggerSelectionTisTos_cpp_cppflags)  $(src)TriggerSelectionTisTos.cpp

#-- end of cpp_library ------------------
#-- start of cleanup_header --------------

clean :: TisTosTobbingclean
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

TisTosTobbingclean ::
#-- end of cleanup_header ---------------
#-- start of cleanup_library -------------
	$(cleanup_echo) library
	-$(cleanup_silent) cd $(bin); /bin/rm -f $(binobj)$(library_prefix)TisTosTobbing$(library_suffix).a $(binobj)$(library_prefix)TisTosTobbing$(library_suffix).s? $(binobj)TisTosTobbing.stamp $(binobj)TisTosTobbing.shstamp
#-- end of cleanup_library ---------------

