#
# File generated ${now}
#



CMTROOT = /afs/cern.ch/sw/contrib/CMT/v1r9
MUONDETROOT = /afs/cern.ch/user/d/dhcroft/newmycmt/Det/MuonDet/v1r1
DETDESCROOT = /afs/cern.ch/lhcb/software/NEW/Det/DetDesc/v9r2
MUONKERNELROOT = /afs/cern.ch/user/d/dhcroft/newmycmt/Muon/MuonKernel/v1r0
LHCBEVENTROOT = /afs/cern.ch/lhcb/software/NEW/Event/LHCbEvent/v12r2
GAUDIOBJDESCROOT = /afs/cern.ch/sw/Gaudi/releases/GaudiObjDesc/v2r0
GAUDIINTROSPECTIONROOT = /afs/cern.ch/sw/Gaudi/releases/GaudiIntrospection/v2r0
GAUDIKERNELROOT = /afs/cern.ch/sw/Gaudi/releases/GaudiKernel/v11r1
GAUDIPOLICYROOT = /afs/cern.ch/sw/Gaudi/releases/GaudiPolicy/v5r4
AIDAROOT = /afs/cern.ch/sw/Gaudi/releases/AIDA/v1r0
CLHEPROOT = /afs/cern.ch/sw/Gaudi/releases/CLHEP/v17r10
XERCESROOT = /afs/cern.ch/sw/Gaudi/releases/XERCES/v15r2p2
EXTERNALLIBSROOT = /afs/cern.ch/sw/Gaudi/releases/ExternalLibs/v3r5
use_requirements = requirements $(CMTROOT)/mgr/requirements $(MUONDETROOT)/cmt/requirements $(DETDESCROOT)/cmt/requirements $(MUONKERNELROOT)/cmt/requirements $(LHCBEVENTROOT)/cmt/requirements $(GAUDIOBJDESCROOT)/cmt/requirements $(GAUDIINTROSPECTIONROOT)/cmt/requirements $(GAUDIKERNELROOT)/cmt/requirements $(GAUDIPOLICYROOT)/cmt/requirements $(AIDAROOT)/cmt/requirements $(CLHEPROOT)/cmt/requirements $(XERCESROOT)/cmt/requirements $(EXTERNALLIBSROOT)/cmt/requirements 
constituents = MuonTools 

MuonTools_tag = $(tag)

all :: config

csh :: $(MuonTools_tag).csh
	@/bin/echo $(MuonTools_tag).csh ok

$(MuonTools_tag).csh : cmt_path.make requirements ${use_requirements}
	@if test "$(nocheck)" = ""; then \
	  /bin/echo "Rebuilding $@"; \
	  $(cmtexe) setup -quiet -tag=$(MuonTools_tag) | sed -e 's#`${CMTROOT}/mgr/cmt system`#$(MuonTools_tag)#' >$(MuonTools_tag).csh; \
	fi

sh :: $(MuonTools_tag).sh
	@/bin/echo $(MuonTools_tag).sh ok

$(MuonTools_tag).sh : cmt_path.make requirements ${use_requirements}
	@if test "$(nocheck)" = ""; then \
	  /bin/echo "Rebuilding $@"; \
	  $(cmtexe) setup -quiet -sh -tag=$(MuonTools_tag) | sed -e 's#`${CMTROOT}/mgr/cmt system`#$(MuonTools_tag)#' >$(MuonTools_tag).csh; \
	fi

config :: checkuses ##$(MuonTools_tag).make
	@/bin/echo $(MuonTools_tag).make ok

env.make ::
	@printenv >env.make.tmp; $(cmtexe) -quiet check files env.make.tmp env.make

$(MuonTools_tag).make : requirements $(use_requirements)
	@if test ! -d $(bin) ; then $(mkdir) -p $(bin); fi
	@if test -f $(MuonTools_tag).make; then /bin/rm -f $(MuonTools_tag).make; fi
	@$(cmtexe) -quiet build tag_makefile -tag=$(MuonTools_tag) >t$$$$; mv t$$$$ $(MuonTools_tag).make

-include $(MuonTools_tag).make


