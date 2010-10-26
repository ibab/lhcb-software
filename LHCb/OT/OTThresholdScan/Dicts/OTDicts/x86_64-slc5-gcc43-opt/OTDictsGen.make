#-- start of make_header -----------------

#====================================
#  Document OTDictsGen
#
#   Generated Tue Oct 26 14:03:45 2010  by dvaneijk
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_OTDictsGen_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_OTDictsGen_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_OTDictsGen

OTDicts_tag = $(tag)

#cmt_local_tagfile_OTDictsGen = $(OTDicts_tag)_OTDictsGen.make
cmt_local_tagfile_OTDictsGen = $(bin)$(OTDicts_tag)_OTDictsGen.make

else

tags      = $(tag),$(CMTEXTRATAGS)

OTDicts_tag = $(tag)

#cmt_local_tagfile_OTDictsGen = $(OTDicts_tag).make
cmt_local_tagfile_OTDictsGen = $(bin)$(OTDicts_tag).make

endif

include $(cmt_local_tagfile_OTDictsGen)
#-include $(cmt_local_tagfile_OTDictsGen)

ifdef cmt_OTDictsGen_has_target_tag

cmt_final_setup_OTDictsGen = $(bin)setup_OTDictsGen.make
#cmt_final_setup_OTDictsGen = $(bin)OTDicts_OTDictsGensetup.make
cmt_local_OTDictsGen_makefile = $(bin)OTDictsGen.make

else

cmt_final_setup_OTDictsGen = $(bin)setup.make
#cmt_final_setup_OTDictsGen = $(bin)OTDictssetup.make
cmt_local_OTDictsGen_makefile = $(bin)OTDictsGen.make

endif

cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)OTDictssetup.make

#OTDictsGen :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'OTDictsGen'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = OTDictsGen/
#OTDictsGen::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
#-- start of dependency ------------------
ifneq ($(MAKECMDGOALS),OTDictsGenclean)
ifneq ($(MAKECMDGOALS),uninstall)

#$(bin)OTDictsGen_dependencies.make :: dirs

ifndef QUICK
$(bin)OTDictsGen_dependencies.make : /afs/cern.ch/user/d/dvaneijk/cmtuser/Bender_v15r1p1/Dicts/OTDicts/dict/OTDicts.h $(use_requirements) $(cmt_final_setup_OTDictsGen)
	$(echo) "(OTDictsGen.make) Rebuilding $@"; \
	  $(build_dependencies) OTDictsGen -all_sources -out=$@ /afs/cern.ch/user/d/dvaneijk/cmtuser/Bender_v15r1p1/Dicts/OTDicts/dict/OTDicts.h
endif

#$(OTDictsGen_dependencies)

-include $(bin)OTDictsGen_dependencies.make

endif
endif
#-- end of dependency -------------------
# File: cmt/fragments/reflex_dictionary_generator
# Author: Pere Mato

# Makefile fragment to generate the Reflex dictionary together with a local rootmap file
# and a merged file in the installation area if InstallArea is present


.PHONY: OTDictsGen OTDictsGenclean MergeRootMap MergeRootMapclean

ppcmd=-I

dictdir=../$(tag)/dict/OTDicts
gensrcdict=$(dictdir)/OTDicts_dict.cpp
ifdef OTDicts_rootmap
  rootmapname=OTDictsDict.rootmap
  rootmapdict=$(dictdir)/$(rootmapname)
  rootmapopts=--rootmap=$(rootmapname) --rootmap-lib=$(library_prefix)OTDictsDict
else
  rootmapdict=
endif

OTDictsGen ::  $(gensrcdict) $(rootmapdict) MergeRootMap
	@:

.NOTPARALLEL: $(gensrcdict) $(rootmapdict)

$(gensrcdict) $(rootmapdict) :: $(OTDicts_h_dependencies) $(OTDicts_reflex_selection_file)
	echo $@ OTDicts.h
	@-mkdir -p $(dictdir)
	$(genreflex_cmd) /afs/cern.ch/user/d/dvaneijk/cmtuser/Bender_v15r1p1/Dicts/OTDicts/dict/OTDicts.h -o $(gensrcdict) $(OTDicts_reflex_options)  \
			 $(OTDicts_reflex_options) $(rootmapopts) $(gccxml_cppflags) $(pp_cppflags) $(includes) $(use_pp_cppflags)

OTDictsGenclean ::
	@echo 'Deleting $(gensrcdict) $(rootmapdict)'
	@$(uninstall_command) $(gensrcdict) $(rootmapdict)


#---Dealing with merging the rootmap files 
ifdef  OTDicts_rootmap

mergedrootmapdir=$(dir $(OTDicts_rootmap))
mergedrootmap=$(OTDicts_rootmap)
stamprootmap=$(mergedrootmapdir)/$(rootmapname).stamp

MergeRootMap :: $(stamprootmap) $(mergedrootmap)
	@:

$(stamprootmap) $(mergedrootmap) :: $(rootmapdict)
	@echo "Running merge_rootmap  OTDictsGen" 
	@touch $(mergedrootmap)
	$(merge_rootmap_cmd) --do-merge --input-file $(rootmapdict) --merged-file $(mergedrootmap)

OTDictsGenclean ::
	@echo "Un-merging rootmap"
	@$(merge_rootmap_cmd) --un-merge --input-file $(rootmapdict) --merged-file $(mergedrootmap)
	@$(uninstall_command) $(stamprootmap)

else

MergeRootMap ::
        @:

endif
OTDictsGenclean ::
	@/bin/rm -rf ../$(tag)/dict
#-- start of cleanup_header --------------

clean :: OTDictsGenclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(OTDictsGen.make) $@: No rule for such target" >&2
#	@echo "#CMT> Warning: $@: No rule for such target" >&2; exit
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(OTDictsGen.make): $@: File no longer generated" >&2; exit 0; fi
else
.DEFAULT::
	$(echo) "(OTDictsGen.make) PEDANTIC: $@: No rule for such target" >&2
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(OTDictsGen.make): $@: File no longer generated" >&2; exit 0;\
	 elif test $@ = "$(cmt_final_setup)" -o\
	 $@ = "$(cmt_final_setup_OTDictsGen)" ; then\
	 found=n; for s in 1 2 3 4 5; do\
	 sleep $$s; test ! -f $@ || { found=y; break; }\
	 done; if test $$found = n; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(OTDictsGen.make) PEDANTIC: $@: Seems to be missing. Ignore it for now" >&2; exit 0 ; fi;\
	 elif test `expr $@ : '.*/'` -ne 0 ; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(OTDictsGen.make) PEDANTIC: $@: Seems to be a missing file. Please check" >&2; exit 2 ; \
	 else\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(OTDictsGen.make) PEDANTIC: $@: Seems to be a fake target due to some pattern. Just ignore it" >&2 ; exit 0; fi
endif

OTDictsGenclean ::
#-- end of cleanup_header ---------------
