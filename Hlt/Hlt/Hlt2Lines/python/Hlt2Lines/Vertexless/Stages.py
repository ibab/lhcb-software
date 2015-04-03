## --------------------------------------------------------------------------------
## Lines for decays with out a decay vertex
## Eg, B+ -> K+ pi0, Lambda_b -> Lambda0 Gamma, B0 -> K0 pi0
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


# ## Will use for V0's
# class MassWindowFilter(Hlt2ParticleFilter):
#     """Apply mass cut on the inputs."""
#     def __init__(self, nickname, inputs):
#         cut = "ADMASS('%(PARTICLE)s') < %(MASS_WIN)s*MeV"
#         super(MassWindowFilter, self).__init__('VertexlessMassFilter_%s' % nickname,
#                                                cut,
#                                                inputs,
#                                                nickname=nickname)


## Allow input specification for testing configurations simultaneously
class KaonFilter(Hlt2ParticleFilter):
    """Filter the Hlt1 TOS track"""
    def __init__(self, name, inputs):
        cut = ("(TRCHI2DOF < %(TRACK_TRCHI2DOF_MAX)s)"
               "& (PT > %(TRACK_PT_MIN)s)"
               "& (P > %(TRACK_P_MIN)s)"
               "& (MIPCHI2DV(PRIMARY) > %(TRACK_IPCHI2_MIN)s)"
               "& (PIDK > %(TRACK_PIDK_MIN)s)")

        super(KaonFilter, self).__init__('VertexlessKaonFilter',
                                         cut,
                                         inputs,
                                         nickname=name,
                                         tistos='TrackTisTos',
                                         UseP2PVRelations=False)


# # Filter calo photons
# ## Try requiring L0 TOS
# class PhotonFilter(Hlt2ParticleFilter):
#     """Filter photons"""
#     def __init__(self):
#         cut = "(PT > %(PHOTON_PT_MIN)s) & (P > %(PHOTON_P_MIN)s)"
#         from Inputs import Hlt2Photons as photons
#         # from Inputs import L0Photons as photons
#         super(PhotonFilter, self).__init__('VertexlessPhotons',
#                                            cut,
#                                            [photons],
#                                            nickname='SharedPhotons',
#                                            tistos='PhotonTisTos',
#                                            shared=True)

## *******************************************************
## *******************************************************
## Temporary workaround for TOSing L0 in Hlt2ParticleFilter
class PhotonFilter1(Hlt2TisTosParticleTagger):
    """Filter photons"""
    def __init__(self):
        from Inputs import Hlt2Photons as photons
        # from Inputs import L0Photons as photons
        super(PhotonFilter1, self).__init__('VertexlessPhotons1',
                                           'PhotonTisTos',
                                           [photons],
                                           nickname='SharedPhotons1',
                                           shared=True)
class PhotonFilter2(Hlt2ParticleFilter):
    """Filter photons"""
    def __init__(self):
        cut = "(PT > %(PHOTON_PT_MIN)s) & (P > %(PHOTON_P_MIN)s)"
        super(PhotonFilter2, self).__init__('VertexlessPhotons2',
                                           cut,
                                           [PhotonFilter1()],
                                           nickname='SharedPhotons2',
                                           shared=True)
## *******************************************************
## *******************************************************
        
# ## Will use for V0's
# class HHCombiner(Hlt2Combiner):
#     """Build vector mesons from input tracks."""
#     def __init__(self, nickname, decay, inputs):
#         daughters_cuts = {'pi+' : "ALL", 'K+' : "ALL"}
#         combination_cut = ("ADAMASS('%(PARTICLE)s') < wide_mass" )
#         mother_cut = ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)"
#                       " & (ADMASS('%(PARTICLE)s') < %(MASS_WIN)s)")
#         super(HHCombiner, self).__init__('RadiativeHHCombiner_%s' % nickname,
#                                          decay,
#                                          inputs,
#                                          nickname=nickname,
#                                          shared=True,
#                                          tistos='TrackTisTos',
#                                          DaughtersCuts=daughters_cuts,
#                                          CombinationCut=combination_cut,
#                                          MotherCut=mother_cut,
#                                          Preambulo=['wide_mass = 1.5*%(MASS_WIN)s'])


# Build the b hadrons
## Instantiate the photons internally
## Exclusive line for each decay or all in one?
class Hb2XGammaCombiner(Hlt2Combiner):
    """Build the b hadron from photons and input hadrons."""
    def __init__(self, name, decay, hadrons):
        # combination_cut = "%(MOTHER_COMB)s"
        combination_cut = "(in_range(%(MASS_MIN)s *MeV, AM, %(MASS_MAX)s *MeV))"
        mother_cut = ("(PT > %(PT_MIN)s) "
                      " & (MTDOCACHI2(1)<%(MTDOCAChi2_MAX)s)")
        super(Hb2XGammaCombiner, self).__init__('VertexlessHb2XGammaCombiner_%s' % name,
                                                decay,
                                                [hadrons, PhotonFilter2()],
                                                nickname=name,
                                                DaughtersCuts={},
                                                CombinationCut=combination_cut,
                                                Preambulo=['from LoKiPhys.functions import MTDOCACHI2'],
                                                MotherCut=mother_cut,
                                                ParticleCombiners={'':'ParticleAdder'}
                                                )
