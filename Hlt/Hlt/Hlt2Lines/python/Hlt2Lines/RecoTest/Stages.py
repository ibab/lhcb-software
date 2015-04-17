# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Stage import Hlt2Stage
from HltTracking.HltPVs import PV3D

from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Inputs import Pions,DownPions,Photons
class CreatePions(Hlt2ParticleFilter):
    def __init__(self,name):
        code = ("(PT >  %(Pt)s) & (TRCHI2DOF<%(TrChi2)s)")
        inputs = [ Pions, DownPions, Photons ]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, dependencies = [PV3D('Hlt2')])

