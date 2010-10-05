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

__author__ = ['Neal Gauvin','Marcin Kucharczyk']
__date__ = '22/01/2010'
__version__ = '$Revision: 1.4 $'


from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from GaudiKernel.SystemOfUnits import *
from Configurables import BestPIDParticleMaker
from CommonParticles.Utils import *
from Configurables import PatPV3D,PVOfflineTool,PVSeed3DTool,LSAdaptPV3DFitter
from Configurables import RecVertices2Particles,DisplVertices,FilterDesktop
from StrippingConf.StrippingLine import StrippingLine

from PhysSelPython.Wrappers import Selection


VerticesLocation = "Rec/Vertices/DisplRV"
RCutMethod = "FromUpstreamPV"

################################################################
## Get reconstructed vertices from optimized PatPV3D
DisplPatPV3D = PatPV3D("DisplPatPV3D")
DisplPatPV3D.OutputVerticesName = VerticesLocation
DisplPatPV3D.addTool(PVOfflineTool)
#DisplPatPV3D.PVOfflineTool.InputVerticesName = "Rec/Vertex/Primary"
#DisplPatPV3D.PVOfflineTool.LookForDisplaced = True
DisplPatPV3D.PVOfflineTool.InputTracks = ["Rec/Track/Best"]
DisplPatPV3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
DisplPatPV3D.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
DisplPatPV3D.PVOfflineTool.addTool(PVSeed3DTool())
DisplPatPV3D.PVOfflineTool.PVSeed3DTool.TrackPairMaxDistance = 0.2*mm
DisplPatPV3D.PVOfflineTool.PVSeed3DTool.MinCloseTracks = 3
DisplPatPV3D.PVOfflineTool.PVSeed3DTool.zMaxSpread = 1*mm
DisplPatPV3D.PVOfflineTool.addTool(LSAdaptPV3DFitter())
DisplPatPV3D.PVOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 2*mm
DisplPatPV3D.PVOfflineTool.LSAdaptPV3DFitter.MinTracks = 4

################################################################
## Charged Particles with best PID
## This is not needed anymore. kept for some possible cross-checking.
## BestPIDParts = BestPIDParticleMaker("BestPIDChargedParts", Particle = 'pion' )
## # configure the track selector
## selector = trackSelector(BestPIDParts)
## selector.TrackTypes = ['Velo','Long','Upstream']

#####################################################################
## Preselection of candidates
## Turn RecVertices (candidates) into Particles
PreselDisplVertices = RecVertices2Particles("PreselDisplVertices")
#PreselDisplVertices.InputLocations = ["Phys/ChargedPartBest"]
#PreselDisplVertices.RecVerticesLocation = ["Rec/Vertex/Primary", "Rec/Vertices/DisplRV"]
PreselDisplVertices.RecVerticesLocation = [VerticesLocation]
PreselDisplVertices.RCutMethod = RCutMethod
PreselDisplVertices.PreyMinMass = 3.0*GeV
PreselDisplVertices.PreyMinSumpt = 3.0*GeV
PreselDisplVertices.RMin = 0.3*mm
PreselDisplVertices.NbTracks = 4 
PreselDisplVertices.RemVtxFromDet = 0
PreselDisplVertices.UsePartFromTES = False
PreselDisplVertices.TrackMaxChi2oNDOF = 10.

#####################################################################
## Single DisplVertex selection and line
SingleDV = DisplVertices("SingleDV")
SingleDV.InputLocations = [PreselDisplVertices.getName()]
SingleDV.MinNBCands = 1
SingleDV.RCutMethod = RCutMethod
SingleDV.RMin = 0.3*mm
SingleDV.NbTracks = 5
SingleDV.PreyMinMass = 6.2*GeV
SingleDV.PreyMinSumpt = 6.0*GeV
SingleDV.RemVtxFromDet = 0

#####################################################################
## Single DisplVertex selection and line
SingleDVHighMass = DisplVertices("SingleDVHighMass")
SingleDVHighMass.InputLocations = [PreselDisplVertices.getName()]
SingleDVHighMass.MinNBCands = 1
SingleDVHighMass.RCutMethod = RCutMethod
SingleDVHighMass.RMin = 0.3*mm
SingleDVHighMass.NbTracks = 5
SingleDVHighMass.PreyMinMass = 15.*GeV
SingleDVHighMass.PreyMinSumpt = 6.0*GeV
SingleDVHighMass.RemVtxFromDet = 0

#####################################################################
## Single DisplVertex selection and line
SingleDVHighPtMuon = DisplVertices("SingleDVHighPtMuon")
SingleDVHighPtMuon.InputLocations = [PreselDisplVertices.getName()]
SingleDVHighPtMuon.MinNBCands = 1
SingleDVHighPtMuon.RCutMethod = RCutMethod
SingleDVHighPtMuon.RMin = 0.3*mm
SingleDVHighPtMuon.NbTracks = 5
SingleDVHighPtMuon.PreyMinMass = 6.2*GeV
SingleDVHighPtMuon.PreyMinSumpt = 6.0*GeV
SingleDVHighPtMuon.RemVtxFromDet = 0
SingleDVHighPtMuon.MuonpT = 5.5*GeV

#####################################################################
## Double DisplVertex selection and line
DoubleDV = DisplVertices("DoubleDV")
DoubleDV.InputLocations = [PreselDisplVertices.getName()]
DoubleDV.MinNBCands = 2
DoubleDV.RCutMethod = RCutMethod
DoubleDV.RMin = 0.3*mm
DoubleDV.NbTracks = 4
DoubleDV.PreyMinMass = 3*GeV
DoubleDV.PreyMinSumpt = 3*GeV
DoubleDV.RemVtxFromDet = 0



################################################################
## Create line for single DisplVertex selection
line1 = StrippingLine('SingleDisplVtx'
                      , prescale = 0.1
                      , algos = [DisplPatPV3D,PreselDisplVertices,SingleDV]
                      ) 
line2 = StrippingLine('DoubleDisplVtx'
                      , prescale = 0.5
                      , algos = [DisplPatPV3D,PreselDisplVertices,DoubleDV]
                      )
line1highptmuon = StrippingLine('SingleDisplVtxHighPtMuon'
                      , prescale = 0.5
                      , algos = [DisplPatPV3D,PreselDisplVertices,SingleDVHighPtMuon]
                      )
line1highmass = StrippingLine('SingleDisplVtxHighMass'
                      , prescale = 1.
                      , algos = [DisplPatPV3D,PreselDisplVertices,SingleDVHighMass]
                      )

###############################################################
# Create line for double DisplVertex selection

###############################################################
# Create line for background study selection
## line3 = StrippingLine('IntwMat'
##              , prescale = 1.0
##              , algos = [DisplPatPV3D,PreselDisplVertices,IntwMat]
##              )

