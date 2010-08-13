"""
Module for the selection of events with displaced vertices.

The selection of displaced vertices is achieved in 3 steps :
      - Reconstruction of all vertices with >= 4 tracks
        with PatPV3D, with optimized cuts for smaller vertices
        than PV's.
      - Hlt2PreSelDV loops on all reconstructed vertices.
        The one with lowest z (upstream) is not considered
        Vertices with at least one backward track are not considered.
        Vertices close to the beam line removed.
        To allow for kinematical cuts, association is done between the Velo
        tracks participating to the vertex and the best Particles
        (from Rec/Track/Best) with best PID.
        Vertices that are found to be close to the detector material
        could be possibly eliminated.
      - Hlt2SelDV : basic cuts on the selected candidates
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
__version__ = '$Revision: 1.3 $'


from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from GaudiKernel.SystemOfUnits import *
from Configurables import BestPIDParticleMaker
from CommonParticles.Utils import *
from Configurables import PatPV3D,PVOfflineTool,PVSeed3DTool,LSAdaptPV3DFitter
from Configurables import RecVertices2Particles,DisplVertices
from StrippingConf.StrippingLine import StrippingLine



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
PreselDisplVertices.PreyMinMass = 2.0*GeV
PreselDisplVertices.RMin = 0.3*mm
PreselDisplVertices.NbTracks = 4 
PreselDisplVertices.RemVtxFromDet = 0
PreselDisplVertices.UsePartFromTES = False

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
## Double DisplVertex selection and line
DoubleDV = DisplVertices("DoubleDV")
DoubleDV.InputLocations = [PreselDisplVertices.getName()]
DoubleDV.MinNBCands = 2
DoubleDV.RCutMethod = RCutMethod
DoubleDV.RMin = 0.3*mm
DoubleDV.NbTracks = 4
DoubleDV.PreyMinMass = 2*GeV
DoubleDV.PreyMinSumpt = 1.5*GeV
DoubleDV.RemVtxFromDet = 0

#####################################################################
## Line for the study of interaction with matter
## Note : it is presently performed by the SingleDisplVtx and
##        DoubleDisplVtx lines.
##        The idea is to have something like the SingleDisplVtx
##        line, but with looser cuts, larger RMin and pre-scaled.
IntwMat = DisplVertices("IntWithMat")
IntwMat.InputLocations = [PreselDisplVertices.getName()]
IntwMat.RCutMethod = RCutMethod
IntwMat.RMin = 3*mm
IntwMat.NbTracks = 5
IntwMat.PreyMinMass = 4*GeV
IntwMat.PreyMinSumpt = 2*GeV
IntwMat.RemVtxFromDet = 0


################################################################
# Create line for single DisplVertex selection
line1 = StrippingLine('SingleDisplVtx'
                      , prescale = 1.0
                      , algos = [DisplPatPV3D,PreselDisplVertices,SingleDV]
                      )
#
###############################################################
# Create line for double DisplVertex selection
line2 = StrippingLine('DoubleDisplVtx'
                      , prescale = 1.0
                      , algos = [DisplPatPV3D,PreselDisplVertices,DoubleDV]
                      )

###############################################################
# Create line for background study selection
## line3 = StrippingLine('IntwMat'
##              , prescale = 1.0
##              , algos = [DisplPatPV3D,PreselDisplVertices,IntwMat]
##              )




