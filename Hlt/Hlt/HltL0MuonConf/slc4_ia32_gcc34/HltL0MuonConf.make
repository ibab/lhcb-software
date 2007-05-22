#-- start of make_header -----------------

#====================================
#  Library HltL0MuonConf
#
#   Generated Mon May 14 15:16:11 2007  by asatta
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_HltL0MuonConf_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_HltL0MuonConf_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_HltL0MuonConf

HltL0MuonConf_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_HltL0MuonConf = /tmp/CMT_$(HltL0MuonConf_tag)_HltL0MuonConf.make$(cmt_lock_pid)
else
cmt_local_tagfile_HltL0MuonConf = $(HltL0MuonConf_tag)_HltL0MuonConf.make
endif

else

tags      = $(tag),$(CMTEXTRATAGS)

HltL0MuonConf_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_HltL0MuonConf = /tmp/CMT_$(HltL0MuonConf_tag).make$(cmt_lock_pid)
else
cmt_local_tagfile_HltL0MuonConf = $(HltL0MuonConf_tag).make
endif

endif

-include $(cmt_local_tagfile_HltL0MuonConf)

ifdef cmt_HltL0MuonConf_has_target_tag

ifdef READONLY
cmt_final_setup_HltL0MuonConf = /tmp/CMT_HltL0MuonConf_HltL0MuonConfsetup.make
cmt_local_HltL0MuonConf_makefile = /tmp/CMT_HltL0MuonConf$(cmt_lock_pid).make
else
cmt_final_setup_HltL0MuonConf = $(bin)HltL0MuonConf_HltL0MuonConfsetup.make
cmt_local_HltL0MuonConf_makefile = $(bin)HltL0MuonConf.make
endif

else

ifdef READONLY
cmt_final_setup_HltL0MuonConf = /tmp/CMT_HltL0MuonConfsetup.make
cmt_local_HltL0MuonConf_makefile = /tmp/CMT_HltL0MuonConf$(cmt_lock_pid).make
else
cmt_final_setup_HltL0MuonConf = $(bin)HltL0MuonConfsetup.make
cmt_local_HltL0MuonConf_makefile = $(bin)HltL0MuonConf.make
endif

endif

ifdef READONLY
cmt_final_setup = /tmp/CMT_HltL0MuonConfsetup.make
else
cmt_final_setup = $(bin)HltL0MuonConfsetup.make
endif

HltL0MuonConf ::


ifdef READONLY
HltL0MuonConf ::
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
	@echo 'HltL0MuonConf'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = HltL0MuonConf/
HltL0MuonConf::
	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
	@echo "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

#-- end of make_header ------------------

#-- start of libary_header ---------------

HltL0MuonConflibname   = $(bin)$(library_prefix)HltL0MuonConf$(library_suffix)
HltL0MuonConflib       = $(HltL0MuonConflibname).a
HltL0MuonConfstamp     = $(bin)HltL0MuonConf.stamp
HltL0MuonConfshstamp   = $(bin)HltL0MuonConf.shstamp

HltL0MuonConf :: dirs  HltL0MuonConfLIB
	@/bin/echo "------> HltL0MuonConf ok"

#-- end of libary_header ----------------
#-- start of libary ----------------------

HltL0MuonConfLIB :: $(HltL0MuonConflib) $(HltL0MuonConfshstamp)
	@/bin/echo "------> HltL0MuonConf : library ok"

$(HltL0MuonConflib) :: $(bin)MuonCollectTsaCluster.o $(bin)RZMuonMatch.o $(bin)HltL0MuonConf_load.o $(bin)L0MuonConfWithT.o $(bin)HltL0MuonConf_dll.o $(bin)L0MuonConfNt.o $(bin)L0MuonAndTConf.o $(bin)SpaceMuonMatch.o $(bin)MuonTConfWithT.o
	$(lib_echo) library
	$(lib_silent) cd $(bin); \
	  $(ar) $(HltL0MuonConflib) $?
	$(lib_silent) $(ranlib) $(HltL0MuonConflib)
	$(lib_silent) cat /dev/null >$(HltL0MuonConfstamp)

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

$(HltL0MuonConflibname).$(shlibsuffix) :: $(HltL0MuonConflib) requirements $(use_requirements) $(HltL0MuonConfstamps)
	$(lib_silent) cd $(bin); QUIET=$(QUIET); $(make_shlib) "$(tags)" HltL0MuonConf $(HltL0MuonConf_shlibflags)

$(HltL0MuonConfshstamp) :: $(HltL0MuonConflibname).$(shlibsuffix)
	@if test -f $(HltL0MuonConflibname).$(shlibsuffix) ; then cat /dev/null >$(HltL0MuonConfshstamp) ; fi

HltL0MuonConfclean ::
	$(cleanup_echo) objects
	$(cleanup_silent) cd $(bin); /bin/rm -f $(bin)MuonCollectTsaCluster.o $(bin)RZMuonMatch.o $(bin)HltL0MuonConf_load.o $(bin)L0MuonConfWithT.o $(bin)HltL0MuonConf_dll.o $(bin)L0MuonConfNt.o $(bin)L0MuonAndTConf.o $(bin)SpaceMuonMatch.o $(bin)MuonTConfWithT.o
	$(cleanup_silent) cd $(bin); /bin/rm -rf HltL0MuonConf_deps HltL0MuonConf_dependencies.make

#-----------------------------------------------------------------
#
#  New section for automatic installation
#
#-----------------------------------------------------------------

install_dir = ${CMTINSTALLAREA}/$(tag)/lib
HltL0MuonConfinstallname = $(library_prefix)HltL0MuonConf$(library_suffix).$(shlibsuffix)

HltL0MuonConf :: HltL0MuonConfinstall

install :: HltL0MuonConfinstall

HltL0MuonConfinstall :: $(install_dir)/$(HltL0MuonConfinstallname)
	@if test ! "${CMTINSTALLAREA}" = ""; then\
	  echo "installation done"; \
	fi

$(install_dir)/$(HltL0MuonConfinstallname) :: $(bin)$(HltL0MuonConfinstallname)
	@if test ! "${CMTINSTALLAREA}" = ""; then \
	  $(cmt_install_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(HltL0MuonConfinstallname)" \
	    -out "$(install_dir)" \
	    -cmd "$(cmt_installarea_command)" \
	    -cmtpath "$($(package)_cmtpath)" ; \
	fi

##HltL0MuonConfclean :: HltL0MuonConfuninstall

uninstall :: HltL0MuonConfuninstall

HltL0MuonConfuninstall ::
	@if test ! "${CMTINSTALLAREA}" = ""; then \
	  $(cmt_uninstall_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(HltL0MuonConfinstallname)" \
	    -out "$(install_dir)" \
	    -cmtpath "$($(package)_cmtpath)" ; \
	fi


#-- end of libary -----------------------
#-- start of dependency ------------------

$(bin)HltL0MuonConf_dependencies.make :: dirs

ifndef QUICK
$(bin)HltL0MuonConf_dependencies.make :: $(src)*.cpp  requirements $(use_requirements) $(cmt_final_setup_HltL0MuonConf)
	@echo "------> (HltL0MuonConf.make) Rebuilding $@"; \
	  args=`echo $? | sed -e "s#requirements.*##"`; \
	  $(build_dependencies) HltL0MuonConf -all_sources $${args}
endif

#$(HltL0MuonConf_dependencies)

-include $(bin)HltL0MuonConf_dependencies.make

#-- end of dependency -------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)MuonCollectTsaCluster.o : $(MuonCollectTsaCluster_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)MuonCollectTsaCluster.o $(use_pp_cppflags) $(HltL0MuonConf_pp_cppflags) $(lib_HltL0MuonConf_pp_cppflags) $(MuonCollectTsaCluster_pp_cppflags) $(use_cppflags) $(HltL0MuonConf_cppflags) $(lib_HltL0MuonConf_cppflags) $(MuonCollectTsaCluster_cppflags) $(MuonCollectTsaCluster_cpp_cppflags)  $(src)MuonCollectTsaCluster.cpp

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)RZMuonMatch.o : $(RZMuonMatch_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)RZMuonMatch.o $(use_pp_cppflags) $(HltL0MuonConf_pp_cppflags) $(lib_HltL0MuonConf_pp_cppflags) $(RZMuonMatch_pp_cppflags) $(use_cppflags) $(HltL0MuonConf_cppflags) $(lib_HltL0MuonConf_cppflags) $(RZMuonMatch_cppflags) $(RZMuonMatch_cpp_cppflags)  $(src)RZMuonMatch.cpp

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)HltL0MuonConf_load.o : $(HltL0MuonConf_load_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)HltL0MuonConf_load.o $(use_pp_cppflags) $(HltL0MuonConf_pp_cppflags) $(lib_HltL0MuonConf_pp_cppflags) $(HltL0MuonConf_load_pp_cppflags) $(use_cppflags) $(HltL0MuonConf_cppflags) $(lib_HltL0MuonConf_cppflags) $(HltL0MuonConf_load_cppflags) $(HltL0MuonConf_load_cpp_cppflags)  $(src)HltL0MuonConf_load.cpp

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)L0MuonConfWithT.o : $(L0MuonConfWithT_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)L0MuonConfWithT.o $(use_pp_cppflags) $(HltL0MuonConf_pp_cppflags) $(lib_HltL0MuonConf_pp_cppflags) $(L0MuonConfWithT_pp_cppflags) $(use_cppflags) $(HltL0MuonConf_cppflags) $(lib_HltL0MuonConf_cppflags) $(L0MuonConfWithT_cppflags) $(L0MuonConfWithT_cpp_cppflags)  $(src)L0MuonConfWithT.cpp

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)HltL0MuonConf_dll.o : $(HltL0MuonConf_dll_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)HltL0MuonConf_dll.o $(use_pp_cppflags) $(HltL0MuonConf_pp_cppflags) $(lib_HltL0MuonConf_pp_cppflags) $(HltL0MuonConf_dll_pp_cppflags) $(use_cppflags) $(HltL0MuonConf_cppflags) $(lib_HltL0MuonConf_cppflags) $(HltL0MuonConf_dll_cppflags) $(HltL0MuonConf_dll_cpp_cppflags)  $(src)HltL0MuonConf_dll.cpp

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)L0MuonConfNt.o : $(L0MuonConfNt_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)L0MuonConfNt.o $(use_pp_cppflags) $(HltL0MuonConf_pp_cppflags) $(lib_HltL0MuonConf_pp_cppflags) $(L0MuonConfNt_pp_cppflags) $(use_cppflags) $(HltL0MuonConf_cppflags) $(lib_HltL0MuonConf_cppflags) $(L0MuonConfNt_cppflags) $(L0MuonConfNt_cpp_cppflags)  $(src)L0MuonConfNt.cpp

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)L0MuonAndTConf.o : $(L0MuonAndTConf_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)L0MuonAndTConf.o $(use_pp_cppflags) $(HltL0MuonConf_pp_cppflags) $(lib_HltL0MuonConf_pp_cppflags) $(L0MuonAndTConf_pp_cppflags) $(use_cppflags) $(HltL0MuonConf_cppflags) $(lib_HltL0MuonConf_cppflags) $(L0MuonAndTConf_cppflags) $(L0MuonAndTConf_cpp_cppflags)  $(src)L0MuonAndTConf.cpp

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)SpaceMuonMatch.o : $(SpaceMuonMatch_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)SpaceMuonMatch.o $(use_pp_cppflags) $(HltL0MuonConf_pp_cppflags) $(lib_HltL0MuonConf_pp_cppflags) $(SpaceMuonMatch_pp_cppflags) $(use_cppflags) $(HltL0MuonConf_cppflags) $(lib_HltL0MuonConf_cppflags) $(SpaceMuonMatch_cppflags) $(SpaceMuonMatch_cpp_cppflags)  $(src)SpaceMuonMatch.cpp

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------
#
$(bin)$(binobj)MuonTConfWithT.o : $(MuonTConfWithT_cpp_dependencies)
	$(cpp_echo) $@
	$(cpp_silent) cd $(bin); $(cppcomp) -o $(binobj)MuonTConfWithT.o $(use_pp_cppflags) $(HltL0MuonConf_pp_cppflags) $(lib_HltL0MuonConf_pp_cppflags) $(MuonTConfWithT_pp_cppflags) $(use_cppflags) $(HltL0MuonConf_cppflags) $(lib_HltL0MuonConf_cppflags) $(MuonTConfWithT_cppflags) $(MuonTConfWithT_cpp_cppflags)  $(src)MuonTConfWithT.cpp

#-- end of cpp_library ------------------
#-- start of cleanup_header --------------

clean :: HltL0MuonConfclean
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

HltL0MuonConfclean ::
#-- end of cleanup_header ---------------
#-- start of cleanup_library -------------
	$(cleanup_echo) library
	-$(cleanup_silent) cd $(bin); /bin/rm -f $(binobj)$(library_prefix)HltL0MuonConf$(library_suffix).a $(binobj)$(library_prefix)HltL0MuonConf$(library_suffix).s? $(binobj)HltL0MuonConf.stamp $(binobj)HltL0MuonConf.shstamp
#-- end of cleanup_library ---------------

