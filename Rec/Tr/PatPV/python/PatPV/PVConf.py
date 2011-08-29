
from Configurables import PatPVOffline, PVSeed3DOfflineTool, PVSeedTool
from Configurables import PVOfflineTool, SimplePVSeedTool, SimplePVFitter, LSAdaptPVFitter

class LoosePV(object):
    '''Simple python class to configure loose PV settings for reconstruction''' 
    def configureAlg(self, PVAlg = PatPVOffline("PatPVOffline")):
        '''pass in an alg to configure'''
        
        PVAlg.addTool(PVOfflineTool, "PVOfflineTool")
        
        self.configureTool(PVAlg.PVOfflineTool)
    
    def configureTool(self, PVTool):
        '''pass in a tool method to configure'''
        PVTool.addTool(PVSeedTool, "PVSeedTool")
        PVTool.PVSeedTool.minClusterMult = 2
        PVTool.PVSeedTool.minCloseTracksInCluster = 2
        PVTool.PVSeedTool.ratioSig2HighMult = 1
        PVTool.PVSeedTool.ratioSig2LowMult =1
        
        PVTool.addTool(LSAdaptPVFitter,'LSAdaptPVFitter')
        PVTool.LSAdaptPVFitter.MinTracks = 2
        

class VLoosePV(object):
    '''Simple python class to configure Very loose PV settings for reconstruction'''
    def configureAlg(self, PVAlg = PatPVOffline("PatPVOffline")):
        '''pass in an alg to configure'''
        
        PVAlg.addTool(PVOfflineTool, "PVOfflineTool")
        
        self.configureTool(PVAlg.PVOfflineTool)
    
    def configureTool(self, PVTool):
        '''pass in a tool method to configure'''
        PVTool.RequireVelo = False
        PVTool.PVFitterName = "LSAdaptPVFitter"
        PVTool.PVSeedingName = "PVSeed3DOfflineTool"
        
        PVTool.addTool(PVSeed3DOfflineTool,"PVSeed3DOfflineTool")
        
        PVTool.addTool(LSAdaptPVFitter,'LSAdaptPVFitter')
        PVTool.LSAdaptPVFitter.MinTracks = 2
        PVTool.LSAdaptPVFitter.MinTracks    = 3
        PVTool.LSAdaptPVFitter.acceptTrack  = 0.000000000001
        PVTool.LSAdaptPVFitter.trackMaxChi2 = 25.
        PVTool.LSAdaptPVFitter.minIter      = 3
        
class StandardPV(object):
    '''Simple python class to configure Standard PV settings for reconstruction'''
    def configureAlg(self, PVAlg = PatPVOffline("PatPVOffline")):
        '''pass in an alg to configure'''
        
        PVAlg.addTool(PVOfflineTool, "PVOfflineTool")
        
        self.configureTool(PVAlg.PVOfflineTool)
    
    def configureTool(self, PVTool):
        '''pass in a tool method to configure'''
        PVTool.UseBeamSpotRCut = True
        PVTool.BeamSpotRCut = 0.3

