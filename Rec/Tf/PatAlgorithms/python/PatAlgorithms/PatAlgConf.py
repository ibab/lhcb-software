from TrackSys.Configuration import *
from Configurables import ( PatForward, PatForwardTool, PatFwdTool)
from Configurables import  (PatDownstream, Tf__OTHitCreator)
from Configurables import (PatSeeding, PatSeedingTool)


class ForwardConf(object):
  '''Configure a forward algorithm'''
  def configureAlg(self, PFAlg=PatForward("PatForward")):
    
    PFAlg.addTool( PatForwardTool,"PatForwardTool" )
    self.configureTool(PFAlg.PatForwardTool)
    
  def configureTool(self, PFTool):
    PFTool.AddTTClusterName = "PatAddTTCoord"
    PFTool.addTool( PatFwdTool("PatFwdTool"))
    
    if TrackSys().fieldOff():
      PFTool.PatFwdTool.withoutBField  = True
      PFTool.WithoutBField = True
  
#class DownstreamConf(object):
#  '''Configure a downstream algorithm'''
#  def configureAlg(self, PDAlg=PatDownstream("PatDownstream")):
    

class SeedingConf(object):
  '''Configure a seeding algorithm'''
  def configureAlg(self, SeedAlg=PatSeeding("PatSeeding")):
    SeedAlg.addTool(PatSeedingTool, name="PatSeedingTool")
    self.configureTool(SeedAlg.PatSeedingTool)
    
  def configureTool(self, SeedTool):
    if TrackSys().fieldOff():
      SeedTool.xMagTol = 4e2
      SeedTool.zMagnet = 0.
      SeedTool.FieldOff = True
      SeedTool.MinMomentum = 5e4
    

class CosmicConf(object):
  '''Configure for cosmics'''
  #### Obsolete I think!! #####
  def configureAlg(self, SeedAlg=PatSeeding("PatSeeding")):
    SeedAlg.addTool(PatSeedingTool, name="PatSeedingTool")
    self.configureTool(SeedAlg.PatSeedingTool)
        
  def configureTool(self, SeedTool, hitCreator=Tf__OTHitCreator("ToolSvc.OTHitCreator")):
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
