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

class GoodKsFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        cut = ("(PT > %(KS_PT_MIN)s)" +
               " & (ADMASS('KS0') < %(KS_MASS_WINDOW)s)" +
               " & (VFASPF(VCHI2PDOF) < %(KS_VCHI2NDOF_MAX)s)")
        Hlt2ParticleFilter.__init__(self, name, cut, inputs=[KsLLTF, KsDD], shared = True)

goodKs = GoodKsFilter('GoodKs')

class Phi2KsKsCombiner(Hlt2Combiner):
    def __init__(self, name):
        cc =       (" (APT > %(Phi_PT_MIN)s)" +
                    " & (ADAMASS('phi(1020)') < %(Phi_MASS_WINDOW)s)")
        mc =       ("(VFASPF(VCHI2/VDOF) < %(Phi_VCHI2NDOF_MAX)s)")
        from HltTracking.HltPVs import PV3D
        inputs = [goodKs]
        Hlt2Combiner.__init__(self,'Phi2KsKs', "[phi(1020) -> KS0 KS0]cc", inputs,
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

class D02KsKsCombiner(Hlt2Combiner):
    def __init__(self, name):
        cc =       (" (APT > %(D0_PT_MIN)s)" +
                    " & (ADAMASS('D0') < %(D0_MASS_WINDOW)s)")
        mc =       ("(VFASPF(VCHI2/VDOF) < %(D0_VCHI2NDOF_MAX)s)")
        from HltTracking.HltPVs import PV3D
        inputs = [goodKs]
        Hlt2Combiner.__init__(self,'D02KsKs', "[D0 -> KS0 KS0]cc", inputs,
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])
