#-- start of make_header -----------------

#====================================
#  Document TisTosTobbingRootMap
#
#   Generated Thu Aug  9 03:23:33 2007  by tskwarni
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_TisTosTobbingRootMap_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_TisTosTobbingRootMap_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_TisTosTobbingRootMap

TisTosTobbing_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_TisTosTobbingRootMap = /tmp/CMT_$(TisTosTobbing_tag)_TisTosTobbingRootMap.make$(cmt_lock_pid)
else
cmt_local_tagfile_TisTosTobbingRootMap = $(TisTosTobbing_tag)_TisTosTobbingRootMap.make
endif

else

tags      = $(tag),$(CMTEXTRATAGS)

TisTosTobbing_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_TisTosTobbingRootMap = /tmp/CMT_$(TisTosTobbing_tag).make$(cmt_lock_pid)
else
cmt_local_tagfile_TisTosTobbingRootMap = $(TisTosTobbing_tag).make
endif

endif

-include $(cmt_local_tagfile_TisTosTobbingRootMap)

ifdef cmt_TisTosTobbingRootMap_has_target_tag

ifdef READONLY
cmt_final_setup_TisTosTobbingRootMap = /tmp/CMT_TisTosTobbing_TisTosTobbingRootMapsetup.make
cmt_local_TisTosTobbingRootMap_makefile = /tmp/CMT_TisTosTobbingRootMap$(cmt_lock_pid).make
else
cmt_final_setup_TisTosTobbingRootMap = $(bin)TisTosTobbing_TisTosTobbingRootMapsetup.make
cmt_local_TisTosTobbingRootMap_makefile = $(bin)TisTosTobbingRootMap.make
endif

else

ifdef READONLY
cmt_final_setup_TisTosTobbingRootMap = /tmp/CMT_TisTosTobbingsetup.make
cmt_local_TisTosTobbingRootMap_makefile = /tmp/CMT_TisTosTobbingRootMap$(cmt_lock_pid).make
else
cmt_final_setup_TisTosTobbingRootMap = $(bin)TisTosTobbingsetup.make
cmt_local_TisTosTobbingRootMap_makefile = $(bin)TisTosTobbingRootMap.make
endif

endif

ifdef READONLY
cmt_final_setup = /tmp/CMT_TisTosTobbingsetup.make
else
cmt_final_setup = $(bin)TisTosTobbingsetup.make
endif

TisTosTobbingRootMap ::


ifdef READONLY
TisTosTobbingRootMap ::
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
	@echo 'TisTosTobbingRootMap'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = TisTosTobbingRootMap/
TisTosTobbingRootMap::
	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
	@echo "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

#-- end of make_header ------------------

##
rootmapfile = TisTosTobbing.rootmap
ROOTMAP_DIR = ../$(tag)
fulllibname = libTisTosTobbing.$(shlibsuffix)

TisTosTobbingRootMap :: ${ROOTMAP_DIR}/$(rootmapfile)
	@:

${ROOTMAP_DIR}/$(rootmapfile) :: $(bin)$(fulllibname)
	@echo 'Generating rootmap file for $(fulllibname)'
	cd ../$(tag);$(genmap_cmd) -i $(fulllibname) -o ${ROOTMAP_DIR}/$(rootmapfile) $(TisTosTobbingRootMap_genmapflags)

install :: TisTosTobbingRootMapinstall
TisTosTobbingRootMapinstall :: TisTosTobbingRootMap

uninstall :: TisTosTobbingRootMapuninstall
TisTosTobbingRootMapuninstall :: TisTosTobbingRootMapclean

TisTosTobbingRootMapclean ::
	@echo 'Deleting $(rootmapfile)'
	@rm -f ${ROOTMAP_DIR}/$(rootmapfile)

#-- start of cleanup_header --------------

clean :: TisTosTobbingRootMapclean
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

TisTosTobbingRootMapclean ::
#-- end of cleanup_header ---------------
