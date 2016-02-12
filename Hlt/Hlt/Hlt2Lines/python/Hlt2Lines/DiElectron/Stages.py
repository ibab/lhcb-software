# =============================================================================
# @file:   Set of "stages" for lines devoted to the study of DiElectron decays
# @author: Miguel Ramos Pernas miguel.ramos.pernas@cern.ch
# @author: Jessica Prisciandaro jessica.prisciandaro@cern.ch
# @date:   2016-02-12
# =============================================================================

__all__    = ( 'SoftDiElectronCombiner' )

from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from HltTracking.HltPVs import PV3D

class SoftDiElectronCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):

        dc = { 'e+': "(MIPCHI2DV(PRIMARY) < %(MaxIpChi2)s)" }

        cc = ( "(ACHILD(MIPDV(PRIMARY),1) * ACHILD(MIPDV(PRIMARY),2) > %(IpProd)s)" +
               " & (ACHILD(TRGHOSTPROB,1) + ACHILD(TRGHOSTPROB,2) < %(SumGP)s)" +
               " & ((ACHILD(MIPCHI2DV(PRIMARY),1) - %(MinIpChi2)s) * (ACHILD(MIPCHI2DV(PRIMARY),2) - %(MinIpChi2)s) > %(IpChi2Prod)s)" )

        mc = ( "(PCUTA(ALV(1,2) < %(CosAngle)s))" +
               " & (BPVVDZ > %(VDZ)s)" +
               " & ((MIPDV(PRIMARY)/BPVVDZ) < %(IpDzRatio)s)" +
               " & (VFASPF(VX*VX + VY*VY) > %(Rho2)s)" +
               " & (DOCAMAX < %(DOCA)s)" +
               " & (VFASPF(VZ) < %(SVZ)s)" +
               " & (MM < %(Mass)s)" +
               " & (BPVDIRA > %(Dira)s)" )

        Hlt2Combiner.__init__(self,
                              name,
                              decay,
                              inputs,
                              DaughtersCuts = dc,
                              CombinationCut = cc,
                              MotherCut = mc,
                              shared = True,
                              dependencies = [PV3D('Hlt2')],
                              UseP2PVRelations = False)

from Inputs import TrackFittedDetachedDiElectron
DiElectronFromKS0 = SoftDiElectronCombiner("ElSoft", "KS0 -> e+ e-", [TrackFittedDetachedDiElectron])
