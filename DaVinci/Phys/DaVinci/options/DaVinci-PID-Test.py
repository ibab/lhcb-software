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
DaVinci().DataType = "2012"                    # Must be given
DaVinci().Simulation   = True
DaVinci().HistogramFile = "DVHistos.root"    # Histogram file
#DaVinci().HistogramFile = "pid/test12-pid.root"    # Histogram file
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

#from Configurables import ChargedProtoANNPIDConf
#nnpidseq = GaudiSequencer("ANNGPIDSeq")
#annconf = ChargedProtoANNPIDConf( DataType = DaVinci().DataType )
#annconf.TrackTypes = ["Long"]
#annconf.PIDTypes   = ["Kaon","Proton"]
#annconf.RecoSequencer = nnpidseq
#DaVinci().UserAlgorithms += [nnpidseq]
    
#from Configurables import GlobalRecoChecks
#moniSeq = GaudiSequencer("ProtoMoniSeq")
#GlobalRecoChecks().Sequencer = moniSeq
#DaVinci().UserAlgorithms += [moniSeq]

from Configurables import ParticleEffPurMoni

from StandardParticles import ( StdLooseANNElectrons, StdLooseElectrons,
                                StdTightANNElectrons, StdTightElectrons )
le = ParticleEffPurMoni('LongElectronPerf')
le.Inputs = [ "Phys/StdLooseANNElectrons/Particles", "Phys/StdLooseElectrons/Particles",
              "Phys/StdTightANNElectrons/Particles", "Phys/StdTightElectrons/Particles" ]
DaVinci().UserAlgorithms += [ le ]

from StandardParticles import ( StdLooseMuons,
                                StdTightANNMuons, StdTightMuons )
lmu = ParticleEffPurMoni('LongMuonPerf')
lmu.Inputs = [ "Phys/StdLooseMuons",
               "Phys/StdTightANNMuons", "Phys/StdTightMuons" ]
DaVinci().UserAlgorithms += [ lmu ]

from StandardParticles import ( StdLooseANNPions, StdLoosePions,
                                StdTightANNPions, StdTightPions )
lpi = ParticleEffPurMoni('LongPionPerf')
lpi.Inputs = [ "Phys/StdLooseANNPions", "Phys/StdLoosePions",
               "Phys/StdTightANNPions", "Phys/StdTightPions" ]
DaVinci().UserAlgorithms += [ lpi ]

from StandardParticles import ( StdLooseANNKaons, StdLooseKaons,
                                StdTightANNKaons, StdTightKaons )
lk = ParticleEffPurMoni('LongKaonPerf')
lk.Inputs = [ "Phys/StdLooseANNKaons", "Phys/StdLooseKaons",
              "Phys/StdTightANNKaons", "Phys/StdTightKaons" ]
DaVinci().UserAlgorithms += [ lk ]

from StandardParticles import ( StdLooseANNProtons, StdLooseProtons,
                                StdTightANNProtons, StdTightProtons )
lp = ParticleEffPurMoni('LongProtonPerf')
lp.Inputs = [ "Phys/StdLooseANNProtons", "Phys/StdLooseProtons",
              "Phys/StdTightANNProtons", "Phys/StdTightProtons" ]
DaVinci().UserAlgorithms += [ lp ]

from StandardParticles import ( StdLooseANNDownElectrons,
                                StdLooseANNDownPions,
                                StdLooseANNDownKaons,
                                StdLooseANNDownProtons,
                                StdTightANNDownElectrons,
                                StdTightANNDownMuons,
                                StdTightANNDownPions,
                                StdTightANNDownKaons,
                                StdTightANNDownProtons )
DaVinci().UserAlgorithms += [
    ParticleEffPurMoni( "DownElectronPerf", Inputs = ["Phys/StdLooseANNDownElectrons",
                                                      "Phys/StdTightANNDownElectrons"] ),
    ParticleEffPurMoni( "DownMuonPerf", Inputs = ["Phys/StdTightANNDownMuons"] ),
    ParticleEffPurMoni( "DownPionPerf", Inputs = ["Phys/StdLooseANNDownPions",
                                                  "Phys/StdTightANNDownPions"] ),
    ParticleEffPurMoni( "DownKaonPerf", Inputs = ["Phys/StdLooseANNDownKaons",
                                                  "Phys/StdTightANNDownKaons"] ),
    ParticleEffPurMoni( "DownProtonPerf", Inputs = ["Phys/StdLooseANNDownProtons",
                                                    "Phys/StdTightANNDownProtons"] )
    ]

from StandardParticles import ( StdLooseANNUpElectrons,
                                StdLooseANNUpPions,
                                StdLooseANNUpKaons,
                                StdLooseANNUpProtons,
                                StdTightANNUpElectrons,
                                StdTightANNUpMuons,
                                StdTightANNUpPions,
                                StdTightANNUpKaons,
                                StdTightANNUpProtons )
DaVinci().UserAlgorithms += [
    ParticleEffPurMoni( "UpElectronPerf", Inputs = ["Phys/StdLooseANNUpElectrons",
                                                    "Phys/StdTightANNUpElectrons"] ),
    ParticleEffPurMoni( "UpMuonPerf", Inputs = ["Phys/StdTightANNUpMuons"] ),
    ParticleEffPurMoni( "UpPionPerf", Inputs = ["Phys/StdLooseANNUpPions",
                                                "Phys/StdTightANNUpPions"] ),
    ParticleEffPurMoni( "UpKaonPerf", Inputs = ["Phys/StdLooseANNUpKaons",
                                                "Phys/StdTightANNUpKaons"] ),
    ParticleEffPurMoni( "UpProtonPerf", Inputs = ["Phys/StdLooseANNUpProtons",
                                                  "Phys/StdTightANNUpProtons"] )
    ]


#importOptions( "$PRCONFIGOPTS/DaVinci/Stripping/Collision12-4TeV-Reco14-Stripping20-BHADRONCOMPLETEEVENT.DST.py" )
from GaudiConf import IOHelper
IOHelper().inputFiles( [
    'PFN:/usera/jonesc/NFS/data/MC/MC12/DST/10000000/00024859_00000040_1.allstreams.dst',
    'PFN:/usera/jonesc/NFS/data/MC/MC12/DST/10000000/00024859_00000041_1.allstreams.dst',
    'PFN:/usera/jonesc/NFS/data/MC/MC12/DST/10000000/00024859_00000042_1.allstreams.dst',
    'PFN:/usera/jonesc/NFS/data/MC/MC12/DST/10000000/00024859_00000043_1.allstreams.dst'
    ] )
