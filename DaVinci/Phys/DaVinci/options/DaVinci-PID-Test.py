########################################################################
#
# $Id: DaVinci.py,v 1.45 2010-03-17 17:18:28 pkoppenb Exp $
#
# Options for a typical DaVinci job
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import DaVinci, PhysConf
########################################################################

################################################################
#
# Standard configuration
#
DaVinci().EvtMax = 100000                      # Number of events
DaVinci().PrintFreq = 1000
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().DataType = "2011"                    # Must be given
DaVinci().Simulation   = True
#DaVinci().HistogramFile = "DVHistos.root"    # Histogram file
DaVinci().HistogramFile = "pid/test12-pid.root"    # Histogram file
DaVinci().TupleFile = "DVNtuples.root"          # Ntuple
#DaVinci().MoniSequence = [ exampleSeq ]        # Monitoring
# DaVinci().MainOptions  = "" # None
########################################################################
# HLT
# DaVinci().ReplaceL0BanksWithEmulated = True ## enable if you want to rerun L0
# DaVinci().Hlt2Requires = 'L0+Hlt1'          ## change if you want Hlt2 irrespective of Hlt1
# DaVinci().Hlt = True             ##
########################################################################
MessageSvc().Format = "% F%20W%S%7W%R%T %0W%M"
########################################################################
#
# To run : gaudirun.py options/DaVinci.py options/DaVinciTestData.py
#
########################################################################

from Configurables import ChargedProtoANNPIDConf
nnpidseq = GaudiSequencer("ANNGPIDSeq")
annconf = ChargedProtoANNPIDConf( DataType = DaVinci().DataType )
#annconf.TrackTypes = ["Long"]
#annconf.PIDTypes   = ["Kaon","Proton"]
annconf.RecoSequencer = nnpidseq
DaVinci().UserAlgorithms += [nnpidseq]
    
#from Configurables import GlobalRecoChecks
#moniSeq = GaudiSequencer("ProtoMoniSeq")
#GlobalRecoChecks().Sequencer = moniSeq
#DaVinci().UserAlgorithms += [moniSeq]

from Configurables import ParticleEffPurMoni

from StandardParticles import ( StdLooseANNElectrons, StdLooseElectrons,
                                StdTightANNElectrons, StdTightElectrons )
le = ParticleEffPurMoni('ElectronPerf')
le.Inputs = [ "Phys/StdLooseANNElectrons/Particles", "Phys/StdLooseElectrons/Particles",
              "Phys/StdTightANNElectrons/Particles", "Phys/StdTightElectrons/Particles" ]
DaVinci().UserAlgorithms += [ le ]

from StandardParticles import ( StdLooseMuons,
                                StdTightANNMuons, StdTightMuons )
lmu = ParticleEffPurMoni('MuonPerf')
lmu.Inputs = [ "Phys/StdLooseMuons",
               "Phys/StdTightANNMuons", "Phys/StdTightMuons" ]
DaVinci().UserAlgorithms += [ lmu ]

from StandardParticles import ( StdLooseANNPions, StdLoosePions,
                                StdTightANNPions, StdTightPions )
lpi = ParticleEffPurMoni('PionPerf')
lpi.Inputs = [ "Phys/StdLooseANNPions", "Phys/StdLoosePions",
               "Phys/StdTightANNPions", "Phys/StdTightPions" ]
DaVinci().UserAlgorithms += [ lpi ]

from StandardParticles import ( StdLooseANNKaons, StdLooseKaons,
                                StdTightANNKaons, StdTightKaons )
lk = ParticleEffPurMoni('KaonPerf')
lk.Inputs = [ "Phys/StdLooseANNKaons", "Phys/StdLooseKaons",
              "Phys/StdTightANNKaons", "Phys/StdTightKaons" ]
DaVinci().UserAlgorithms += [ lk ]

from StandardParticles import ( StdLooseANNProtons, StdLooseProtons,
                                StdTightANNProtons, StdTightProtons )
lp = ParticleEffPurMoni('ProtonPerf')
lp.Inputs = [ "Phys/StdLooseANNProtons", "Phys/StdLooseProtons",
              "Phys/StdTightANNProtons", "Phys/StdTightProtons" ]
DaVinci().UserAlgorithms += [ lp ]

## from StandardParticles import ( StdLooseANNDownElectrons,
##                                 StdLooseANNDownPions,
##                                 StdLooseANNDownKaons,
##                                 StdLooseANNDownProtons,
##                                 StdTightANNDownElectrons,
##                                 StdTightANNDownMuons,
##                                 StdTightANNDownPions,
##                                 StdTightANNDownKaons,
##                                 StdTightANNDownProtons )
## DaVinci().UserAlgorithms += [
##     ParticleEffPurMoni( "ElPerfDown", Inputs = ["Phys/StdLooseANNDownElectrons",
##                                                 "Phys/StdTightANNDownElectrons"] ),
##     ParticleEffPurMoni( "MuPerfDown", Inputs = ["Phys/StdTightANNDownMuons"] ),
##     ParticleEffPurMoni( "PiPerfDown", Inputs = ["Phys/StdLooseANNDownPions",
##                                                 "Phys/StdTightANNDownPions"] ),
##     ParticleEffPurMoni( "KaPerfDown", Inputs = ["Phys/StdLooseANNDownKaons",
##                                                 "Phys/StdTightANNDownKaons"] ),
##     ParticleEffPurMoni( "PrPerfDown", Inputs = ["Phys/StdLooseANNDownProtons",
##                                                 "Phys/StdTightANNDownProtons"] )
##     ]

## from StandardParticles import ( StdLooseANNUpElectrons,
##                                 StdLooseANNUpPions,
##                                 StdLooseANNUpKaons,
##                                 StdLooseANNUpProtons,
##                                 StdTightANNUpElectrons,
##                                 StdTightANNUpMuons,
##                                 StdTightANNUpPions,
##                                 StdTightANNUpKaons,
##                                 StdTightANNUpProtons )
## DaVinci().UserAlgorithms += [
##     ParticleEffPurMoni( "ElPerfUp", Inputs = ["Phys/StdLooseANNUpElectrons",
##                                               "Phys/StdTightANNUpElectrons"] ),
##     ParticleEffPurMoni( "MuPerfUp", Inputs = ["Phys/StdTightANNUpMuons"] ),
##     ParticleEffPurMoni( "PiPerfUp", Inputs = ["Phys/StdLooseANNUpPions",
##                                               "Phys/StdTightANNUpPions"] ),
##     ParticleEffPurMoni( "KaPerfUp", Inputs = ["Phys/StdLooseANNUpKaons",
##                                               "Phys/StdTightANNUpKaons"] ),
##     ParticleEffPurMoni( "PrPerfUp", Inputs = ["Phys/StdLooseANNUpProtons",
##                                               "Phys/StdTightANNUpProtons"] )
##     ]
                                

# Additional options for accessing Oracle conditions DB.
#from Configurables import CondDB
#CondDB(UseOracle = True)
