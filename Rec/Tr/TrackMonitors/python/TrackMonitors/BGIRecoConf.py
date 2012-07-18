from Gaudi.Configuration import appendPostConfigAction
from LHCbKernel.Configuration import LHCbConfigurableUser
from Configurables import PVSplit

class BGIRecoConf(LHCbConfigurableUser):

  __slots__ = {
      "RecoVELOSeq"     : None                # The sequencer to modify      
    , "RecoVertexSeq"   : None                # The sequencer to modify
    
    # PV reconstruction options
    , "PV3DTuning"      : False               # custom parameters for PVSeed3DTool and LSAdaptPV3DFitter
    , "PVAlgorithm"     : "PatPV3D"           # PatPVOffline (def) or PatPV3D - no difference at all, but PatPVOffline is preconfigured with BeamSpotCut
    , "PVSeedTool"      : "PVSeed3DTool"      # PVSeed3DTool (def) (fixed for the moment)    
    , "PVFitterTool"    : "LSAdaptPV3DFitter" # LSAdaptPVFitter (def) or LSAdaptPV3DFitter
    , "InputTracks"     : "Rec/Track/Best"    #
    , "PrimaryVertices" : "Rec/Vertex/Best"   #
    
    # PVSplit options
    , "PVSplitName"     : "MyPVSplit"         #
    , "RandomShuffle"   : True                # Whether to shuffle tracks
    , "SplitMethod"     : "Middle"            # How to split track container
    , "SplitVertices"   : "Rec/Vertex/Split"  #
    }


  def __apply_configuration__(self):
    veloSeq = self.getProp("RecoVELOSeq")
    if veloSeq == None: raise RuntimeError("ERROR : RecoVELOSeq not set")

    vertexSeq = self.getProp("RecoVertexSeq")
    if vertexSeq == None: raise RuntimeError("ERROR : RecoVertexSeq not set")

    def action():
      alreadyModified = any([isinstance(alg, PVSplit)
                             for alg in vertexSeq.Members])
      if not alreadyModified:
        self._modify_velo_seq(veloSeq)
      self._modify_vertex_seq(vertexSeq, alreadyModified)

    appendPostConfigAction(action)


  def _modify_velo_seq(self, sequence):
    '''
    Modify RecoVertexSeq sequence.
    '''
    from Configurables import FastVeloTracking

    myFastVelo = FastVeloTracking("MyFastVelo")
    myFastVelo.ZVertexMin = -2000
    myFastVelo.ZVertexMax = 2000

    sequence.Members = [myFastVelo]


  def _configure_pvalg(self, pvAlgorithm):
    import Configurables
    from Configurables import PVOfflineTool
    
    pvSeedToolName = self.getProp("PVSeedTool")
    pvFitterToolName = self.getProp("PVFitterTool")

    pvAlgorithm.addTool(PVOfflineTool, 'PVOfflineTool')
    pvOfflineTool = pvAlgorithm.PVOfflineTool
    pvOfflineTool.RequireVelo = True
    pvOfflineTool.InputTracks = [self.getProp('InputTracks')]
    pvOfflineTool.PVSeedingName = pvSeedToolName
    pvOfflineTool.PVFitterName = pvFitterToolName

    pv3dTuning = self.getProp('PV3DTuning')
    
    if pvSeedToolName == 'PVSeed3DTool':
      from Configurables import PVSeed3DTool
      pvOfflineTool.addTool(PVSeed3DTool, name=pvSeedToolName)
      if pv3dTuning:
        pvOfflineTool.PVSeed3DTool.MinCloseTracks = 1.
        pvOfflineTool.PVSeed3DTool.TrackPairMaxDistance = 2.
        pvOfflineTool.PVSeed3DTool.zMaxSpread = 10.

    if pvFitterToolName == 'LSAdaptPV3DFitter':
      from Configurables import LSAdaptPV3DFitter
      pvOfflineTool.addTool(LSAdaptPV3DFitter, name=pvFitterToolName)
      if pv3dTuning:
        pvOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 4.
        pvOfflineTool.LSAdaptPV3DFitter.TrackErrorScaleFactor = 2.
        pvOfflineTool.LSAdaptPV3DFitter.MinTracks = 5


  def _modify_vertex_seq(self, sequence, alreadyModified=False):
    '''
    Modify RecoVertexSeq sequence.
    '''
    import Configurables
    
    if not alreadyModified:
      pvAlgorithmName = self.getProp("PVAlgorithm")
      PVAlgorithm = getattr(Configurables, pvAlgorithmName)
      # !!! Always create algorithms with non-default names to avoid getting
      # !!! an already-created-and-behind-your-back-configured algorithm.
      pvAlgorithm = PVAlgorithm('My' + pvAlgorithmName)
      outputPropertyName = {'PatPVOffline': 'OutputVertices',
                            'PatPV3D': 'OutputVerticesName'}[pvAlgorithmName]
      setattr(pvAlgorithm, outputPropertyName, self.getProp('PrimaryVertices'))
      self._configure_pvalg(pvAlgorithm)

    pvSplitAlg = Configurables.PVSplit(self.getProp('PVSplitName'))
    pvSplitAlg.InputVerticesLocation = self.getProp('PrimaryVertices')
    pvSplitAlg.OutputVerticesLocation = self.getProp('SplitVertices')
    pvSplitAlg.RandomShuffle = self.getProp('RandomShuffle')
    pvSplitAlg.SplitMethod = self.getProp('SplitMethod')
    self._configure_pvalg(pvSplitAlg)
#    pvSplitAlg.OutputLevel = DEBUG
#    pvSplitAlg.PVOfflineTool.OutputLevel = INFO

    if not alreadyModified:
      sequence.Members = [pvAlgorithm, pvSplitAlg]
    else:
      sequence.Members.append(pvSplitAlg)
