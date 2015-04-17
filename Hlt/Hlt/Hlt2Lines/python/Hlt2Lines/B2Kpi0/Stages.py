## --------------------------------------------------------------------------------
## Lines for modes with out a reconstructible decay vertex
## Ie, B+ -> K+ pi0 and B0 -> K0 pi0
##
## Author: Jason Andrews, jea@umd.edu
## Shameless copy of Albert Puig's Radiative lines
## --------------------------------------------------------------------------------
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Hlt2Lines.Utilities.Hlt2TisTosFilter import Hlt2TisTosParticleTagger
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner

# The GEC
class TrackGEC(Hlt2VoidFilter):
    """Apply the GEC in number of tracks."""
    def __init__(self):
        from HltTracking.Hlt2TrackingConfigurations import \
             Hlt2BiKalmanFittedForwardTracking as Hlt2LongTracking
        tracks = Hlt2LongTracking().hlt2PrepareTracks()
        code = ("CONTAINS('%s')" % tracks.outputSelection()) + " < %(NTRACK_MAX)s"
        super(TrackGEC, self).__init__('VertexlessTrackGEC',
                                       code,
                                       [tracks],
                                       nickname='TrackGEC',
                                       shared=True)

class KaonFilter(Hlt2ParticleFilter):
    """Filter the Hlt1 TOS track"""
    def __init__(self, name, inputs):
        cut = ("(TRCHI2DOF < %(TRACK_TRCHI2DOF_MAX)s)"
               "& (TRGHOSTPROB < %(TRACK_TRGHOSTPROB_MAX)s)"
               "& (PT > %(TRACK_PT_MIN)s)"
               "& (P > %(TRACK_P_MIN)s)"
               "& (MIPCHI2DV(PRIMARY) > %(TRACK_IPCHI2_MIN)s)"
               "& (PIDK > %(TRACK_PIDK_MIN)s)")
        super(KaonFilter, self).__init__('VertexlessKaons',
                                         cut,
                                         inputs,
                                         nickname=name,
                                         tistos='TRACK_TISTOS',
                                         UseP2PVRelations=False)

class KS0Filter(Hlt2ParticleFilter):
    """Filter the Hlt1 TOS K0"""
    def __init__(self, name, inputs):
        cut = ("(PT > %(KS0_PT_MIN)s) & (P > %(KS0_P_MIN)s)"
               "& (ADMASS('KS0') < %(KS0_ADMASS)s)"
               "& (CHILDCUT((TRGHOSTPROB < %(TRACK_TRGHOSTPROB_MAX)s),1))"
               "& (CHILDCUT((TRGHOSTPROB < %(TRACK_TRGHOSTPROB_MAX)s),2))"
               "& (CHILDCUT((TRCHI2DOF < %(TRACK_TRCHI2DOF_MAX)s),1))"
               "& (CHILDCUT((TRCHI2DOF < %(TRACK_TRCHI2DOF_MAX)s),2))"
               "& (VFASPF(VCHI2PDOF) < %(KS0_VCHI2PDOF_MAX)s)"
               "& (MIPCHI2DV(PRIMARY) > %(KS0_IPCHI2_MIN)s)")
        super(KS0Filter, self).__init__('VertexlessK0s',
                                        cut,
                                        inputs,
                                        nickname=name,
                                        tistos='KS0_TISTOS',
                                        UseP2PVRelations=False)

class Pi0TOSFilter(Hlt2TisTosParticleTagger):
    """Filter pi0s"""
    def __init__(self):
        from Inputs import MergedPi0s
        super(Pi0TOSFilter, self).__init__('VertexlessPi0TOSFilter',
                                           'PI0_TISTOS',
                                           [MergedPi0s],
                                           nickname='SharedL0TOSPi0s',
                                           shared=True)

class Pi0Filter(Hlt2ParticleFilter):
    """Filter pi0s"""
    def __init__(self, name, inputs):
        cut = "(PT > %(PI0_PT_MIN)s) & (P > %(PI0_P_MIN)s)"
        super(Pi0Filter, self).__init__('VertexlessPi0Filter',
                                           cut,
                                           inputs,
                                           nickname=name,
                                           shared=True)

class Hb2XGammaCombiner(Hlt2Combiner):
    """Build the b hadron from hadrons and photons."""
    def __init__(self, name, decay, inputs):
        combination_cut = ("(in_range(%(MASS_MIN)s *MeV, AM, %(MASS_MAX)s *MeV))"
                           " & (ASUM(PT)> %(ASUM_PT_MIN)s)")
        mother_cut = "(PT > %(PT_MIN)s) & (MTDOCACHI2(1)<%(MTDOCACHI2_MAX)s)"
        super(Hb2XGammaCombiner, self).__init__('VertexlessHb2XGammaCombiner_%s' % name,
                                                decay,
                                                inputs,
                                                nickname=name,
                                                DaughtersCuts={},
                                                CombinationCut=combination_cut,
                                                MotherCut=mother_cut,
                                                ParticleCombiners={'':'ParticleAdder'}
                                                )
