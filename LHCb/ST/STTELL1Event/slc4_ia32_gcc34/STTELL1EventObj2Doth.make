#-- start of make_header -----------------

#====================================
#  Document STTELL1EventObj2Doth
#
#   Generated Tue Oct 23 11:16:58 2007  by akeune
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_STTELL1EventObj2Doth_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_STTELL1EventObj2Doth_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_STTELL1EventObj2Doth

STTELL1Event_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_STTELL1EventObj2Doth = /tmp/CMT_$(STTELL1Event_tag)_STTELL1EventObj2Doth.make$(cmt_lock_pid)
else
cmt_local_tagfile_STTELL1EventObj2Doth = $(STTELL1Event_tag)_STTELL1EventObj2Doth.make
endif

else

tags      = $(tag),$(CMTEXTRATAGS)

STTELL1Event_tag = $(tag)

ifdef READONLY
cmt_local_tagfile_STTELL1EventObj2Doth = /tmp/CMT_$(STTELL1Event_tag).make$(cmt_lock_pid)
else
cmt_local_tagfile_STTELL1EventObj2Doth = $(STTELL1Event_tag).make
endif

endif

-include $(cmt_local_tagfile_STTELL1EventObj2Doth)

ifdef cmt_STTELL1EventObj2Doth_has_target_tag

ifdef READONLY
cmt_final_setup_STTELL1EventObj2Doth = /tmp/CMT_STTELL1Event_STTELL1EventObj2Dothsetup.make
cmt_local_STTELL1EventObj2Doth_makefile = /tmp/CMT_STTELL1EventObj2Doth$(cmt_lock_pid).make
else
cmt_final_setup_STTELL1EventObj2Doth = $(bin)STTELL1Event_STTELL1EventObj2Dothsetup.make
cmt_local_STTELL1EventObj2Doth_makefile = $(bin)STTELL1EventObj2Doth.make
endif

else

ifdef READONLY
cmt_final_setup_STTELL1EventObj2Doth = /tmp/CMT_STTELL1Eventsetup.make
cmt_local_STTELL1EventObj2Doth_makefile = /tmp/CMT_STTELL1EventObj2Doth$(cmt_lock_pid).make
else
cmt_final_setup_STTELL1EventObj2Doth = $(bin)STTELL1Eventsetup.make
cmt_local_STTELL1EventObj2Doth_makefile = $(bin)STTELL1EventObj2Doth.make
endif

endif

ifdef READONLY
cmt_final_setup = /tmp/CMT_STTELL1Eventsetup.make
else
cmt_final_setup = $(bin)STTELL1Eventsetup.make
endif

STTELL1EventObj2Doth ::


ifdef READONLY
STTELL1EventObj2Doth ::
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
	@echo 'STTELL1EventObj2Doth'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = STTELL1EventObj2Doth/
STTELL1EventObj2Doth::
	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
	@echo "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

#-- end of make_header ------------------


pythonexe     = $(Python_home)/bin/python
parsetool     = $(GAUDIOBJDESCROOT)/scripts/godII.py
dest          = ../Event/
dtdfile       = ../$(xmlsrc)/gdd.dtd 

STTELL1EventObj2Doth_output = $(dest)

STTELL1EventObj2Doth :: $(dtdfile) $(STTELL1EventObj2Doth_output)STTELL1Data.obj2doth $(STTELL1EventObj2Doth_output)STTELL1EventInfo.obj2doth
	@echo "-----> STTELL1EventObj2Doth ok"

$(dtdfile) : $(GAUDIOBJDESCROOT)/xml_files/gdd.dtd
	@echo "Copying global DTD to current package"
	@cp $(GAUDIOBJDESCROOT)/xml_files/gdd.dtd $(dtdfile)
STTELL1Data_xml_dependencies = ../xml/STTELL1Data.xml
STTELL1EventInfo_xml_dependencies = ../xml/STTELL1EventInfo.xml
$(dest)STTELL1Data.obj2doth : ../xml/STTELL1Data.xml 
	@echo Producing Header files from ../xml/STTELL1Data.xml
	@if !(test -d $(dest)); then mkdir $(dest); fi 
	export LD_LIBRARY_PATH=$(Python_home)/lib; cd $(dest); $(pythonexe) $(parsetool) $(GODflags) $(STTELL1EventObj2Doth_GODflags) -g src -r $(GAUDIOBJDESCROOT) ../xml/STTELL1Data.xml
	@echo /dev/null > $@
$(dest)STTELL1EventInfo.obj2doth : ../xml/STTELL1EventInfo.xml 
	@echo Producing Header files from ../xml/STTELL1EventInfo.xml
	@if !(test -d $(dest)); then mkdir $(dest); fi 
	export LD_LIBRARY_PATH=$(Python_home)/lib; cd $(dest); $(pythonexe) $(parsetool) $(GODflags) $(STTELL1EventObj2Doth_GODflags) -g src -r $(GAUDIOBJDESCROOT) ../xml/STTELL1EventInfo.xml
	@echo /dev/null > $@
 
clean :: STTELL1EventObj2Dothclean
	@cd .

STTELL1EventObj2Dothclean ::
	$(cleanup_echo) .obj2doth files
	-$(cleanup_silent) rm $(dest)*.obj2doth 
	-$(cleanup_silent) rm -f $(dtdfile)


#-- start of cleanup_header --------------

clean :: STTELL1EventObj2Dothclean
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

STTELL1EventObj2Dothclean ::
#-- end of cleanup_header ---------------
