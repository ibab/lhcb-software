########################################################################
#
# Job example of Displaced Vertices selection 
#
# @author Neal Gauvin
# @date 18-Dec-2008
# @info Usage : DaVinci myJob.py
#
############################# MENU #####################################
name    = "test"
nbevent = -1
geom    = "MC09"       # Default is "DC06","2008","MC09"
prey    = "~chi_10"    # "~chi_10", "H_30" 
mother  = "H_10"        # "H_10","H_20" 
#######################################################################
klz = False
MonteC = True
MethodRCut = "FromUpstreamPV"
########################################################################
from os import environ
from math import *
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from GaudiKernel.SystemOfUnits import *
# If you want to import .opts options, do this first
##############################################################################
# load some input data 
evt = [ "DATAFILE='PFN:/castor/cern.ch/user/n/ngueissa/Susy/TriggerSamples/KaplanNeutralino_mN=48GeV_tN=10ps_MC09.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
##############################################################################

##############################################################################
# load options for creation of e,muons,protons,pi,K,photons
from Configurables import BestPIDParticleMaker
from CommonParticles.Utils import *
ChargedP = BestPIDParticleMaker("ChargedPartBest", Particle = 'pion' )
ChargedP.OutputLevel = 3
#ChargedP.Particles = ['electron', 'pion', 'muon']

# configure the track selector
selector = trackSelector ( ChargedP ) 
selector.TrackTypes = [ 'Velo', 'Long', 'Upstream' ]

## configure Data-On-Demand service 
#locations = updateDoD ( ChargedP )

#from Configurables import CombinedParticleMaker
#ChargedP = CombinedParticleMaker("ChargedPartBest", Particle = 'pion' )

##############################################################################

DVSeq = GaudiSequencer("DVSeq")
DVSeq.IgnoreFilterPassed = False

########################################################################
from Configurables import PatPV3D, PVOfflineTool, PVSeed3DTool, \
     LSAdaptPV3DFitter

RV = PatPV3D( "RV" )
DVSeq.Members += [ RV ]
RV.OutputVerticesName = "Rec/Vertices/RV"
RV.OutputLevel = 3
RV.addTool(PVOfflineTool)
RV.PVOfflineTool.InputTracks = ["Rec/Track/Best"]
RV.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
RV.PVOfflineTool.addTool(PVSeed3DTool())
RV.PVOfflineTool.PVSeed3DTool.TrackPairMaxDistance = 0.2*mm
RV.PVOfflineTool.PVSeed3DTool.zMaxSpread = 1*mm
RV.PVOfflineTool.addTool( LSAdaptPV3DFitter())
RV.PVOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 2*mm

########################################################################

########################################################################
from Configurables import  RecVertices2Particles
RV2P = RecVertices2Particles( "RV2P" )
DVSeq.Members += [ RV2P ]
RV2P.InputLocations = [ "Phys/ChargedPartBest" ]
RV2P.RecVerticesLocation = ["Rec/Vertices/RV"]
RV2P.OutputLevel = 3

#Selection Cuts
RV2P.Prey = prey        # D = prey
RV2P.PreyMinMass = 8*GeV   # D = 0.0, 10 for trigger
RV2P.RMin = 0.3*mm         #Min dist to z axis, D = 0.3 mm
RV2P.NbTracks = 0 
RV2P.IsolationDistance = 0 # D = 0
RV2P.KeepLowestZ = klz
RV2P.RemVtxFromDet = 0*mm  #Switch off for tupling !
RV2P.RCutMethod = MethodRCut
########################################################################

########################################################################
from Configurables import DisplVertices
DV = DisplVertices( "DV" )
DVSeq.Members += [ DV ]
DV.InputLocations = [ "Phys/RV2P" ]
DV.RCutMethod = MethodRCut
DV.OutputLevel = 3
DV.MC = MonteC
DV.Prey = prey
DV.MotherPrey = mother
DV.SaveTuple = False        #### Switch RV2P.RemVtxFromDet down for tupling !!!
DV.SelectForSample = False
DV.SaveTrigInfos = False
if( DV.SaveTrigInfos ): DVSeq.IgnoreFilterPassed = True

#Selection Cuts
DV.NbTracks = 1
DV.PreyMinMass = 1*GeV
DV.RMin = 0.

########################################################################
from Configurables import DaVinci
DaVinci().EvtMax = nbevent
DaVinci().PrintFreq = 100
DaVinci().SkipEvents = 0
DaVinci().DataType = geom                       # Default is "DC06","2008"
DaVinci().Simulation   = True
DaVinci().HistogramFile = name + "_histo.root"    # Histogram file
DaVinci().TupleFile = name + ".root"    
DaVinci().UserAlgorithms = [ChargedP, DVSeq] #ChargedP, DVSeq # The algorithms
DaVinci().Input = evt
##############################################################################







