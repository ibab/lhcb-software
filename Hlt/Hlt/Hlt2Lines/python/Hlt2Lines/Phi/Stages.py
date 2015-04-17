from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter

from HltTracking.HltPVs import PV3D
from Inputs import UnbiasedPhi2KK

class PhiFilter(Hlt2ParticleFilter) :
  def __init__(self, name):
    
    code = ("(PT > %(PhiPT)s) " +
            "& (MINTREE('K+'==ABSID,PT) > %(KaonPT)s) " +
            "& (MINTREE('K+'==ABSID,BPVIPCHI2()) > %(KaonIPS)s) " +
            "& (MINTREE('K+'==ABSID,PIDK) > %(KaonPID)s) " +
            "& (MAXTREE('K+'==ABSID,TRCHI2DOF) < %(TrChi2DOF)s) " +
            "& (VFASPF(VCHI2PDOF) < %(PhiVCHI2)s) " +
            "& (in_range( PDGM('phi(1020)') - %(PhiMassWin)s , M , PDGM('phi(1020)') + %(PhiMassWin)s ) )")
            
    inputs = [UnbiasedPhi2KK]
    Hlt2ParticleFilter.__init__(self, name, code, inputs, tistos = 'TisTosSpec', dependencies = [PV3D('Hlt2')], shared = True)
