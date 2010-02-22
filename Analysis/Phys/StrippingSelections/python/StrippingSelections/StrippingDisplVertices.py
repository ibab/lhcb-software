## #####################################################################
# Stripping selection for displaced vertices
#
# @authors: Marcin Kucharczyk & Neal Gauvin
# @date: 2010-01-22
# 
## #####################################################################
from os import environ, path
from math import *
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from GaudiKernel.SystemOfUnits import *
from Configurables import BestPIDParticleMaker
from CommonParticles.Utils import *
from Configurables import PatPV3D,PVOfflineTool,PVSeed3DTool,LSAdaptPV3DFitter
from Configurables import RecVertices2Particles,DisplVertices
from StrippingConf.StrippingLine import StrippingLine
## Charged pions
ChargedPion = BestPIDParticleMaker("ChargedPartBest", Particle = 'pion' )
selector = trackSelector(ChargedPion)
selector.TrackTypes = ['Velo','Long','Upstream']
## PatPV3D
DisplPatPV3D = PatPV3D("DisplPatPV3D")
DisplPatPV3D.OutputVerticesName = "Rec/Vertices/DisplRV"
DisplPatPV3D.addTool(PVOfflineTool)
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
## Preselection
PreselDisplVertices = RecVertices2Particles("PreselDisplVertices")
PreselDisplVertices.InputLocations = ["Phys/ChargedPartBest"]
PreselDisplVertices.RecVerticesLocation = ["Rec/Vertices/DisplRV"]
PreselDisplVertices.PreyMinMass = 1.0*GeV
PreselDisplVertices.RMin = 0.3*mm
PreselDisplVertices.NbTracks = 3 
PreselDisplVertices.IsolationDistance = 0
PreselDisplVertices.KeepLowestZ = False
PreselDisplVertices.RemVtxFromDet = 0*mm
PreselDisplVertices.RCutMethod = "FromUpstreamPV"
## Single DisplVertex selection
SingleDV = DisplVertices("SingleDV")
SingleDV.InputLocations = ["Phys/PreselDisplVertices"]
SingleDV.RCutMethod = "FromUpstreamPV"
SingleDV.MC = False
SingleDV.SaveTuple = False
SingleDV.SaveTrigInfos = False
SingleDV.RemVtxFromDet = 0*mm
SingleDV.NbTracks = 5
SingleDV.PreyMinMass = 6.2*GeV
SingleDV.PreyMinSumpt = 6.0*GeV
SingleDV.RMin = 0.3*mm
## Double DisplVertex selection
DoubleDV = DisplVertices("DoubleDV")
DoubleDV.InputLocations = ["Phys/PreselDisplVertices"]
DoubleDV.RCutMethod = "FromUpstreamPV"
DoubleDV.MC = False
DoubleDV.SaveTuple = False
DoubleDV.SaveTrigInfos = False
DoubleDV.RemVtxFromDet = 0*mm
DoubleDV.MinNBCands = 2
DoubleDV.NbTracks = 4
DoubleDV.PreyMinMass = 3.5*GeV
DoubleDV.PreyMinSumpt = 2.5*GeV
DoubleDV.RMin = 0.3*mm
## Background study selection
SelBckgrd = DisplVertices("SelBckgrd")
SelBckgrd.InputLocations = ["Phys/PreselDisplVertices"]
SelBckgrd.RCutMethod = "FromUpstreamPV"
SelBckgrd.MC = False
SelBckgrd.SaveTuple = False
SelBckgrd.SaveTrigInfos = False
SelBckgrd.NbTracks = 5
SelBckgrd.PreyMinMass = 6*GeV
SelBckgrd.PreyMinSumpt = 5*GeV
SelBckgrd.RMin = 3*mm

# Create line for single DisplVertex selection
line1 = StrippingLine('SingleDisplVtx'
             , prescale = 1.0
             , algos = [ChargedPion,DisplPatPV3D,PreselDisplVertices,SingleDV]
             )
# Create line for double DisplVertex selection
line2 = StrippingLine('DoubleDisplVtx'
             , prescale = 1.0
             , algos = [ChargedPion,DisplPatPV3D,PreselDisplVertices,DoubleDV]
             )
# Create line for background study selection
line3 = StrippingLine('BckgrdDisplVtx'
             , prescale = 1.0
             , algos = [ChargedPion,DisplPatPV3D,PreselDisplVertices,SelBckgrd]
             )




