# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Stage import Hlt2Stage

# The GEC
class Photons(Hlt2Stage):
    def __init__(self, full):
        self.__full = full

        name = "Photons" if full else "PhotonsFromL0"
        from HltLine.HltDecodeRaw import DecodeL0CALO
        from HltTracking.HltPVs   import PV3D
        super(Photons, self).__init__(name, [], [PV3D('Hlt2'), DecodeL0CALO])

    def stage(self, stages, cuts):
        from Inputs import Photons, PhotonsFromL0
        return Photons if self.__full else PhotonsFromL0

class ParticleFlow(Hlt2Stage):
    def __init__(self):
        from HltTracking.HltPVs import PV3D
        super(ParticleFlow, self).__init__("ParticleFlow", [], [PV3D('Hlt2')])

    def stage(self, stages, cuts):
        import Reco
        return Reco.ParticleFlow
