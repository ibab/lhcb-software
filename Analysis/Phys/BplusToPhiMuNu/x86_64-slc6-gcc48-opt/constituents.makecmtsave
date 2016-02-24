
#-- start of constituents_header ------

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

tags      = $(tag),$(CMTEXTRATAGS)

BplusToPhiMuNu_tag = $(tag)

#cmt_local_tagfile = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile = $(bin)$(BplusToPhiMuNu_tag).make

#-include $(cmt_local_tagfile)
include $(cmt_local_tagfile)

#cmt_local_setup = $(bin)setup$$$$.make
#cmt_local_setup = $(bin)$(package)setup$$$$.make
#cmt_final_setup = $(bin)BplusToPhiMuNusetup.make
cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)$(package)setup.make

#--------------------------------------------------------

#cmt_lock_setup = /tmp/lock$(cmt_lock_pid).make
#cmt_temp_tag = /tmp/tag$(cmt_lock_pid).make

#first :: $(cmt_local_tagfile)
#	@echo $(cmt_local_tagfile) ok
#ifndef QUICK
#first :: $(cmt_final_setup) ;
#else
#first :: ;
#endif

##	@bin=`$(cmtexe) show macro_value bin`

#$(cmt_local_tagfile) : $(cmt_lock_setup)
#	@echo "#CMT> Error: $@: No such file" >&2; exit 1
#$(cmt_local_tagfile) :
#	@echo "#CMT> Warning: $@: No such file" >&2; exit
#	@echo "#CMT> Info: $@: No need to rebuild file" >&2; exit

#$(cmt_final_setup) : $(cmt_local_tagfile) 
#	$(echo) "(constituents.make) Rebuilding $@"
#	@if test ! -d $(@D); then $(mkdir) -p $(@D); fi; \
#	  if test -f $(cmt_local_setup); then /bin/rm -f $(cmt_local_setup); fi; \
#	  trap '/bin/rm -f $(cmt_local_setup)' 0 1 2 15; \
#	  $(cmtexe) -tag=$(tags) show setup >>$(cmt_local_setup); \
#	  if test ! -f $@; then \
#	    mv $(cmt_local_setup) $@; \
#	  else \
#	    if /usr/bin/diff $(cmt_local_setup) $@ >/dev/null ; then \
#	      : ; \
#	    else \
#	      mv $(cmt_local_setup) $@; \
#	    fi; \
#	  fi

#	@/bin/echo $@ ok   

#config :: checkuses
#	@exit 0
#checkuses : ;

env.make ::
	printenv >env.make.tmp; $(cmtexe) check files env.make.tmp env.make

ifndef QUICK
all :: build_library_links
	$(echo) "(constituents.make) all done"
endif

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

dirs :: requirements
	@if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi
#	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
#	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

requirements :
	@if test ! -r requirements ; then echo "No requirements file" ; fi

build_library_links : dirs
	$(echo) "(constituents.make) Rebuilding library links"; \
	 $(build_library_links)
#	if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi; \
#	$(build_library_links)

makefiles : ;

.DEFAULT ::
	$(echo) "(constituents.make) $@: No rule for such target" >&2
#	@echo "#CMT> Warning: $@: Using default commands" >&2; exit

#	@if test "$@" = "$(cmt_lock_setup)"; then \
	#  /bin/rm -f $(cmt_lock_setup); \
	 # touch $(cmt_lock_setup); \
	#fi

#-- end of constituents_header ------
#-- start of group ------

all_groups :: all

all :: $(all_dependencies)  $(all_pre_constituents) $(all_constituents)  $(all_post_constituents)
	$(echo) "all ok."

#	@/bin/echo " all ok."

clean :: allclean

allclean ::  $(all_constituentsclean)
	$(echo) $(all_constituentsclean)
	$(echo) "allclean ok."

#	@echo $(all_constituentsclean)
#	@/bin/echo " allclean ok."

allclean :: makefilesclean

#-- end of group ------
#-- start of group ------

all_groups :: cmt_actions

cmt_actions :: $(cmt_actions_dependencies)  $(cmt_actions_pre_constituents) $(cmt_actions_constituents)  $(cmt_actions_post_constituents)
	$(echo) "cmt_actions ok."

#	@/bin/echo " cmt_actions ok."

clean :: allclean

cmt_actionsclean ::  $(cmt_actions_constituentsclean)
	$(echo) $(cmt_actions_constituentsclean)
	$(echo) "cmt_actionsclean ok."

#	@echo $(cmt_actions_constituentsclean)
#	@/bin/echo " cmt_actionsclean ok."

cmt_actionsclean :: makefilesclean

#-- end of group ------
#-- start of constituent ------

cmt_BplusToPhiMuNu_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_BplusToPhiMuNu_has_target_tag

#cmt_local_tagfile_BplusToPhiMuNu = $(BplusToPhiMuNu_tag)_BplusToPhiMuNu.make
cmt_local_tagfile_BplusToPhiMuNu = $(bin)$(BplusToPhiMuNu_tag)_BplusToPhiMuNu.make
cmt_local_setup_BplusToPhiMuNu = $(bin)setup_BplusToPhiMuNu$$$$.make
cmt_final_setup_BplusToPhiMuNu = $(bin)setup_BplusToPhiMuNu.make
#cmt_final_setup_BplusToPhiMuNu = $(bin)BplusToPhiMuNu_BplusToPhiMuNusetup.make
cmt_local_BplusToPhiMuNu_makefile = $(bin)BplusToPhiMuNu.make

BplusToPhiMuNu_extratags = -tag_add=target_BplusToPhiMuNu

#$(cmt_local_tagfile_BplusToPhiMuNu) : $(cmt_lock_setup)
ifndef QUICK
$(cmt_local_tagfile_BplusToPhiMuNu) ::
else
$(cmt_local_tagfile_BplusToPhiMuNu) :
endif
	$(echo) "(constituents.make) Rebuilding $@"; \
	  if test -f $(cmt_local_tagfile_BplusToPhiMuNu); then /bin/rm -f $(cmt_local_tagfile_BplusToPhiMuNu); fi ; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNu_extratags) build tag_makefile >>$(cmt_local_tagfile_BplusToPhiMuNu)
	$(echo) "(constituents.make) Rebuilding $(cmt_final_setup_BplusToPhiMuNu)"; \
	  test ! -f $(cmt_local_setup_BplusToPhiMuNu) || \rm -f $(cmt_local_setup_BplusToPhiMuNu); \
	  trap '\rm -f $(cmt_local_setup_BplusToPhiMuNu)' 0 1 2 15; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNu_extratags) show setup >$(cmt_local_setup_BplusToPhiMuNu) && \
	  if [ -f $(cmt_final_setup_BplusToPhiMuNu) ] && \
	    \cmp -s $(cmt_final_setup_BplusToPhiMuNu) $(cmt_local_setup_BplusToPhiMuNu); then \
	    \rm $(cmt_local_setup_BplusToPhiMuNu); else \
	    \mv -f $(cmt_local_setup_BplusToPhiMuNu) $(cmt_final_setup_BplusToPhiMuNu); fi

else

#cmt_local_tagfile_BplusToPhiMuNu = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_BplusToPhiMuNu = $(bin)$(BplusToPhiMuNu_tag).make
cmt_final_setup_BplusToPhiMuNu = $(bin)setup.make
#cmt_final_setup_BplusToPhiMuNu = $(bin)BplusToPhiMuNusetup.make
cmt_local_BplusToPhiMuNu_makefile = $(bin)BplusToPhiMuNu.make

endif

ifdef STRUCTURED_OUTPUT
BplusToPhiMuNudirs :
	@if test ! -d $(bin)BplusToPhiMuNu; then $(mkdir) -p $(bin)BplusToPhiMuNu; fi
	$(echo) "STRUCTURED_OUTPUT="$(bin)BplusToPhiMuNu
else
BplusToPhiMuNudirs : ;
endif

#ifndef QUICK
#ifdef STRUCTURED_OUTPUT
# BplusToPhiMuNudirs ::
#	@if test ! -d $(bin)BplusToPhiMuNu; then $(mkdir) -p $(bin)BplusToPhiMuNu; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)BplusToPhiMuNu
#
#$(cmt_local_BplusToPhiMuNu_makefile) :: $(BplusToPhiMuNu_dependencies) $(cmt_local_tagfile_BplusToPhiMuNu) build_library_links dirs BplusToPhiMuNudirs
#else
#$(cmt_local_BplusToPhiMuNu_makefile) :: $(BplusToPhiMuNu_dependencies) $(cmt_local_tagfile_BplusToPhiMuNu) build_library_links dirs
#endif
#else
#$(cmt_local_BplusToPhiMuNu_makefile) :: $(cmt_local_tagfile_BplusToPhiMuNu)
#endif

makefiles : $(cmt_local_BplusToPhiMuNu_makefile)

ifndef QUICK
$(cmt_local_BplusToPhiMuNu_makefile) : $(BplusToPhiMuNu_dependencies) $(cmt_local_tagfile_BplusToPhiMuNu) build_library_links
else
$(cmt_local_BplusToPhiMuNu_makefile) : $(cmt_local_tagfile_BplusToPhiMuNu)
endif
	$(echo) "(constituents.make) Building BplusToPhiMuNu.make"; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNu_extratags) build constituent_makefile -out=$(cmt_local_BplusToPhiMuNu_makefile) BplusToPhiMuNu

BplusToPhiMuNu :: $(BplusToPhiMuNu_dependencies) $(cmt_local_BplusToPhiMuNu_makefile) dirs BplusToPhiMuNudirs
	$(echo) "(constituents.make) Starting BplusToPhiMuNu"
	@$(MAKE) -f $(cmt_local_BplusToPhiMuNu_makefile) BplusToPhiMuNu
	$(echo) "(constituents.make) BplusToPhiMuNu done"

clean :: BplusToPhiMuNuclean

BplusToPhiMuNuclean :: $(BplusToPhiMuNuclean_dependencies) ##$(cmt_local_BplusToPhiMuNu_makefile)
	$(echo) "(constituents.make) Starting BplusToPhiMuNuclean"
	@-if test -f $(cmt_local_BplusToPhiMuNu_makefile); then \
	  $(MAKE) -f $(cmt_local_BplusToPhiMuNu_makefile) BplusToPhiMuNuclean; \
	fi
	$(echo) "(constituents.make) BplusToPhiMuNuclean done"
#	@-$(MAKE) -f $(cmt_local_BplusToPhiMuNu_makefile) BplusToPhiMuNuclean

##	  /bin/rm -f $(cmt_local_BplusToPhiMuNu_makefile) $(bin)BplusToPhiMuNu_dependencies.make

install :: BplusToPhiMuNuinstall

BplusToPhiMuNuinstall :: $(BplusToPhiMuNu_dependencies) $(cmt_local_BplusToPhiMuNu_makefile)
	$(echo) "(constituents.make) Starting install BplusToPhiMuNu"
	@-$(MAKE) -f $(cmt_local_BplusToPhiMuNu_makefile) install
	$(echo) "(constituents.make) install BplusToPhiMuNu done"

uninstall :: BplusToPhiMuNuuninstall

$(foreach d,$(BplusToPhiMuNu_dependencies),$(eval $(d)uninstall_dependencies += BplusToPhiMuNuuninstall))

BplusToPhiMuNuuninstall :: $(BplusToPhiMuNuuninstall_dependencies) $(cmt_local_BplusToPhiMuNu_makefile)
	$(echo) "(constituents.make) Starting uninstall BplusToPhiMuNu"
	@$(MAKE) -f $(cmt_local_BplusToPhiMuNu_makefile) uninstall
	$(echo) "(constituents.make) uninstall BplusToPhiMuNu done"

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(constituents.make) Starting $@ BplusToPhiMuNu"
	$(echo) Using default action for $@
	$(echo) "(constituents.make) $@ BplusToPhiMuNu done"
endif

#-- end of constituent ------
#-- start of constituent_lock ------

cmt_BplusToPhiMuNuComponentsList_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_BplusToPhiMuNuComponentsList_has_target_tag

#cmt_local_tagfile_BplusToPhiMuNuComponentsList = $(BplusToPhiMuNu_tag)_BplusToPhiMuNuComponentsList.make
cmt_local_tagfile_BplusToPhiMuNuComponentsList = $(bin)$(BplusToPhiMuNu_tag)_BplusToPhiMuNuComponentsList.make
cmt_local_setup_BplusToPhiMuNuComponentsList = $(bin)setup_BplusToPhiMuNuComponentsList$$$$.make
cmt_final_setup_BplusToPhiMuNuComponentsList = $(bin)setup_BplusToPhiMuNuComponentsList.make
#cmt_final_setup_BplusToPhiMuNuComponentsList = $(bin)BplusToPhiMuNu_BplusToPhiMuNuComponentsListsetup.make
cmt_local_BplusToPhiMuNuComponentsList_makefile = $(bin)BplusToPhiMuNuComponentsList.make

BplusToPhiMuNuComponentsList_extratags = -tag_add=target_BplusToPhiMuNuComponentsList

#$(cmt_local_tagfile_BplusToPhiMuNuComponentsList) : $(cmt_lock_setup)
ifndef QUICK
$(cmt_local_tagfile_BplusToPhiMuNuComponentsList) ::
else
$(cmt_local_tagfile_BplusToPhiMuNuComponentsList) :
endif
	$(echo) "(constituents.make) Rebuilding $@"; \
	  if test -f $(cmt_local_tagfile_BplusToPhiMuNuComponentsList); then /bin/rm -f $(cmt_local_tagfile_BplusToPhiMuNuComponentsList); fi ; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNuComponentsList_extratags) build tag_makefile >>$(cmt_local_tagfile_BplusToPhiMuNuComponentsList)
	$(echo) "(constituents.make) Rebuilding $(cmt_final_setup_BplusToPhiMuNuComponentsList)"; \
	  test ! -f $(cmt_local_setup_BplusToPhiMuNuComponentsList) || \rm -f $(cmt_local_setup_BplusToPhiMuNuComponentsList); \
	  trap '\rm -f $(cmt_local_setup_BplusToPhiMuNuComponentsList)' 0 1 2 15; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNuComponentsList_extratags) show setup >$(cmt_local_setup_BplusToPhiMuNuComponentsList) && \
	  if [ -f $(cmt_final_setup_BplusToPhiMuNuComponentsList) ] && \
	    \cmp -s $(cmt_final_setup_BplusToPhiMuNuComponentsList) $(cmt_local_setup_BplusToPhiMuNuComponentsList); then \
	    \rm $(cmt_local_setup_BplusToPhiMuNuComponentsList); else \
	    \mv -f $(cmt_local_setup_BplusToPhiMuNuComponentsList) $(cmt_final_setup_BplusToPhiMuNuComponentsList); fi

else

#cmt_local_tagfile_BplusToPhiMuNuComponentsList = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_BplusToPhiMuNuComponentsList = $(bin)$(BplusToPhiMuNu_tag).make
cmt_final_setup_BplusToPhiMuNuComponentsList = $(bin)setup.make
#cmt_final_setup_BplusToPhiMuNuComponentsList = $(bin)BplusToPhiMuNusetup.make
cmt_local_BplusToPhiMuNuComponentsList_makefile = $(bin)BplusToPhiMuNuComponentsList.make

endif

ifdef STRUCTURED_OUTPUT
BplusToPhiMuNuComponentsListdirs :
	@if test ! -d $(bin)BplusToPhiMuNuComponentsList; then $(mkdir) -p $(bin)BplusToPhiMuNuComponentsList; fi
	$(echo) "STRUCTURED_OUTPUT="$(bin)BplusToPhiMuNuComponentsList
else
BplusToPhiMuNuComponentsListdirs : ;
endif

#ifndef QUICK
#ifdef STRUCTURED_OUTPUT
# BplusToPhiMuNuComponentsListdirs ::
#	@if test ! -d $(bin)BplusToPhiMuNuComponentsList; then $(mkdir) -p $(bin)BplusToPhiMuNuComponentsList; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)BplusToPhiMuNuComponentsList
#
#$(cmt_local_BplusToPhiMuNuComponentsList_makefile) :: $(BplusToPhiMuNuComponentsList_dependencies) $(cmt_local_tagfile_BplusToPhiMuNuComponentsList) build_library_links dirs BplusToPhiMuNuComponentsListdirs
#else
#$(cmt_local_BplusToPhiMuNuComponentsList_makefile) :: $(BplusToPhiMuNuComponentsList_dependencies) $(cmt_local_tagfile_BplusToPhiMuNuComponentsList) build_library_links dirs
#endif
#else
#$(cmt_local_BplusToPhiMuNuComponentsList_makefile) :: $(cmt_local_tagfile_BplusToPhiMuNuComponentsList)
#endif

makefiles : $(cmt_local_BplusToPhiMuNuComponentsList_makefile)

ifndef QUICK
$(cmt_local_BplusToPhiMuNuComponentsList_makefile) : $(BplusToPhiMuNuComponentsList_dependencies) $(cmt_local_tagfile_BplusToPhiMuNuComponentsList) build_library_links
else
$(cmt_local_BplusToPhiMuNuComponentsList_makefile) : $(cmt_local_tagfile_BplusToPhiMuNuComponentsList)
endif
	$(echo) "(constituents.make) Building BplusToPhiMuNuComponentsList.make"; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNuComponentsList_extratags) build constituent_makefile -out=$(cmt_local_BplusToPhiMuNuComponentsList_makefile) BplusToPhiMuNuComponentsList

BplusToPhiMuNuComponentsList :: $(BplusToPhiMuNuComponentsList_dependencies) $(cmt_local_BplusToPhiMuNuComponentsList_makefile) dirs BplusToPhiMuNuComponentsListdirs
	$(echo) "(constituents.make) Creating BplusToPhiMuNuComponentsList${lock_suffix} and Starting BplusToPhiMuNuComponentsList"
	@${lock_command} BplusToPhiMuNuComponentsList${lock_suffix} || exit $$?; \
	  retval=$$?; \
	  trap '${unlock_command} BplusToPhiMuNuComponentsList${lock_suffix}; exit $${retval}' 1 2 15; \
	  $(MAKE) -f $(cmt_local_BplusToPhiMuNuComponentsList_makefile) BplusToPhiMuNuComponentsList; \
	  retval=$$?; ${unlock_command} BplusToPhiMuNuComponentsList${lock_suffix}; exit $${retval}
	$(echo) "(constituents.make) BplusToPhiMuNuComponentsList done"

clean :: BplusToPhiMuNuComponentsListclean

BplusToPhiMuNuComponentsListclean :: $(BplusToPhiMuNuComponentsListclean_dependencies) ##$(cmt_local_BplusToPhiMuNuComponentsList_makefile)
	$(echo) "(constituents.make) Starting BplusToPhiMuNuComponentsListclean"
	@-if test -f $(cmt_local_BplusToPhiMuNuComponentsList_makefile); then \
	  $(MAKE) -f $(cmt_local_BplusToPhiMuNuComponentsList_makefile) BplusToPhiMuNuComponentsListclean; \
	fi
	$(echo) "(constituents.make) BplusToPhiMuNuComponentsListclean done"
#	@-$(MAKE) -f $(cmt_local_BplusToPhiMuNuComponentsList_makefile) BplusToPhiMuNuComponentsListclean

##	  /bin/rm -f $(cmt_local_BplusToPhiMuNuComponentsList_makefile) $(bin)BplusToPhiMuNuComponentsList_dependencies.make

install :: BplusToPhiMuNuComponentsListinstall

BplusToPhiMuNuComponentsListinstall :: $(BplusToPhiMuNuComponentsList_dependencies) $(cmt_local_BplusToPhiMuNuComponentsList_makefile)
	$(echo) "(constituents.make) Starting install BplusToPhiMuNuComponentsList"
	@-$(MAKE) -f $(cmt_local_BplusToPhiMuNuComponentsList_makefile) install
	$(echo) "(constituents.make) install BplusToPhiMuNuComponentsList done"

uninstall :: BplusToPhiMuNuComponentsListuninstall

$(foreach d,$(BplusToPhiMuNuComponentsList_dependencies),$(eval $(d)uninstall_dependencies += BplusToPhiMuNuComponentsListuninstall))

BplusToPhiMuNuComponentsListuninstall :: $(BplusToPhiMuNuComponentsListuninstall_dependencies) $(cmt_local_BplusToPhiMuNuComponentsList_makefile)
	$(echo) "(constituents.make) Starting uninstall BplusToPhiMuNuComponentsList"
	@$(MAKE) -f $(cmt_local_BplusToPhiMuNuComponentsList_makefile) uninstall
	$(echo) "(constituents.make) uninstall BplusToPhiMuNuComponentsList done"

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(constituents.make) Starting $@ BplusToPhiMuNuComponentsList"
	$(echo) Using default action for $@
	$(echo) "(constituents.make) $@ BplusToPhiMuNuComponentsList done"
endif

#-- end of constituent_lock ------
#-- start of constituent_lock ------

cmt_BplusToPhiMuNuMergeComponentsList_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_BplusToPhiMuNuMergeComponentsList_has_target_tag

#cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList = $(BplusToPhiMuNu_tag)_BplusToPhiMuNuMergeComponentsList.make
cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList = $(bin)$(BplusToPhiMuNu_tag)_BplusToPhiMuNuMergeComponentsList.make
cmt_local_setup_BplusToPhiMuNuMergeComponentsList = $(bin)setup_BplusToPhiMuNuMergeComponentsList$$$$.make
cmt_final_setup_BplusToPhiMuNuMergeComponentsList = $(bin)setup_BplusToPhiMuNuMergeComponentsList.make
#cmt_final_setup_BplusToPhiMuNuMergeComponentsList = $(bin)BplusToPhiMuNu_BplusToPhiMuNuMergeComponentsListsetup.make
cmt_local_BplusToPhiMuNuMergeComponentsList_makefile = $(bin)BplusToPhiMuNuMergeComponentsList.make

BplusToPhiMuNuMergeComponentsList_extratags = -tag_add=target_BplusToPhiMuNuMergeComponentsList

#$(cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList) : $(cmt_lock_setup)
ifndef QUICK
$(cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList) ::
else
$(cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList) :
endif
	$(echo) "(constituents.make) Rebuilding $@"; \
	  if test -f $(cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList); then /bin/rm -f $(cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList); fi ; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNuMergeComponentsList_extratags) build tag_makefile >>$(cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList)
	$(echo) "(constituents.make) Rebuilding $(cmt_final_setup_BplusToPhiMuNuMergeComponentsList)"; \
	  test ! -f $(cmt_local_setup_BplusToPhiMuNuMergeComponentsList) || \rm -f $(cmt_local_setup_BplusToPhiMuNuMergeComponentsList); \
	  trap '\rm -f $(cmt_local_setup_BplusToPhiMuNuMergeComponentsList)' 0 1 2 15; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNuMergeComponentsList_extratags) show setup >$(cmt_local_setup_BplusToPhiMuNuMergeComponentsList) && \
	  if [ -f $(cmt_final_setup_BplusToPhiMuNuMergeComponentsList) ] && \
	    \cmp -s $(cmt_final_setup_BplusToPhiMuNuMergeComponentsList) $(cmt_local_setup_BplusToPhiMuNuMergeComponentsList); then \
	    \rm $(cmt_local_setup_BplusToPhiMuNuMergeComponentsList); else \
	    \mv -f $(cmt_local_setup_BplusToPhiMuNuMergeComponentsList) $(cmt_final_setup_BplusToPhiMuNuMergeComponentsList); fi

else

#cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList = $(bin)$(BplusToPhiMuNu_tag).make
cmt_final_setup_BplusToPhiMuNuMergeComponentsList = $(bin)setup.make
#cmt_final_setup_BplusToPhiMuNuMergeComponentsList = $(bin)BplusToPhiMuNusetup.make
cmt_local_BplusToPhiMuNuMergeComponentsList_makefile = $(bin)BplusToPhiMuNuMergeComponentsList.make

endif

ifdef STRUCTURED_OUTPUT
BplusToPhiMuNuMergeComponentsListdirs :
	@if test ! -d $(bin)BplusToPhiMuNuMergeComponentsList; then $(mkdir) -p $(bin)BplusToPhiMuNuMergeComponentsList; fi
	$(echo) "STRUCTURED_OUTPUT="$(bin)BplusToPhiMuNuMergeComponentsList
else
BplusToPhiMuNuMergeComponentsListdirs : ;
endif

#ifndef QUICK
#ifdef STRUCTURED_OUTPUT
# BplusToPhiMuNuMergeComponentsListdirs ::
#	@if test ! -d $(bin)BplusToPhiMuNuMergeComponentsList; then $(mkdir) -p $(bin)BplusToPhiMuNuMergeComponentsList; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)BplusToPhiMuNuMergeComponentsList
#
#$(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile) :: $(BplusToPhiMuNuMergeComponentsList_dependencies) $(cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList) build_library_links dirs BplusToPhiMuNuMergeComponentsListdirs
#else
#$(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile) :: $(BplusToPhiMuNuMergeComponentsList_dependencies) $(cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList) build_library_links dirs
#endif
#else
#$(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile) :: $(cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList)
#endif

makefiles : $(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile)

ifndef QUICK
$(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile) : $(BplusToPhiMuNuMergeComponentsList_dependencies) $(cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList) build_library_links
else
$(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile) : $(cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList)
endif
	$(echo) "(constituents.make) Building BplusToPhiMuNuMergeComponentsList.make"; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNuMergeComponentsList_extratags) build constituent_makefile -out=$(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile) BplusToPhiMuNuMergeComponentsList

BplusToPhiMuNuMergeComponentsList :: $(BplusToPhiMuNuMergeComponentsList_dependencies) $(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile) dirs BplusToPhiMuNuMergeComponentsListdirs
	$(echo) "(constituents.make) Creating BplusToPhiMuNuMergeComponentsList${lock_suffix} and Starting BplusToPhiMuNuMergeComponentsList"
	@${lock_command} BplusToPhiMuNuMergeComponentsList${lock_suffix} || exit $$?; \
	  retval=$$?; \
	  trap '${unlock_command} BplusToPhiMuNuMergeComponentsList${lock_suffix}; exit $${retval}' 1 2 15; \
	  $(MAKE) -f $(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile) BplusToPhiMuNuMergeComponentsList; \
	  retval=$$?; ${unlock_command} BplusToPhiMuNuMergeComponentsList${lock_suffix}; exit $${retval}
	$(echo) "(constituents.make) BplusToPhiMuNuMergeComponentsList done"

clean :: BplusToPhiMuNuMergeComponentsListclean

BplusToPhiMuNuMergeComponentsListclean :: $(BplusToPhiMuNuMergeComponentsListclean_dependencies) ##$(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile)
	$(echo) "(constituents.make) Starting BplusToPhiMuNuMergeComponentsListclean"
	@-if test -f $(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile); then \
	  $(MAKE) -f $(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile) BplusToPhiMuNuMergeComponentsListclean; \
	fi
	$(echo) "(constituents.make) BplusToPhiMuNuMergeComponentsListclean done"
#	@-$(MAKE) -f $(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile) BplusToPhiMuNuMergeComponentsListclean

##	  /bin/rm -f $(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile) $(bin)BplusToPhiMuNuMergeComponentsList_dependencies.make

install :: BplusToPhiMuNuMergeComponentsListinstall

BplusToPhiMuNuMergeComponentsListinstall :: $(BplusToPhiMuNuMergeComponentsList_dependencies) $(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile)
	$(echo) "(constituents.make) Starting install BplusToPhiMuNuMergeComponentsList"
	@-$(MAKE) -f $(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile) install
	$(echo) "(constituents.make) install BplusToPhiMuNuMergeComponentsList done"

uninstall :: BplusToPhiMuNuMergeComponentsListuninstall

$(foreach d,$(BplusToPhiMuNuMergeComponentsList_dependencies),$(eval $(d)uninstall_dependencies += BplusToPhiMuNuMergeComponentsListuninstall))

BplusToPhiMuNuMergeComponentsListuninstall :: $(BplusToPhiMuNuMergeComponentsListuninstall_dependencies) $(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile)
	$(echo) "(constituents.make) Starting uninstall BplusToPhiMuNuMergeComponentsList"
	@$(MAKE) -f $(cmt_local_BplusToPhiMuNuMergeComponentsList_makefile) uninstall
	$(echo) "(constituents.make) uninstall BplusToPhiMuNuMergeComponentsList done"

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(constituents.make) Starting $@ BplusToPhiMuNuMergeComponentsList"
	$(echo) Using default action for $@
	$(echo) "(constituents.make) $@ BplusToPhiMuNuMergeComponentsList done"
endif

#-- end of constituent_lock ------
#-- start of constituent_lock ------

cmt_BplusToPhiMuNuConf_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_BplusToPhiMuNuConf_has_target_tag

#cmt_local_tagfile_BplusToPhiMuNuConf = $(BplusToPhiMuNu_tag)_BplusToPhiMuNuConf.make
cmt_local_tagfile_BplusToPhiMuNuConf = $(bin)$(BplusToPhiMuNu_tag)_BplusToPhiMuNuConf.make
cmt_local_setup_BplusToPhiMuNuConf = $(bin)setup_BplusToPhiMuNuConf$$$$.make
cmt_final_setup_BplusToPhiMuNuConf = $(bin)setup_BplusToPhiMuNuConf.make
#cmt_final_setup_BplusToPhiMuNuConf = $(bin)BplusToPhiMuNu_BplusToPhiMuNuConfsetup.make
cmt_local_BplusToPhiMuNuConf_makefile = $(bin)BplusToPhiMuNuConf.make

BplusToPhiMuNuConf_extratags = -tag_add=target_BplusToPhiMuNuConf

#$(cmt_local_tagfile_BplusToPhiMuNuConf) : $(cmt_lock_setup)
ifndef QUICK
$(cmt_local_tagfile_BplusToPhiMuNuConf) ::
else
$(cmt_local_tagfile_BplusToPhiMuNuConf) :
endif
	$(echo) "(constituents.make) Rebuilding $@"; \
	  if test -f $(cmt_local_tagfile_BplusToPhiMuNuConf); then /bin/rm -f $(cmt_local_tagfile_BplusToPhiMuNuConf); fi ; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNuConf_extratags) build tag_makefile >>$(cmt_local_tagfile_BplusToPhiMuNuConf)
	$(echo) "(constituents.make) Rebuilding $(cmt_final_setup_BplusToPhiMuNuConf)"; \
	  test ! -f $(cmt_local_setup_BplusToPhiMuNuConf) || \rm -f $(cmt_local_setup_BplusToPhiMuNuConf); \
	  trap '\rm -f $(cmt_local_setup_BplusToPhiMuNuConf)' 0 1 2 15; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNuConf_extratags) show setup >$(cmt_local_setup_BplusToPhiMuNuConf) && \
	  if [ -f $(cmt_final_setup_BplusToPhiMuNuConf) ] && \
	    \cmp -s $(cmt_final_setup_BplusToPhiMuNuConf) $(cmt_local_setup_BplusToPhiMuNuConf); then \
	    \rm $(cmt_local_setup_BplusToPhiMuNuConf); else \
	    \mv -f $(cmt_local_setup_BplusToPhiMuNuConf) $(cmt_final_setup_BplusToPhiMuNuConf); fi

else

#cmt_local_tagfile_BplusToPhiMuNuConf = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_BplusToPhiMuNuConf = $(bin)$(BplusToPhiMuNu_tag).make
cmt_final_setup_BplusToPhiMuNuConf = $(bin)setup.make
#cmt_final_setup_BplusToPhiMuNuConf = $(bin)BplusToPhiMuNusetup.make
cmt_local_BplusToPhiMuNuConf_makefile = $(bin)BplusToPhiMuNuConf.make

endif

ifdef STRUCTURED_OUTPUT
BplusToPhiMuNuConfdirs :
	@if test ! -d $(bin)BplusToPhiMuNuConf; then $(mkdir) -p $(bin)BplusToPhiMuNuConf; fi
	$(echo) "STRUCTURED_OUTPUT="$(bin)BplusToPhiMuNuConf
else
BplusToPhiMuNuConfdirs : ;
endif

#ifndef QUICK
#ifdef STRUCTURED_OUTPUT
# BplusToPhiMuNuConfdirs ::
#	@if test ! -d $(bin)BplusToPhiMuNuConf; then $(mkdir) -p $(bin)BplusToPhiMuNuConf; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)BplusToPhiMuNuConf
#
#$(cmt_local_BplusToPhiMuNuConf_makefile) :: $(BplusToPhiMuNuConf_dependencies) $(cmt_local_tagfile_BplusToPhiMuNuConf) build_library_links dirs BplusToPhiMuNuConfdirs
#else
#$(cmt_local_BplusToPhiMuNuConf_makefile) :: $(BplusToPhiMuNuConf_dependencies) $(cmt_local_tagfile_BplusToPhiMuNuConf) build_library_links dirs
#endif
#else
#$(cmt_local_BplusToPhiMuNuConf_makefile) :: $(cmt_local_tagfile_BplusToPhiMuNuConf)
#endif

makefiles : $(cmt_local_BplusToPhiMuNuConf_makefile)

ifndef QUICK
$(cmt_local_BplusToPhiMuNuConf_makefile) : $(BplusToPhiMuNuConf_dependencies) $(cmt_local_tagfile_BplusToPhiMuNuConf) build_library_links
else
$(cmt_local_BplusToPhiMuNuConf_makefile) : $(cmt_local_tagfile_BplusToPhiMuNuConf)
endif
	$(echo) "(constituents.make) Building BplusToPhiMuNuConf.make"; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNuConf_extratags) build constituent_makefile -out=$(cmt_local_BplusToPhiMuNuConf_makefile) BplusToPhiMuNuConf

BplusToPhiMuNuConf :: $(BplusToPhiMuNuConf_dependencies) $(cmt_local_BplusToPhiMuNuConf_makefile) dirs BplusToPhiMuNuConfdirs
	$(echo) "(constituents.make) Creating BplusToPhiMuNuConf${lock_suffix} and Starting BplusToPhiMuNuConf"
	@${lock_command} BplusToPhiMuNuConf${lock_suffix} || exit $$?; \
	  retval=$$?; \
	  trap '${unlock_command} BplusToPhiMuNuConf${lock_suffix}; exit $${retval}' 1 2 15; \
	  $(MAKE) -f $(cmt_local_BplusToPhiMuNuConf_makefile) BplusToPhiMuNuConf; \
	  retval=$$?; ${unlock_command} BplusToPhiMuNuConf${lock_suffix}; exit $${retval}
	$(echo) "(constituents.make) BplusToPhiMuNuConf done"

clean :: BplusToPhiMuNuConfclean

BplusToPhiMuNuConfclean :: $(BplusToPhiMuNuConfclean_dependencies) ##$(cmt_local_BplusToPhiMuNuConf_makefile)
	$(echo) "(constituents.make) Starting BplusToPhiMuNuConfclean"
	@-if test -f $(cmt_local_BplusToPhiMuNuConf_makefile); then \
	  $(MAKE) -f $(cmt_local_BplusToPhiMuNuConf_makefile) BplusToPhiMuNuConfclean; \
	fi
	$(echo) "(constituents.make) BplusToPhiMuNuConfclean done"
#	@-$(MAKE) -f $(cmt_local_BplusToPhiMuNuConf_makefile) BplusToPhiMuNuConfclean

##	  /bin/rm -f $(cmt_local_BplusToPhiMuNuConf_makefile) $(bin)BplusToPhiMuNuConf_dependencies.make

install :: BplusToPhiMuNuConfinstall

BplusToPhiMuNuConfinstall :: $(BplusToPhiMuNuConf_dependencies) $(cmt_local_BplusToPhiMuNuConf_makefile)
	$(echo) "(constituents.make) Starting install BplusToPhiMuNuConf"
	@-$(MAKE) -f $(cmt_local_BplusToPhiMuNuConf_makefile) install
	$(echo) "(constituents.make) install BplusToPhiMuNuConf done"

uninstall :: BplusToPhiMuNuConfuninstall

$(foreach d,$(BplusToPhiMuNuConf_dependencies),$(eval $(d)uninstall_dependencies += BplusToPhiMuNuConfuninstall))

BplusToPhiMuNuConfuninstall :: $(BplusToPhiMuNuConfuninstall_dependencies) $(cmt_local_BplusToPhiMuNuConf_makefile)
	$(echo) "(constituents.make) Starting uninstall BplusToPhiMuNuConf"
	@$(MAKE) -f $(cmt_local_BplusToPhiMuNuConf_makefile) uninstall
	$(echo) "(constituents.make) uninstall BplusToPhiMuNuConf done"

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(constituents.make) Starting $@ BplusToPhiMuNuConf"
	$(echo) Using default action for $@
	$(echo) "(constituents.make) $@ BplusToPhiMuNuConf done"
endif

#-- end of constituent_lock ------
#-- start of constituent_lock ------

cmt_BplusToPhiMuNu_python_init_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_BplusToPhiMuNu_python_init_has_target_tag

#cmt_local_tagfile_BplusToPhiMuNu_python_init = $(BplusToPhiMuNu_tag)_BplusToPhiMuNu_python_init.make
cmt_local_tagfile_BplusToPhiMuNu_python_init = $(bin)$(BplusToPhiMuNu_tag)_BplusToPhiMuNu_python_init.make
cmt_local_setup_BplusToPhiMuNu_python_init = $(bin)setup_BplusToPhiMuNu_python_init$$$$.make
cmt_final_setup_BplusToPhiMuNu_python_init = $(bin)setup_BplusToPhiMuNu_python_init.make
#cmt_final_setup_BplusToPhiMuNu_python_init = $(bin)BplusToPhiMuNu_BplusToPhiMuNu_python_initsetup.make
cmt_local_BplusToPhiMuNu_python_init_makefile = $(bin)BplusToPhiMuNu_python_init.make

BplusToPhiMuNu_python_init_extratags = -tag_add=target_BplusToPhiMuNu_python_init

#$(cmt_local_tagfile_BplusToPhiMuNu_python_init) : $(cmt_lock_setup)
ifndef QUICK
$(cmt_local_tagfile_BplusToPhiMuNu_python_init) ::
else
$(cmt_local_tagfile_BplusToPhiMuNu_python_init) :
endif
	$(echo) "(constituents.make) Rebuilding $@"; \
	  if test -f $(cmt_local_tagfile_BplusToPhiMuNu_python_init); then /bin/rm -f $(cmt_local_tagfile_BplusToPhiMuNu_python_init); fi ; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNu_python_init_extratags) build tag_makefile >>$(cmt_local_tagfile_BplusToPhiMuNu_python_init)
	$(echo) "(constituents.make) Rebuilding $(cmt_final_setup_BplusToPhiMuNu_python_init)"; \
	  test ! -f $(cmt_local_setup_BplusToPhiMuNu_python_init) || \rm -f $(cmt_local_setup_BplusToPhiMuNu_python_init); \
	  trap '\rm -f $(cmt_local_setup_BplusToPhiMuNu_python_init)' 0 1 2 15; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNu_python_init_extratags) show setup >$(cmt_local_setup_BplusToPhiMuNu_python_init) && \
	  if [ -f $(cmt_final_setup_BplusToPhiMuNu_python_init) ] && \
	    \cmp -s $(cmt_final_setup_BplusToPhiMuNu_python_init) $(cmt_local_setup_BplusToPhiMuNu_python_init); then \
	    \rm $(cmt_local_setup_BplusToPhiMuNu_python_init); else \
	    \mv -f $(cmt_local_setup_BplusToPhiMuNu_python_init) $(cmt_final_setup_BplusToPhiMuNu_python_init); fi

else

#cmt_local_tagfile_BplusToPhiMuNu_python_init = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_BplusToPhiMuNu_python_init = $(bin)$(BplusToPhiMuNu_tag).make
cmt_final_setup_BplusToPhiMuNu_python_init = $(bin)setup.make
#cmt_final_setup_BplusToPhiMuNu_python_init = $(bin)BplusToPhiMuNusetup.make
cmt_local_BplusToPhiMuNu_python_init_makefile = $(bin)BplusToPhiMuNu_python_init.make

endif

ifdef STRUCTURED_OUTPUT
BplusToPhiMuNu_python_initdirs :
	@if test ! -d $(bin)BplusToPhiMuNu_python_init; then $(mkdir) -p $(bin)BplusToPhiMuNu_python_init; fi
	$(echo) "STRUCTURED_OUTPUT="$(bin)BplusToPhiMuNu_python_init
else
BplusToPhiMuNu_python_initdirs : ;
endif

#ifndef QUICK
#ifdef STRUCTURED_OUTPUT
# BplusToPhiMuNu_python_initdirs ::
#	@if test ! -d $(bin)BplusToPhiMuNu_python_init; then $(mkdir) -p $(bin)BplusToPhiMuNu_python_init; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)BplusToPhiMuNu_python_init
#
#$(cmt_local_BplusToPhiMuNu_python_init_makefile) :: $(BplusToPhiMuNu_python_init_dependencies) $(cmt_local_tagfile_BplusToPhiMuNu_python_init) build_library_links dirs BplusToPhiMuNu_python_initdirs
#else
#$(cmt_local_BplusToPhiMuNu_python_init_makefile) :: $(BplusToPhiMuNu_python_init_dependencies) $(cmt_local_tagfile_BplusToPhiMuNu_python_init) build_library_links dirs
#endif
#else
#$(cmt_local_BplusToPhiMuNu_python_init_makefile) :: $(cmt_local_tagfile_BplusToPhiMuNu_python_init)
#endif

makefiles : $(cmt_local_BplusToPhiMuNu_python_init_makefile)

ifndef QUICK
$(cmt_local_BplusToPhiMuNu_python_init_makefile) : $(BplusToPhiMuNu_python_init_dependencies) $(cmt_local_tagfile_BplusToPhiMuNu_python_init) build_library_links
else
$(cmt_local_BplusToPhiMuNu_python_init_makefile) : $(cmt_local_tagfile_BplusToPhiMuNu_python_init)
endif
	$(echo) "(constituents.make) Building BplusToPhiMuNu_python_init.make"; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNu_python_init_extratags) build constituent_makefile -out=$(cmt_local_BplusToPhiMuNu_python_init_makefile) BplusToPhiMuNu_python_init

BplusToPhiMuNu_python_init :: $(BplusToPhiMuNu_python_init_dependencies) $(cmt_local_BplusToPhiMuNu_python_init_makefile) dirs BplusToPhiMuNu_python_initdirs
	$(echo) "(constituents.make) Creating BplusToPhiMuNu_python_init${lock_suffix} and Starting BplusToPhiMuNu_python_init"
	@${lock_command} BplusToPhiMuNu_python_init${lock_suffix} || exit $$?; \
	  retval=$$?; \
	  trap '${unlock_command} BplusToPhiMuNu_python_init${lock_suffix}; exit $${retval}' 1 2 15; \
	  $(MAKE) -f $(cmt_local_BplusToPhiMuNu_python_init_makefile) BplusToPhiMuNu_python_init; \
	  retval=$$?; ${unlock_command} BplusToPhiMuNu_python_init${lock_suffix}; exit $${retval}
	$(echo) "(constituents.make) BplusToPhiMuNu_python_init done"

clean :: BplusToPhiMuNu_python_initclean

BplusToPhiMuNu_python_initclean :: $(BplusToPhiMuNu_python_initclean_dependencies) ##$(cmt_local_BplusToPhiMuNu_python_init_makefile)
	$(echo) "(constituents.make) Starting BplusToPhiMuNu_python_initclean"
	@-if test -f $(cmt_local_BplusToPhiMuNu_python_init_makefile); then \
	  $(MAKE) -f $(cmt_local_BplusToPhiMuNu_python_init_makefile) BplusToPhiMuNu_python_initclean; \
	fi
	$(echo) "(constituents.make) BplusToPhiMuNu_python_initclean done"
#	@-$(MAKE) -f $(cmt_local_BplusToPhiMuNu_python_init_makefile) BplusToPhiMuNu_python_initclean

##	  /bin/rm -f $(cmt_local_BplusToPhiMuNu_python_init_makefile) $(bin)BplusToPhiMuNu_python_init_dependencies.make

install :: BplusToPhiMuNu_python_initinstall

BplusToPhiMuNu_python_initinstall :: $(BplusToPhiMuNu_python_init_dependencies) $(cmt_local_BplusToPhiMuNu_python_init_makefile)
	$(echo) "(constituents.make) Starting install BplusToPhiMuNu_python_init"
	@-$(MAKE) -f $(cmt_local_BplusToPhiMuNu_python_init_makefile) install
	$(echo) "(constituents.make) install BplusToPhiMuNu_python_init done"

uninstall :: BplusToPhiMuNu_python_inituninstall

$(foreach d,$(BplusToPhiMuNu_python_init_dependencies),$(eval $(d)uninstall_dependencies += BplusToPhiMuNu_python_inituninstall))

BplusToPhiMuNu_python_inituninstall :: $(BplusToPhiMuNu_python_inituninstall_dependencies) $(cmt_local_BplusToPhiMuNu_python_init_makefile)
	$(echo) "(constituents.make) Starting uninstall BplusToPhiMuNu_python_init"
	@$(MAKE) -f $(cmt_local_BplusToPhiMuNu_python_init_makefile) uninstall
	$(echo) "(constituents.make) uninstall BplusToPhiMuNu_python_init done"

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(constituents.make) Starting $@ BplusToPhiMuNu_python_init"
	$(echo) Using default action for $@
	$(echo) "(constituents.make) $@ BplusToPhiMuNu_python_init done"
endif

#-- end of constituent_lock ------
#-- start of constituent_lock ------

cmt_zip_BplusToPhiMuNu_python_modules_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_zip_BplusToPhiMuNu_python_modules_has_target_tag

#cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules = $(BplusToPhiMuNu_tag)_zip_BplusToPhiMuNu_python_modules.make
cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules = $(bin)$(BplusToPhiMuNu_tag)_zip_BplusToPhiMuNu_python_modules.make
cmt_local_setup_zip_BplusToPhiMuNu_python_modules = $(bin)setup_zip_BplusToPhiMuNu_python_modules$$$$.make
cmt_final_setup_zip_BplusToPhiMuNu_python_modules = $(bin)setup_zip_BplusToPhiMuNu_python_modules.make
#cmt_final_setup_zip_BplusToPhiMuNu_python_modules = $(bin)BplusToPhiMuNu_zip_BplusToPhiMuNu_python_modulessetup.make
cmt_local_zip_BplusToPhiMuNu_python_modules_makefile = $(bin)zip_BplusToPhiMuNu_python_modules.make

zip_BplusToPhiMuNu_python_modules_extratags = -tag_add=target_zip_BplusToPhiMuNu_python_modules

#$(cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules) : $(cmt_lock_setup)
ifndef QUICK
$(cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules) ::
else
$(cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules) :
endif
	$(echo) "(constituents.make) Rebuilding $@"; \
	  if test -f $(cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules); then /bin/rm -f $(cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules); fi ; \
	  $(cmtexe) -tag=$(tags) $(zip_BplusToPhiMuNu_python_modules_extratags) build tag_makefile >>$(cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules)
	$(echo) "(constituents.make) Rebuilding $(cmt_final_setup_zip_BplusToPhiMuNu_python_modules)"; \
	  test ! -f $(cmt_local_setup_zip_BplusToPhiMuNu_python_modules) || \rm -f $(cmt_local_setup_zip_BplusToPhiMuNu_python_modules); \
	  trap '\rm -f $(cmt_local_setup_zip_BplusToPhiMuNu_python_modules)' 0 1 2 15; \
	  $(cmtexe) -tag=$(tags) $(zip_BplusToPhiMuNu_python_modules_extratags) show setup >$(cmt_local_setup_zip_BplusToPhiMuNu_python_modules) && \
	  if [ -f $(cmt_final_setup_zip_BplusToPhiMuNu_python_modules) ] && \
	    \cmp -s $(cmt_final_setup_zip_BplusToPhiMuNu_python_modules) $(cmt_local_setup_zip_BplusToPhiMuNu_python_modules); then \
	    \rm $(cmt_local_setup_zip_BplusToPhiMuNu_python_modules); else \
	    \mv -f $(cmt_local_setup_zip_BplusToPhiMuNu_python_modules) $(cmt_final_setup_zip_BplusToPhiMuNu_python_modules); fi

else

#cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules = $(bin)$(BplusToPhiMuNu_tag).make
cmt_final_setup_zip_BplusToPhiMuNu_python_modules = $(bin)setup.make
#cmt_final_setup_zip_BplusToPhiMuNu_python_modules = $(bin)BplusToPhiMuNusetup.make
cmt_local_zip_BplusToPhiMuNu_python_modules_makefile = $(bin)zip_BplusToPhiMuNu_python_modules.make

endif

ifdef STRUCTURED_OUTPUT
zip_BplusToPhiMuNu_python_modulesdirs :
	@if test ! -d $(bin)zip_BplusToPhiMuNu_python_modules; then $(mkdir) -p $(bin)zip_BplusToPhiMuNu_python_modules; fi
	$(echo) "STRUCTURED_OUTPUT="$(bin)zip_BplusToPhiMuNu_python_modules
else
zip_BplusToPhiMuNu_python_modulesdirs : ;
endif

#ifndef QUICK
#ifdef STRUCTURED_OUTPUT
# zip_BplusToPhiMuNu_python_modulesdirs ::
#	@if test ! -d $(bin)zip_BplusToPhiMuNu_python_modules; then $(mkdir) -p $(bin)zip_BplusToPhiMuNu_python_modules; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)zip_BplusToPhiMuNu_python_modules
#
#$(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile) :: $(zip_BplusToPhiMuNu_python_modules_dependencies) $(cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules) build_library_links dirs zip_BplusToPhiMuNu_python_modulesdirs
#else
#$(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile) :: $(zip_BplusToPhiMuNu_python_modules_dependencies) $(cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules) build_library_links dirs
#endif
#else
#$(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile) :: $(cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules)
#endif

makefiles : $(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile)

ifndef QUICK
$(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile) : $(zip_BplusToPhiMuNu_python_modules_dependencies) $(cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules) build_library_links
else
$(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile) : $(cmt_local_tagfile_zip_BplusToPhiMuNu_python_modules)
endif
	$(echo) "(constituents.make) Building zip_BplusToPhiMuNu_python_modules.make"; \
	  $(cmtexe) -tag=$(tags) $(zip_BplusToPhiMuNu_python_modules_extratags) build constituent_makefile -out=$(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile) zip_BplusToPhiMuNu_python_modules

zip_BplusToPhiMuNu_python_modules :: $(zip_BplusToPhiMuNu_python_modules_dependencies) $(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile) dirs zip_BplusToPhiMuNu_python_modulesdirs
	$(echo) "(constituents.make) Creating zip_BplusToPhiMuNu_python_modules${lock_suffix} and Starting zip_BplusToPhiMuNu_python_modules"
	@${lock_command} zip_BplusToPhiMuNu_python_modules${lock_suffix} || exit $$?; \
	  retval=$$?; \
	  trap '${unlock_command} zip_BplusToPhiMuNu_python_modules${lock_suffix}; exit $${retval}' 1 2 15; \
	  $(MAKE) -f $(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile) zip_BplusToPhiMuNu_python_modules; \
	  retval=$$?; ${unlock_command} zip_BplusToPhiMuNu_python_modules${lock_suffix}; exit $${retval}
	$(echo) "(constituents.make) zip_BplusToPhiMuNu_python_modules done"

clean :: zip_BplusToPhiMuNu_python_modulesclean

zip_BplusToPhiMuNu_python_modulesclean :: $(zip_BplusToPhiMuNu_python_modulesclean_dependencies) ##$(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile)
	$(echo) "(constituents.make) Starting zip_BplusToPhiMuNu_python_modulesclean"
	@-if test -f $(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile); then \
	  $(MAKE) -f $(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile) zip_BplusToPhiMuNu_python_modulesclean; \
	fi
	$(echo) "(constituents.make) zip_BplusToPhiMuNu_python_modulesclean done"
#	@-$(MAKE) -f $(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile) zip_BplusToPhiMuNu_python_modulesclean

##	  /bin/rm -f $(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile) $(bin)zip_BplusToPhiMuNu_python_modules_dependencies.make

install :: zip_BplusToPhiMuNu_python_modulesinstall

zip_BplusToPhiMuNu_python_modulesinstall :: $(zip_BplusToPhiMuNu_python_modules_dependencies) $(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile)
	$(echo) "(constituents.make) Starting install zip_BplusToPhiMuNu_python_modules"
	@-$(MAKE) -f $(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile) install
	$(echo) "(constituents.make) install zip_BplusToPhiMuNu_python_modules done"

uninstall :: zip_BplusToPhiMuNu_python_modulesuninstall

$(foreach d,$(zip_BplusToPhiMuNu_python_modules_dependencies),$(eval $(d)uninstall_dependencies += zip_BplusToPhiMuNu_python_modulesuninstall))

zip_BplusToPhiMuNu_python_modulesuninstall :: $(zip_BplusToPhiMuNu_python_modulesuninstall_dependencies) $(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile)
	$(echo) "(constituents.make) Starting uninstall zip_BplusToPhiMuNu_python_modules"
	@$(MAKE) -f $(cmt_local_zip_BplusToPhiMuNu_python_modules_makefile) uninstall
	$(echo) "(constituents.make) uninstall zip_BplusToPhiMuNu_python_modules done"

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(constituents.make) Starting $@ zip_BplusToPhiMuNu_python_modules"
	$(echo) Using default action for $@
	$(echo) "(constituents.make) $@ zip_BplusToPhiMuNu_python_modules done"
endif

#-- end of constituent_lock ------
#-- start of constituent_lock ------

cmt_BplusToPhiMuNuConfDbMerge_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_BplusToPhiMuNuConfDbMerge_has_target_tag

#cmt_local_tagfile_BplusToPhiMuNuConfDbMerge = $(BplusToPhiMuNu_tag)_BplusToPhiMuNuConfDbMerge.make
cmt_local_tagfile_BplusToPhiMuNuConfDbMerge = $(bin)$(BplusToPhiMuNu_tag)_BplusToPhiMuNuConfDbMerge.make
cmt_local_setup_BplusToPhiMuNuConfDbMerge = $(bin)setup_BplusToPhiMuNuConfDbMerge$$$$.make
cmt_final_setup_BplusToPhiMuNuConfDbMerge = $(bin)setup_BplusToPhiMuNuConfDbMerge.make
#cmt_final_setup_BplusToPhiMuNuConfDbMerge = $(bin)BplusToPhiMuNu_BplusToPhiMuNuConfDbMergesetup.make
cmt_local_BplusToPhiMuNuConfDbMerge_makefile = $(bin)BplusToPhiMuNuConfDbMerge.make

BplusToPhiMuNuConfDbMerge_extratags = -tag_add=target_BplusToPhiMuNuConfDbMerge

#$(cmt_local_tagfile_BplusToPhiMuNuConfDbMerge) : $(cmt_lock_setup)
ifndef QUICK
$(cmt_local_tagfile_BplusToPhiMuNuConfDbMerge) ::
else
$(cmt_local_tagfile_BplusToPhiMuNuConfDbMerge) :
endif
	$(echo) "(constituents.make) Rebuilding $@"; \
	  if test -f $(cmt_local_tagfile_BplusToPhiMuNuConfDbMerge); then /bin/rm -f $(cmt_local_tagfile_BplusToPhiMuNuConfDbMerge); fi ; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNuConfDbMerge_extratags) build tag_makefile >>$(cmt_local_tagfile_BplusToPhiMuNuConfDbMerge)
	$(echo) "(constituents.make) Rebuilding $(cmt_final_setup_BplusToPhiMuNuConfDbMerge)"; \
	  test ! -f $(cmt_local_setup_BplusToPhiMuNuConfDbMerge) || \rm -f $(cmt_local_setup_BplusToPhiMuNuConfDbMerge); \
	  trap '\rm -f $(cmt_local_setup_BplusToPhiMuNuConfDbMerge)' 0 1 2 15; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNuConfDbMerge_extratags) show setup >$(cmt_local_setup_BplusToPhiMuNuConfDbMerge) && \
	  if [ -f $(cmt_final_setup_BplusToPhiMuNuConfDbMerge) ] && \
	    \cmp -s $(cmt_final_setup_BplusToPhiMuNuConfDbMerge) $(cmt_local_setup_BplusToPhiMuNuConfDbMerge); then \
	    \rm $(cmt_local_setup_BplusToPhiMuNuConfDbMerge); else \
	    \mv -f $(cmt_local_setup_BplusToPhiMuNuConfDbMerge) $(cmt_final_setup_BplusToPhiMuNuConfDbMerge); fi

else

#cmt_local_tagfile_BplusToPhiMuNuConfDbMerge = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_BplusToPhiMuNuConfDbMerge = $(bin)$(BplusToPhiMuNu_tag).make
cmt_final_setup_BplusToPhiMuNuConfDbMerge = $(bin)setup.make
#cmt_final_setup_BplusToPhiMuNuConfDbMerge = $(bin)BplusToPhiMuNusetup.make
cmt_local_BplusToPhiMuNuConfDbMerge_makefile = $(bin)BplusToPhiMuNuConfDbMerge.make

endif

ifdef STRUCTURED_OUTPUT
BplusToPhiMuNuConfDbMergedirs :
	@if test ! -d $(bin)BplusToPhiMuNuConfDbMerge; then $(mkdir) -p $(bin)BplusToPhiMuNuConfDbMerge; fi
	$(echo) "STRUCTURED_OUTPUT="$(bin)BplusToPhiMuNuConfDbMerge
else
BplusToPhiMuNuConfDbMergedirs : ;
endif

#ifndef QUICK
#ifdef STRUCTURED_OUTPUT
# BplusToPhiMuNuConfDbMergedirs ::
#	@if test ! -d $(bin)BplusToPhiMuNuConfDbMerge; then $(mkdir) -p $(bin)BplusToPhiMuNuConfDbMerge; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)BplusToPhiMuNuConfDbMerge
#
#$(cmt_local_BplusToPhiMuNuConfDbMerge_makefile) :: $(BplusToPhiMuNuConfDbMerge_dependencies) $(cmt_local_tagfile_BplusToPhiMuNuConfDbMerge) build_library_links dirs BplusToPhiMuNuConfDbMergedirs
#else
#$(cmt_local_BplusToPhiMuNuConfDbMerge_makefile) :: $(BplusToPhiMuNuConfDbMerge_dependencies) $(cmt_local_tagfile_BplusToPhiMuNuConfDbMerge) build_library_links dirs
#endif
#else
#$(cmt_local_BplusToPhiMuNuConfDbMerge_makefile) :: $(cmt_local_tagfile_BplusToPhiMuNuConfDbMerge)
#endif

makefiles : $(cmt_local_BplusToPhiMuNuConfDbMerge_makefile)

ifndef QUICK
$(cmt_local_BplusToPhiMuNuConfDbMerge_makefile) : $(BplusToPhiMuNuConfDbMerge_dependencies) $(cmt_local_tagfile_BplusToPhiMuNuConfDbMerge) build_library_links
else
$(cmt_local_BplusToPhiMuNuConfDbMerge_makefile) : $(cmt_local_tagfile_BplusToPhiMuNuConfDbMerge)
endif
	$(echo) "(constituents.make) Building BplusToPhiMuNuConfDbMerge.make"; \
	  $(cmtexe) -tag=$(tags) $(BplusToPhiMuNuConfDbMerge_extratags) build constituent_makefile -out=$(cmt_local_BplusToPhiMuNuConfDbMerge_makefile) BplusToPhiMuNuConfDbMerge

BplusToPhiMuNuConfDbMerge :: $(BplusToPhiMuNuConfDbMerge_dependencies) $(cmt_local_BplusToPhiMuNuConfDbMerge_makefile) dirs BplusToPhiMuNuConfDbMergedirs
	$(echo) "(constituents.make) Creating BplusToPhiMuNuConfDbMerge${lock_suffix} and Starting BplusToPhiMuNuConfDbMerge"
	@${lock_command} BplusToPhiMuNuConfDbMerge${lock_suffix} || exit $$?; \
	  retval=$$?; \
	  trap '${unlock_command} BplusToPhiMuNuConfDbMerge${lock_suffix}; exit $${retval}' 1 2 15; \
	  $(MAKE) -f $(cmt_local_BplusToPhiMuNuConfDbMerge_makefile) BplusToPhiMuNuConfDbMerge; \
	  retval=$$?; ${unlock_command} BplusToPhiMuNuConfDbMerge${lock_suffix}; exit $${retval}
	$(echo) "(constituents.make) BplusToPhiMuNuConfDbMerge done"

clean :: BplusToPhiMuNuConfDbMergeclean

BplusToPhiMuNuConfDbMergeclean :: $(BplusToPhiMuNuConfDbMergeclean_dependencies) ##$(cmt_local_BplusToPhiMuNuConfDbMerge_makefile)
	$(echo) "(constituents.make) Starting BplusToPhiMuNuConfDbMergeclean"
	@-if test -f $(cmt_local_BplusToPhiMuNuConfDbMerge_makefile); then \
	  $(MAKE) -f $(cmt_local_BplusToPhiMuNuConfDbMerge_makefile) BplusToPhiMuNuConfDbMergeclean; \
	fi
	$(echo) "(constituents.make) BplusToPhiMuNuConfDbMergeclean done"
#	@-$(MAKE) -f $(cmt_local_BplusToPhiMuNuConfDbMerge_makefile) BplusToPhiMuNuConfDbMergeclean

##	  /bin/rm -f $(cmt_local_BplusToPhiMuNuConfDbMerge_makefile) $(bin)BplusToPhiMuNuConfDbMerge_dependencies.make

install :: BplusToPhiMuNuConfDbMergeinstall

BplusToPhiMuNuConfDbMergeinstall :: $(BplusToPhiMuNuConfDbMerge_dependencies) $(cmt_local_BplusToPhiMuNuConfDbMerge_makefile)
	$(echo) "(constituents.make) Starting install BplusToPhiMuNuConfDbMerge"
	@-$(MAKE) -f $(cmt_local_BplusToPhiMuNuConfDbMerge_makefile) install
	$(echo) "(constituents.make) install BplusToPhiMuNuConfDbMerge done"

uninstall :: BplusToPhiMuNuConfDbMergeuninstall

$(foreach d,$(BplusToPhiMuNuConfDbMerge_dependencies),$(eval $(d)uninstall_dependencies += BplusToPhiMuNuConfDbMergeuninstall))

BplusToPhiMuNuConfDbMergeuninstall :: $(BplusToPhiMuNuConfDbMergeuninstall_dependencies) $(cmt_local_BplusToPhiMuNuConfDbMerge_makefile)
	$(echo) "(constituents.make) Starting uninstall BplusToPhiMuNuConfDbMerge"
	@$(MAKE) -f $(cmt_local_BplusToPhiMuNuConfDbMerge_makefile) uninstall
	$(echo) "(constituents.make) uninstall BplusToPhiMuNuConfDbMerge done"

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(constituents.make) Starting $@ BplusToPhiMuNuConfDbMerge"
	$(echo) Using default action for $@
	$(echo) "(constituents.make) $@ BplusToPhiMuNuConfDbMerge done"
endif

#-- end of constituent_lock ------
#-- start of constituent_lock ------

cmt_make_has_target_tag = 1

#--------------------------------------------------------

ifdef cmt_make_has_target_tag

#cmt_local_tagfile_make = $(BplusToPhiMuNu_tag)_make.make
cmt_local_tagfile_make = $(bin)$(BplusToPhiMuNu_tag)_make.make
cmt_local_setup_make = $(bin)setup_make$$$$.make
cmt_final_setup_make = $(bin)setup_make.make
#cmt_final_setup_make = $(bin)BplusToPhiMuNu_makesetup.make
cmt_local_make_makefile = $(bin)make.make

make_extratags = -tag_add=target_make

#$(cmt_local_tagfile_make) : $(cmt_lock_setup)
ifndef QUICK
$(cmt_local_tagfile_make) ::
else
$(cmt_local_tagfile_make) :
endif
	$(echo) "(constituents.make) Rebuilding $@"; \
	  if test -f $(cmt_local_tagfile_make); then /bin/rm -f $(cmt_local_tagfile_make); fi ; \
	  $(cmtexe) -tag=$(tags) $(make_extratags) build tag_makefile >>$(cmt_local_tagfile_make)
	$(echo) "(constituents.make) Rebuilding $(cmt_final_setup_make)"; \
	  test ! -f $(cmt_local_setup_make) || \rm -f $(cmt_local_setup_make); \
	  trap '\rm -f $(cmt_local_setup_make)' 0 1 2 15; \
	  $(cmtexe) -tag=$(tags) $(make_extratags) show setup >$(cmt_local_setup_make) && \
	  if [ -f $(cmt_final_setup_make) ] && \
	    \cmp -s $(cmt_final_setup_make) $(cmt_local_setup_make); then \
	    \rm $(cmt_local_setup_make); else \
	    \mv -f $(cmt_local_setup_make) $(cmt_final_setup_make); fi

else

#cmt_local_tagfile_make = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_make = $(bin)$(BplusToPhiMuNu_tag).make
cmt_final_setup_make = $(bin)setup.make
#cmt_final_setup_make = $(bin)BplusToPhiMuNusetup.make
cmt_local_make_makefile = $(bin)make.make

endif

ifdef STRUCTURED_OUTPUT
makedirs :
	@if test ! -d $(bin)make; then $(mkdir) -p $(bin)make; fi
	$(echo) "STRUCTURED_OUTPUT="$(bin)make
else
makedirs : ;
endif

#ifndef QUICK
#ifdef STRUCTURED_OUTPUT
# makedirs ::
#	@if test ! -d $(bin)make; then $(mkdir) -p $(bin)make; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)make
#
#$(cmt_local_make_makefile) :: $(make_dependencies) $(cmt_local_tagfile_make) build_library_links dirs makedirs
#else
#$(cmt_local_make_makefile) :: $(make_dependencies) $(cmt_local_tagfile_make) build_library_links dirs
#endif
#else
#$(cmt_local_make_makefile) :: $(cmt_local_tagfile_make)
#endif

makefiles : $(cmt_local_make_makefile)

ifndef QUICK
$(cmt_local_make_makefile) : $(make_dependencies) $(cmt_local_tagfile_make) build_library_links
else
$(cmt_local_make_makefile) : $(cmt_local_tagfile_make)
endif
	$(echo) "(constituents.make) Building make.make"; \
	  $(cmtexe) -tag=$(tags) $(make_extratags) build constituent_makefile -out=$(cmt_local_make_makefile) make

make :: $(make_dependencies) $(cmt_local_make_makefile) dirs makedirs
	$(echo) "(constituents.make) Creating make${lock_suffix} and Starting make"
	@${lock_command} make${lock_suffix} || exit $$?; \
	  retval=$$?; \
	  trap '${unlock_command} make${lock_suffix}; exit $${retval}' 1 2 15; \
	  $(MAKE) -f $(cmt_local_make_makefile) make; \
	  retval=$$?; ${unlock_command} make${lock_suffix}; exit $${retval}
	$(echo) "(constituents.make) make done"

clean :: makeclean

makeclean :: $(makeclean_dependencies) ##$(cmt_local_make_makefile)
	$(echo) "(constituents.make) Starting makeclean"
	@-if test -f $(cmt_local_make_makefile); then \
	  $(MAKE) -f $(cmt_local_make_makefile) makeclean; \
	fi
	$(echo) "(constituents.make) makeclean done"
#	@-$(MAKE) -f $(cmt_local_make_makefile) makeclean

##	  /bin/rm -f $(cmt_local_make_makefile) $(bin)make_dependencies.make

install :: makeinstall

makeinstall :: $(make_dependencies) $(cmt_local_make_makefile)
	$(echo) "(constituents.make) Starting install make"
	@-$(MAKE) -f $(cmt_local_make_makefile) install
	$(echo) "(constituents.make) install make done"

uninstall :: makeuninstall

$(foreach d,$(make_dependencies),$(eval $(d)uninstall_dependencies += makeuninstall))

makeuninstall :: $(makeuninstall_dependencies) $(cmt_local_make_makefile)
	$(echo) "(constituents.make) Starting uninstall make"
	@$(MAKE) -f $(cmt_local_make_makefile) uninstall
	$(echo) "(constituents.make) uninstall make done"

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(constituents.make) Starting $@ make"
	$(echo) Using default action for $@
	$(echo) "(constituents.make) $@ make done"
endif

#-- end of constituent_lock ------
#-- start of constituent_lock ------

cmt_CompilePython_has_target_tag = 1

#--------------------------------------------------------

ifdef cmt_CompilePython_has_target_tag

#cmt_local_tagfile_CompilePython = $(BplusToPhiMuNu_tag)_CompilePython.make
cmt_local_tagfile_CompilePython = $(bin)$(BplusToPhiMuNu_tag)_CompilePython.make
cmt_local_setup_CompilePython = $(bin)setup_CompilePython$$$$.make
cmt_final_setup_CompilePython = $(bin)setup_CompilePython.make
#cmt_final_setup_CompilePython = $(bin)BplusToPhiMuNu_CompilePythonsetup.make
cmt_local_CompilePython_makefile = $(bin)CompilePython.make

CompilePython_extratags = -tag_add=target_CompilePython

#$(cmt_local_tagfile_CompilePython) : $(cmt_lock_setup)
ifndef QUICK
$(cmt_local_tagfile_CompilePython) ::
else
$(cmt_local_tagfile_CompilePython) :
endif
	$(echo) "(constituents.make) Rebuilding $@"; \
	  if test -f $(cmt_local_tagfile_CompilePython); then /bin/rm -f $(cmt_local_tagfile_CompilePython); fi ; \
	  $(cmtexe) -tag=$(tags) $(CompilePython_extratags) build tag_makefile >>$(cmt_local_tagfile_CompilePython)
	$(echo) "(constituents.make) Rebuilding $(cmt_final_setup_CompilePython)"; \
	  test ! -f $(cmt_local_setup_CompilePython) || \rm -f $(cmt_local_setup_CompilePython); \
	  trap '\rm -f $(cmt_local_setup_CompilePython)' 0 1 2 15; \
	  $(cmtexe) -tag=$(tags) $(CompilePython_extratags) show setup >$(cmt_local_setup_CompilePython) && \
	  if [ -f $(cmt_final_setup_CompilePython) ] && \
	    \cmp -s $(cmt_final_setup_CompilePython) $(cmt_local_setup_CompilePython); then \
	    \rm $(cmt_local_setup_CompilePython); else \
	    \mv -f $(cmt_local_setup_CompilePython) $(cmt_final_setup_CompilePython); fi

else

#cmt_local_tagfile_CompilePython = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_CompilePython = $(bin)$(BplusToPhiMuNu_tag).make
cmt_final_setup_CompilePython = $(bin)setup.make
#cmt_final_setup_CompilePython = $(bin)BplusToPhiMuNusetup.make
cmt_local_CompilePython_makefile = $(bin)CompilePython.make

endif

ifdef STRUCTURED_OUTPUT
CompilePythondirs :
	@if test ! -d $(bin)CompilePython; then $(mkdir) -p $(bin)CompilePython; fi
	$(echo) "STRUCTURED_OUTPUT="$(bin)CompilePython
else
CompilePythondirs : ;
endif

#ifndef QUICK
#ifdef STRUCTURED_OUTPUT
# CompilePythondirs ::
#	@if test ! -d $(bin)CompilePython; then $(mkdir) -p $(bin)CompilePython; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)CompilePython
#
#$(cmt_local_CompilePython_makefile) :: $(CompilePython_dependencies) $(cmt_local_tagfile_CompilePython) build_library_links dirs CompilePythondirs
#else
#$(cmt_local_CompilePython_makefile) :: $(CompilePython_dependencies) $(cmt_local_tagfile_CompilePython) build_library_links dirs
#endif
#else
#$(cmt_local_CompilePython_makefile) :: $(cmt_local_tagfile_CompilePython)
#endif

makefiles : $(cmt_local_CompilePython_makefile)

ifndef QUICK
$(cmt_local_CompilePython_makefile) : $(CompilePython_dependencies) $(cmt_local_tagfile_CompilePython) build_library_links
else
$(cmt_local_CompilePython_makefile) : $(cmt_local_tagfile_CompilePython)
endif
	$(echo) "(constituents.make) Building CompilePython.make"; \
	  $(cmtexe) -tag=$(tags) $(CompilePython_extratags) build constituent_makefile -out=$(cmt_local_CompilePython_makefile) CompilePython

CompilePython :: $(CompilePython_dependencies) $(cmt_local_CompilePython_makefile) dirs CompilePythondirs
	$(echo) "(constituents.make) Creating CompilePython${lock_suffix} and Starting CompilePython"
	@${lock_command} CompilePython${lock_suffix} || exit $$?; \
	  retval=$$?; \
	  trap '${unlock_command} CompilePython${lock_suffix}; exit $${retval}' 1 2 15; \
	  $(MAKE) -f $(cmt_local_CompilePython_makefile) CompilePython; \
	  retval=$$?; ${unlock_command} CompilePython${lock_suffix}; exit $${retval}
	$(echo) "(constituents.make) CompilePython done"

clean :: CompilePythonclean

CompilePythonclean :: $(CompilePythonclean_dependencies) ##$(cmt_local_CompilePython_makefile)
	$(echo) "(constituents.make) Starting CompilePythonclean"
	@-if test -f $(cmt_local_CompilePython_makefile); then \
	  $(MAKE) -f $(cmt_local_CompilePython_makefile) CompilePythonclean; \
	fi
	$(echo) "(constituents.make) CompilePythonclean done"
#	@-$(MAKE) -f $(cmt_local_CompilePython_makefile) CompilePythonclean

##	  /bin/rm -f $(cmt_local_CompilePython_makefile) $(bin)CompilePython_dependencies.make

install :: CompilePythoninstall

CompilePythoninstall :: $(CompilePython_dependencies) $(cmt_local_CompilePython_makefile)
	$(echo) "(constituents.make) Starting install CompilePython"
	@-$(MAKE) -f $(cmt_local_CompilePython_makefile) install
	$(echo) "(constituents.make) install CompilePython done"

uninstall :: CompilePythonuninstall

$(foreach d,$(CompilePython_dependencies),$(eval $(d)uninstall_dependencies += CompilePythonuninstall))

CompilePythonuninstall :: $(CompilePythonuninstall_dependencies) $(cmt_local_CompilePython_makefile)
	$(echo) "(constituents.make) Starting uninstall CompilePython"
	@$(MAKE) -f $(cmt_local_CompilePython_makefile) uninstall
	$(echo) "(constituents.make) uninstall CompilePython done"

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(constituents.make) Starting $@ CompilePython"
	$(echo) Using default action for $@
	$(echo) "(constituents.make) $@ CompilePython done"
endif

#-- end of constituent_lock ------
#-- start of constituent_lock ------

cmt_qmtest_run_has_target_tag = 1

#--------------------------------------------------------

ifdef cmt_qmtest_run_has_target_tag

#cmt_local_tagfile_qmtest_run = $(BplusToPhiMuNu_tag)_qmtest_run.make
cmt_local_tagfile_qmtest_run = $(bin)$(BplusToPhiMuNu_tag)_qmtest_run.make
cmt_local_setup_qmtest_run = $(bin)setup_qmtest_run$$$$.make
cmt_final_setup_qmtest_run = $(bin)setup_qmtest_run.make
#cmt_final_setup_qmtest_run = $(bin)BplusToPhiMuNu_qmtest_runsetup.make
cmt_local_qmtest_run_makefile = $(bin)qmtest_run.make

qmtest_run_extratags = -tag_add=target_qmtest_run

#$(cmt_local_tagfile_qmtest_run) : $(cmt_lock_setup)
ifndef QUICK
$(cmt_local_tagfile_qmtest_run) ::
else
$(cmt_local_tagfile_qmtest_run) :
endif
	$(echo) "(constituents.make) Rebuilding $@"; \
	  if test -f $(cmt_local_tagfile_qmtest_run); then /bin/rm -f $(cmt_local_tagfile_qmtest_run); fi ; \
	  $(cmtexe) -tag=$(tags) $(qmtest_run_extratags) build tag_makefile >>$(cmt_local_tagfile_qmtest_run)
	$(echo) "(constituents.make) Rebuilding $(cmt_final_setup_qmtest_run)"; \
	  test ! -f $(cmt_local_setup_qmtest_run) || \rm -f $(cmt_local_setup_qmtest_run); \
	  trap '\rm -f $(cmt_local_setup_qmtest_run)' 0 1 2 15; \
	  $(cmtexe) -tag=$(tags) $(qmtest_run_extratags) show setup >$(cmt_local_setup_qmtest_run) && \
	  if [ -f $(cmt_final_setup_qmtest_run) ] && \
	    \cmp -s $(cmt_final_setup_qmtest_run) $(cmt_local_setup_qmtest_run); then \
	    \rm $(cmt_local_setup_qmtest_run); else \
	    \mv -f $(cmt_local_setup_qmtest_run) $(cmt_final_setup_qmtest_run); fi

else

#cmt_local_tagfile_qmtest_run = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_qmtest_run = $(bin)$(BplusToPhiMuNu_tag).make
cmt_final_setup_qmtest_run = $(bin)setup.make
#cmt_final_setup_qmtest_run = $(bin)BplusToPhiMuNusetup.make
cmt_local_qmtest_run_makefile = $(bin)qmtest_run.make

endif

ifdef STRUCTURED_OUTPUT
qmtest_rundirs :
	@if test ! -d $(bin)qmtest_run; then $(mkdir) -p $(bin)qmtest_run; fi
	$(echo) "STRUCTURED_OUTPUT="$(bin)qmtest_run
else
qmtest_rundirs : ;
endif

#ifndef QUICK
#ifdef STRUCTURED_OUTPUT
# qmtest_rundirs ::
#	@if test ! -d $(bin)qmtest_run; then $(mkdir) -p $(bin)qmtest_run; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)qmtest_run
#
#$(cmt_local_qmtest_run_makefile) :: $(qmtest_run_dependencies) $(cmt_local_tagfile_qmtest_run) build_library_links dirs qmtest_rundirs
#else
#$(cmt_local_qmtest_run_makefile) :: $(qmtest_run_dependencies) $(cmt_local_tagfile_qmtest_run) build_library_links dirs
#endif
#else
#$(cmt_local_qmtest_run_makefile) :: $(cmt_local_tagfile_qmtest_run)
#endif

makefiles : $(cmt_local_qmtest_run_makefile)

ifndef QUICK
$(cmt_local_qmtest_run_makefile) : $(qmtest_run_dependencies) $(cmt_local_tagfile_qmtest_run) build_library_links
else
$(cmt_local_qmtest_run_makefile) : $(cmt_local_tagfile_qmtest_run)
endif
	$(echo) "(constituents.make) Building qmtest_run.make"; \
	  $(cmtexe) -tag=$(tags) $(qmtest_run_extratags) build constituent_makefile -out=$(cmt_local_qmtest_run_makefile) qmtest_run

qmtest_run :: $(qmtest_run_dependencies) $(cmt_local_qmtest_run_makefile) dirs qmtest_rundirs
	$(echo) "(constituents.make) Creating qmtest_run${lock_suffix} and Starting qmtest_run"
	@${lock_command} qmtest_run${lock_suffix} || exit $$?; \
	  retval=$$?; \
	  trap '${unlock_command} qmtest_run${lock_suffix}; exit $${retval}' 1 2 15; \
	  $(MAKE) -f $(cmt_local_qmtest_run_makefile) qmtest_run; \
	  retval=$$?; ${unlock_command} qmtest_run${lock_suffix}; exit $${retval}
	$(echo) "(constituents.make) qmtest_run done"

clean :: qmtest_runclean

qmtest_runclean :: $(qmtest_runclean_dependencies) ##$(cmt_local_qmtest_run_makefile)
	$(echo) "(constituents.make) Starting qmtest_runclean"
	@-if test -f $(cmt_local_qmtest_run_makefile); then \
	  $(MAKE) -f $(cmt_local_qmtest_run_makefile) qmtest_runclean; \
	fi
	$(echo) "(constituents.make) qmtest_runclean done"
#	@-$(MAKE) -f $(cmt_local_qmtest_run_makefile) qmtest_runclean

##	  /bin/rm -f $(cmt_local_qmtest_run_makefile) $(bin)qmtest_run_dependencies.make

install :: qmtest_runinstall

qmtest_runinstall :: $(qmtest_run_dependencies) $(cmt_local_qmtest_run_makefile)
	$(echo) "(constituents.make) Starting install qmtest_run"
	@-$(MAKE) -f $(cmt_local_qmtest_run_makefile) install
	$(echo) "(constituents.make) install qmtest_run done"

uninstall :: qmtest_rununinstall

$(foreach d,$(qmtest_run_dependencies),$(eval $(d)uninstall_dependencies += qmtest_rununinstall))

qmtest_rununinstall :: $(qmtest_rununinstall_dependencies) $(cmt_local_qmtest_run_makefile)
	$(echo) "(constituents.make) Starting uninstall qmtest_run"
	@$(MAKE) -f $(cmt_local_qmtest_run_makefile) uninstall
	$(echo) "(constituents.make) uninstall qmtest_run done"

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(constituents.make) Starting $@ qmtest_run"
	$(echo) Using default action for $@
	$(echo) "(constituents.make) $@ qmtest_run done"
endif

#-- end of constituent_lock ------
#-- start of constituent_lock ------

cmt_qmtest_summarize_has_target_tag = 1

#--------------------------------------------------------

ifdef cmt_qmtest_summarize_has_target_tag

#cmt_local_tagfile_qmtest_summarize = $(BplusToPhiMuNu_tag)_qmtest_summarize.make
cmt_local_tagfile_qmtest_summarize = $(bin)$(BplusToPhiMuNu_tag)_qmtest_summarize.make
cmt_local_setup_qmtest_summarize = $(bin)setup_qmtest_summarize$$$$.make
cmt_final_setup_qmtest_summarize = $(bin)setup_qmtest_summarize.make
#cmt_final_setup_qmtest_summarize = $(bin)BplusToPhiMuNu_qmtest_summarizesetup.make
cmt_local_qmtest_summarize_makefile = $(bin)qmtest_summarize.make

qmtest_summarize_extratags = -tag_add=target_qmtest_summarize

#$(cmt_local_tagfile_qmtest_summarize) : $(cmt_lock_setup)
ifndef QUICK
$(cmt_local_tagfile_qmtest_summarize) ::
else
$(cmt_local_tagfile_qmtest_summarize) :
endif
	$(echo) "(constituents.make) Rebuilding $@"; \
	  if test -f $(cmt_local_tagfile_qmtest_summarize); then /bin/rm -f $(cmt_local_tagfile_qmtest_summarize); fi ; \
	  $(cmtexe) -tag=$(tags) $(qmtest_summarize_extratags) build tag_makefile >>$(cmt_local_tagfile_qmtest_summarize)
	$(echo) "(constituents.make) Rebuilding $(cmt_final_setup_qmtest_summarize)"; \
	  test ! -f $(cmt_local_setup_qmtest_summarize) || \rm -f $(cmt_local_setup_qmtest_summarize); \
	  trap '\rm -f $(cmt_local_setup_qmtest_summarize)' 0 1 2 15; \
	  $(cmtexe) -tag=$(tags) $(qmtest_summarize_extratags) show setup >$(cmt_local_setup_qmtest_summarize) && \
	  if [ -f $(cmt_final_setup_qmtest_summarize) ] && \
	    \cmp -s $(cmt_final_setup_qmtest_summarize) $(cmt_local_setup_qmtest_summarize); then \
	    \rm $(cmt_local_setup_qmtest_summarize); else \
	    \mv -f $(cmt_local_setup_qmtest_summarize) $(cmt_final_setup_qmtest_summarize); fi

else

#cmt_local_tagfile_qmtest_summarize = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_qmtest_summarize = $(bin)$(BplusToPhiMuNu_tag).make
cmt_final_setup_qmtest_summarize = $(bin)setup.make
#cmt_final_setup_qmtest_summarize = $(bin)BplusToPhiMuNusetup.make
cmt_local_qmtest_summarize_makefile = $(bin)qmtest_summarize.make

endif

ifdef STRUCTURED_OUTPUT
qmtest_summarizedirs :
	@if test ! -d $(bin)qmtest_summarize; then $(mkdir) -p $(bin)qmtest_summarize; fi
	$(echo) "STRUCTURED_OUTPUT="$(bin)qmtest_summarize
else
qmtest_summarizedirs : ;
endif

#ifndef QUICK
#ifdef STRUCTURED_OUTPUT
# qmtest_summarizedirs ::
#	@if test ! -d $(bin)qmtest_summarize; then $(mkdir) -p $(bin)qmtest_summarize; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)qmtest_summarize
#
#$(cmt_local_qmtest_summarize_makefile) :: $(qmtest_summarize_dependencies) $(cmt_local_tagfile_qmtest_summarize) build_library_links dirs qmtest_summarizedirs
#else
#$(cmt_local_qmtest_summarize_makefile) :: $(qmtest_summarize_dependencies) $(cmt_local_tagfile_qmtest_summarize) build_library_links dirs
#endif
#else
#$(cmt_local_qmtest_summarize_makefile) :: $(cmt_local_tagfile_qmtest_summarize)
#endif

makefiles : $(cmt_local_qmtest_summarize_makefile)

ifndef QUICK
$(cmt_local_qmtest_summarize_makefile) : $(qmtest_summarize_dependencies) $(cmt_local_tagfile_qmtest_summarize) build_library_links
else
$(cmt_local_qmtest_summarize_makefile) : $(cmt_local_tagfile_qmtest_summarize)
endif
	$(echo) "(constituents.make) Building qmtest_summarize.make"; \
	  $(cmtexe) -tag=$(tags) $(qmtest_summarize_extratags) build constituent_makefile -out=$(cmt_local_qmtest_summarize_makefile) qmtest_summarize

qmtest_summarize :: $(qmtest_summarize_dependencies) $(cmt_local_qmtest_summarize_makefile) dirs qmtest_summarizedirs
	$(echo) "(constituents.make) Creating qmtest_summarize${lock_suffix} and Starting qmtest_summarize"
	@${lock_command} qmtest_summarize${lock_suffix} || exit $$?; \
	  retval=$$?; \
	  trap '${unlock_command} qmtest_summarize${lock_suffix}; exit $${retval}' 1 2 15; \
	  $(MAKE) -f $(cmt_local_qmtest_summarize_makefile) qmtest_summarize; \
	  retval=$$?; ${unlock_command} qmtest_summarize${lock_suffix}; exit $${retval}
	$(echo) "(constituents.make) qmtest_summarize done"

clean :: qmtest_summarizeclean

qmtest_summarizeclean :: $(qmtest_summarizeclean_dependencies) ##$(cmt_local_qmtest_summarize_makefile)
	$(echo) "(constituents.make) Starting qmtest_summarizeclean"
	@-if test -f $(cmt_local_qmtest_summarize_makefile); then \
	  $(MAKE) -f $(cmt_local_qmtest_summarize_makefile) qmtest_summarizeclean; \
	fi
	$(echo) "(constituents.make) qmtest_summarizeclean done"
#	@-$(MAKE) -f $(cmt_local_qmtest_summarize_makefile) qmtest_summarizeclean

##	  /bin/rm -f $(cmt_local_qmtest_summarize_makefile) $(bin)qmtest_summarize_dependencies.make

install :: qmtest_summarizeinstall

qmtest_summarizeinstall :: $(qmtest_summarize_dependencies) $(cmt_local_qmtest_summarize_makefile)
	$(echo) "(constituents.make) Starting install qmtest_summarize"
	@-$(MAKE) -f $(cmt_local_qmtest_summarize_makefile) install
	$(echo) "(constituents.make) install qmtest_summarize done"

uninstall :: qmtest_summarizeuninstall

$(foreach d,$(qmtest_summarize_dependencies),$(eval $(d)uninstall_dependencies += qmtest_summarizeuninstall))

qmtest_summarizeuninstall :: $(qmtest_summarizeuninstall_dependencies) $(cmt_local_qmtest_summarize_makefile)
	$(echo) "(constituents.make) Starting uninstall qmtest_summarize"
	@$(MAKE) -f $(cmt_local_qmtest_summarize_makefile) uninstall
	$(echo) "(constituents.make) uninstall qmtest_summarize done"

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(constituents.make) Starting $@ qmtest_summarize"
	$(echo) Using default action for $@
	$(echo) "(constituents.make) $@ qmtest_summarize done"
endif

#-- end of constituent_lock ------
#-- start of constituent_lock ------

cmt_TestPackage_has_target_tag = 1

#--------------------------------------------------------

ifdef cmt_TestPackage_has_target_tag

#cmt_local_tagfile_TestPackage = $(BplusToPhiMuNu_tag)_TestPackage.make
cmt_local_tagfile_TestPackage = $(bin)$(BplusToPhiMuNu_tag)_TestPackage.make
cmt_local_setup_TestPackage = $(bin)setup_TestPackage$$$$.make
cmt_final_setup_TestPackage = $(bin)setup_TestPackage.make
#cmt_final_setup_TestPackage = $(bin)BplusToPhiMuNu_TestPackagesetup.make
cmt_local_TestPackage_makefile = $(bin)TestPackage.make

TestPackage_extratags = -tag_add=target_TestPackage

#$(cmt_local_tagfile_TestPackage) : $(cmt_lock_setup)
ifndef QUICK
$(cmt_local_tagfile_TestPackage) ::
else
$(cmt_local_tagfile_TestPackage) :
endif
	$(echo) "(constituents.make) Rebuilding $@"; \
	  if test -f $(cmt_local_tagfile_TestPackage); then /bin/rm -f $(cmt_local_tagfile_TestPackage); fi ; \
	  $(cmtexe) -tag=$(tags) $(TestPackage_extratags) build tag_makefile >>$(cmt_local_tagfile_TestPackage)
	$(echo) "(constituents.make) Rebuilding $(cmt_final_setup_TestPackage)"; \
	  test ! -f $(cmt_local_setup_TestPackage) || \rm -f $(cmt_local_setup_TestPackage); \
	  trap '\rm -f $(cmt_local_setup_TestPackage)' 0 1 2 15; \
	  $(cmtexe) -tag=$(tags) $(TestPackage_extratags) show setup >$(cmt_local_setup_TestPackage) && \
	  if [ -f $(cmt_final_setup_TestPackage) ] && \
	    \cmp -s $(cmt_final_setup_TestPackage) $(cmt_local_setup_TestPackage); then \
	    \rm $(cmt_local_setup_TestPackage); else \
	    \mv -f $(cmt_local_setup_TestPackage) $(cmt_final_setup_TestPackage); fi

else

#cmt_local_tagfile_TestPackage = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_TestPackage = $(bin)$(BplusToPhiMuNu_tag).make
cmt_final_setup_TestPackage = $(bin)setup.make
#cmt_final_setup_TestPackage = $(bin)BplusToPhiMuNusetup.make
cmt_local_TestPackage_makefile = $(bin)TestPackage.make

endif

ifdef STRUCTURED_OUTPUT
TestPackagedirs :
	@if test ! -d $(bin)TestPackage; then $(mkdir) -p $(bin)TestPackage; fi
	$(echo) "STRUCTURED_OUTPUT="$(bin)TestPackage
else
TestPackagedirs : ;
endif

#ifndef QUICK
#ifdef STRUCTURED_OUTPUT
# TestPackagedirs ::
#	@if test ! -d $(bin)TestPackage; then $(mkdir) -p $(bin)TestPackage; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)TestPackage
#
#$(cmt_local_TestPackage_makefile) :: $(TestPackage_dependencies) $(cmt_local_tagfile_TestPackage) build_library_links dirs TestPackagedirs
#else
#$(cmt_local_TestPackage_makefile) :: $(TestPackage_dependencies) $(cmt_local_tagfile_TestPackage) build_library_links dirs
#endif
#else
#$(cmt_local_TestPackage_makefile) :: $(cmt_local_tagfile_TestPackage)
#endif

makefiles : $(cmt_local_TestPackage_makefile)

ifndef QUICK
$(cmt_local_TestPackage_makefile) : $(TestPackage_dependencies) $(cmt_local_tagfile_TestPackage) build_library_links
else
$(cmt_local_TestPackage_makefile) : $(cmt_local_tagfile_TestPackage)
endif
	$(echo) "(constituents.make) Building TestPackage.make"; \
	  $(cmtexe) -tag=$(tags) $(TestPackage_extratags) build constituent_makefile -out=$(cmt_local_TestPackage_makefile) TestPackage

TestPackage :: $(TestPackage_dependencies) $(cmt_local_TestPackage_makefile) dirs TestPackagedirs
	$(echo) "(constituents.make) Creating TestPackage${lock_suffix} and Starting TestPackage"
	@${lock_command} TestPackage${lock_suffix} || exit $$?; \
	  retval=$$?; \
	  trap '${unlock_command} TestPackage${lock_suffix}; exit $${retval}' 1 2 15; \
	  $(MAKE) -f $(cmt_local_TestPackage_makefile) TestPackage; \
	  retval=$$?; ${unlock_command} TestPackage${lock_suffix}; exit $${retval}
	$(echo) "(constituents.make) TestPackage done"

clean :: TestPackageclean

TestPackageclean :: $(TestPackageclean_dependencies) ##$(cmt_local_TestPackage_makefile)
	$(echo) "(constituents.make) Starting TestPackageclean"
	@-if test -f $(cmt_local_TestPackage_makefile); then \
	  $(MAKE) -f $(cmt_local_TestPackage_makefile) TestPackageclean; \
	fi
	$(echo) "(constituents.make) TestPackageclean done"
#	@-$(MAKE) -f $(cmt_local_TestPackage_makefile) TestPackageclean

##	  /bin/rm -f $(cmt_local_TestPackage_makefile) $(bin)TestPackage_dependencies.make

install :: TestPackageinstall

TestPackageinstall :: $(TestPackage_dependencies) $(cmt_local_TestPackage_makefile)
	$(echo) "(constituents.make) Starting install TestPackage"
	@-$(MAKE) -f $(cmt_local_TestPackage_makefile) install
	$(echo) "(constituents.make) install TestPackage done"

uninstall :: TestPackageuninstall

$(foreach d,$(TestPackage_dependencies),$(eval $(d)uninstall_dependencies += TestPackageuninstall))

TestPackageuninstall :: $(TestPackageuninstall_dependencies) $(cmt_local_TestPackage_makefile)
	$(echo) "(constituents.make) Starting uninstall TestPackage"
	@$(MAKE) -f $(cmt_local_TestPackage_makefile) uninstall
	$(echo) "(constituents.make) uninstall TestPackage done"

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(constituents.make) Starting $@ TestPackage"
	$(echo) Using default action for $@
	$(echo) "(constituents.make) $@ TestPackage done"
endif

#-- end of constituent_lock ------
#-- start of constituent_lock ------

cmt_TestProject_has_target_tag = 1

#--------------------------------------------------------

ifdef cmt_TestProject_has_target_tag

#cmt_local_tagfile_TestProject = $(BplusToPhiMuNu_tag)_TestProject.make
cmt_local_tagfile_TestProject = $(bin)$(BplusToPhiMuNu_tag)_TestProject.make
cmt_local_setup_TestProject = $(bin)setup_TestProject$$$$.make
cmt_final_setup_TestProject = $(bin)setup_TestProject.make
#cmt_final_setup_TestProject = $(bin)BplusToPhiMuNu_TestProjectsetup.make
cmt_local_TestProject_makefile = $(bin)TestProject.make

TestProject_extratags = -tag_add=target_TestProject

#$(cmt_local_tagfile_TestProject) : $(cmt_lock_setup)
ifndef QUICK
$(cmt_local_tagfile_TestProject) ::
else
$(cmt_local_tagfile_TestProject) :
endif
	$(echo) "(constituents.make) Rebuilding $@"; \
	  if test -f $(cmt_local_tagfile_TestProject); then /bin/rm -f $(cmt_local_tagfile_TestProject); fi ; \
	  $(cmtexe) -tag=$(tags) $(TestProject_extratags) build tag_makefile >>$(cmt_local_tagfile_TestProject)
	$(echo) "(constituents.make) Rebuilding $(cmt_final_setup_TestProject)"; \
	  test ! -f $(cmt_local_setup_TestProject) || \rm -f $(cmt_local_setup_TestProject); \
	  trap '\rm -f $(cmt_local_setup_TestProject)' 0 1 2 15; \
	  $(cmtexe) -tag=$(tags) $(TestProject_extratags) show setup >$(cmt_local_setup_TestProject) && \
	  if [ -f $(cmt_final_setup_TestProject) ] && \
	    \cmp -s $(cmt_final_setup_TestProject) $(cmt_local_setup_TestProject); then \
	    \rm $(cmt_local_setup_TestProject); else \
	    \mv -f $(cmt_local_setup_TestProject) $(cmt_final_setup_TestProject); fi

else

#cmt_local_tagfile_TestProject = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_TestProject = $(bin)$(BplusToPhiMuNu_tag).make
cmt_final_setup_TestProject = $(bin)setup.make
#cmt_final_setup_TestProject = $(bin)BplusToPhiMuNusetup.make
cmt_local_TestProject_makefile = $(bin)TestProject.make

endif

ifdef STRUCTURED_OUTPUT
TestProjectdirs :
	@if test ! -d $(bin)TestProject; then $(mkdir) -p $(bin)TestProject; fi
	$(echo) "STRUCTURED_OUTPUT="$(bin)TestProject
else
TestProjectdirs : ;
endif

#ifndef QUICK
#ifdef STRUCTURED_OUTPUT
# TestProjectdirs ::
#	@if test ! -d $(bin)TestProject; then $(mkdir) -p $(bin)TestProject; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)TestProject
#
#$(cmt_local_TestProject_makefile) :: $(TestProject_dependencies) $(cmt_local_tagfile_TestProject) build_library_links dirs TestProjectdirs
#else
#$(cmt_local_TestProject_makefile) :: $(TestProject_dependencies) $(cmt_local_tagfile_TestProject) build_library_links dirs
#endif
#else
#$(cmt_local_TestProject_makefile) :: $(cmt_local_tagfile_TestProject)
#endif

makefiles : $(cmt_local_TestProject_makefile)

ifndef QUICK
$(cmt_local_TestProject_makefile) : $(TestProject_dependencies) $(cmt_local_tagfile_TestProject) build_library_links
else
$(cmt_local_TestProject_makefile) : $(cmt_local_tagfile_TestProject)
endif
	$(echo) "(constituents.make) Building TestProject.make"; \
	  $(cmtexe) -tag=$(tags) $(TestProject_extratags) build constituent_makefile -out=$(cmt_local_TestProject_makefile) TestProject

TestProject :: $(TestProject_dependencies) $(cmt_local_TestProject_makefile) dirs TestProjectdirs
	$(echo) "(constituents.make) Creating TestProject${lock_suffix} and Starting TestProject"
	@${lock_command} TestProject${lock_suffix} || exit $$?; \
	  retval=$$?; \
	  trap '${unlock_command} TestProject${lock_suffix}; exit $${retval}' 1 2 15; \
	  $(MAKE) -f $(cmt_local_TestProject_makefile) TestProject; \
	  retval=$$?; ${unlock_command} TestProject${lock_suffix}; exit $${retval}
	$(echo) "(constituents.make) TestProject done"

clean :: TestProjectclean

TestProjectclean :: $(TestProjectclean_dependencies) ##$(cmt_local_TestProject_makefile)
	$(echo) "(constituents.make) Starting TestProjectclean"
	@-if test -f $(cmt_local_TestProject_makefile); then \
	  $(MAKE) -f $(cmt_local_TestProject_makefile) TestProjectclean; \
	fi
	$(echo) "(constituents.make) TestProjectclean done"
#	@-$(MAKE) -f $(cmt_local_TestProject_makefile) TestProjectclean

##	  /bin/rm -f $(cmt_local_TestProject_makefile) $(bin)TestProject_dependencies.make

install :: TestProjectinstall

TestProjectinstall :: $(TestProject_dependencies) $(cmt_local_TestProject_makefile)
	$(echo) "(constituents.make) Starting install TestProject"
	@-$(MAKE) -f $(cmt_local_TestProject_makefile) install
	$(echo) "(constituents.make) install TestProject done"

uninstall :: TestProjectuninstall

$(foreach d,$(TestProject_dependencies),$(eval $(d)uninstall_dependencies += TestProjectuninstall))

TestProjectuninstall :: $(TestProjectuninstall_dependencies) $(cmt_local_TestProject_makefile)
	$(echo) "(constituents.make) Starting uninstall TestProject"
	@$(MAKE) -f $(cmt_local_TestProject_makefile) uninstall
	$(echo) "(constituents.make) uninstall TestProject done"

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(constituents.make) Starting $@ TestProject"
	$(echo) Using default action for $@
	$(echo) "(constituents.make) $@ TestProject done"
endif

#-- end of constituent_lock ------
#-- start of constituent_lock ------

cmt_new_rootsys_has_target_tag = 1

#--------------------------------------------------------

ifdef cmt_new_rootsys_has_target_tag

#cmt_local_tagfile_new_rootsys = $(BplusToPhiMuNu_tag)_new_rootsys.make
cmt_local_tagfile_new_rootsys = $(bin)$(BplusToPhiMuNu_tag)_new_rootsys.make
cmt_local_setup_new_rootsys = $(bin)setup_new_rootsys$$$$.make
cmt_final_setup_new_rootsys = $(bin)setup_new_rootsys.make
#cmt_final_setup_new_rootsys = $(bin)BplusToPhiMuNu_new_rootsyssetup.make
cmt_local_new_rootsys_makefile = $(bin)new_rootsys.make

new_rootsys_extratags = -tag_add=target_new_rootsys

#$(cmt_local_tagfile_new_rootsys) : $(cmt_lock_setup)
ifndef QUICK
$(cmt_local_tagfile_new_rootsys) ::
else
$(cmt_local_tagfile_new_rootsys) :
endif
	$(echo) "(constituents.make) Rebuilding $@"; \
	  if test -f $(cmt_local_tagfile_new_rootsys); then /bin/rm -f $(cmt_local_tagfile_new_rootsys); fi ; \
	  $(cmtexe) -tag=$(tags) $(new_rootsys_extratags) build tag_makefile >>$(cmt_local_tagfile_new_rootsys)
	$(echo) "(constituents.make) Rebuilding $(cmt_final_setup_new_rootsys)"; \
	  test ! -f $(cmt_local_setup_new_rootsys) || \rm -f $(cmt_local_setup_new_rootsys); \
	  trap '\rm -f $(cmt_local_setup_new_rootsys)' 0 1 2 15; \
	  $(cmtexe) -tag=$(tags) $(new_rootsys_extratags) show setup >$(cmt_local_setup_new_rootsys) && \
	  if [ -f $(cmt_final_setup_new_rootsys) ] && \
	    \cmp -s $(cmt_final_setup_new_rootsys) $(cmt_local_setup_new_rootsys); then \
	    \rm $(cmt_local_setup_new_rootsys); else \
	    \mv -f $(cmt_local_setup_new_rootsys) $(cmt_final_setup_new_rootsys); fi

else

#cmt_local_tagfile_new_rootsys = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_new_rootsys = $(bin)$(BplusToPhiMuNu_tag).make
cmt_final_setup_new_rootsys = $(bin)setup.make
#cmt_final_setup_new_rootsys = $(bin)BplusToPhiMuNusetup.make
cmt_local_new_rootsys_makefile = $(bin)new_rootsys.make

endif

ifdef STRUCTURED_OUTPUT
new_rootsysdirs :
	@if test ! -d $(bin)new_rootsys; then $(mkdir) -p $(bin)new_rootsys; fi
	$(echo) "STRUCTURED_OUTPUT="$(bin)new_rootsys
else
new_rootsysdirs : ;
endif

#ifndef QUICK
#ifdef STRUCTURED_OUTPUT
# new_rootsysdirs ::
#	@if test ! -d $(bin)new_rootsys; then $(mkdir) -p $(bin)new_rootsys; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)new_rootsys
#
#$(cmt_local_new_rootsys_makefile) :: $(new_rootsys_dependencies) $(cmt_local_tagfile_new_rootsys) build_library_links dirs new_rootsysdirs
#else
#$(cmt_local_new_rootsys_makefile) :: $(new_rootsys_dependencies) $(cmt_local_tagfile_new_rootsys) build_library_links dirs
#endif
#else
#$(cmt_local_new_rootsys_makefile) :: $(cmt_local_tagfile_new_rootsys)
#endif

makefiles : $(cmt_local_new_rootsys_makefile)

ifndef QUICK
$(cmt_local_new_rootsys_makefile) : $(new_rootsys_dependencies) $(cmt_local_tagfile_new_rootsys) build_library_links
else
$(cmt_local_new_rootsys_makefile) : $(cmt_local_tagfile_new_rootsys)
endif
	$(echo) "(constituents.make) Building new_rootsys.make"; \
	  $(cmtexe) -tag=$(tags) $(new_rootsys_extratags) build constituent_makefile -out=$(cmt_local_new_rootsys_makefile) new_rootsys

new_rootsys :: $(new_rootsys_dependencies) $(cmt_local_new_rootsys_makefile) dirs new_rootsysdirs
	$(echo) "(constituents.make) Creating new_rootsys${lock_suffix} and Starting new_rootsys"
	@${lock_command} new_rootsys${lock_suffix} || exit $$?; \
	  retval=$$?; \
	  trap '${unlock_command} new_rootsys${lock_suffix}; exit $${retval}' 1 2 15; \
	  $(MAKE) -f $(cmt_local_new_rootsys_makefile) new_rootsys; \
	  retval=$$?; ${unlock_command} new_rootsys${lock_suffix}; exit $${retval}
	$(echo) "(constituents.make) new_rootsys done"

clean :: new_rootsysclean

new_rootsysclean :: $(new_rootsysclean_dependencies) ##$(cmt_local_new_rootsys_makefile)
	$(echo) "(constituents.make) Starting new_rootsysclean"
	@-if test -f $(cmt_local_new_rootsys_makefile); then \
	  $(MAKE) -f $(cmt_local_new_rootsys_makefile) new_rootsysclean; \
	fi
	$(echo) "(constituents.make) new_rootsysclean done"
#	@-$(MAKE) -f $(cmt_local_new_rootsys_makefile) new_rootsysclean

##	  /bin/rm -f $(cmt_local_new_rootsys_makefile) $(bin)new_rootsys_dependencies.make

install :: new_rootsysinstall

new_rootsysinstall :: $(new_rootsys_dependencies) $(cmt_local_new_rootsys_makefile)
	$(echo) "(constituents.make) Starting install new_rootsys"
	@-$(MAKE) -f $(cmt_local_new_rootsys_makefile) install
	$(echo) "(constituents.make) install new_rootsys done"

uninstall :: new_rootsysuninstall

$(foreach d,$(new_rootsys_dependencies),$(eval $(d)uninstall_dependencies += new_rootsysuninstall))

new_rootsysuninstall :: $(new_rootsysuninstall_dependencies) $(cmt_local_new_rootsys_makefile)
	$(echo) "(constituents.make) Starting uninstall new_rootsys"
	@$(MAKE) -f $(cmt_local_new_rootsys_makefile) uninstall
	$(echo) "(constituents.make) uninstall new_rootsys done"

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(constituents.make) Starting $@ new_rootsys"
	$(echo) Using default action for $@
	$(echo) "(constituents.make) $@ new_rootsys done"
endif

#-- end of constituent_lock ------
#-- start of constituents_trailer ------

clean :: remove_library_links

remove_library_links ::
	$(echo) "(constituents.make) Removing library links"; \
	  $(remove_library_links)

makefilesclean ::

###	@/bin/rm -f checkuses

###	/bin/rm -f *.make*

clean :: makefilesclean

binclean :: clean
	$(echo) "(constituents.make) Removing binary directory $(bin)"
	@if test ! "$(bin)" = "./"; then \
	  /bin/rm -rf $(bin); \
	fi

#-- end of constituents_trailer ------
