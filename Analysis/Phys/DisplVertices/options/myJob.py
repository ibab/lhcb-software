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
nbevent = 1000
geom    = "2010"       # Default is "DC06","2008","MC09"
prey    = "~chi_10"    # "~chi_10", "H_30" 
mother  = "H_10"        # "H_10","H_20" 
#######################################################################
MethodRCut = "FromUpstreamPV"  #"FromBeamLine"
########################################################################
from os import environ
from math import *
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from GaudiKernel.SystemOfUnits import *
# If you want to import .opts options, do this first
##############################################################################
# load some input data 
evt = [ "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2010/DST/00007676/0000/00007676_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
##############################################################################

##############################################################################
# Create a Sequencer
DVSeq = GaudiSequencer("DVSeq")
DVSeq.IgnoreFilterPassed = False

########################################################################
#Eventually apply a filter
from Configurables import LoKi__HDRFilter as StripFilter
SingleFilter = StripFilter( 'SingleFilter',
                            Code="HLT_PASS('StrippingSingleDisplVtxDecision')",
                            Location="/Event/Strip/Phys/DecReports" )
DoubleFilter = StripFilter( 'DoubleFilter',
                            Code="HLT_PASS('StrippingDoubleDisplVtxDecision')",
                            Location="/Event/Strip/Phys/DecReports" )
OrFilter = StripFilter( 'OrFilter',
                        Code="HLT_PASS('StrippingDoubleDisplVtxDecision') or HLT_PASS('StrippingSingleDisplVtxDecision')",
                        Location="/Event/Strip/Phys/DecReports" )
#DVSeq.Members += [ OrFilter ]
#DVSeq.Members += [ SingleFilter ]
#DVSeq.Members += [ DoubleFilter ]
########################################################################
# You may want to reconstruct the beam line...
from Configurables import CalibrateIP
BL = CalibrateIP( "BeamLine" )
BL.OutputLocation = "/Event/BeamLine"
BL.OutputLevel = 3
#Must be short for offline real data... 10-100
#Can be really long for MC data...
BL.BeamLineCycle = 50
# The default ensures the beam line is not saved on TES before any estmation is done. Can be avoided in the MC where I know where the BL is...
# Or uncomment the following
#BL.LocalVeloFrame = True
# to use PVs as the UpPV...
BL.MinZ = -150.
BL.MaxZ = 150.
BL.MaxY = 1.5
BL.MaxX = 1.5

########################################################################
# Reconstruct the displaced vertices... A few possibilities.
from Configurables import PatPV3D, PVOfflineTool, PVSeed3DTool, \
     LSAdaptPV3DFitter
#PatPV3D with optimised parameters (recommanded).
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
RV.PVOfflineTool.LSAdaptPV3DFitter.MinTracks = 4

#Default PV reconstruction
from Configurables import PatPVOffline
myPVLocation = "/Event/Rec/Vertex/myPVOffline"
myPVOffl = PatPVOffline("myPVOffl")
myPVOffl.OutputVertices = myPVLocation
#DVSeq.Members += [ myPVOffl ]
########################################################################

########################################################################
# Preselection of candidates
from Configurables import  RecVertices2Particles
RV2P = RecVertices2Particles( "RV2P" )
DVSeq.Members += [ RV2P ]

RV2P.RecVerticesLocation = ["Rec/Vertices/RV"]
#RV2P.RecVerticesLocation = [myPVLocation]
RV2P.OutputLevel = 3

#Selection Cuts
RV2P.Prey = prey       
RV2P.PreyMinSumpt = 4*GeV
RV2P.PreyMinMass = 4*GeV  
RV2P.RMin = 0.3*mm         #Min dist to z axis, D = 0.3 mm
RV2P.NbTracks = 4 
RV2P.IsolationDistance = 0 # D = 0
RV2P.RemVtxFromDet = 0*mm  #Switch off for tupling !
RV2P.RCutMethod = MethodRCut
RV2P.BeamLineLocation = BL.OutputLocation
########################################################################

########################################################################
# Create as many selections as you desire
from Configurables import DisplVertices
DV = DisplVertices( "DV" )
DVSeq.Members += [ DV ]
DV.InputLocations = [ "Phys/RV2P" ]
DV.RCutMethod = MethodRCut
DV.OutputLevel = 3
DV.Prey = prey
DV.MotherPrey = mother
DV.SaveTuple = False    
DV.SaveTrigInfos = False

#Selection Cuts
DV.NbTracks = 5
DV.PreyMinSumpt = 6*GeV
DV.PreyMinMass = 8*GeV
DV.RMin = 4*mm

########################################################################
from Configurables import DaVinci
DaVinci().EvtMax = nbevent
DaVinci().PrintFreq = 100
DaVinci().SkipEvents = 0
DaVinci().DataType = geom                       # Default is "DC06","2008"
DaVinci().Simulation   = True
DaVinci().HistogramFile = name + "_histo.root"    # Histogram file
DaVinci().TupleFile = name + ".root"    
DaVinci().UserAlgorithms = [DVSeq] 
DaVinci().Input = evt
##############################################################################







