# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Stage import Hlt2Stage

# The GEC
class Photons(Hlt2Stage):
    def __init__(self, full):
        if full:
            from Inputs import Photons
            self.__photons = Photons
        else:
            from Inputs import PhotonsFromL0
            self.__photons = PhotonsFromL0
        name = "Photons" if full else "PhotonsFromL0"
        from HltLine.HltDecodeRaw import DecodeL0CALO
        from HltTracking.HltPVs   import PV3D
        super(Photons, self).__init__("Photons", [], [PV3D('Hlt2'), DecodeL0CALO])
        
    def stage(self, cuts):
        return self.__photons
