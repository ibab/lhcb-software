#-- start of make_header -----------------

#====================================
#  Library BplusToPhiMuNu
#
#   Generated Wed Feb 24 12:24:56 2016  by ismith
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_BplusToPhiMuNu_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_BplusToPhiMuNu_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_BplusToPhiMuNu

BplusToPhiMuNu_tag = $(tag)

#cmt_local_tagfile_BplusToPhiMuNu = $(BplusToPhiMuNu_tag)_BplusToPhiMuNu.make
cmt_local_tagfile_BplusToPhiMuNu = $(bin)$(BplusToPhiMuNu_tag)_BplusToPhiMuNu.make

else

tags      = $(tag),$(CMTEXTRATAGS)

BplusToPhiMuNu_tag = $(tag)

#cmt_local_tagfile_BplusToPhiMuNu = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_BplusToPhiMuNu = $(bin)$(BplusToPhiMuNu_tag).make

endif

include $(cmt_local_tagfile_BplusToPhiMuNu)
#-include $(cmt_local_tagfile_BplusToPhiMuNu)

ifdef cmt_BplusToPhiMuNu_has_target_tag

cmt_final_setup_BplusToPhiMuNu = $(bin)setup_BplusToPhiMuNu.make
#cmt_final_setup_BplusToPhiMuNu = $(bin)BplusToPhiMuNu_BplusToPhiMuNusetup.make
cmt_local_BplusToPhiMuNu_makefile = $(bin)BplusToPhiMuNu.make

else

cmt_final_setup_BplusToPhiMuNu = $(bin)setup.make
#cmt_final_setup_BplusToPhiMuNu = $(bin)BplusToPhiMuNusetup.make
cmt_local_BplusToPhiMuNu_makefile = $(bin)BplusToPhiMuNu.make

endif

cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)BplusToPhiMuNusetup.make

#BplusToPhiMuNu :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'BplusToPhiMuNu'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = BplusToPhiMuNu/
#BplusToPhiMuNu::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
#-- start of libary_header ---------------

BplusToPhiMuNulibname   = $(bin)$(library_prefix)BplusToPhiMuNu$(library_suffix)
BplusToPhiMuNulib       = $(BplusToPhiMuNulibname).a
BplusToPhiMuNustamp     = $(bin)BplusToPhiMuNu.stamp
BplusToPhiMuNushstamp   = $(bin)BplusToPhiMuNu.shstamp

BplusToPhiMuNu :: dirs  BplusToPhiMuNuLIB
	$(echo) "BplusToPhiMuNu ok"

#-- end of libary_header ----------------
#-- start of libary ----------------------

BplusToPhiMuNuLIB :: $(BplusToPhiMuNulib) $(BplusToPhiMuNushstamp)
	$(echo) "BplusToPhiMuNu : library ok"

$(BplusToPhiMuNulib) :: $(bin)IsoBDT_Bs.o $(bin)TMVAClassification_Bs2KmuNu.class.o $(bin)TupleToolBu2phimunu_2.o $(bin)TupleToolBu2phimunu.o $(bin)TupleToolSLTools.o $(bin)DaVinciP2VVAngles.o
	$(lib_echo) "static library $@"
	$(lib_silent) [ ! -f $@ ] || \rm -f $@
	$(lib_silent) $(ar) $(BplusToPhiMuNulib) $(bin)IsoBDT_Bs.o $(bin)TMVAClassification_Bs2KmuNu.class.o $(bin)TupleToolBu2phimunu_2.o $(bin)TupleToolBu2phimunu.o $(bin)TupleToolSLTools.o $(bin)DaVinciP2VVAngles.o
	$(lib_silent) $(ranlib) $(BplusToPhiMuNulib)
	$(lib_silent) cat /dev/null >$(BplusToPhiMuNustamp)

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

$(BplusToPhiMuNulibname).$(shlibsuffix) :: $(BplusToPhiMuNulib) requirements $(use_requirements) $(BplusToPhiMuNustamps)
	$(lib_echo) "shared library $@"
	$(lib_silent) if test "$(makecmd)"; then QUIET=; else QUIET=1; fi; QUIET=$${QUIET} bin=$(bin) $(make_shlib) "$(tags)" BplusToPhiMuNu $(BplusToPhiMuNu_shlibflags)

$(BplusToPhiMuNushstamp) :: $(BplusToPhiMuNulibname).$(shlibsuffix)
	$(lib_silent) if test -f $(BplusToPhiMuNulibname).$(shlibsuffix) ; then cat /dev/null >$(BplusToPhiMuNushstamp) ; fi

BplusToPhiMuNuclean ::
	$(cleanup_echo) objects BplusToPhiMuNu
	$(cleanup_silent) /bin/rm -f $(bin)IsoBDT_Bs.o $(bin)TMVAClassification_Bs2KmuNu.class.o $(bin)TupleToolBu2phimunu_2.o $(bin)TupleToolBu2phimunu.o $(bin)TupleToolSLTools.o $(bin)DaVinciP2VVAngles.o
	$(cleanup_silent) /bin/rm -f $(patsubst %.o,%.d,$(bin)IsoBDT_Bs.o $(bin)TMVAClassification_Bs2KmuNu.class.o $(bin)TupleToolBu2phimunu_2.o $(bin)TupleToolBu2phimunu.o $(bin)TupleToolSLTools.o $(bin)DaVinciP2VVAngles.o) $(patsubst %.o,%.dep,$(bin)IsoBDT_Bs.o $(bin)TMVAClassification_Bs2KmuNu.class.o $(bin)TupleToolBu2phimunu_2.o $(bin)TupleToolBu2phimunu.o $(bin)TupleToolSLTools.o $(bin)DaVinciP2VVAngles.o) $(patsubst %.o,%.d.stamp,$(bin)IsoBDT_Bs.o $(bin)TMVAClassification_Bs2KmuNu.class.o $(bin)TupleToolBu2phimunu_2.o $(bin)TupleToolBu2phimunu.o $(bin)TupleToolSLTools.o $(bin)DaVinciP2VVAngles.o)
	$(cleanup_silent) cd $(bin); /bin/rm -rf BplusToPhiMuNu_deps BplusToPhiMuNu_dependencies.make

#-----------------------------------------------------------------
#
#  New section for automatic installation
#
#-----------------------------------------------------------------

install_dir = ${CMTINSTALLAREA}/$(tag)/lib
BplusToPhiMuNuinstallname = $(library_prefix)BplusToPhiMuNu$(library_suffix).$(shlibsuffix)

BplusToPhiMuNu :: BplusToPhiMuNuinstall

install :: BplusToPhiMuNuinstall

BplusToPhiMuNuinstall :: $(install_dir)/$(BplusToPhiMuNuinstallname)
ifdef CMTINSTALLAREA
	$(echo) "installation done"
endif

$(install_dir)/$(BplusToPhiMuNuinstallname) :: $(bin)$(BplusToPhiMuNuinstallname)
ifdef CMTINSTALLAREA
	$(install_silent) $(cmt_install_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(BplusToPhiMuNuinstallname)" \
	    -out "$(install_dir)" \
	    -cmd "$(cmt_installarea_command)" \
	    -cmtpath "$($(package)_cmtpath)"
endif

##BplusToPhiMuNuclean :: BplusToPhiMuNuuninstall

uninstall :: BplusToPhiMuNuuninstall

BplusToPhiMuNuuninstall ::
ifdef CMTINSTALLAREA
	$(cleanup_silent) $(cmt_uninstall_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(BplusToPhiMuNuinstallname)" \
	    -out "$(install_dir)" \
	    -cmtpath "$($(package)_cmtpath)"
endif

#-- end of libary -----------------------
#-- start of dependency ------------------
ifneq ($(MAKECMDGOALS),BplusToPhiMuNuclean)
ifneq ($(MAKECMDGOALS),uninstall)

#$(bin)BplusToPhiMuNu_dependencies.make :: dirs

ifndef QUICK
$(bin)BplusToPhiMuNu_dependencies.make : $(src)IsoBDT_Bs.cpp $(src)TMVAClassification_Bs2KmuNu.class.cpp $(src)TupleToolBu2phimunu_2.cpp $(src)TupleToolBu2phimunu.cpp $(src)TupleToolSLTools.cpp $(src)DaVinciP2VVAngles.cpp $(use_requirements) $(cmt_final_setup_BplusToPhiMuNu)
	$(echo) "(BplusToPhiMuNu.make) Rebuilding $@"; \
	  $(build_dependencies) BplusToPhiMuNu -all_sources -out=$@ $(src)IsoBDT_Bs.cpp $(src)TMVAClassification_Bs2KmuNu.class.cpp $(src)TupleToolBu2phimunu_2.cpp $(src)TupleToolBu2phimunu.cpp $(src)TupleToolSLTools.cpp $(src)DaVinciP2VVAngles.cpp
endif

#$(BplusToPhiMuNu_dependencies)

-include $(bin)BplusToPhiMuNu_dependencies.make

endif
endif
#-- end of dependency -------------------
#-- start of cpp_library -----------------

ifneq ($(MAKECMDGOALS),BplusToPhiMuNuclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)IsoBDT_Bs.d
endif
endif


$(bin)$(binobj)IsoBDT_Bs.o $(bin)$(binobj)IsoBDT_Bs.d : $(src)IsoBDT_Bs.cpp  $(use_requirements) $(cmt_final_setup_BplusToPhiMuNu)
	$(cpp_echo) $(src)IsoBDT_Bs.cpp
	@mkdir -p $(@D)
	$(cpp_silent) $(cppcomp) $(use_pp_cppflags) $(BplusToPhiMuNu_pp_cppflags) $(app_BplusToPhiMuNu_pp_cppflags) $(IsoBDT_Bs_pp_cppflags) $(use_cppflags) $(BplusToPhiMuNu_cppflags) $(lib_BplusToPhiMuNu_cppflags) $(app_BplusToPhiMuNu_cppflags) $(IsoBDT_Bs_cppflags) $(IsoBDT_Bs_cpp_cppflags)  -MP -MMD -MT $(bin)$(binobj)IsoBDT_Bs.o -MT $(bin)$(binobj)IsoBDT_Bs.d -MF $(bin)$(binobj)IsoBDT_Bs.d -o $(bin)$(binobj)IsoBDT_Bs.o $(src)IsoBDT_Bs.cpp


#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq ($(MAKECMDGOALS),BplusToPhiMuNuclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)TMVAClassification_Bs2KmuNu.class.d
endif
endif


$(bin)$(binobj)TMVAClassification_Bs2KmuNu.class.o $(bin)$(binobj)TMVAClassification_Bs2KmuNu.class.d : $(src)TMVAClassification_Bs2KmuNu.class.cpp  $(use_requirements) $(cmt_final_setup_BplusToPhiMuNu)
	$(cpp_echo) $(src)TMVAClassification_Bs2KmuNu.class.cpp
	@mkdir -p $(@D)
	$(cpp_silent) $(cppcomp) $(use_pp_cppflags) $(BplusToPhiMuNu_pp_cppflags) $(app_BplusToPhiMuNu_pp_cppflags) $(TMVAClassification_Bs2KmuNu.class_pp_cppflags) $(use_cppflags) $(BplusToPhiMuNu_cppflags) $(lib_BplusToPhiMuNu_cppflags) $(app_BplusToPhiMuNu_cppflags) $(TMVAClassification_Bs2KmuNu.class_cppflags) $(TMVAClassification_Bs2KmuNu.class_cpp_cppflags)  -MP -MMD -MT $(bin)$(binobj)TMVAClassification_Bs2KmuNu.class.o -MT $(bin)$(binobj)TMVAClassification_Bs2KmuNu.class.d -MF $(bin)$(binobj)TMVAClassification_Bs2KmuNu.class.d -o $(bin)$(binobj)TMVAClassification_Bs2KmuNu.class.o $(src)TMVAClassification_Bs2KmuNu.class.cpp


#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq ($(MAKECMDGOALS),BplusToPhiMuNuclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)TupleToolBu2phimunu_2.d
endif
endif


$(bin)$(binobj)TupleToolBu2phimunu_2.o $(bin)$(binobj)TupleToolBu2phimunu_2.d : $(src)TupleToolBu2phimunu_2.cpp  $(use_requirements) $(cmt_final_setup_BplusToPhiMuNu)
	$(cpp_echo) $(src)TupleToolBu2phimunu_2.cpp
	@mkdir -p $(@D)
	$(cpp_silent) $(cppcomp) $(use_pp_cppflags) $(BplusToPhiMuNu_pp_cppflags) $(app_BplusToPhiMuNu_pp_cppflags) $(TupleToolBu2phimunu_2_pp_cppflags) $(use_cppflags) $(BplusToPhiMuNu_cppflags) $(lib_BplusToPhiMuNu_cppflags) $(app_BplusToPhiMuNu_cppflags) $(TupleToolBu2phimunu_2_cppflags) $(TupleToolBu2phimunu_2_cpp_cppflags)  -MP -MMD -MT $(bin)$(binobj)TupleToolBu2phimunu_2.o -MT $(bin)$(binobj)TupleToolBu2phimunu_2.d -MF $(bin)$(binobj)TupleToolBu2phimunu_2.d -o $(bin)$(binobj)TupleToolBu2phimunu_2.o $(src)TupleToolBu2phimunu_2.cpp


#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq ($(MAKECMDGOALS),BplusToPhiMuNuclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)TupleToolBu2phimunu.d
endif
endif


$(bin)$(binobj)TupleToolBu2phimunu.o $(bin)$(binobj)TupleToolBu2phimunu.d : $(src)TupleToolBu2phimunu.cpp  $(use_requirements) $(cmt_final_setup_BplusToPhiMuNu)
	$(cpp_echo) $(src)TupleToolBu2phimunu.cpp
	@mkdir -p $(@D)
	$(cpp_silent) $(cppcomp) $(use_pp_cppflags) $(BplusToPhiMuNu_pp_cppflags) $(app_BplusToPhiMuNu_pp_cppflags) $(TupleToolBu2phimunu_pp_cppflags) $(use_cppflags) $(BplusToPhiMuNu_cppflags) $(lib_BplusToPhiMuNu_cppflags) $(app_BplusToPhiMuNu_cppflags) $(TupleToolBu2phimunu_cppflags) $(TupleToolBu2phimunu_cpp_cppflags)  -MP -MMD -MT $(bin)$(binobj)TupleToolBu2phimunu.o -MT $(bin)$(binobj)TupleToolBu2phimunu.d -MF $(bin)$(binobj)TupleToolBu2phimunu.d -o $(bin)$(binobj)TupleToolBu2phimunu.o $(src)TupleToolBu2phimunu.cpp


#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq ($(MAKECMDGOALS),BplusToPhiMuNuclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)TupleToolSLTools.d
endif
endif


$(bin)$(binobj)TupleToolSLTools.o $(bin)$(binobj)TupleToolSLTools.d : $(src)TupleToolSLTools.cpp  $(use_requirements) $(cmt_final_setup_BplusToPhiMuNu)
	$(cpp_echo) $(src)TupleToolSLTools.cpp
	@mkdir -p $(@D)
	$(cpp_silent) $(cppcomp) $(use_pp_cppflags) $(BplusToPhiMuNu_pp_cppflags) $(app_BplusToPhiMuNu_pp_cppflags) $(TupleToolSLTools_pp_cppflags) $(use_cppflags) $(BplusToPhiMuNu_cppflags) $(lib_BplusToPhiMuNu_cppflags) $(app_BplusToPhiMuNu_cppflags) $(TupleToolSLTools_cppflags) $(TupleToolSLTools_cpp_cppflags)  -MP -MMD -MT $(bin)$(binobj)TupleToolSLTools.o -MT $(bin)$(binobj)TupleToolSLTools.d -MF $(bin)$(binobj)TupleToolSLTools.d -o $(bin)$(binobj)TupleToolSLTools.o $(src)TupleToolSLTools.cpp


#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq ($(MAKECMDGOALS),BplusToPhiMuNuclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)DaVinciP2VVAngles.d
endif
endif


$(bin)$(binobj)DaVinciP2VVAngles.o $(bin)$(binobj)DaVinciP2VVAngles.d : $(src)DaVinciP2VVAngles.cpp  $(use_requirements) $(cmt_final_setup_BplusToPhiMuNu)
	$(cpp_echo) $(src)DaVinciP2VVAngles.cpp
	@mkdir -p $(@D)
	$(cpp_silent) $(cppcomp) $(use_pp_cppflags) $(BplusToPhiMuNu_pp_cppflags) $(app_BplusToPhiMuNu_pp_cppflags) $(DaVinciP2VVAngles_pp_cppflags) $(use_cppflags) $(BplusToPhiMuNu_cppflags) $(lib_BplusToPhiMuNu_cppflags) $(app_BplusToPhiMuNu_cppflags) $(DaVinciP2VVAngles_cppflags) $(DaVinciP2VVAngles_cpp_cppflags)  -MP -MMD -MT $(bin)$(binobj)DaVinciP2VVAngles.o -MT $(bin)$(binobj)DaVinciP2VVAngles.d -MF $(bin)$(binobj)DaVinciP2VVAngles.d -o $(bin)$(binobj)DaVinciP2VVAngles.o $(src)DaVinciP2VVAngles.cpp


#-- end of cpp_library ------------------
#-- start of cleanup_header --------------

clean :: BplusToPhiMuNuclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(BplusToPhiMuNu.make) $@: No rule for such target" >&2
#	@echo "#CMT> Warning: $@: No rule for such target" >&2; exit
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNu.make): $@: File no longer generated" >&2; exit 0; fi
else
.DEFAULT::
	$(echo) "(BplusToPhiMuNu.make) PEDANTIC: $@: No rule for such target" >&2
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNu.make): $@: File no longer generated" >&2; exit 0;\
	 elif test $@ = "$(cmt_final_setup)" -o\
	 $@ = "$(cmt_final_setup_BplusToPhiMuNu)" ; then\
	 found=n; for s in 1 2 3 4 5; do\
	 sleep $$s; test ! -f $@ || { found=y; break; }\
	 done; if test $$found = n; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNu.make) PEDANTIC: $@: Seems to be missing. Ignore it for now" >&2; exit 0 ; fi;\
	 elif test `expr $@ : '.*/'` -ne 0 ; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNu.make) PEDANTIC: $@: Seems to be a missing file. Please check" >&2; exit 2 ; \
	 else\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNu.make) PEDANTIC: $@: Seems to be a fake target due to some pattern. Just ignore it" >&2 ; exit 0; fi
endif

BplusToPhiMuNuclean ::
#-- end of cleanup_header ---------------
#-- start of cleanup_library -------------
	$(cleanup_echo) library BplusToPhiMuNu
	-$(cleanup_silent) cd $(bin); /bin/rm -f $(library_prefix)BplusToPhiMuNu$(library_suffix).a $(library_prefix)BplusToPhiMuNu$(library_suffix).s? BplusToPhiMuNu.stamp BplusToPhiMuNu.shstamp
#-- end of cleanup_library ---------------
