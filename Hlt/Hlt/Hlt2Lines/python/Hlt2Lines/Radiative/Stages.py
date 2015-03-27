#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   Stages.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   05.03.2015
# =============================================================================
"""Candidate building stages for HLT2 radiative decays lines."""

from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Hlt2Lines.Utilities.Hlt2TisTosFilter import Hlt2TisTosParticleTagger
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner


# The GEC
class TrackGEC(Hlt2VoidFilter):
    """Apply the GEC in number of tracks."""
    def __init__(self):
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking as Hlt2LongTracking
        tracks = Hlt2LongTracking().hlt2PrepareTracks()
        code = ("CONTAINS('%s')" % tracks.outputSelection()) + " < %(NTRACK_MAX)s"
        super(TrackGEC, self).__init__('RadiativeTrackGEC',
                                       code,
                                       [tracks],
                                       nickname='TrackGEC',
                                       shared=True)


# Mass filters
class MassFilter(Hlt2ParticleFilter):
    """Apply mass cut on the inputs."""
    def __init__(self, nickname, inputs):
        cut = "in_range(%(M_MIN)s, M ,%(M_MAX)s)"
        super(MassFilter, self).__init__('RadiativeMassFilter_%s' % nickname,
                                         cut,
                                         inputs,
                                         nickname=nickname)


class MassWindowFilter(Hlt2ParticleFilter):
    """Apply mass cut on the inputs."""
    def __init__(self, nickname, inputs):
        cut = "ADMASS('%(PARTICLE)s') < %(MASS_WIN)s*MeV"
        super(MassWindowFilter, self).__init__('RadiativeMassFilter_%s' % nickname,
                                               cut,
                                               inputs,
                                               nickname=nickname)


# Filter input tracks
class ParticleFilter(Hlt2ParticleFilter):
    """Filter children tracks for exclusive lines."""
    def __init__(self, nickname, inputs):
        cut = ("(TRCHI2DOF < %(TRACK_TRCHI2DOF_MAX)s)"
               "& (PT > %(TRACK_PT_MIN)s)"
               "& (P > %(TRACK_P_MIN)s)"
               "& (MIPCHI2DV(PRIMARY) > %(TRACK_IPCHI2_MIN)s)")

        super(ParticleFilter, self).__init__('RadiativeParticleFilter_%s' % nickname,
                                             cut,
                                             inputs,
                                             nickname=nickname,
                                             shared=True,
                                             UseP2PVRelations=False)


# Filter calo photons
class PhotonFilter(Hlt2TisTosParticleTagger):
    """Filter photons according to the TISSTOS specs."""
    def __init__(self):
        from Inputs import Hlt2Photons
        super(PhotonFilter, self).__init__('RadiativeL0Photons',
                                           'PhotonTisTos',
                                           [Hlt2Photons],
                                           shared=True)

# CALO photons builder (some cuts needed on non-converted photons only,
# before pairing with converted counterparts
class CALOPhotonFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        photon_cut = "( (PT > %(CALO_PT)s*MeV) & (P > %(CALO_P)s*MeV) )"
        super(CALOPhotonFilter, self).__init__('RadiativeCALOHardPhoton_%s' % name,
                                               photon_cut,
                                               inputs,
                                               nickname=name,
                                               shared=True)

# Converted photons builder
class ConvPhotonFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        photon_cut = "( (PT > %(ee_PT)s*MeV) & (P > %(ee_P)s*MeV) & (M < %(ee_Mass)s*MeV) )"
        super(ConvPhotonFilter, self).__init__('RadiativeConvPhoton_%s' % name,
                                               photon_cut,
                                               inputs,
                                               nickname=name,
                                               shared=True)


class ConvPhotonLL(ConvPhotonFilter):
    def __init__(self):
        from Inputs import Hlt2DiElectron_LL
        super(ConvPhotonLL, self).__init__('ConvLL', [Hlt2DiElectron_LL])


class ConvPhotonDD(ConvPhotonFilter):
    def __init__(self):
        from Inputs import Hlt2DiElectron_DD
        super(ConvPhotonDD, self).__init__('ConvDD', [Hlt2DiElectron_DD])


# Build the vector mesons
class HHCombiner(Hlt2Combiner):
    """Build vector mesons from input tracks."""
    def __init__(self, nickname, decay, inputs):
        daughters_cuts = {'pi+' : "ALL", 'K+' : "ALL"}
        combination_cut = ("ADAMASS('%(PARTICLE)s') < wide_mass" )
        mother_cut = ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)"
                      " & (ADMASS('%(PARTICLE)s') < %(MASS_WIN)s)")
        super(HHCombiner, self).__init__('RadiativeHHCombiner_%s' % nickname,
                                         decay,
                                         inputs,
                                         nickname=nickname,
                                         shared=True,
                                         tistos='TrackTisTos',
                                         DaughtersCuts=daughters_cuts,
                                         CombinationCut=combination_cut,
                                         MotherCut=mother_cut,
                                         Preambulo=['wide_mass = 1.5*%(MASS_WIN)s'])


# Build the B mesons
class B2XGammaCombiner(Hlt2Combiner):
    """Build the B from photons and input vector mesons."""
    def __init__(self, name, decay, vector_meson):
        combination_cut = "ADAMASS('%(PARTICLE)s') < wide_mass"
        mother_cut = ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)"
                      " & (PT > %(PT_MIN)s) "
                      " & (BPVDIRA > cos_dira_angle)"
                      " & (BPVIPCHI2() < %(BPVIPCHI2_MAX)s)"
                      " & (ADMASS('%(PARTICLE)s') < %(MASS_WIN)s)")
        super(B2XGammaCombiner, self).__init__('RadiativeB2XGammaCombiner_%s' % name,
                                               decay,
                                               [vector_meson, PhotonFilter()],
                                               nickname=name,
                                               DaughtersCuts={},
                                               CombinationCut=combination_cut,
                                               MotherCut=mother_cut,
                                               Preambulo=['from math import cos',
                                                          'cos_dira_angle = cos(%(BPVDIRA_MIN)s)',
                                                          'wide_mass = 1.5*%(MASS_WIN)s'])


class B2GammaGammaCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        mother_cut = ("(M<%(BsMax)s*MeV) & "
                      "(M > %(BsMin)s*MeV) & "
                      "(PT > %(B_PT)s*MeV) & "
                      "(P > %(B_P)s*MeV)")
        if "LL" in name:
            mother_cut += " & ((INTREE( (ID=='gamma') & (ISBASIC) )) & (INTREE( HASTRACK & ISLONG )))"
        elif "DD" in name:
            mother_cut += " & ((INTREE( (ID=='gamma') & (ISBASIC) )) & (INTREE( HASTRACK & ISDOWN )))"
        comb_cut = "((ACHILD(PT,1)+ACHILD(PT,2)) > %(SUM_PT)s*MeV)"
        super(B2GammaGammaCombiner, self).__init__('RadiativeB2GammaGammaCombiner_%s' % name,
                                                   decay,
                                                   inputs,
                                                   nickname=name,
                                                   CombinationCut=comb_cut,
                                                   MotherCut=mother_cut,
                                                   ParticleCombiners={ '' : 'ParticleAdder'})

# EOF
