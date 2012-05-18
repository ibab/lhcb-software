#!/usr/bin/env gaudirun.py
# =============================================================================
## The general configuration stuff
# =============================================================================
from  Gaudi.Configuration import *
# =============================================================================
## The general configuratiin stuff from DaVinci
from Configurables import  DaVinci, GaudiSequencer, L0Conf
# =============================================================================
## pick-up the 'configurable' for our specific algorithm 
# =============================================================================
from Configurables import LoKi__PFbjets as PFJets
#====================================================================================================
# Condbtag and DDDBtag
#====================================================================================================
#from Configurables import ( DaVinci,PhysDesktop,CombineParticles,FilterDesktop)                                                                                                                                          
from Configurables import ( DaVinci,CombineParticles,FilterDesktop)

#for MC, use the same tag for production
#https://twiki.cern.ch/twiki/bin/view/LHCb/RecommendedTags

#DaVinci().DDDBtag	= "head-20101206"
#DaVinci().CondDBtag	= "sim-20101210-vc-md100"
DaVinci().DataType	= '2011'
DaVinci().SkipEvents	= 0

from Configurables import LoKi__HDRFilter as StripFilter
SingleFilter = StripFilter( 'SingleFilter',
                           Code="HLT_PASS('StrippingWMuLineDecision')",
                           Location="/Event/Strip/Phys/DecReports" )

#DaVinci().EventPreFilters += [SingleFilter]

from PhysConf.Filters import LoKi_Filters
fltrs = LoKi_Filters (
#HLT_Code     = " HLT_PASS_RE ('Hlt1MBMicro.*Decision') | HLT_PASS_RE ('Hlt1.*Hadron.*Decision') " ,
STRIP_Code = "HLT_PASS ('StrippingWMuLineDecision')"             
#VOID_Code  = " EXISTS ('/Event/Strip') & EXISTS ('/Event/Charm') "
)

fltrSeq = fltrs.sequence ( 'MyFilters' )  

DaVinci.EventPreFilters = [fltrSeq]

###### From Victor
#import GaudiPython

## Configure DaVinci

## Particle Flow
from JetAccessories.ParticleFlow_Config import ParticleFlowConf
p = ParticleFlowConf("PF",['Photons','NeutralHadrons','Charged','Pi0s','V0s'],False,{'CandidateToBanLocation':''})

DaVinci.UserAlgorithms = p.algorithms

## Jet Algorithm
from JetAccessories.JetMaker_Config import JetMakerConf
#j        = JetMakerConf("StdJets"         ,['Phys/PFBannedParticles/Particles','Phys/PFParticles/Particles'], 0.6 , 10000., True )
#j        = JetMakerConf("StdJets"         ,['Phys/PFParticles/Particles'], 0.6 , 5000., True )
j        = JetMakerConf("StdJets"         ,['Phys/PFBannedParticles/Particles','Phys/PFParticles/Particles'], 0.5 , 5000., True, False, False ,98)
fatjets        = JetMakerConf("FatJets"         ,['Phys/PFBannedParticles/Particles','Phys/PFParticles/Particles'], 1.5 , 50000., True , False, False, 99, "Cambridge")
## Add the jet making to the
DaVinci.UserAlgorithms += j.algorithms
DaVinci.UserAlgorithms += fatjets.algorithms

#gaudi = GaudiPython.AppMgr() 
#TES = gaudi.evtsvc()
###### From Victor

loop = PFJets(
	"PFJets"                           ,
#	InputLocations = ['MC/Particles','Phys/StdLooseMuons,Phys/StdNoPIDsMuons'],
	Inputs = ['Phys/StdJets','Phys/FatJets','Phys/StdAllLooseMuons'],
	NTupleLUN      = 'LOOP'                ## Logical File Unit for N-tuples 
	)

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#MessageSvc().OutputLevel = 5

#loop.OutputLevel = 5
#sc.sequence().OutputLevel = 5


#DaVinci  (
    #UserAlgorithms = [loop ]  ,  ## the list of algorithms 
#    HistogramFile = "JEShistosMC.root"
#    )



#importOptions('Data_EW_2011.py')
#importOptions('datastrip17.py')
# job will only run over events passing the line.

DaVinci.UserAlgorithms += [loop]
DaVinci.HistogramFile  = "JEShistosMC.root"
#DaVinci.EvtMax         = 10000   ## number of events

## define the output N-tuple:
NTupleSvc(Output = [ "LOOP DATAFILE='ZJESAllMC.root' OPT='NEW' TYP='ROOT'"])

# =============================================================================
# The END 
# =============================================================================
