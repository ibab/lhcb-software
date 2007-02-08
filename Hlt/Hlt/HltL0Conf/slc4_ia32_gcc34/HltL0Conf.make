#-- start of make_header -----------------

#====================================
#  Library HltL0Conf
#
#   Generated Thu Feb  8 18:06:36 2007  by hernando
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_HltL0Conf_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_HltL0Conf_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_HltL0Conf

HltL0Conf_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_HltL0Conf = /tmp/CMT_$(HltL0Conf_tag)_HltL0Conf.make$(cmt_lock_pid)
else
cmt_local_tagfile_HltL0Conf = $(HltL0Conf_tag)_HltL0Conf.make
endif

else

tags      = $(tag),$(CMTEXTRATAGS)

HltL0Conf_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_HltL0Conf = /tmp/CMT_$(HltL0Conf_tag).make$(cmt_lock_pid)
else
cmt_local_tagfile_HltL0Conf = $(HltL0Conf_tag).make
endif

endif

-include $(cmt_local_tagfile_HltL0Conf)

ifdef cmt_HltL0Conf_has_target_tag

ifdef READONLY
cmt_final_setup_HltL0Conf = /tmp/CMT_HltL0Conf_HltL0Confsetup.make
cmt_local_HltL0Conf_makefile = /tmp/CMT_HltL0Conf$(cmt_lock_pid).make
else
cmt_final_setup_HltL0Conf = $(bin)HltL0Conf_HltL0Confsetup.make
cmt_local_HltL0Conf_makefile = $(bin)HltL0Conf.make
endif

else

ifdef READONLY
cmt_final_setup_HltL0Conf = /tmp/CMT_HltL0Confsetup.make
cmt_local_HltL0Conf_makefile = /tmp/CMT_HltL0Conf$(cmt_lock_pid).make
else
cmt_final_setup_HltL0Conf = $(bin)HltL0Confsetup.make
cmt_local_HltL0Conf_makefile = $(bin)HltL0Conf.make
endif

endif

ifdef READONLY
cmt_final_setup = /tmp/CMT_HltL0Confsetup.make
else
cmt_final_setup = $(bin)HltL0Confsetup.make
endif

HltL0Conf ::


ifdef READONLY
HltL0Conf ::
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
	@echo 'HltL0Conf'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = HltL0Conf/
HltL0Conf::
	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
	@echo "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

#-- end of make_header ------------------

#-- start of libary_header ---------------

HltL0Conflibname   = $(bin)$(library_prefix)HltL0Conf$(library_suffix)
HltL0Conflib       = $(HltL0Conflibname).a
HltL0Confstamp     = $(bin)HltL0Conf.stamp
HltL0Confshstamp   = $(bin)HltL0Conf.shstamp

HltL0Conf :: dirs  HltL0ConfLIB
	@/bin/echo "------> HltL0Conf ok"

#-- end of libary_header ----------------
#-- start of libary ----------------------

HltL0ConfLIB :: $(HltL0Conflib) $(HltL0Confshstamp)
	@/bin/echo "------> HltL0Conf : library ok"

$(HltL0Conflib) :: $(bin)PrepareElectronSeed.o $(bin)PrepareHadronSeed.o $(bin)PrepareMuonSeed.o $(bin)TsaConfirmTool.o
	$(lib_echo) library
	$(lib_silent) cd $(bin); \
	  $(ar) $(HltL0Conflib) $?
	$(lib_silent) $(ranlib) $(HltL0Conflib)
	$(lib_silent) cat /dev/null >$(HltL0Confstamp)

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

$(HltL0Conflibname).$(shlibsuffix) :: $(HltL0Conflib) requirements $(use_requirements) $(HltL0Confstamps)
	$(lib_silent) cd $(bin); QUIET=$(QUIET); $(make_shlib) "$(tags)" HltL0Conf $(HltL0Conf_shlibflags)

$(HltL0Confshstamp) :: $(HltL0Conflibname).$(shlibsuffix)
	@if test -f $(HltL0Conflibname).$(shlibsuffix) ; then cat /dev/null >$(HltL0Confshstamp) ; fi

HltL0Confclean ::
	$(cleanup_echo) objects
	$(cleanup_silent) cd $(bin); /bin/rm -f $(bin)PrepareElectronSeed.o $(bin)PrepareHadronSeed.o $(bin)PrepareMuonSeed.o $(bin)TsaConfirmTool.o
	$(cleanup_silent) cd $(bin); /bin/rm -rf HltL0Conf_deps HltL0Conf_dependencies.make

#-----------------------------------------------------------------
#
#  New section for automatic installation
#
#-----------------------------------------------------------------

install_dir = ${CMTINSTALLAREA}/$(tag)/lib
HltL0Confinstallname = $(library_prefix)HltL0Conf$(library_suffix).$(shlibsuffix)

HltL0Conf :: HltL0Confinstall

install :: HltL0Confinstall

HltL0Confinstall :: $(install_dir)/$(HltL0Confinstallname)
	@if test ! "${CMTINSTALLAREA}" = ""; then\
	  echo "installation done"; \
	fi

$(install_dir)/$(HltL0Confinstallname) :: $(bin)$(HltL0Confinstallname)
	@if test ! "${CMTINSTALLAREA}" = ""; then \
	  $(cmt_install_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(HltL0Confinstallname)" \
	    -out "$(install_dir)" \
	    -cmd "$(cmt_installarea_command)" \
	    -cmtpath "$($(package)_cmtpath)" ; \
	fi

##HltL0Confclean :: HltL0Confuninstall

uninstall :: HltL0Confuninstall

HltL0Confuninstall ::
	@if test ! "${CMTINSTALLAREA}" = ""; then \
	  $(cmt_uninstall_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(HltL0Confinstallname)" \
	    -out "$(install_dir)" \
	    -cmtpath "$($(package)_cmtpath)" ; \
	fi


#-- end of libary -----------------------
#-- start of dependency ------------------

$(bin)HltL0Conf_dependencies.make :: dirs

ifndef QUICK
$(bin)HltL0Conf_dependencies.make :: $(src)*.cpp  requirements $(use_requirements) $(cmt_final_setup_HltL0Conf)
	@echo "------> (HltL0Conf.make) Rebuilding $@"; \
	  args=`echo $? | sed -e "s#requirements.*##"`; \
	  $(build_dependencies) HltL0Conf -all_sources $${args}
endif

#$(HltL0Conf_dependencies)

-include $(bin)HltL0Conf_dependencies.make

#-- end of dependency -------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)PrepareElectronSeed.o : $(PrepareElectronSeed_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)PrepareElectronSeed.o $(use_pp_cppflags) $(HltL0Conf_pp_cppflags) $(lib_HltL0Conf_pp_cppflags) $(PrepareElectronSeed_pp_cppflags) $(use_cppflags) $(HltL0Conf_cppflags) $(lib_HltL0Conf_cppflags) $(PrepareElectronSeed_cppflags) $(PrepareElectronSeed_cpp_cppflags)  $(src)PrepareElectronSeed.cpp

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)PrepareHadronSeed.o : $(PrepareHadronSeed_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)PrepareHadronSeed.o $(use_pp_cppflags) $(HltL0Conf_pp_cppflags) $(lib_HltL0Conf_pp_cppflags) $(PrepareHadronSeed_pp_cppflags) $(use_cppflags) $(HltL0Conf_cppflags) $(lib_HltL0Conf_cppflags) $(PrepareHadronSeed_cppflags) $(PrepareHadronSeed_cpp_cppflags)  $(src)PrepareHadronSeed.cpp

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)PrepareMuonSeed.o : $(PrepareMuonSeed_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)PrepareMuonSeed.o $(use_pp_cppflags) $(HltL0Conf_pp_cppflags) $(lib_HltL0Conf_pp_cppflags) $(PrepareMuonSeed_pp_cppflags) $(use_cppflags) $(HltL0Conf_cppflags) $(lib_HltL0Conf_cppflags) $(PrepareMuonSeed_cppflags) $(PrepareMuonSeed_cpp_cppflags)  $(src)PrepareMuonSeed.cpp

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)TsaConfirmTool.o : $(TsaConfirmTool_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)TsaConfirmTool.o $(use_pp_cppflags) $(HltL0Conf_pp_cppflags) $(lib_HltL0Conf_pp_cppflags) $(TsaConfirmTool_pp_cppflags) $(use_cppflags) $(HltL0Conf_cppflags) $(lib_HltL0Conf_cppflags) $(TsaConfirmTool_cppflags) $(TsaConfirmTool_cpp_cppflags)  $(src)TsaConfirmTool.cpp

#-- end of cpp_library ------------------
#-- start of cleanup_header --------------

clean :: HltL0Confclean
	@cd .

ifndef PEDANTIC
.DEFAULT::
	@echo "You should provide a target for this file  $@"
#	@exit 0
endif

#ifdef PEDANTIC
#.DEFAULT::
#	@echo "You should provide a target for this file  $@"
#	@exit 1
#endif




HltL0Confclean ::
#-- end of cleanup_header ---------------
#-- start of cleanup_library -------------
	$(cleanup_echo) library
	-$(cleanup_silent) cd $(bin); /bin/rm -f $(binobj)$(library_prefix)HltL0Conf$(library_suffix).a $(binobj)$(library_prefix)HltL0Conf$(library_suffix).s? $(binobj)HltL0Conf.stamp $(binobj)HltL0Conf.shstamp
#-- end of cleanup_library ---------------

