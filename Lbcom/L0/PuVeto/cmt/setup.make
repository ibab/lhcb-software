#
# File generated ${now}
#



CMTROOT = /afs/cern.ch/sw/contrib/CMT/v1r9
L1EVENTROOT = /afs/cern.ch/lhcb/software/NEW/Event/L1Event/v6r0
VELOEVENTROOT = /afs/cern.ch/lhcb/software/NEW/Event/VeloEvent/v6r0
L0DUROOT = /afs/cern.ch/lhcb/software/NEW/L0/L0DU/v3r0
L0CALOROOT = /afs/cern.ch/lhcb/software/NEW/L0/L0Calo/v3r0
CALOEVENTROOT = /afs/cern.ch/lhcb/software/NEW/Event/CaloEvent/v7r0
L0MUONROOT = /afs/cern.ch/lhcb/software/NEW/L0/L0Muon/v4r0
L0EVENTROOT = /afs/cern.ch/user/o/ocallot/newmycmt/Event/L0Event/v4r1
LHCBEVENTROOT = /afs/cern.ch/lhcb/software/NEW/Event/LHCbEvent/v12r1
GAUDIOBJDESCROOT = /afs/cern.ch/user/o/ocallot/newmycmt/GaudiObjDesc/v2r1
GAUDIINTROSPECTIONROOT = /afs/cern.ch/sw/Gaudi/releases/GaudiIntrospection/v2r0
VELODETROOT = /afs/cern.ch/user/o/ocallot/newmycmt/Det/VeloDet/v3r0
CALODETROOT = /afs/cern.ch/lhcb/software/NEW/Det/CaloDet/v3r0
DETDESCROOT = /afs/cern.ch/lhcb/software/NEW/Det/DetDesc/v9r2
CALOKERNELROOT = /afs/cern.ch/lhcb/software/NEW/Calo/CaloKernel/v1r0
GAUDIKERNELROOT = /afs/cern.ch/sw/Gaudi/releases/GaudiKernel/v11r0
GAUDIPOLICYROOT = /afs/cern.ch/sw/Gaudi/releases/GaudiPolicy/v5r4
AIDAROOT = /afs/cern.ch/sw/Gaudi/releases/AIDA/v1r0
CLHEPROOT = /afs/cern.ch/sw/Gaudi/releases/CLHEP/v17r10
XERCESROOT = /afs/cern.ch/sw/Gaudi/releases/XERCES/v15r2
EXTERNALLIBSROOT = /afs/cern.ch/sw/Gaudi/releases/ExternalLibs/v3r3
use_requirements = requirements $(CMTROOT)/mgr/requirements $(L1EVENTROOT)/cmt/requirements $(VELOEVENTROOT)/cmt/requirements $(L0DUROOT)/cmt/requirements $(L0CALOROOT)/cmt/requirements $(CALOEVENTROOT)/cmt/requirements $(L0MUONROOT)/cmt/requirements $(L0EVENTROOT)/cmt/requirements $(LHCBEVENTROOT)/cmt/requirements $(GAUDIOBJDESCROOT)/cmt/requirements $(GAUDIINTROSPECTIONROOT)/cmt/requirements $(VELODETROOT)/cmt/requirements $(CALODETROOT)/cmt/requirements $(DETDESCROOT)/cmt/requirements $(CALOKERNELROOT)/cmt/requirements $(GAUDIKERNELROOT)/cmt/requirements $(GAUDIPOLICYROOT)/cmt/requirements $(AIDAROOT)/cmt/requirements $(CLHEPROOT)/cmt/requirements $(XERCESROOT)/cmt/requirements $(EXTERNALLIBSROOT)/cmt/requirements 
constituents = PuVeto 

PuVeto_tag = $(tag)

all :: config

csh :: $(PuVeto_tag).csh
	@/bin/echo $(PuVeto_tag).csh ok

$(PuVeto_tag).csh : cmt_path.make requirements ${use_requirements}
	@if test "$(nocheck)" = ""; then \
	  /bin/echo "Rebuilding $@"; \
	  $(cmtexe) setup -quiet -tag=$(PuVeto_tag) | sed -e 's#`${CMTROOT}/mgr/cmt system`#$(PuVeto_tag)#' >$(PuVeto_tag).csh; \
	fi

sh :: $(PuVeto_tag).sh
	@/bin/echo $(PuVeto_tag).sh ok

$(PuVeto_tag).sh : cmt_path.make requirements ${use_requirements}
	@if test "$(nocheck)" = ""; then \
	  /bin/echo "Rebuilding $@"; \
	  $(cmtexe) setup -quiet -sh -tag=$(PuVeto_tag) | sed -e 's#`${CMTROOT}/mgr/cmt system`#$(PuVeto_tag)#' >$(PuVeto_tag).csh; \
	fi

config :: checkuses ##$(PuVeto_tag).make
	@/bin/echo $(PuVeto_tag).make ok

env.make ::
	@printenv >env.make.tmp; $(cmtexe) -quiet check files env.make.tmp env.make

$(PuVeto_tag).make : requirements $(use_requirements)
	@if test ! -d $(bin) ; then $(mkdir) -p $(bin); fi
	@if test -f $(PuVeto_tag).make; then /bin/rm -f $(PuVeto_tag).make; fi
	@$(cmtexe) -quiet build tag_makefile -tag=$(PuVeto_tag) >t$$$$; mv t$$$$ $(PuVeto_tag).make

-include $(PuVeto_tag).make


