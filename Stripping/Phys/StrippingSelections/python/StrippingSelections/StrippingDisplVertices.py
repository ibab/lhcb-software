"""
Module for the selection of events with displaced vertices.

The selection of displaced vertices is achieved in 3 steps :
      - Reconstruction of all vertices with >= 4 tracks
        with PatPV3D, with optimized cuts for smaller vertices
        than PV's.
      - RecVertices2Particles loops on all reconstructed vertices.
        The one with lowest z (upstream) is not considered
        Vertices with at least one backward track are not considered.
        Vertices close to the beam line removed.
        To allow for kinematical cuts, association is done between the Velo
        tracks participating to the vertex and the best Particles
        (from Rec/Track/Best) with best PID.
        Vertices that are found to be close to the detector material
        could be possibly eliminated.
      - DisplVertices : basic cuts on the selected candidates
        2 kinds of cuts are applied to the set of displaced vertices :
           >=1 prey passing thighter cuts
               --> when hunting single long-lived particles
               --> Achieved in SingleDisplVtx
           >=2 preys passing looser cuts
               --> when looking for 2 particles, coming from a mother one.
               --> Achieved in DoubleDisplVtx

"""

__author__ = ['Neal Gauvin','Marcin Kucharczyk', 'Victor Coco']
__date__ = '25/10/2010'
__version__ = '$Revision: 2.0 $'


from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from GaudiKernel.SystemOfUnits import *
from Configurables import BestPIDParticleMaker
from CommonParticles.Utils import *
from Configurables import PatPV3D,PVOfflineTool,PVSeed3DTool,LSAdaptPV3DFitter
from Configurables import RecVertices2Particles,DisplVertices,FilterDesktop,LoKi__VoidFilter
from StrippingConf.StrippingLine import StrippingLine

from PhysSelPython.Wrappers import Selection


VerticesLocation = "Rec/Vertices/DisplRV"
RCutMethod = "FromBeamLine" 
RCutMethodInfo = "FromPreyInfo" 


from Configurables import CalibrateIP
BL = CalibrateIP( "BeamLine" )
BL.OutputLocation = "/Event/BeamLine"
BL.OutputLevel = 3
BL.BeamLineCycle = 100
BL.MinZ = -400.
BL.MaxZ = 400.
BL.MaxY = 1.5
BL.MaxX = 1.5


DisplPatPV3D = PatPV3D("DisplPatPV3D")
DisplPatPV3D.OutputVerticesName = VerticesLocation
DisplPatPV3D.addTool(PVOfflineTool)
DisplPatPV3D.PVOfflineTool.InputTracks = ["Rec/Track/Best"]
DisplPatPV3D.PVOfflineTool.PVsChi2Separation = 0
DisplPatPV3D.PVOfflineTool.PVsChi2SeparationLowMult = 0
DisplPatPV3D.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
DisplPatPV3D.PVOfflineTool.addTool(PVSeed3DTool())
DisplPatPV3D.PVOfflineTool.PVSeed3DTool.MinCloseTracks = 3
DisplPatPV3D.PVOfflineTool.addTool( LSAdaptPV3DFitter())
DisplPatPV3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
DisplPatPV3D.PVOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 2*mm
DisplPatPV3D.PVOfflineTool.LSAdaptPV3DFitter.MinTracks = 4


PreselDisplVertices = RecVertices2Particles("PreselDisplVertices")
PreselDisplVertices.RecVerticesLocation = [VerticesLocation]
PreselDisplVertices.RCutMethod = RCutMethod
PreselDisplVertices.BeamLineLocation="/Event/BeamLine"
PreselDisplVertices.PreyMinMass = 3.0*GeV
PreselDisplVertices.PreyMinSumpt = 3.0*GeV
PreselDisplVertices.RMin = 0.3*mm
PreselDisplVertices.NbTracks = 4 
PreselDisplVertices.KeepLowestZ = True
PreselDisplVertices.RemVtxFromDet = 5
PreselDisplVertices.UsePartFromTES = False
PreselDisplVertices.TrackMaxChi2oNDOF = 5.

## #####################################################################
## ## Single DisplVertex selection and line
SingleDV = DisplVertices("SingleDV")
SingleDV.InputLocations = [PreselDisplVertices.getName()]
SingleDV.MinNBCands = 1
SingleDV.RCutMethod = RCutMethodInfo
SingleDV.BeamLineLocation="/Event/BeamLine"
SingleDV.RMin = 0.3*mm
SingleDV.NbTracks = 5
SingleDV.PreyMinMass = 5.*GeV
SingleDV.PreyMinSumpt = 5.0*GeV
SingleDV.RemVtxFromDet = 0

SingleDVLowMass = DisplVertices("SingleDVLowMass")
SingleDVLowMass.InputLocations = [PreselDisplVertices.getName()]
SingleDVLowMass.MinNBCands = 1
SingleDVLowMass.RCutMethod = RCutMethodInfo
SingleDVLowMass.BeamLineLocation="/Event/BeamLine"
SingleDVLowMass.RMin = 0.3*mm
SingleDVLowMass.NbTracks = 6
SingleDVLowMass.PreyMinSumpt = 6.0*GeV
SingleDVLowMass.PreyMinMass = 6.8*GeV
SingleDVLowMass.PreyMaxMass = 12.*GeV
SingleDVLowMass.RemVtxFromDet = 5

## #####################################################################
## ## Single DisplVertex selection and line
SingleDVHighMass = DisplVertices("SingleDVHighMass")
SingleDVHighMass.InputLocations = [PreselDisplVertices.getName()]
SingleDVHighMass.MinNBCands = 1
SingleDVHighMass.RCutMethod = RCutMethodInfo
SingleDVHighMass.BeamLineLocation="/Event/BeamLine"
SingleDVHighMass.RMin = 0.3*mm
SingleDVHighMass.NbTracks = 5
SingleDVHighMass.PreyMinMass = 12.*GeV
SingleDVHighMass.PreyMinSumpt = 6.0*GeV
SingleDVHighMass.RemVtxFromDet = 0


## #####################################################################
## ## Double DisplVertex selection and line
DoubleDV = DisplVertices("DoubleDV")
DoubleDV.InputLocations = [PreselDisplVertices.getName()]
DoubleDV.MinNBCands = 2
DoubleDV.RCutMethod = RCutMethodInfo
DoubleDV.BeamLineLocation="/Event/BeamLine"
DoubleDV.RMin = 0.3*mm
DoubleDV.NbTracks = 4
DoubleDV.PreyMinMass = 3*GeV
DoubleDV.PreyMinSumpt = 3*GeV
DoubleDV.RemVtxFromDet = 0



line1 = StrippingLine('SingleDisplVtx'
                         , prescale = 0.05
                         , algos = [BL,DisplPatPV3D,PreselDisplVertices,SingleDV]
                         ) 

line1LowMass = StrippingLine('SingleDisplVtxLowMass'
                      , prescale = 1.
                      , algos = [BL,DisplPatPV3D,PreselDisplVertices,SingleDVLowMass])

line1HighMass = StrippingLine('SingleDisplVtxHighMass'
                      , prescale = 1.
                      , algos = [BL,DisplPatPV3D,PreselDisplVertices,SingleDVHighMass])
                      
line2 = StrippingLine('DoubleDisplVtx'
                      , prescale = 1.
                      , algos = [BL,DisplPatPV3D,PreselDisplVertices,DoubleDV]
                      )

