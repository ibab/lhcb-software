###
#  @author M. Kenzie    matthew.william.kenzie@cern.ch
#  @author J. Harrison  jonathan.harrison@manchester.ac.uk
#  @date 2015-04-17
#
#  Please contact the above authors before editing this file
#
##

from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner

from HltTracking.HltPVs import PV3D
from Inputs import UnbiasedPhi2KK, KsLLTF, KsDD

class IncPhiFilter(Hlt2ParticleFilter) :
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

class Bs2PhiPhiCombiner(Hlt2Combiner):
  def __init__(self, name):
    inputs = [UnbiasedPhi2KK]
    mc = ("(ALL)")
    Hlt2Combiner.__init__(self, name, "B_s0 -> phi(1020) phi(1020)", inputs,
                          dependencies = [PV3D('Hlt2')],
                          tistos = 'TisTosSpec',
                          #DaughtersCuts  = dc,
                          #CombinationCut = cc,
                          MotherCut      = mc,
                          Preambulo = [])
        
class GoodKsFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        cut = ("(PT > %(KS_PT_MIN)s)" +
               " & (BPVVDCHI2 > %(KS_FD_MIN)s)" +
               " & CHILDCUT((TRGHOSTPROB < %(GHOSTPROB_MAX)s),1)" +
               " & CHILDCUT((TRGHOSTPROB < %(GHOSTPROB_MAX)s),2)" +
               " & (ADMASS('KS0') < %(KS_MASS_WINDOW)s)" +
               " & (VFASPF(VCHI2PDOF) < %(KS_VCHI2NDOF_MAX)s)")
        Hlt2ParticleFilter.__init__(self, name, cut, inputs=[KsLLTF, KsDD], shared = True)

goodKs = GoodKsFilter('GoodKs')

class Phi2KsKsCombiner(Hlt2Combiner):
    def __init__(self, name):
        cc =       (" (APT > %(Phi_PT_MIN)s)" +
                    " & (ACUTDOCACHI2(%(Phi_DOCACHI2_MAX)s,''))" +
                    " & (ADAMASS('phi(1020)') < %(Phi_MASS_WINDOW)s)")
        mc =       ("(VFASPF(VCHI2/VDOF) < %(Phi_VCHI2NDOF_MAX)s)")
        from HltTracking.HltPVs import PV3D
        inputs = [goodKs]
        Hlt2Combiner.__init__(self, name, "[phi(1020) -> KS0 KS0]cc", inputs,
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

class D02KsKsCombiner(Hlt2Combiner):
    def __init__(self, name):
        cc =       (" (APT > %(D0_PT_MIN)s)" +
                    " & (ACUTDOCACHI2(%(D0_DOCACHI2_MAX)s,''))" +
                    " & (ADAMASS('D0') < %(D0_MASS_WINDOW)s)")
        mc =       ("(VFASPF(VCHI2/VDOF) < %(D0_VCHI2NDOF_MAX)s)")
        from HltTracking.HltPVs import PV3D
        inputs = [goodKs]
        Hlt2Combiner.__init__(self, name, "[D0 -> KS0 KS0]cc", inputs,
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])
