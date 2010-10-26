#-- start of make_header -----------------

#====================================
#  Library OTDictsDict
#
#   Generated Mon Sep  6 10:23:46 2010  by dvaneijk
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_OTDictsDict_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_OTDictsDict_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_OTDictsDict

OTDicts_tag = $(tag)

#cmt_local_tagfile_OTDictsDict = $(OTDicts_tag)_OTDictsDict.make
cmt_local_tagfile_OTDictsDict = $(bin)$(OTDicts_tag)_OTDictsDict.make

else

tags      = $(tag),$(CMTEXTRATAGS)

OTDicts_tag = $(tag)

#cmt_local_tagfile_OTDictsDict = $(OTDicts_tag).make
cmt_local_tagfile_OTDictsDict = $(bin)$(OTDicts_tag).make

endif

include $(cmt_local_tagfile_OTDictsDict)
#-include $(cmt_local_tagfile_OTDictsDict)

ifdef cmt_OTDictsDict_has_target_tag

cmt_final_setup_OTDictsDict = $(bin)setup_OTDictsDict.make
#cmt_final_setup_OTDictsDict = $(bin)OTDicts_OTDictsDictsetup.make
cmt_local_OTDictsDict_makefile = $(bin)OTDictsDict.make

else

cmt_final_setup_OTDictsDict = $(bin)setup.make
#cmt_final_setup_OTDictsDict = $(bin)OTDictssetup.make
cmt_local_OTDictsDict_makefile = $(bin)OTDictsDict.make

endif

cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)OTDictssetup.make

#OTDictsDict :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'OTDictsDict'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = OTDictsDict/
#OTDictsDict::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
#-- start of libary_header ---------------

OTDictsDictlibname   = $(bin)$(library_prefix)OTDictsDict$(library_suffix)
OTDictsDictlib       = $(OTDictsDictlibname).a
OTDictsDictstamp     = $(bin)OTDictsDict.stamp
OTDictsDictshstamp   = $(bin)OTDictsDict.shstamp

OTDictsDict :: dirs  OTDictsDictLIB
	$(echo) "OTDictsDict ok"

#-- end of libary_header ----------------
#-- start of libary ----------------------

OTDictsDictLIB :: $(OTDictsDictlib) $(OTDictsDictshstamp)
	$(echo) "OTDictsDict : library ok"

$(OTDictsDictlib) :: $(bin)OTDicts_dict.o
	$(lib_echo) "static library $@"
	$(lib_silent) [ ! -f $@ ] || \rm -f $@
	$(lib_silent) $(ar) $(OTDictsDictlib) $(bin)OTDicts_dict.o
	$(lib_silent) $(ranlib) $(OTDictsDictlib)
	$(lib_silent) cat /dev/null >$(OTDictsDictstamp)

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

$(OTDictsDictlibname).$(shlibsuffix) :: $(OTDictsDictlib) requirements $(use_requirements) $(OTDictsDictstamps)
	$(lib_echo) "shared library $@"
	$(lib_silent) if test "$(makecmd)"; then QUIET=; else QUIET=1; fi; QUIET=$${QUIET} bin=$(bin) $(make_shlib) "$(tags)" OTDictsDict $(OTDictsDict_shlibflags)

$(OTDictsDictshstamp) :: $(OTDictsDictlibname).$(shlibsuffix)
	$(lib_silent) if test -f $(OTDictsDictlibname).$(shlibsuffix) ; then cat /dev/null >$(OTDictsDictshstamp) ; fi

OTDictsDictclean ::
	$(cleanup_echo) objects OTDictsDict
	$(cleanup_silent) /bin/rm -f $(bin)OTDicts_dict.o
	$(cleanup_silent) /bin/rm -f $(patsubst %.o,%.d,$(bin)OTDicts_dict.o) $(patsubst %.o,%.dep,$(bin)OTDicts_dict.o) $(patsubst %.o,%.d.stamp,$(bin)OTDicts_dict.o)
	$(cleanup_silent) cd $(bin); /bin/rm -rf OTDictsDict_deps OTDictsDict_dependencies.make

#-----------------------------------------------------------------
#
#  New section for automatic installation
#
#-----------------------------------------------------------------

install_dir = ${CMTINSTALLAREA}/$(tag)/lib
OTDictsDictinstallname = $(library_prefix)OTDictsDict$(library_suffix).$(shlibsuffix)

OTDictsDict :: OTDictsDictinstall

install :: OTDictsDictinstall

OTDictsDictinstall :: $(install_dir)/$(OTDictsDictinstallname)
ifdef CMTINSTALLAREA
	$(echo) "installation done"
endif

$(install_dir)/$(OTDictsDictinstallname) :: $(bin)$(OTDictsDictinstallname)
ifdef CMTINSTALLAREA
	$(install_silent) $(cmt_install_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(OTDictsDictinstallname)" \
	    -out "$(install_dir)" \
	    -cmd "$(cmt_installarea_command)" \
	    -cmtpath "$($(package)_cmtpath)"
endif

##OTDictsDictclean :: OTDictsDictuninstall

uninstall :: OTDictsDictuninstall

OTDictsDictuninstall ::
ifdef CMTINSTALLAREA
	$(cleanup_silent) $(cmt_uninstall_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(OTDictsDictinstallname)" \
	    -out "$(install_dir)" \
	    -cmtpath "$($(package)_cmtpath)"
endif

#-- end of libary -----------------------
#-- start of dependency ------------------
ifneq ($(MAKECMDGOALS),OTDictsDictclean)
ifneq ($(MAKECMDGOALS),uninstall)

#$(bin)OTDictsDict_dependencies.make :: dirs

ifndef QUICK
$(bin)OTDictsDict_dependencies.make : ../slc4_ia32_gcc34/dict/OTDicts/OTDicts_dict.cpp $(use_requirements) $(cmt_final_setup_OTDictsDict)
	$(echo) "(OTDictsDict.make) Rebuilding $@"; \
	  $(build_dependencies) OTDictsDict -all_sources -out=$@ ../slc4_ia32_gcc34/dict/OTDicts/OTDicts_dict.cpp
endif

#$(OTDictsDict_dependencies)

-include $(bin)OTDictsDict_dependencies.make

endif
endif
#-- end of dependency -------------------
#-- start of cpp_library -----------------

ifneq ($(cppdepflags),)

ifneq ($(MAKECMDGOALS),OTDictsDictclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)OTDicts_dict.d

$(bin)$(binobj)OTDicts_dict.d : $(use_requirements) $(cmt_final_setup_OTDictsDict)

$(bin)$(binobj)OTDicts_dict.d : ../slc4_ia32_gcc34/dict/OTDicts/OTDicts_dict.cpp
	$(dep_echo) $@
	$(cpp_silent) $(cppcomp) $(cppdepflags) -o $(@D)/OTDicts_dict.dep $(use_pp_cppflags) $(OTDictsDict_pp_cppflags) $(lib_OTDictsDict_pp_cppflags) $(OTDicts_dict_pp_cppflags) $(use_cppflags) $(OTDictsDict_cppflags) $(lib_OTDictsDict_cppflags) $(OTDicts_dict_cppflags) $(OTDicts_dict_cpp_cppflags) -I../slc4_ia32_gcc34/dict/OTDicts ../slc4_ia32_gcc34/dict/OTDicts/OTDicts_dict.cpp
	$(cpp_silent) $(format_dependencies) $@ $(@D)/OTDicts_dict.o ../slc4_ia32_gcc34/dict/OTDicts/OTDicts_dict.cpp $(@D)/OTDicts_dict.dep
endif
endif

$(bin)$(binobj)OTDicts_dict.o : ../slc4_ia32_gcc34/dict/OTDicts/OTDicts_dict.cpp
else
$(bin)OTDictsDict_dependencies.make : $(OTDicts_dict_cpp_dependencies)

$(bin)$(binobj)OTDicts_dict.o : $(OTDicts_dict_cpp_dependencies)
endif
	$(cpp_echo) ../slc4_ia32_gcc34/dict/OTDicts/OTDicts_dict.cpp
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(OTDictsDict_pp_cppflags) $(lib_OTDictsDict_pp_cppflags) $(OTDicts_dict_pp_cppflags) $(use_cppflags) $(OTDictsDict_cppflags) $(lib_OTDictsDict_cppflags) $(OTDicts_dict_cppflags) $(OTDicts_dict_cpp_cppflags) -I../slc4_ia32_gcc34/dict/OTDicts ../slc4_ia32_gcc34/dict/OTDicts/OTDicts_dict.cpp

#-- end of cpp_library ------------------
#-- start of cleanup_header --------------

clean :: OTDictsDictclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(OTDictsDict.make) $@: No rule for such target" >&2
#	@echo "#CMT> Warning: $@: No rule for such target" >&2; exit
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(OTDictsDict.make): $@: File no longer generated" >&2; exit 0; fi
else
.DEFAULT::
	$(echo) "(OTDictsDict.make) PEDANTIC: $@: No rule for such target" >&2
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(OTDictsDict.make): $@: File no longer generated" >&2; exit 0;\
	 elif test $@ = "$(cmt_final_setup)" -o\
	 $@ = "$(cmt_final_setup_OTDictsDict)" ; then\
	 found=n; for s in 1 2 3 4 5; do\
	 sleep $$s; test ! -f $@ || { found=y; break; }\
	 done; if test $$found = n; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(OTDictsDict.make) PEDANTIC: $@: Seems to be missing. Ignore it for now" >&2; exit 0 ; fi;\
	 elif test `expr $@ : '.*/'` -ne 0 ; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(OTDictsDict.make) PEDANTIC: $@: Seems to be a missing file. Please check" >&2; exit 2 ; \
	 else\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(OTDictsDict.make) PEDANTIC: $@: Seems to be a fake target due to some pattern. Just ignore it" >&2 ; exit 0; fi
endif

OTDictsDictclean ::
#-- end of cleanup_header ---------------
#-- start of cleanup_library -------------
	$(cleanup_echo) library OTDictsDict
	-$(cleanup_silent) cd $(bin); /bin/rm -f $(library_prefix)OTDictsDict$(library_suffix).a $(library_prefix)OTDictsDict$(library_suffix).s? OTDictsDict.stamp OTDictsDict.shstamp
#-- end of cleanup_library ---------------
