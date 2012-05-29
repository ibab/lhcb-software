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
from Configurables import LoKi__PFbjetsMC as PFJetsMC
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
DaVinci().Simulation	= True                

###### From Victor
#import GaudiPython

## Configure DaVinci
## Particle Flow
from JetAccessories.ParticleFlow_Config import ParticleFlowConf
inputs = ParticleFlowConf("PF",['Photons','NeutralHadrons','Charged','Pi0s','V0s','PFNeutrals'],False,{"NSigmaForCaloRecovery": 3})


## Jet Recontruction
RParameter = 0.5
PtMin = 5000.
PVAssociation = True
doJEC = False
doJetID = False
JetIDNumber = 98
AlgType = "anti-kt"

from JetAccessories.JetMaker_Config import JetMakerConf
jetmaker = JetMakerConf("StdJets"  ,['Phys/PFBannedParticles/Particles','Phys/PFParticles/Particles','Phys/PFNeutralParticles/Particles'],RParameter, PtMin, PVAssociation, doJEC, doJetID, JetIDNumber, AlgType)

RParameter=1.5
Ptmin=40000
JetIDNumber = 99
AlgType = "Cambridge"
fatjetmaker = JetMakerConf("FatJets"  ,['Phys/PFBannedParticles/Particles','Phys/PFParticles/Particles','Phys/PFNeutralParticles/Particles'],RParameter, PtMin, PVAssociation, doJEC, doJetID, JetIDNumber, AlgType)


DaVinci.UserAlgorithms = inputs.algorithms

## Add the jet making to the
DaVinci.UserAlgorithms += jetmaker.algorithms
DaVinci.UserAlgorithms += fatjetmaker.algorithms

#gaudi = GaudiPython.AppMgr() 
#TES = gaudi.evtsvc()
###### From Victor

loop = PFJetsMC(
	"PFJetsMC"                           ,
#	InputLocations = ['MC/Particles','Phys/StdLooseMuons,Phys/StdNoPIDsMuons'],
	Inputs = ['Phys/StdJets','Phys/FatJets','Phys/StdAllLooseMuons'],
	#Inputs = ['Phys/StdJets','Phys/FatJets','Phys/StdLooseMuons'],
	NTupleLUN      = 'LOOP'                ## Logical File Unit for N-tuples 
	)

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#MessageSvc().OutputLevel = 5

#loop.OutputLevel = 5
#sc.sequence().OutputLevel = 5

#from Configurables import LoKi__HDRFilter as StripFilter
#SingleFilter = StripFilter( 'SingleFilter',
#                           Code="HLT_PASS('StrippingZ02MuMuDecision')",
#                           Location="/Event/Strip/Phys/DecReports" )

#DaVinci  (
    #UserAlgorithms = [loop ]  ,  ## the list of algorithms 
#    HistogramFile = "JEShistosMC.root"
#    )



#importOptions('Zmumu_MC11.py')

DaVinci.UserAlgorithms += [loop]
DaVinci.HistogramFile  = "JEShistosMC.root"
#DaVinci.EvtMax         = 1000   ## number of events

## define the output N-tuple:
NTupleSvc(Output = [ "LOOP DATAFILE='ZJESAllMC.root' OPT='NEW' TYP='ROOT'"])

# =============================================================================
# The END 
# =============================================================================
