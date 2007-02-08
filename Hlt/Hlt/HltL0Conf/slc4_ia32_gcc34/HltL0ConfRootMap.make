#-- start of make_header -----------------

#====================================
#  Document HltL0ConfRootMap
#
#   Generated Thu Feb  8 18:06:53 2007  by hernando
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_HltL0ConfRootMap_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_HltL0ConfRootMap_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_HltL0ConfRootMap

HltL0Conf_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_HltL0ConfRootMap = /tmp/CMT_$(HltL0Conf_tag)_HltL0ConfRootMap.make$(cmt_lock_pid)
else
cmt_local_tagfile_HltL0ConfRootMap = $(HltL0Conf_tag)_HltL0ConfRootMap.make
endif

else

tags      = $(tag),$(CMTEXTRATAGS)

HltL0Conf_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_HltL0ConfRootMap = /tmp/CMT_$(HltL0Conf_tag).make$(cmt_lock_pid)
else
cmt_local_tagfile_HltL0ConfRootMap = $(HltL0Conf_tag).make
endif

endif

-include $(cmt_local_tagfile_HltL0ConfRootMap)

ifdef cmt_HltL0ConfRootMap_has_target_tag

ifdef READONLY
cmt_final_setup_HltL0ConfRootMap = /tmp/CMT_HltL0Conf_HltL0ConfRootMapsetup.make
cmt_local_HltL0ConfRootMap_makefile = /tmp/CMT_HltL0ConfRootMap$(cmt_lock_pid).make
else
cmt_final_setup_HltL0ConfRootMap = $(bin)HltL0Conf_HltL0ConfRootMapsetup.make
cmt_local_HltL0ConfRootMap_makefile = $(bin)HltL0ConfRootMap.make
endif

else

ifdef READONLY
cmt_final_setup_HltL0ConfRootMap = /tmp/CMT_HltL0Confsetup.make
cmt_local_HltL0ConfRootMap_makefile = /tmp/CMT_HltL0ConfRootMap$(cmt_lock_pid).make
else
cmt_final_setup_HltL0ConfRootMap = $(bin)HltL0Confsetup.make
cmt_local_HltL0ConfRootMap_makefile = $(bin)HltL0ConfRootMap.make
endif

endif

ifdef READONLY
cmt_final_setup = /tmp/CMT_HltL0Confsetup.make
else
cmt_final_setup = $(bin)HltL0Confsetup.make
endif

HltL0ConfRootMap ::


ifdef READONLY
HltL0ConfRootMap ::
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
	@echo 'HltL0ConfRootMap'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = HltL0ConfRootMap/
HltL0ConfRootMap::
	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
	@echo "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

#-- end of make_header ------------------

rootmapfile = HltL0Conf.rootmap
fulllibname = libHltL0Conf.$(shlibsuffix)
ifdef CMTINSTALLAREA 
ROOTMAP_DIR = ${CMTINSTALLAREA}/$(tag)/lib
else
ROOTMAP_DIR = ../$(tag)
endif

HltL0ConfRootMap :: ${ROOTMAP_DIR}/$(rootmapfile)

${ROOTMAP_DIR}/$(rootmapfile) :: $(bin)$(fulllibname)
	@echo 'Generating rootmap file for $(fulllibname)'
	cd ../$(tag);$(genmap_cmd) -i $(fulllibname) -o ${ROOTMAP_DIR}/$(rootmapfile) $(HltL0ConfRootMap_genmapflags)

install :: HltL0ConfRootMapinstall
HltL0ConfRootMapinstall :: HltL0ConfRootMap

uninstall :: HltL0ConfRootMapuninstall
HltL0ConfRootMapuninstall :: HltL0ConfRootMapclean

HltL0ConfRootMapclean ::
	@echo 'Deleting $(rootmapfile)'
	@rm -f ${ROOTMAP_DIR}/$(rootmapfile)

#-- start of cleanup_header --------------

clean :: HltL0ConfRootMapclean
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




HltL0ConfRootMapclean ::
#-- end of cleanup_header ---------------
