#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file   Stages.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   05.03.2015
# =============================================================================
"""Candidate building stages for HLT2 radiative decays lines."""

from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter, Hlt2ParticleFilter
from Hlt2Lines.Utilities.Hlt2MergedStage import Hlt2MergedStage
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
class PIDFilter(Hlt2ParticleFilter):
    """Filter tracks using PID criteria."""
    def __init__(self, nickname, inputs):
        cut = ("((HASTRACK) & ('%(TRACK_TYPE)s'==ABSID)"
               "& (%(PID_VAR)s>%(PID_CUT_MIN)s))")
        super(PIDFilter, self).__init__('RadiativePIDFilter_%s' % nickname,
                                             cut,
                                             inputs,
                                             nickname=nickname,
                                             shared=True)

class ParticleFilter(Hlt2ParticleFilter):
    """Filter children tracks for exclusive lines."""
    def __init__(self, nickname, inputs, tistos=None):
        cut = ("(TRCHI2DOF < %(TRACK_TRCHI2DOF_MAX)s)"
               "& (PT > %(TRACK_PT_MIN)s)"
               "& (P > %(TRACK_P_MIN)s)"
               "& (MIPCHI2DV(PRIMARY) > %(TRACK_IPCHI2_MIN)s)")
        if not tistos:
            tistos = []
        super(ParticleFilter, self).__init__('RadiativeParticleFilter_%s' % nickname,
                                             cut,
                                             inputs,
                                             tistos=tistos,
                                             nickname=nickname,
                                             shared=True,
                                             UseP2PVRelations=False)


# Filter calo photons
class PhotonFilter(Hlt2ParticleFilter):
    """Filter photons according to their pt."""
    def __init__(self):
        from Inputs import Hlt2Photons
        super(PhotonFilter, self).__init__('RadiativeHighPtPhotons',
                                           '(PT > %(PT_MIN)s)',
                                           [Hlt2Photons],
                                           nickname='CaloPhotons',
                                           shared=True)


class PhotonTOSFilter(Hlt2TisTosParticleTagger):
    """Filter photons according to the TISSTOS specs."""
    def __init__(self):
        super(PhotonTOSFilter, self).__init__('RadiativeL0Photons',
                                              'PhotonTisTos',
                                              [PhotonFilter()],
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
        super(ConvPhotonFilter, self).__init__('RadiativePhoton_%s' % name,
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


class ConvPhotonAll(Hlt2MergedStage):
    def __init__(self):
        super(ConvPhotonAll, self).__init__('RadiativePhoton_ConvAll',
                                            [ConvPhotonLL(), ConvPhotonDD()],
                                            shared=True)


# Build the vector mesons
class HHCombiner(Hlt2Combiner):
    """Build vector mesons from input tracks."""
    def __init__(self, nickname, decay, inputs, tistos=None):
        daughters_cuts = {'pi+' : "ALL", 'K+' : "ALL"}
        combination_cut = ("ADAMASS('%(PARTICLE)s') < wide_mass" )
        mother_cut = ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)"
                      " & (ADMASS('%(PARTICLE)s') < %(MASS_WIN)s)")
        if not tistos:
            tistos = []
        super(HHCombiner, self).__init__('RadiativeHHCombiner_%s' % nickname,
                                         decay,
                                         inputs,
                                         nickname=nickname,
                                         shared=True,
                                         tistos=tistos,
                                         DaughtersCuts=daughters_cuts,
                                         CombinationCut=combination_cut,
                                         MotherCut=mother_cut,
                                         Preambulo=['wide_mass = 1.5*%(MASS_WIN)s'])


# Filter lambdas
class Lambda0Filter(Hlt2ParticleFilter):
    """Filter L0."""
    def __init__(self, name, inputs):
        lambda0_cut = """(MIPDV(PRIMARY) > %(IP_MIN)s) &
                         (DOCA(1,2) < %(DOCA_MAX)s) &
                         (BPVVDCHI2>0) &
                         (VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s) &
                         (PT > %(PT_MIN)s) &
                         (2 == NINTREE((ISBASIC) &
                                       (P > %(TRACK_P_MIN)s) &
                                       (PT > %(TRACK_PT_MIN)s) &
                                       (TRCHI2DOF < %(TRACK_TRCHI2DOF_MAX)s) &
                                       (MIPCHI2DV(PRIMARY) > %(TRACK_IPCHI2_MIN)s))) &
                         INTREE((ABSID=='p+') & (PIDp > %(P_PIDP_MIN)s))"""
        super(Lambda0Filter, self).__init__('RadiativeLambda0Filter_%s' % name,
                                            lambda0_cut,
                                            inputs,
                                            nickname=name,
                                            shared=True)


# Build the B mesons
class RadiativeCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs, converted_photons, **combiner_args):
        if converted_photons:
            photons = [ConvPhotonAll()]
        else:
            photons = [PhotonTOSFilter()]
        super(RadiativeCombiner, self).__init__('RadiativeCombiner_%s' % name,
                                                decay,
                                                inputs+photons,
                                                nickname=name,
                                                **combiner_args)


class B2XGammaCombiner(RadiativeCombiner):
    """Build the B from photons and input vector mesons."""
    def __init__(self, name, decay, vector_meson, converted=False):
        combination_cut = "ADAMASS('%(PARTICLE)s') < wide_mass"
        mother_cut = ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)"
                      " & (PT > %(PT_MIN)s) "
                      " & (BPVDIRA > cos_dira_angle)"
                      " & (BPVIPCHI2() < %(BPVIPCHI2_MAX)s)"
                      " & (ADMASS('%(PARTICLE)s') < %(MASS_WIN)s)")
        super(B2XGammaCombiner, self).__init__(name,
                                               decay,
                                               [vector_meson],
                                               converted,
                                               CombinationCut=combination_cut,
                                               MotherCut=mother_cut,
                                               Preambulo=['from math import cos',
                                                          'cos_dira_angle = cos(%(BPVDIRA_MIN)s)',
                                                          'wide_mass = 1.5*%(MASS_WIN)s'])


class B2XGammaUnbiasedCombiner(RadiativeCombiner):
    """Build the B from photons and input vector mesons in an unbiased way."""
    def __init__(self, name, decay, vector_meson, converted=False):
        combination_cut = ("(ADAMASS('%(PARTICLE)s') < wide_mass)"
                           "& (ASUM(PT)>%(SUM_PT_MIN)s)")
        mother_cut = ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)"
                      " & (PT > %(PT_MIN)s) "
                      " & (BPVLTIME()>%(TAU_MIN)s)"
                      " & (ADMASS('%(PARTICLE)s') < %(MASS_WIN)s)")
        super(B2XGammaUnbiasedCombiner, self).__init__(name,
                                                       decay,
                                                       [vector_meson],
                                                       converted,
                                                       DaughtersCuts={},
                                                       CombinationCut=combination_cut,
                                                       MotherCut=mother_cut,
                                                       Preambulo=['wide_mass = 1.5*%(MASS_WIN)s'])


class Lb2L0GammaCombiner(RadiativeCombiner):
    """Build the Lb from photons and input lambda0 mesons.

    Use ParticleAdder.

    """
    def __init__(self, name, decay, lambda0):
        mother_cut = "(PT > %(PT_MIN)s) & (MTDOCACHI2(1) < %(MTDOCACHI2_MAX)s)"
        comb_cut = """(ASUM(PT) > %(SUM_PT_MIN)s) &
                      (APT > %(PT_MIN)s) &
                      (ADAMASS('Lambda_b0') < %(MASS_WIN)s)"""
        super(Lb2L0GammaCombiner, self).__init__(name,
                                                 decay,
                                                 [lambda0],
                                                 False,
                                                 CombinationCut=comb_cut,
                                                 MotherCut=mother_cut,
                                                 ParticleCombiners={ '' : 'ParticleAdder'})


class TopoCombiner(RadiativeCombiner):
    """Combine a photon and an nbody object a la topo."""
    def __init__(self, name, n_bodies, nbody_object, converted=False):
        if n_bodies == 2:
            decay = 'B*0 -> B0 gamma'
        else:
            decay = 'B*0 -> B*0 gamma'
        comb_cut = "(APT > %(APT_MIN)s) & (AM < %(CORRM_MAX)s)"
        mother_cut = """(HASVERTEX)
                        & (VFASPF(VCHI2) < %(VCHI2PDOF_MAX)s)
                        & (BPVVDCHI2 > %(VDCHI2_MIN)s)
                        & (in_range(%(ETA_MIN)s, BPVETA, %(ETA_MAX)s))
                        & (in_range(%(CORRM_MIN)s, BPVCORRM, %(CORRM_MAX)s))
                        & (BPVDIRA > %(DIRA_MIN)s)"""
        super(TopoCombiner, self).__init__(name,
                                           decay,
                                           [nbody_object],
                                           converted,
                                           CombinationCut=comb_cut,
                                           MotherCut=mother_cut)


class B2GammaGammaCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        mother_cut = ("(M < %(BsMax)s*MeV) & "
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

# BBDT Filter
class BonsaiBDTFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs, props):
        import os
        params = os.path.join('$PARAMFILESROOT/data', props['BBDT_PARAMS' % n])
        var_map = props['BBDT_VARMAP' % n]
        bbdt = self._get_classifier("RadBBDT_%s" % name, params, var_map)
        cut = "(VALUE('{}/{}') > %(BBDT_CUT)s)".format(bbdt.Type.getType(), bbdt.Name)
        super(BonsaiBDTFilter, self).__init__('RadiativeBBDTFilter_%s' % name,
                                              cut,
                                              inputs,
                                              nickname=name,
                                              tools=[bbdt])

    def _get_classifier(self, name, params, var_map, preambulo=None):
        from HltLine.HltLine import Hlt1Tool as Tool
        from Configurables import LoKi__Hybrid__DictOfFunctors as DictOfFunctors
        from Configurables import LoKi__Hybrid__DictValue as DictValue
        from Configurables import LoKi__Hybrid__DictTransform_BBDecTreeTransform_ as Transform
        tool_name = 'BBDecTree'
        key = 'BDT'
        options = {tool_name+'File' : params,
                   'Name'           : key,
                   'KeepVars'       : '0'}
        func_dict = Tool(type=DictOfFunctors,
                         name='TopoMVAdict'+name,
                         Preambulo=preambulo,
                         Variables=var_map)
        transform  = Tool(type=Transform,
                          name=tool_name,
                          tools=[func_dict],
                          Options=options,
                          Source="LoKi::Hybrid::DictOfFunctors/TopoMVAdict"+name)
        classifier = Tool(type=DictValue,
                          name=name,
                          tools=[transform],
                          Key=key,
                          Source='LoKi::Hybrid::DictTransform<{tool_name}Transform>/{tool_name}'.format(tool_name=tool_name))
        return classifier

# EOF
