from TrackSys.Configuration import *
from Configurables import ( PatForward, PatForwardTool, PatFwdTool)
from Configurables import  (PatDownstream, Tf__OTHitCreator)
from Configurables import (PatSeeding, PatSeedingTool)

class ForwardConf(object):
  '''Configure a forward algorithm'''
  def configureAlg(self, PFAlg=PatForward("PatForward")):
    
    PFAlg.addTool( PatForwardTool,"PatForwardTool" )
    self.configureTool(PFAlg.PatForwardTool)
    globalCuts = TrackSys().getProp("GlobalCuts")
    if("IT" in globalCuts): PFAlg.maxITHits = globalCuts["IT"]
    if("OT" in globalCuts): PFAlg.maxOTHits = globalCuts["OT"]
    # Remove velo track cut if hits already suppressed in Velo decoding
    if("Velo" in globalCuts): PFAlg.MaxNVelo = 999999
    
  def configureTool(self, PFTool):
    PFTool.AddTTClusterName = "PatAddTTCoord"
    PFTool.addTool( PatFwdTool("PatFwdTool"))
    
    if TrackSys().fieldOff():
      PFTool.PatFwdTool.withoutBField  = True
      PFTool.WithoutBField = True

  def configureAlgRun2HLT1(self, PFAlg = PatForward("PatForwardHLT1")):
    '''Configure forward algorithm for HLT1 stage of run II offline tracking'''
    
    PFAlg.InputTracksName =  "Rec/Track/VeloTT"
    PFAlg.OutputTracksName = "Rec/Track/ForwardHLT1"
    PFAlg.ForwardToolName = "PatForwardTool/PatForwardToolHLT1"
    PFAlg.addTool( PatForwardTool("PatForwardToolHLT1") ) 

    PFAlg.PatForwardToolHLT1.MinPt = 500.
    PFAlg.PatForwardToolHLT1.MinMomentum = 3000.
    PFAlg.PatForwardToolHLT1.SecondLoop = False
    PFAlg.PatForwardToolHLT1.UseMomentumEstimate = True
    PFAlg.PatForwardToolHLT1.UseWrongSignWindow = True
    PFAlg.PatForwardToolHLT1.WrongSignPT = 2000.
    PFAlg.PatForwardToolHLT1.PreselectionPT = 0.8*500.
    PFAlg.PatForwardToolHLT1.Preselection = True   
    PFAlg.PatForwardToolHLT1.AddTTClusterName = "PatAddTTCoord"

    globalCuts = TrackSys().getProp("GlobalCuts")
    if("IT" in globalCuts): PFAlg.maxITHits = globalCuts["IT"]
    if("OT" in globalCuts): PFAlg.maxOTHits = globalCuts["OT"]
    # Remove velo track cut if hits already suppressed in Velo decoding
    if("Velo" in globalCuts): PFAlg.MaxNVelo = 999999

  def configureAlgRun2HLT2(self, PFAlg = PatForward("PatForwardHLT2")):
    '''Configure forward algorithm for HLT2 stage of run II offline tracking'''
    PFAlg.ForwardToolName = "PatForwardTool/PatForwardToolHLT2"
    PFAlg.addTool(PatForwardTool, name = "PatForwardToolHLT2")
    PFAlg.OutputTracksName = "Rec/Track/ForwardHLT2"
    #PFAlg.PatForwardToolHLT1.MinPt = 500.
    #PFAlg.PatForwardToolHLT1.MinMomentum = 3000.
    PFAlg.PatForwardToolHLT2.SecondLoop = True
    
    
#class DownstreamConf(object):
#  '''Configure a downstream algorithm'''
#  def configureAlg(self, PDAlg=PatDownstream("PatDownstream")):
    

class SeedingConf(object):
  '''Configure a seeding algorithm'''
  def configureAlg(self, SeedAlg=PatSeeding("PatSeeding")):
    SeedAlg.addTool(PatSeedingTool, name="PatSeedingTool")
    self.configureTool(SeedAlg.PatSeedingTool)
    
  def configureTool(self, SeedTool):
    if "earlyData"  in TrackSys().getProp("SpecialData"):
      SeedTool.MaxRangeIT = 35. # mm
      SeedTool.TolCollectOT = 4.0 # mm
      SeedTool.TolCollectIT = 0.6 # mm
      SeedTool.MaxChi2HitOT = 56.25
      SeedTool.MaxChi2HitIT = 36.0
      SeedTool.MinXPlanes = 4
      SeedTool.MinTotalPlanes = 8
      SeedTool.OTNHitsLowThresh = 15
      SeedTool.MaxMisses = 2
      SeedTool.MaxHoles = 4
    if TrackSys().fieldOff():
      SeedTool.xMagTol = 4e2
      SeedTool.zMagnet = 0.
      SeedTool.FieldOff = True
      SeedTool.MinMomentum = 5e4
    globalCuts = TrackSys().getProp("GlobalCuts")
    if("IT" in globalCuts): SeedTool.MaxITHits = globalCuts["IT"]
    if("OT" in globalCuts): SeedTool.MaxOTHits = globalCuts["OT"]

class CosmicConf(object):
  '''Configure for cosmics'''
  #### Obsolete I think!! #####
  def configureAlg(self, SeedAlg=PatSeeding("PatSeeding")):
    SeedAlg.addTool(PatSeedingTool, name="PatSeedingTool")
    self.configureTool(SeedAlg.PatSeedingTool)
        
  def configureTool(self, SeedTool, hitCreator=Tf__OTHitCreator("ToolSvc.OTHitCreator")):
    if "earlyData"  in TrackSys().getProp("SpecialData"):
      SeedTool.MaxRangeIT = 35. # mm
      SeedTool.TolCollectOT = 4.0 # mm
      SeedTool.TolCollectIT = 0.6 # mm
      SeedTool.MaxChi2HitOT = 56.25
      SeedTool.MaxChi2HitIT = 36.0
      SeedTool.MinXPlanes = 4
      SeedTool.MinTotalPlanes = 8
      SeedTool.OTNHitsLowThresh = 15
      SeedTool.MaxMisses = 2
      SeedTool.MaxHoles = 4
    #no magnetic field
    #options to tune PatSeeding for tracking with B field off
    SeedTool.xMagTol = 4e2
    SeedTool.zMagnet = 0.
    SeedTool.FieldOff = True
    SeedTool.MinMomentum = 5e4
    #relax requirements on number of hits/planes
    #(this may need tweaking if some C frames are in "open" position)
    SeedTool.MinXPlanes = 4
    SeedTool.MinTotalPlanes = 8
    SeedTool.OTNHitsLowThresh = 9
    SeedTool.MaxMisses = 2
    #relax pointing requirement (no PV for cosmics!)
    SeedTool.MaxYAtOrigin = 4e5
    SeedTool.MaxYAtOriginLowQual = 8e5
    SeedTool.xMagTol = 4e5
    SeedTool.MinMomentum = 1e-4 #pointing requirement in disguise!
    #cosmic reconstruction
    SeedTool.Cosmics = True
    #detector not yet aligned - chi^2 should not contribute to track quality
    SeedTool.QualityWeights        = [ 1.0, 0.0 ]
    
    #depending on misalignments/usage of IT, one may need to tweak
    #the chi^2 cuts and/or CurveTol, TolCollect{IT,OT} or MaxRange{IT,OT}
    #(this is not an exhaustive list, it is just meant as a hint where to
    #look)
    
    #switch off drift times for the pattern recognition
    hitCreator.NoDriftTimes = True
