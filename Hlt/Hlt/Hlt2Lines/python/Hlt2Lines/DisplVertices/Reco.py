from HltLine.HltLine import bindMembers
from Hlt2Lines.Utilities.Hlt2Stage import Hlt2Stage

from HltTracking.HltPVs import PV3D

import GaudiKernel.SystemOfUnits as units

linePrefix = ""

## TODO: move to Utils/Hlt2Tracks.py
class Hlt2VeloTracks(Hlt2Stage):
    def __init__(self, name="Hlt2VeloTracks"):
        super(Hlt2VeloTracks, self).__init__(name, inputs=[])

    def clone(self, name):
        return Hlt2VeloTracks(name)

    def stage(self, cuts):
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        return Hlt2BiKalmanFittedForwardTracking().hlt2VeloTracking()

class VeloVertexFinder(Hlt2Stage):
    def __init__(self, prefix=linePrefix, name="VeloVertexFinder", dependencies=[], **kwargs):
        self.__prefix = prefix
        self.__kwargs = kwargs
        self.outrv = "Rec/Vertex/{0}Velo".format(linePrefix)
        dependencies = dependencies + [ PV3D("Hlt2"), Hlt2VeloTracks() ]
        super(VeloVertexFinder, self).__init__( name, inputs=[], dependencies=dependencies )

    def clone(self, name, **kwargs):
        args = deepcopy(self.__kwargs)
        args["name"] = name
        for arg, default in (("prefix", self.__prefix),):
            args[arg] = kwargs.pop(arg) if arg in kwargs else default
        args.update(kwargs)
        return VeloVertexFinder(**args)

    def stage(self, cuts):
        algs = []
        from HltTracking.HltTrackNames import HltSharedTrackLoc
        tracksForVertexing = HltSharedTrackLoc["Velo"]
        ## Step 1 : apply Velo track selection (if required)
        filterCuts = cuts.get("FilterVelo")
        if filterCuts["Apply"]:
            from Configurables import SelectVeloTracksNotFromPV
            vWithIP = SelectVeloTracksNotFromPV("Hlt2"+self.__prefix+self._name()+"NonPromptVelo",
                        PVLocation=PV3D("Hlt2").output,
                        MinIP                = filterCuts["MinIP"],
                        MinIPChi2            = filterCuts["MinIPChi2"],
                        RejectSplashEvents   = filterCuts["RejectSplash"],
                        RemoveBackwardTracks = filterCuts["RejectBackward"],
                        MinNumTracks         = filterCuts["MinNumTracks"],
                        MaxNumInputTracks    = filterCuts["MaxNumInput"],
                        MaxNumOutputTracks   = filterCuts["MaxNumOutput"]
                        )
            vWithIP.Inputs = [ tracksForVertexing ]
            tracksForVertexing = "Hlt/Track/{0}{1}".format(self.__prefix, self._name())
            vWithIP.Output = tracksForVertexing
            algs.append(vWithIP)

        ## Step 2 : run the vertex finding
        from Configurables import PVOfflineTool, PatPV3D, PVSeed3DTool, LSAdaptPV3DFitter
        vf = PVOfflineTool("Hlt2"+self.__prefix+self._name()+"VeloOfflineTool"
                          , InputTracks   = [tracksForVertexing]
                          , PVSeedingName = "PVSeed3DTool"
                          , PVFitterName  = "LSAdaptPV3DFitter"
                          )
        vf.addTool(PVSeed3DTool)
        vf.PVSeed3DTool.TrackPairMaxDistance = 0.2*units.mm
        vf.PVSeed3DTool.zMaxSpread           = 1.0*units.mm
        vf.PVSeed3DTool.MinCloseTracks       = 3
        vf.addTool(LSAdaptPV3DFitter)
        vf.LSAdaptPV3DFitter.maxChi2         = 400.  ## default value, to be tuned
        vf.LSAdaptPV3DFitter.maxRDPV         = 1000. ## do not veto highly displaced vertices
        vf.LSAdaptPV3DFitter.MinTracks       = 4

        vx = PatPV3D("Hlt2"+self.__prefix+self._name()+"3DVelo", OutputVerticesName=self.outrv)
        vx.addTool(vf, name="PVOfflineTool")
        algs.append(vx)

        return bindMembers(None, self.dependencies(cuts) + self.inputStages(cuts) + algs)
