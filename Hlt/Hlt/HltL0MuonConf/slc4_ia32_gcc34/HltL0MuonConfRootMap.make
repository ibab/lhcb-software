#-- start of make_header -----------------

#====================================
#  Document HltL0MuonConfRootMap
#
#   Generated Mon May 14 15:17:26 2007  by asatta
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_HltL0MuonConfRootMap_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_HltL0MuonConfRootMap_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_HltL0MuonConfRootMap

HltL0MuonConf_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_HltL0MuonConfRootMap = /tmp/CMT_$(HltL0MuonConf_tag)_HltL0MuonConfRootMap.make$(cmt_lock_pid)
else
cmt_local_tagfile_HltL0MuonConfRootMap = $(HltL0MuonConf_tag)_HltL0MuonConfRootMap.make
endif

else

tags      = $(tag),$(CMTEXTRATAGS)

HltL0MuonConf_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_HltL0MuonConfRootMap = /tmp/CMT_$(HltL0MuonConf_tag).make$(cmt_lock_pid)
else
cmt_local_tagfile_HltL0MuonConfRootMap = $(HltL0MuonConf_tag).make
endif

endif

-include $(cmt_local_tagfile_HltL0MuonConfRootMap)

ifdef cmt_HltL0MuonConfRootMap_has_target_tag

ifdef READONLY
cmt_final_setup_HltL0MuonConfRootMap = /tmp/CMT_HltL0MuonConf_HltL0MuonConfRootMapsetup.make
cmt_local_HltL0MuonConfRootMap_makefile = /tmp/CMT_HltL0MuonConfRootMap$(cmt_lock_pid).make
else
cmt_final_setup_HltL0MuonConfRootMap = $(bin)HltL0MuonConf_HltL0MuonConfRootMapsetup.make
cmt_local_HltL0MuonConfRootMap_makefile = $(bin)HltL0MuonConfRootMap.make
endif

else

ifdef READONLY
cmt_final_setup_HltL0MuonConfRootMap = /tmp/CMT_HltL0MuonConfsetup.make
cmt_local_HltL0MuonConfRootMap_makefile = /tmp/CMT_HltL0MuonConfRootMap$(cmt_lock_pid).make
else
cmt_final_setup_HltL0MuonConfRootMap = $(bin)HltL0MuonConfsetup.make
cmt_local_HltL0MuonConfRootMap_makefile = $(bin)HltL0MuonConfRootMap.make
endif

endif

ifdef READONLY
cmt_final_setup = /tmp/CMT_HltL0MuonConfsetup.make
else
cmt_final_setup = $(bin)HltL0MuonConfsetup.make
endif

HltL0MuonConfRootMap ::


ifdef READONLY
HltL0MuonConfRootMap ::
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
	@echo 'HltL0MuonConfRootMap'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = HltL0MuonConfRootMap/
HltL0MuonConfRootMap::
	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
	@echo "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

#-- end of make_header ------------------

rootmapfile = HltL0MuonConf.rootmap
fulllibname = libHltL0MuonConf.$(shlibsuffix)
ifdef CMTINSTALLAREA 
ROOTMAP_DIR = ${CMTINSTALLAREA}/$(tag)/lib
else
ROOTMAP_DIR = ../$(tag)
endif

HltL0MuonConfRootMap :: ${ROOTMAP_DIR}/$(rootmapfile)

${ROOTMAP_DIR}/$(rootmapfile) :: $(bin)$(fulllibname)
	@echo 'Generating rootmap file for $(fulllibname)'
	cd ../$(tag);$(genmap_cmd) -i $(fulllibname) -o ${ROOTMAP_DIR}/$(rootmapfile) $(HltL0MuonConfRootMap_genmapflags)

install :: HltL0MuonConfRootMapinstall
HltL0MuonConfRootMapinstall :: HltL0MuonConfRootMap

uninstall :: HltL0MuonConfRootMapuninstall
HltL0MuonConfRootMapuninstall :: HltL0MuonConfRootMapclean

HltL0MuonConfRootMapclean ::
	@echo 'Deleting $(rootmapfile)'
	@rm -f ${ROOTMAP_DIR}/$(rootmapfile)

#-- start of cleanup_header --------------

clean :: HltL0MuonConfRootMapclean
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

HltL0MuonConfRootMapclean ::
#-- end of cleanup_header ---------------
