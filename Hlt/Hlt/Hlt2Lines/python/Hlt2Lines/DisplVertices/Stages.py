from HltLine.HltLine import bindMembers
from Hlt2Lines.Utilities.Hlt2Stage import Hlt2Stage
from Hlt2Lines.Utilities.Hlt2TisTosFilter import Hlt2TisTosStage
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner

from HltTracking.HltPVs import PV3D
from Inputs import BiKalmanFittedPions
from Inputs import BiKalmanFittedDownPions
from Reco import VeloVertexFinder

linePrefix = "NDisplVertices"
LLPLHCbName = "~chi_10"

SingleCuts = [ "MinNumTracks", "MinRho", "MinMass", "MinSumPT", "ApplyMatterVeto", "MaxFractE1Track", "MaxFractTrwHitBefore" ]
DoubleResonanceCuts = [ "MinHighestMass", "ApplyMatterVetoOne" ]
RecoCuts = [ "MaxChi2NonVeloOnly", "UseVeloTracks", "ComputeMatterVeto" ]

LLPSelectionPreambulo = [
      "from LoKiPhys.decorators import *"
    , "from LoKiTracks.decorators import *"
    , "INMATTER                           = ( INFO( 51, -1000.0 ) > 0.5 )"
    , "ENDVERTEXRHO                       =   INFO( 52, -1000.0 )"
    , "FRACTDAUGHTERTRACKSWITHUPSTREAMHIT =   INFO( 55, -1000.0 )"
    , "MAXFRACTENERGYINSINGLETRACK        =   INFO( 56, -1000.0 )"
    ]

LLPSingleSelCuts = [
      "~ISBASIC"
    , "M > %(MinMass)s"
    , "NDAUGS >= %(MinNumTracks)s"
    , "ENDVERTEXRHO > %(MinRho)s"
    , "FRACTDAUGHTERTRACKSWITHUPSTREAMHIT < %(MaxFractTrwHitBefore)s"
    , "MAXFRACTENERGYINSINGLETRACK < %(MaxFractE1Track)s"
    ]

class VeloGEC(Hlt2Stage):
    def __init__(self, prefix=linePrefix, name="VeloGEC", **kwargs):
        self.__prefix = __prefix
        self.__kwargs = kwargs
        self.__cache = {}
        super(VeloGEC, self).__init__(name, inputs=[], dependencies=[])

    def clone(self, name, **kwargs):
        args = deepcopy(self.__kwargs)
        args["name"] = name
        for arg, default in (("prefix", self.__prefix),):
            args[arg] = kwargs.pop(arg) if arg in kwargs else default
        args.update(kwargs)
        return VeloGEC(**args)

    def stage(self, stages, cuts):
        key = self._hashCuts(cuts)
        if key in self.__cache:
            cached = self.__cache[key]
            return cached

        from Configurables import Hlt__GEC
        gec = Hlt__GEC("Hlt2"+self.__prefix+self._name(),
                    IsActivity = False, MaxVeloHits = -1, MaxITHits = -1, MaxOTHits = -1,
                    MaxVeloBalance=cuts.get(self._name(), "MaxVeloBalance")
                    )
        self.__cache[key] = gec
        return gec

class RecVertex2ParticleVelo(Hlt2TisTosStage):
    def __init__(self, prefix=linePrefix, name="RV2P", dependencies=[], **kwargs):
        self.__prefix = prefix
        self.__kwargs = kwargs
        inputs = [ BiKalmanFittedPions ]
        dependencies = dependencies + [ PV3D("Hlt2"), VeloVertexFinder(prefix) ]
        super(RecVertex2ParticleVelo, self).__init__(name, inputs=inputs, dependencies=dependencies, shared=True)
        self.__cache = {}
        
    def clone(self, name, **kwargs):
        args = deepcopy(self.__kwargs)
        args["name"] = name
        for arg, default in (("prefix", self.__prefix),):
            args[arg] = kwargs.pop(arg) if arg in kwargs else default
        args.update(kwargs)
        return RecVertex2ParticleVelo(**args)

    def stage(self, stages, cuts):
        key = self._hashCuts(cuts)
        if key in self.__cache:
            cached = self.__cache[key]
            stages += cached[1]
            return cached[0]

        from Configurables import LLParticlesFromRecVertices
        deps = []
        inputStages = self.inputStages(deps, cuts)
        rv2p = LLParticlesFromRecVertices("Hlt2"+self.__prefix+self._name(),
                        Inputs             = [ i.outputSelection() for i in inputStages ],
                        RecVertexLocations = [ VeloVertexFinder(self.__prefix).outrv ],
                        Output             = "Hlt2/{0}{1}/Particles".format(self.__prefix, self._name()),
                        WriteP2PVRelations = False,
                        ForceP2PVBuild     = False
                        )
        rv2pCuts = cuts.get("RV2PVelo")
        for prop in RecoCuts + SingleCuts:
            if prop in rv2p.properties():
                rv2p.setProp(prop, rv2pCuts[prop])

        deps = self.dependencies(cuts)
        stages += deps + inputStages
        self.__cache[key] = (rv2p, deps)
        return rv2p

class SingleFilter(Hlt2ParticleFilter):
    def __init__(self, name="SingleFilter", tistos=[]):
        code = " & ".join("( {0} )".format(selCut) for selCut in LLPSingleSelCuts) + "%(extra)s"
        inputs = [ RecVertex2ParticleVelo(linePrefix) ]
        super(SingleFilter, self).__init__(linePrefix+name, code, inputs, nickname=name,
                    Preambulo          = LLPSelectionPreambulo,
                    DecayDescriptor    = LLPLHCbName,
                    WriteP2PVRelations = False,
                    ForceP2PVBuild     = False,
                    )

class DoubleFilter(Hlt2Combiner):
    def __init__(self, name="DoubleFilter"):
        inputs = [ RecVertex2ParticleVelo(linePrefix) ]
        super(DoubleFilter, self).__init__(linePrefix+name, "H_10 -> {0} {0}".format(LLPLHCbName), inputs, nickname=name,
                    Preambulo          = LLPSelectionPreambulo,
                    DaughtersCuts      = { LLPLHCbName : " & ".join("( {0} )".format(selCut) for selCut in LLPSingleSelCuts) + "%(extraSingle)s" },
                    CombinationCut     = "AHASCHILD( M > %(MinHighestMass)s )%(extraComb)s",
                    MotherCut          = "ALL",
                    WriteP2PVRelations = False,
                    ForceP2PVBuild     = False,
                    )
