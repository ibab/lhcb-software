from TrackSys.Configuration import *
from Configurables import ( PatForward, PatForwardTool, PatFwdTool, TrackUsedLHCbID)
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

  def configureGEC(self, PFAlg=PatForward("PatForward")):
    globalCuts = TrackSys().getProp("GlobalCuts")
    if("IT" in globalCuts): PFAlg.maxITHits = globalCuts["IT"]
    if("OT" in globalCuts): PFAlg.maxOTHits = globalCuts["OT"]
    # Remove velo track cut if hits already suppressed in Velo decoding
    if("Velo" in globalCuts): PFAlg.MaxNVelo = 999999
    
  def configureAlgRun2HLT1(self, PFAlg = PatForward("PatForwardHLT1")):
    '''Configure forward algorithm for HLT1 stage of run II offline tracking'''
    self.configureGEC(PFAlg)
    PFAlg.InputTracksName =  "Rec/Track/VeloTT"
    PFAlg.OutputTracksName = "Rec/Track/ForwardHLT1"
    PFAlg.addTool( PatForwardTool ) 

    PFAlg.PatForwardTool.MinPt = 500.
    PFAlg.PatForwardTool.MinMomentum = 3000.    
    PFAlg.PatForwardTool.UseMomentumEstimate = True
    PFAlg.PatForwardTool.UseWrongSignWindow = True
    PFAlg.PatForwardTool.WrongSignPT = 2000.
    PFAlg.PatForwardTool.PreselectionPT = 0.8*500.
    PFAlg.PatForwardTool.Preselection = True
    PFAlg.PatForwardTool.AddTTClusterName = "PatAddTTCoord"
    # optimized settings for 2016 Data taking
    PFAlg.PatForwardTool.SecondLoop          = False
    PFAlg.PatForwardTool.MaxXCandidateSize   = [50,24]
    PFAlg.PatForwardTool.MaxSpreadX          = 0.65
    PFAlg.PatForwardTool.MaxSpreadSlopeX     = 0.0075
    PFAlg.PatForwardTool.MaxChi2X            = 32
    PFAlg.PatForwardTool.MaxChi2Y            = 24
    PFAlg.PatForwardTool.NNAfterStereoFit    = True
    PFAlg.PatForwardTool.NNAfterStereoFitCut = 0.1
    PFAlg.PatForwardTool.NextBestResponse    = 0.7
    

  def configureAlgRun2HLT2(self, PFAlg = PatForward("PatForwardHLT2")):
    '''Configure forward algorithm for HLT2 stage of run II offline tracking'''
    self.configureGEC(PFAlg)
    PFAlg.addTool(PatForwardTool)
    PFAlg.OutputTracksName = "Rec/Track/ForwardHLT2"
    PFAlg.PatForwardTool.MinMomentum = 1000.    
    PFAlg.PatForwardTool.AddTTClusterName = "PatAddTTCoord"
    PFAlg.PatForwardTool.addTool( TrackUsedLHCbID, name = "TrackUsedLHCbID")
    PFAlg.PatForwardTool.UsedLHCbIDToolName = "TrackUsedLHCbID"
    PFAlg.PatForwardTool.TrackUsedLHCbID.inputContainers = ["Rec/Track/FittedHLT1ForwardTracks"]
    PFAlg.PatForwardTool.VeloVetoTracksNames = [ "Rec/Track/FittedHLT1ForwardTracks"]
    # optimized settings for 2016 Data taking
    # for a faster algorithm switch off SecondLoop and NNBXF (Scenario A)
    PFAlg.PatForwardTool.SecondLoop          = True
    PFAlg.PatForwardTool.MaxXCandidateSize   = [70,26,12]
    PFAlg.PatForwardTool.MaxSpreadX          = 1.5
    PFAlg.PatForwardTool.MaxSpreadSlopeX     = 0.007
    PFAlg.PatForwardTool.MaxChi2X            = 52
    PFAlg.PatForwardTool.MaxChi2Y            = 42
    PFAlg.PatForwardTool.NNBeforeXFit        = True
    PFAlg.PatForwardTool.NNAfterStereoFit    = True
    PFAlg.PatForwardTool.NNBeforeXFitCut     = 0.12
    PFAlg.PatForwardTool.NNAfterStereoFitCut = 0.09
    PFAlg.PatForwardTool.NextBestResponse    = 0.75
  
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
