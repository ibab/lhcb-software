## Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Inputs import KS0_LL, KS0_DD, Lambda_LL, Lambda_DD

## ========================================================================= ##
## Global event cuts
## ========================================================================= ##
class TrackGEC(Hlt2VoidFilter):
    '''
    Cut on the number of reconstructed long tracks.
    Historically useful, may be obsolete.
    '''
    def __init__(self, name):
        from Configurables import LoKi__Hybrid__CoreFactory as Factory
        modules =  Factory().Modules
        for i in [ 'LoKiTrigger.decorators' ] : 
            if i not in modules : modules.append(i)

        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking as Hlt2LongTracking
        tracks = Hlt2LongTracking().hlt2PrepareTracks()
        code = ("CONTAINS('%s')" % tracks.outputSelection()) + " < %(NTRACK_MAX)s"
        Hlt2VoidFilter.__init__(self, 'TrackGEC', code, [tracks], shared = True)



## ========================================================================= ##
## Filters for basic particles
## ========================================================================= ##

## ------------------------------------------------------------------------- ##
class InParticleFilter(Hlt2ParticleFilter) : # {
    '''
    Filter charged particles on TRCHI2DOF, PT, P, and (optionally) a PID
    variable.  Does NOT filter on IP or IPCHI2.

    Always creates a shared instance of the filter.

    Configuration dictionaries must contain the following keys:
        'Trk_ALL_TRCHI2DOF_MAX' :  upper limit on TRCHI2DOF
        'Trk_ALL_PT_MIN'        :  lower limit on PT
        'Trk_ALL_P_MIN'         :  lower limit on P

    If filtering on a PID variable, the pidVar parameter must be set to the
    name of the PID functor on which to cut, and the configuration dictionary
    must contain the key 'PID_LIM' defining the one-sided limit for that
    variable.  By default the cut on the PID variable is a lower limit.
    It may be changed to an upper limit by setting the parameter
    pidULim = True.
    '''
    def __init__(self, name, inputs, pidVar = None, pidULim = False): # {
        cut = ("  (TRCHI2DOF < %(Trk_ALL_TRCHI2DOF_MAX)s )" +
               "& (PT > %(Trk_ALL_PT_MIN)s)" +
               "& (P > %(Trk_ALL_P_MIN)s)" )
        if pidVar : # {
            pidCut = "& (%s %s %%(PID_LIM)s)" % (pidVar, ('<' if pidULim else '>'))
            cut = cut + pidCut
        # }

        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)
    # }
# }


## Shared instances of InParticleFilter
## If these are not made into central shared particles, their names should
##   be updated to flag them as CharmHad shared particles to avoid name
##   conflicts with other subdirectories.
## ------------------------------------------------------------------------- ##
from Inputs import Hlt2LoosePions, Hlt2LooseKaons, Hlt2LooseProtons
from Inputs import Hlt2NoPIDsPions, Hlt2NoPIDsKaons, Hlt2NoPIDsProtons
SharedPromptChild_pi = InParticleFilter( 'SharedPromptChild_pi',
                                         [Hlt2LoosePions], 'PIDK', True )
SharedPromptChild_K = InParticleFilter( 'SharedPromptChild_K',
                                        [Hlt2LooseKaons], 'PIDK' )
SharedPromptChild_p = InParticleFilter( 'SharedPromptChild_p',
                                        [Hlt2LooseProtons], 'PIDp' )
SharedSoftTagChild_pi = InParticleFilter( 'SharedSoftTagChild_pi',
                                          [Hlt2NoPIDsPions] )


## ------------------------------------------------------------------------- ##
class DetachedInParticleFilter(Hlt2ParticleFilter) : # {
    '''
    Filter charged particles on TRCHI2DOF, PT, P, MIPCHI2DV(PRIMARY),
    and (optionally) a PID variable.

    Always creates a shared instance of the filter.

    Configuration dictionaries must contain the following keys:
        'Trk_ALL_TRCHI2DOF_MAX' :  upper limit on TRCHI2DOF
        'Trk_ALL_PT_MIN'        :  lower limit on PT
        'Trk_ALL_P_MIN'         :  lower limit on P
        'Trk_ALL_MIPCHI2DV_MIN' :  lower limit on MIPCHI2DV(PRIMARY)

    If filtering on a PID variable, the pidVar parameter must be set to the
    name of the PID functor on which to cut, and the configuration dictionary
    must contain the key 'PID_LIM' defining the one-sided limit for that
    variable.  By default the cut on the PID variable is a lower limit.
    It may be changed to an upper limit by setting the parameter
    pidULim = True.
    '''
    def __init__(self, name, inputs, pidVar = None, pidULim = False): # {
        cut = ("  (TRCHI2DOF < %(Trk_ALL_TRCHI2DOF_MAX)s )" +
               "& (PT > %(Trk_ALL_PT_MIN)s)" +
               "& (P > %(Trk_ALL_P_MIN)s)" +
               "& (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)")

        if pidVar : # {
            pidCut = "& (%s %s %%(PID_LIM)s)" % (pidVar, ('<' if pidULim else '>'))
            cut = cut + pidCut
        # }

        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True, 
                                    dependencies = [PV3D('Hlt2')],
                                    UseP2PVRelations = False)

    # }
# }


## Shared instances of DetachedInParticleFilter
## These are all associated with specific combiners and should perhaps be
##   defined closer to that context.
## ------------------------------------------------------------------------- ##
SharedDetachedDpmChild_pi = DetachedInParticleFilter( 'SharedDetachedDpmChild_pi', [Hlt2LoosePions], 'PIDK', True )
SharedDetachedDpmChild_K = DetachedInParticleFilter( 'SharedDetachedDpmChild_K',
                                                     [Hlt2LooseKaons], 'PIDK' )
SharedDetachedLcChild_pi = DetachedInParticleFilter('SharedDetachedLcChild_pi',
                                                    [Hlt2LoosePions], 'PIDK', True )
SharedDetachedLcChild_K = DetachedInParticleFilter( 'SharedDetachedLcChild_K',
                                                    [Hlt2LooseKaons], 'PIDK' )
SharedDetachedLcChild_p = DetachedInParticleFilter('SharedDetachedLcChild_p',
                                                   [Hlt2LooseProtons], 'PIDp' )

SharedNoPIDDetachedChild_pi = DetachedInParticleFilter( 'SharedNoPIDDetachedChild_pi', [Hlt2NoPIDsPions] )
SharedNoPIDDetachedChild_K = DetachedInParticleFilter( 'SharedNoPIDDetachedChild_K', [Hlt2NoPIDsKaons] )
SharedNoPIDDetachedChild_p = DetachedInParticleFilter( 'SharedNoPIDDetachedChild_p', [Hlt2NoPIDsProtons] )


class KsFilterForHHKs(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        cut = (" (BPVLTIME() > %(DecayTime_MIN)s) " +
               "& (VFASPF(VZ) > %(VZ_MIN)s)" +
               "& (VFASPF(VZ) < %(VZ_MAX)s)")
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)

SharedKsLL =  KsFilterForHHKs('SharedKsLL',[KS0_LL])
SharedKsDD =  KsFilterForHHKs('SharedKsDD',[KS0_DD])

## ------------------------------------------------------------------------- ##
from Configurables import DaVinci__N3BodyDecays as N3BodyDecays
class HHKshCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):

## HHKs mother cuts
        mc =    ("(VFASPF(VCHI2PDOF) < %(D_VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > %(D_BPVDIRA_MIN)s )" +
                 " & (BPVVDCHI2 > %(D_BPVVDCHI2_MIN)s )" +
                 " & (PT > %(D_PT_MIN)s )" +
                 " & (BPVLTIME() > %(D_BPVLTIME_MIN)s )")

        c12Cut = ("(AM <  %(HHMass_MAX)s)" +
                  " & (ADOCAMAX('') < %(HHMaxDOCA)s)")

        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(ASUMPT_MIN)s )")

        from HltTracking.HltPVs import PV3D

        Hlt2Combiner.__init__(self, 'D02HHKsh', decay, inputs,
                     dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                     tistos = 'TisTosSpec', combiner = N3BodyDecays,
                     Combination12Cut =  c12Cut,
                     CombinationCut = cc, 
                     MotherCut = mc, Preambulo = [])

## ------------------------------------------------------------------------- ##
class NeutralInParticleFilter(Hlt2ParticleFilter) : # {
    '''
    Filter neutral particles on PT and (optionally) an ADMASS window centered
    on the database mass of a particle type identified in a constructor
    argument.
    class NeutralInParticleFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        cut = ("  (PT > %(Trk_ALL_PT_MIN)s)" )
    Configuration dictionaries must contain the following keys:
        'Neut_ALL_PT_MIN' :  lower limit on PT

    If filtering on a ADMASS window, the massPart parameter must be set to
    the name of the particle type (e.g. 'pi0'), and the configuration
    dictionary must contain the key 'Neut_ALL_ADMASS_MAX' defining the window
    half-width, which is the upper limit for ADMASS.
    '''
    def __init__(self, name, inputs, massPart = None) : # {
        cut = "(PT > %(Neut_ALL_PT_MIN)s)"

        if massPart : # {
            masscut = "(ADMASS('%s') < %%(Neut_ALL_ADMASS_MAX)s) & " % (massPart)
            cut = masscut + cut
        # }

        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)
    # }
# }


## Shared instances of NeutralInParticleFilter
## If these are not made into central shared particles, their names should
##   be updated to flag them as CharmHad shared particles to avoid name
##   conflicts with other subdirectories.
## ------------------------------------------------------------------------- ##
from Inputs import Hlt2ResolvedPi0, Hlt2MergedPi0, Hlt2NoPIDsPhotons, Hlt2AllPi0
SharedNeutralChild_pi0R  = NeutralInParticleFilter("SharedNeutralChild_pi0R",
                                                   [Hlt2ResolvedPi0], 'pi0' )
SharedNeutralChild_pi0M  = NeutralInParticleFilter("SharedNeutralChild_pi0M",
                                                   [Hlt2MergedPi0], 'pi0' )
SharedNeutralChild_pi0   = NeutralInParticleFilter("SharedNeutralChild_pi0",
                                                   [Hlt2AllPi0], 'pi0' )
SharedNeutralChild_gamma = NeutralInParticleFilter("SharedNeutralChild_gamma",
                                                   [Hlt2NoPIDsPhotons] )
SharedNeutralLowPtChild_pi0 = NeutralInParticleFilter("SharedNeutralLowPtChild_pi0", [Hlt2AllPi0], 'pi0' )
SharedNeutralLowPtChild_gamma = NeutralInParticleFilter("SharedNeutralLowPtChild_gamma", [Hlt2NoPIDsPhotons] )



## ========================================================================= ##
## 2-body Combiners
## ========================================================================= ##

## ------------------------------------------------------------------------- ##
class TagDecay(Hlt2Combiner) : # {
    """
    Generic 2-body combiner for adding a soft particle to another particle
    candidate, e.g., adding a pion to a D0 to created a D*+ -> D0 pi+ candidate.
    It cuts on the mass difference and on chi^2/Ndof of the 2-body vertex.

    !!!NOTE!!! The implementation of the mass difference cut requires that
    the soft particle is the second product in the decay descriptor:
        GOOD:      [Sigma_c0 -> Lambda_c+ pi-]cc
        NOT GOOD:  [Sigma_c0 -> pi- Lambda_c+]cc

    Configuration dictionaries must contain the following keys:
        'DeltaM_AM_MIN'
        'DeltaM_AM_MAX'    : lower and upper limits of the delta mass window
                             at the CombinationCut level, AM - AM1.
        'TagVCHI2PDOF_MAX' : upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'DeltaM_MIN'
        'DeltaM_MAX'       : lower and upper limits of the delta mass window
                             at the MotherCut level, M - M1.
        'TisTosSpec'       : The configuration string of the Hlt1 TISTOS filter.
    """
    def __init__(self, name, decay, inputs, DaughtersCuts = { }, shared = False):
        cc =    ('in_range( %(DeltaM_AM_MIN)s, (AM - AM1), %(DeltaM_AM_MAX)s )')
        mc =    ("(VFASPF(VCHI2PDOF) < %(TagVCHI2PDOF_MAX)s)" +
                 "& in_range( %(DeltaM_MIN)s, (M - M1), %(DeltaM_MAX)s )")

        ## Since this class allows freedom to externally specify DaughtersCuts,
        ##   we should add a dependence on the PV3D
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec',
                              DaughtersCuts = DaughtersCuts,
                              CombinationCut = cc, 
                              shared = shared,
                              MotherCut = mc, Preambulo = []) 
# }



## ========================================================================= ##
## 3-body Combiners
## ========================================================================= ##

# ------------------------------------------------------------------------- ##
class DetachedHHHCombiner(Hlt2Combiner) : # {
    """
    Combiner for 3 basic track-based particles.  The 'Detached' in the class
    name indicates that it applies cuts to lifetime-biasing non-lifetime
    values like the MIPCHI2DV(PRIMARY) of the decay products and the
    BPVVDCHI2_MIN of the fitted vertex.

    Always creates a shared instance of the filter.

    Configuration dictionaries must contain the following keys:
        'Trk_1OF3_PT_MIN'
        'Trk_2OF3_PT_MIN'
        'Trk_ALL_PT_MIN'        : tiered lower limits on product PT.
                                  All 3 must pass the ALL threshold.
                                  At least 2 must pass the 2OF3 threshold.
                                  At least 1 must pass the 1OF3 threshold.
        'Trk_1OF3_MIPCHI2DV_MIN'
        'Trk_2OF3_MIPCHI2DV_MIN'
        'Trk_ALL_MIPCHI2DV_MIN' : tiered lower limits on product
                                  MIPCHI2DV(PRIMARY)
        'AM_MIN'
        'AM_MAX'                : lower and upper limits on AM in CombinationCut
        'ASUMPT_MIN'            : lower limit on APT1+APT2+APT3 in CombinationCut
        'VCHI2PDOF_MAX'         : upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'BPVDIRA_MIN'           : lower limit on BPVDIRA in MotherCut
        'BPVVDCHI2_MIN'         : lower limit on BPVVDCHI2 in MotherCut
        'BPVLTIME_MIN'          : lower limit on BPVLTIME() in MotherCut
        'TisTosSpec'            : configuration string of the Hlt1 TISTOS filter.
    """
    def __init__(self, name, decay, inputs, nickname = None) : # {
        dc =    {}
        for child in ['pi+','K+','p+'] :
            dc[child] = "(PT > %(Trk_ALL_PT_MIN)s) & (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(ASUMPT_MIN)s )" +
                 " & (AHASCHILD(PT > %(Trk_1OF3_PT_MIN)s))"+
                 " & (ANUM(PT > %(Trk_2OF3_PT_MIN)s) >= 2)"+
                 " & (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Trk_1OF3_MIPCHI2DV_MIN)s))"+
                 " & (ANUM(MIPCHI2DV(PRIMARY) > %(Trk_2OF3_MIPCHI2DV_MIN)s) >= 2)")
        ## P.S. -- Are cuts on both BPVVDCHI2 and BPVLTIME useful?
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > %(BPVDIRA_MIN)s )" +
                 " & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )")

        nickname = name if nickname == None else nickname
        from HltTracking.HltPVs import PV3D
        ## P.S. -- I want to remove the dependence on the GEC.
        ## This kind of automatic name-mangling breaks under cloning.
        Hlt2Combiner.__init__(self, "CharmHad" + name, decay, inputs,
                              nickname = nickname, dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              shared = True, tistos = 'TisTosSpec', DaughtersCuts = dc,
                              CombinationCut = cc, MotherCut = mc, Preambulo = [])
    # }
# }


## Shared instances of DetachedHHHCombiner
## ------------------------------------------------------------------------- ##

## Main line D+/D_s+ -> 3h combiners
D2HHH_DpToKmPipPip = DetachedHHHCombiner( 'D2HHH_DpToKmPipPip'
        , decay = "[D+ -> K- pi+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'D2HHH' )  ## 'D2HHH' defined in D2HHHLines.py

D2HHH_DpToKpPimPip = DetachedHHHCombiner( 'D2HHH_DpToKpPimPip'
        , decay = "[D+ -> K+ pi- pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'D2HHH' )  ## 'D2HHH' defined in D2HHHLines.py

D2HHH_DpToKpKpPim = DetachedHHHCombiner( 'D2HHH_DpToKpKpPim'
        , decay = "[D+ -> K+ K+ pi-]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'D2HHH' )  ## 'D2HHH' defined in D2HHHLines.py

D2HHH_DpToKmKpPim = DetachedHHHCombiner( 'D2HHH_DpToKmKpPim'
        , decay = "[D+ -> K- K+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'D2HHH' )  ## 'D2HHH' defined in D2HHHLines.py

D2HHH_DpToPimPipPip = DetachedHHHCombiner( 'D2HHH_DpToPimPipPip'
        , decay = "[D+ -> pi- pi+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_pi ]
        , nickname = 'D2HHH' )  ## 'D2HHH' defined in D2HHHLines.py

D2HHH_DpToKmKpKm = DetachedHHHCombiner( 'D2HHH_DpToKmKpKm'
        , decay = "[D+ -> K- K+ K+]cc"
        , inputs = [ SharedDetachedDpmChild_K ]
        , nickname = 'D2HHH' )  ## 'D2HHH' defined in D2HHHLines.py


## Main line Lambda_c+ -> 3h combiners
Lc2HHH_LcpToKmPpPip = DetachedHHHCombiner( 'Lc2HHH_LcpToKmPpPip'
        , decay = "[Lambda_c+ -> K- p+ pi+]cc"
        , inputs = [ SharedDetachedLcChild_p, SharedDetachedLcChild_K,
                     SharedDetachedLcChild_pi ]
        , nickname = 'Lc2HHH' ) ## 'Lc2HHH' defined in D2HHHLines.py

Lc2HHH_LcpToKmPpKp = DetachedHHHCombiner( 'Lc2HHH_LcpToKmPpKp'
        , decay = "[Lambda_c+ -> K- p+ K+]cc"
        , inputs = [ SharedDetachedLcChild_p, SharedDetachedLcChild_K ]
        , nickname = 'Lc2HHH' ) ## 'Lc2HHH' defined in D2HHHLines.py

Lc2HHH_LcpToPimPpPip = DetachedHHHCombiner( 'Lc2HHH_LcpToPimPpPip'
        , decay = "[Lambda_c+ -> pi- p+ pi+]cc"
        , inputs = [ SharedDetachedLcChild_p, SharedDetachedLcChild_pi ]
        , nickname = 'Lc2HHH' ) ## 'Lc2HHH' defined in D2HHHLines.py

Lc2HHH_LcpToPimPpKp = DetachedHHHCombiner( 'Lc2HHH_LcpToPimPpKp'
        , decay = "[Lambda_c+ -> pi- p+ K+]cc"
        , inputs = [ SharedDetachedLcChild_p, SharedDetachedLcChild_K,
                     SharedDetachedLcChild_pi ]
        , nickname = 'Lc2HHH' ) ## 'Lc2HHH' defined in D2HHHLines.py


## Combiners for cross-section measurements
XSec_DpToKmPipPip = DetachedHHHCombiner( 'XSec_DpToKmPipPip'
        , decay = "[D+ -> K- pi+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'Dpm2HHH_XSec' )   ## 'Dpm2HHH_XSec' def in XSecLines.py

XSec_DpToKmKpPim = DetachedHHHCombiner( 'XSec_DpToKmKpPim'
        , decay = "[D+ -> K- K+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'Dpm2HHH_XSec' )   ## 'Dpm2HHH_XSec' def in XSecLines.py

XSec_DspToKmKpPim = DetachedHHHCombiner( 'XSec_DspToKmKpPim'
        , decay = "[D_s+ -> K- K+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'Ds2HHH_XSec' )    ## 'Ds2HHH_XSec' def in XSecLines.py

XSec_DspToPimPipPip = DetachedHHHCombiner( 'XSec_DspToPimPipPip'
        , decay = "[D_s+ -> pi- pi+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_pi ]
        , nickname = 'Ds2HHH_XSec' )   ## 'Ds2HHH_XSec' def in XSecLines.py

XSec_LcpToKmPpPip = DetachedHHHCombiner( 'XSec_LcpToKmPpPip'
        , decay = "[Lambda_c+ -> K- p+ pi+]cc"
        , inputs = [ SharedDetachedLcChild_p, SharedDetachedLcChild_K,
                     SharedDetachedLcChild_pi ]
        , nickname = 'Lc2HHH_XSec' ) ## 'Lc2HHH_XSec' defined in XSecLines.py

XSec_LcpToPimPpPip = DetachedHHHCombiner( 'XSec_LcpToPimPpPip'
        , decay = "[Lambda_c+ -> pi- p+ pi+]cc"
        , inputs = [ SharedDetachedLcChild_p, SharedDetachedLcChild_pi ]
        , nickname = 'Lc2HHH_XSec' ) ## 'Lc2HHH_XSec' defined in XSecLines.py

## Combiner for PIDCalib
PIDCalib_LcpToKmPpPip = DetachedHHHCombiner( 'PIDCalib_LcpToKmPpPip'
        , decay = "[Lambda_c+ -> K- p+ pi+]cc"
        , inputs = [ SharedNoPIDDetachedChild_p, SharedDetachedLcChild_K,
                     SharedDetachedLcChild_pi ]
        , nickname = 'Lc2KPPi_PIDCALIB' ) ## 'Lc2KPPi_PIDCALIB' def in D2HHHLines.py

## Combiner for KPi detection asymmetry studies
DetAsym_DpToKmPipPip = DetachedHHHCombiner( 'DetAsym_DpToKmPipPip'
        , decay = "[D+ -> K- pi+ pi+]cc"
        , inputs = [ SharedNoPIDDetachedChild_pi, SharedNoPIDDetachedChild_K ]
        , nickname = 'Dpm2KPiPi_ForKPiAsym' ) ## 'Dpm2KPiPi_ForKPiAsym' def in D2HHHLines.py




# ------------------------------------------------------------------------- ##
class HHHCombiner(Hlt2Combiner):
    def __init__(self, name, decay,inputs):
        dc =    {}
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(ASUMPT_MIN)s )" )
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > %(BPVDIRA_MIN)s )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )")
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

class HHHHCombiner(Hlt2Combiner):
    def __init__(self, name, decay,inputs):
        dc =    {}  
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3+APT4) > %(ASUMPT_MIN)s )" )
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > %(BPVDIRA_MIN)s )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )")
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc, 
                              MotherCut = mc, Preambulo = []) 

class DetachedHHHHCombiner(Hlt2Combiner) :
    def __init__(self, name, decay, inputs):
        dc =    {}
        for child in ['pi+','K+','p+'] :
            dc[child] = "(PT > %(Trk_ALL_PT_MIN)s) & (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3+APT4) > %(ASUMPT_MIN)s )" +
                 " & (AHASCHILD(PT > %(Trk_1OF4_PT_MIN)s))"+
                 " & (ANUM(PT > %(Trk_2OF4_PT_MIN)s) >= 2)"+
                 " & (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Trk_1OF4_MIPCHI2DV_MIN)s))"+
                 " & (ANUM(MIPCHI2DV(PRIMARY) > %(Trk_2OF4_MIPCHI2DV_MIN)s) >= 2)")
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > %(BPVDIRA_MIN)s )" +
                 " & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )")

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, "CharmHad" + name + "_" + type(self).__name__, decay, inputs,
                              nickname = name, dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              shared = True, tistos = 'TisTosSpec', DaughtersCuts = dc,
                              CombinationCut = cc, MotherCut = mc, Preambulo = [])


class DetachedV0HCombiner(Hlt2Combiner):
    def __init__(self, name, decay,inputs):
        dc =    {'KS0'    : "ALL",
                 'Lambda0' : "ALL",
                 'pi+'    : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)",
                 'K+'     : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"}
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(ASUMPT_MIN)s )" )
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > %(BPVDIRA_MIN)s )" +
                 " & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )")
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

class DetachedV0V0Combiner(Hlt2Combiner):
    def __init__(self, name, decay,inputs,lldd = False):
        dc =    {'KS0'    : ( "(PT > %(KS0_ALL_PT_MIN)s) &"+
                              "(MIPCHI2DV(PRIMARY) > %(KS0_ALL_MIPCHI2DV_MIN)s)" )
                }
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2) > %(ASUMPT_MIN)s )" )
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > %(BPVDIRA_MIN)s )" +
                 " & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )")
        if lldd == True :
            mc = "(INTREE((ABSID=='pi+') & (ISLONG)) & INTREE((ABSID=='pi+') & (ISDOWN))) & " + mc
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

## Lifetime unbiased combiner class for D0 -> h h' decays
class D02HHCombiner(Hlt2Combiner) : # { 
    def __init__(self, name, decay, inputs, slotName = None, shared = False) : # { 

        incuts = "(TRCHI2DOF< %(Trk_ALL_TRCHI2DOF_MAX)s )" \
                  "& (PT> %(Trk_PT_MIN)s)" \
                  "& (P> %(Trk_P_MIN)s)" 

        dc = {   'pi+' : incuts
               , 'K+' : incuts
             }   

        ## Assume that the wide mass range is wider than the narrow range.
        combcuts = "in_range(%(WideMass_M_MIN)s,  AM, %(WideMass_M_MAX)s)" \
                   "& ((APT1 > %(Trk_Max_APT_MIN)s) " \
                       "| (APT2 > %(Trk_Max_APT_MIN)s))" \
                   "& (APT > %(D0_PT_MIN)s)" \
                   "& (AMINDOCA('') " \
                       "< %(Pair_AMINDOCA_MAX)s )"

        parentcuts = "(VFASPF(VCHI2PDOF) < %(D0_VCHI2PDOF_MAX)s)" \
                     "& (BPVDIRA > %(D0_BPVDIRA_MIN)s )"  \
                     "& (BPVLTIME() > %(D0_BPVLTIME_MIN)s )"

        from HltTracking.HltPVs import PV3D
        ## The name passed to the base class constructor determines the
        ##   configuration dictionary that is picked up.
        name = name if not shared else 'CharmHad' + name
        Hlt2Combiner.__init__( self, name, decay, inputs,
                               dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                               tistos = 'TisTosSpec',
                               nickname = slotName,
                               shared = shared,
                               DaughtersCuts = dc, 
                               CombinationCut = combcuts,
                               MotherCut = parentcuts,
                               Preambulo = [] )

## Combiner class for D0 -> h h' decays with detachement cuts
class DetachedD02HHCombiner(Hlt2Combiner) : # {
    def __init__(self, name, decay, inputs, slotName = None, shared = False) : # {

        incuts = "(TRCHI2DOF< %(Trk_ALL_TRCHI2DOF_MAX)s )" \
                  "& (PT> %(Trk_PT_MIN)s)" \
                  "& (P> %(Trk_P_MIN)s)" \
                  "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s )"

        dc = {   'pi+' : incuts
               , 'K+' : incuts
             }

        ## Assume that the wide mass range is wider than the narrow range.
        combcuts = "in_range(%(WideMass_M_MIN)s,  AM, %(WideMass_M_MAX)s)" \
                   "& ((APT1 > %(Trk_Max_APT_MIN)s) " \
                       "| (APT2 > %(Trk_Max_APT_MIN)s))" \
                   "& (APT > %(D0_PT_MIN)s)" \
                   "& (AMINDOCA('') " \
                       "< %(Pair_AMINDOCA_MAX)s )"

        parentcuts = "(VFASPF(VCHI2PDOF) < %(D0_VCHI2PDOF_MAX)s)" \
                     "& (BPVVDCHI2> %(D0_BPVVDCHI2_MIN)s )" \
                     "& (BPVDIRA > %(D0_BPVDIRA_MIN)s )"

        from HltTracking.HltPVs import PV3D
        ## The name passed to the base class constructor determines the
        ##   configuration dictionary that is picked up.
        name = name if not shared else 'CharmHad' + name
        Hlt2Combiner.__init__( self, name, decay, inputs,
                               dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                               tistos = 'TisTosSpec',
                               nickname = slotName,
                               shared = shared,
                               DaughtersCuts = dc,
                               CombinationCut = combcuts,
                               MotherCut = parentcuts,
                               Preambulo = [] )
    # }
# }

class DetachedHHChildCombiner(Hlt2Combiner):
    # combiner for 2-body displaced tracks to be used in multi-body D decays
    def __init__(self, name, decay, inputs):
        dc = {} 
        cc = (" ( AM < %(AM_MAX)s ) " +              
              "&( (APT1+APT2) > %(ASUMPT_MIN)s )" +
              "&( ACUTDOCA( %(ADOCA_MAX)s,'LoKi::TrgDistanceCalculator' ) )" +
              "&( AALLSAMEBPV )" )
        mc = (" ( VFASPF(VCHI2)<%(VCHI2_MAX)s ) " +
              "&( BPVVD > %(BPVVD_MIN)s )" +
              "&( BPVCORRM < %(BPVCORRM_MAX)s )" +
              "&( BPVVDCHI2 > %(BPVVDCHI2_MIN)s )")
        
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,                              
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

class DetachedHHHChildCombiner(Hlt2Combiner):
    # combiner for 3-body displaced tracks to be used in multi-body D decays
    def __init__(self, name, decay, inputs):
        if name.find('PIDCALIB')>-1 :
            dc =    {'pi+' : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)",
                     'K+'  : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)",
                     'p+'  : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"}
        else :
            dc =    {}
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(ASUMPT_MIN)s )" +
                 " & (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Trk_1OF3_MIPCHI2DV_MIN)s))"+
                 " & (ANUM(MIPCHI2DV(PRIMARY) > %(Trk_2OF3_MIPCHI2DV_MIN)s) >= 2)")
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > %(BPVDIRA_MIN)s )" +
                 " & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )")
                 
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,                              
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

class AttachParticle(Hlt2Combiner):
    def __init__(self, name, decay,inputs):
        dc =    {}
        cc =    ("  ( in_range( %(AM_MIN)s, AM, %(AM_MAX)s ) " +
                 "& ( APT > %(APT_MIN)s ) " + 
                 "& ( (ACHILD(PT,1) > %(ADAU1PT_MIN)s) & (ACHILD(BPVIPCHI2(),1) > %(ADAU1BPVIPCHI2_MIN)s) ) " + 
                 "& ( (ACHILD(PT,2) > %(ADAU2PT_MIN)s) & (ACHILD(BPVIPCHI2(),2) > %(ADAU2BPVIPCHI2_MIN)s) ) )"
                 )
        mc =    ("   ( in_range( %(DMASS_MIN)s, M, %(DMASS_MAX)s ) " +
                 " & ( BPVIPCHI2()< %(BPVIPCHI2_MAX)s )" +
                 " & ( BPVDIRA > %(BPVDIRA_MIN)s )" +
                 " & ( VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +              
                 " & ( BPVLTIME() > %(BPVLTIME_MIN)s ) )")
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

class D2HHHHCombiner(Hlt2Combiner):
    def __init__(self, name, decay,inputs):
        dc = {}
        cc = (" ( AM < %(AM_MAX)s ) " +
              "&( (APT1+APT2+APT3+APT4) > %(ASUMPT_MIN)s)" +
              "&( AMINDOCA('') < %(AMINDOCA_MAX)s )" +
              "&( AMAXDOCA('') < %(AMAXDOCA_MAX)s )" +
              "& (AALLSAMEBPV)" )
        mc = (" ( VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
              "&( BPVCORRM < %(BPVCORRM_MAX)s )" +
              "&( BPVVDCHI2> %(BPVVDCHI2_MIN)s )" +
              "&( BPVVDR> %(BPVVDR_MIN)s )" +
              "&( BPVDIRA > %(BPVDIRA_MIN)s )" +
              "&( BPVIPCHI2() < %(BPVIPCHI2_MAX)s )")
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

class D2HHHKsCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        dc = {}
        cc = (" (in_range( %(AM_MIN)s, AM, %(AM_MAX)s )) " )
        mc = (" (VFASPF(VCHI2PDOF)< %(VCHI2PDOF_MAX)s )" +
              "&(PT> %(PT_MIN)s)" +
              "&(BPVLTIME() > %(BPVLTIME_MIN)s)" +
              "&(BPVIPCHI2() < %(BPVIPCHI2_MAX)s )"+
              "&(BPVDIRA > %(BPVDIRA_MIN)s )" )
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

# D2HH combiners 

class D2HH0_ee_Combiner(Hlt2Combiner):
    def __init__(self, name):
        e_daughter_cuts  = ("( PT > %(Elec_PT)s ) &" +
                 "(TRCHI2DOF< %(Track_Chi2)s ) &"+
                 "(MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s) ")
        daug_cuts = { 'e+' : e_daughter_cuts,
                      'e-' : e_daughter_cuts }

        from HltTracking.HltPVs import PV3D
        from Inputs import Hlt2NoPIDsElectrons
        inputs = [ Hlt2NoPIDsElectrons ]
        Hlt2Combiner.__init__(self, name, "KS0 -> e+ e-", inputs,
                              dependencies = [ PV3D('Hlt2')],tistos = 'TisTosSpec', 
                              DaughtersCuts = daug_cuts, CombinationCut =  "AALL",
                              MotherCut = "ALL", Preambulo = [])        
   
class D2HH0_3Body_Combiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):                           
        pi_daughters_cut    = ("( PT > %(Daug_PT)s ) &" + 
                               "( P > %(Daug_P)s ) &" +
                               "(TRCHI2DOF< %(Track_Chi2)s ) &" +
                               "(MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s) ")                            
        K_daughters_cut     = ("( PT > %(Daug_PT)s ) &" +
                               "( P  >  %(Daug_P)s ) &" + 
                               "(TRCHI2DOF< %(Track_Chi2)s ) &" +
                               "(MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s) ")                            
        gamma_daughters_cut = ("( PT > %(Gamma_PT)s  ) &" +
                               "( P  > %(Gamma_P)s   )")                               
        e_daughters_cut     = ("( PT > %(Daug_PT)s ) &" +
                               "( P  >  %(Daug_P)s ) &" + 
                               "(TRCHI2DOF< %(Track_Chi2)s ) &"+
                               "(MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s) ")
        daug_cuts =    {'KS0'   : "ALL",
                        'pi+'   : pi_daughters_cut,
                        'K+'    : K_daughters_cut,
                        'gamma' : gamma_daughters_cut,
                        'e+'    : e_daughters_cut }
                        
        D_combination_cut   = ("(AM>%(D_MassWinLow)s) &" +
                               "(AM<%(D_MassWinHigh)s)") 
                               
        neutral_combination_cut = ("(AM>%(Neutral_MassWinLow)s) &" + 
                               "(AM<%(Neutral_MassWinHigh)s) &" + 
                               "(APT > %(Neutral_PT)s)")

        D_mother_cut   = ("( (PT > %(D_PT)s ) &" + 
                          "(VFASPF(VCHI2/VDOF)< %(Vertex_Chi2)s ) )")

        from HltTracking.HltPVs import PV3D
        from Configurables import DaVinci__N3BodyDecays
                
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [ PV3D('Hlt2')], tistos = 'TisTosSpec',
                              combiner = DaVinci__N3BodyDecays, DaughtersCuts = daug_cuts, 
                              CombinationCut =  D_combination_cut, MotherCut = D_mother_cut,
                              Combination12Cut = neutral_combination_cut, Preambulo = [])
                              
class D2HD0_3Body_Combiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):                        
        pi_daughters_cut = ("( PT > %(Daug_PT)s ) &" + 
                            "( P  > %(Daug_P)s  ) &" +
                            "(TRCHI2DOF< %(Track_Chi2)s ) &" +
                            "(MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s) ")
                               
        e_daughters_cut  = ("( PT > %(Elec_PT)s ) &" +
                            "( P  >  %(Elec_P)s ) &" + 
                            "(TRCHI2DOF< %(Track_Chi2)s ) &"+
                            "(MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s) ")
                 
        daug_cuts =    {'pi+'   : pi_daughters_cut,
                        'e+'    : e_daughters_cut}
                        
        D_combination_cut   = ("(AM>%(D_MassWinLow)s) &" +
                               "(AM<%(D_MassWinHigh)s)")
                               
        neutral_combination_cut = ("(AM>%(Neutral_MassWinLow)s) &" + 
                                   "(AM<%(Neutral_MassWinHigh)s) &" + 
                                   "(APT > %(Neutral_PT)s)")

        D_mother_cut   = ("( (PT > %(D_PT)s ) &" + 
                          "(VFASPF(VCHI2/VDOF)< %(Vertex_Chi2)s ) )")

        from HltTracking.HltPVs import PV3D
        from Configurables import DaVinci__N3BodyDecays
                
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [ PV3D('Hlt2')], tistos = 'TisTosSpec',
                              combiner = DaVinci__N3BodyDecays, DaughtersCuts = daug_cuts, 
                              CombinationCut =  D_combination_cut, MotherCut = D_mother_cut,
                              Combination12Cut = neutral_combination_cut, Preambulo = [])

class D2HH0_4Body_Combiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):                           
        pi_daughters_cut = ("( PT > %(Daug_PT)s ) &" + 
                            "( P  > %(Daug_P)s  ) &" +
                            "(TRCHI2DOF< %(Track_Chi2)s )& " +
                            "(MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s) ")                            
        K_daughters_cut = ("( PT > %(Daug_PT)s ) &" + 
                           "( P  > %(Daug_P)s  ) &" +
                           "(TRCHI2DOF< %(Track_Chi2)s )& " +
                           "(MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s) ")                            
        gamma_daughters_cut = ("( PT > %(Gamma_PT)s  ) &" +
                               "( P  > %(Gamma_P)s   )" )                 
        daug_cuts =    {'pi+'   : pi_daughters_cut,
                        'K+'    : K_daughters_cut,
                        'gamma' : gamma_daughters_cut }                        
        D_combination_cut   = ("(AM > %(D_MassWinLow)s ) &" +
                               "(AM < %(D_MassWinHigh)s) &" +
                               "(AMAXDOCA('')  < %(AMAXDOCA)s)")                               
        neutral_combination_cut = ("(AM>%(Neutral_MassWinLow)s) &" + 
                                   "(AM<%(Neutral_MassWinHigh)s) &" + 
                                   "(APT > %(Neutral_PT)s)")
        D_mother_cut   = ("( (PT > %(D_PT)s ) &" + 
                          "(VFASPF(VCHI2/VDOF)< %(Vertex_Chi2)s ) )")

        from HltTracking.HltPVs import PV3D
        from Configurables import DaVinci__N4BodyDecays
        
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [ PV3D('Hlt2')], tistos = 'TisTosSpec',
                              combiner = DaVinci__N4BodyDecays, DaughtersCuts = daug_cuts, 
                              CombinationCut =  D_combination_cut, MotherCut = D_mother_cut,
                              Combination12Cut = 'AALL', Combination123Cut = neutral_combination_cut,
                              Preambulo = [])

# D2EtaH Combiners
class D2RhoHG_3Body_Combiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        
        daug_cuts =    {'rho(770)0'   : "ALL",
                        'pi+'   : "ALL",
                        'K+'    : "ALL",
                        'gamma' : "ALL"
                        }
        
        charged_combination_cut = ("(AM>%(AMRhoH_MIN)s) " +
                                   "& (AM<%(AMRhoH_MAX)s) " )

        D_combination_cut   = ("(AM>%(AM_MIN)s) &" +
                               "(AM<%(AM_MAX)s) &" +
                               "(APT > %(APT_MIN)s) &" +
                               "(AM13>%(AMEta_MIN)s) &" +
                               "(AM13<%(AMEta_MAX)s) " )
        
        D_mother_cut   = ("( in_range( %(DMASS_MIN)s, M, %(DMASS_MAX)s )" +
                          " & ( VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                          " & ( BPVLTIME() > %(BPVLTIME_MIN)s ) )" )
        
        from HltTracking.HltPVs import PV3D
        from Configurables import DaVinci__N3BodyDecays
        
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec',
                              combiner = DaVinci__N3BodyDecays, DaughtersCuts = daug_cuts,
                              CombinationCut =  D_combination_cut,
                              MotherCut = D_mother_cut,
                              Combination12Cut = charged_combination_cut, Preambulo = [])

                        


# Lines definitions
class DetachedHHChild(DetachedHHChildCombiner):
    def __init__(self,name,decay=["K*(892)0 -> K+ pi-"]) :
        # decay descriptors could be optimised : we only need a pair of oppositely charged or same-charged tracks
        # could stay like this only if pid is added already here.
        #        decay =  ["[K*(892)0 -> K+ pi-]cc", "[K*(892)+ -> pi+ pi+]cc", "[K*(892)+ -> K+ K+]cc",
        #          "K*(892)0 -> pi+ pi-", "K*(892)0 -> K+ K-"]
        inputs = [SharedNoPIDDetachedChild_pi,
                  SharedNoPIDDetachedChild_K]
        DetachedHHChildCombiner.__init__(self,name,decay,inputs)

class DetachedHHHChild(DetachedHHHChildCombiner):
    def __init__(self,name,decay=["K*(892)0 -> K+ pi- pi+"]) :
        # decay descriptors could be optimised : we only need a pair of oppositely charged or same-charged tracks
        # could stay like this only if pid is added already here.
        #        decay =  ["[K*(892)0 -> K+ pi-]cc", "[K*(892)+ -> pi+ pi+]cc", "[K*(892)+ -> K+ K+]cc",
        #          "K*(892)0 -> pi+ pi-", "K*(892)0 -> K+ K-"]
        inputs = [SharedNoPIDDetachedChild_pi,
                  SharedNoPIDDetachedChild_K]
        DetachedHHHChildCombiner.__init__(self,name,decay,inputs)


class DetachedRhoPIDChild(DetachedHHChildCombiner):
    def __init__(self,name):
        decay=["rho(770)0 -> pi+ pi-"]
        inputs = [SharedDetachedDpmChild_pi]
        DetachedHHChildCombiner.__init__(self,name,decay,inputs)
        
                
# The lifetime unbiased lines now

class D2KPiPi_SS_LTUNB(HHHCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> K- pi+ pi+]cc"
        kaonsForPromptHHHCombiner = SharedPromptChild_K
        pionsForPromptHHHCombiner = SharedPromptChild_pi
        inputs = [kaonsForPromptHHHCombiner,pionsForPromptHHHCombiner]
        HHHCombiner.__init__(self,name,decay,inputs)

class D2KKPi_OS_LTUNB(HHHCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> K- K+ pi+]cc"
        inputs = [SharedPromptChild_K,
                  SharedPromptChild_pi]
        HHHCombiner.__init__(self,name,decay,inputs)

class Lc2KPPi_LTUNB(HHHCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> K- p+ pi+]cc"
        protonsForPromptHHHCombiner = SharedPromptChild_p
        inputs = [SharedPromptChild_K,
                  SharedPromptChild_pi,
                  protonsForPromptHHHCombiner]
        HHHCombiner.__init__(self,name,decay,inputs)

# Hc->hhhh lines for lifetime measurement

class Xic02PKKPi_LTUNB(HHHHCombiner) :
    def __init__(self,name) :
        decay = "[Xi_c0 -> p+ K- K- pi+]cc"
        inputs = [SharedPromptChild_K,
                  SharedPromptChild_pi,
                  SharedPromptChild_p]
        HHHHCombiner.__init__(self,name,decay,inputs)

class Xic02PKKPi(DetachedHHHHCombiner) :
    def __init__(self,name) :
        decay = "[Xi_c0 -> p+ K- K- pi+]cc"
        inputs = [SharedDetachedLcChild_K,
                  SharedDetachedLcChild_pi,
                  SharedDetachedLcChild_p]
        DetachedHHHHCombiner.__init__(self,name,decay,inputs)

# The V0H lines now

class D2KS0Pi_LL(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> pi+ KS0]cc"
        inputs = [SharedDetachedDpmChild_pi,
                  KS0_LL]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)

class D2KS0K_LL(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> K+ KS0]cc"
        inputs = [SharedDetachedDpmChild_K,
                  KS0_LL]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)        

class D2KS0Pi_DD(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> pi+ KS0]cc"
        inputs = [SharedDetachedDpmChild_pi,
                  KS0_DD]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)

class D2KS0K_DD(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> K+ KS0]cc"
        inputs = [SharedDetachedDpmChild_K,
                  KS0_DD]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)

class Lc2LambdaPi_LL(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> pi+ Lambda0]cc"
        inputs = [SharedDetachedLcChild_pi,
                  Lambda_LL]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)

class Lc2LambdaK_LL(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> K+ Lambda0]cc"
        inputs = [SharedDetachedLcChild_K,
                  Lambda_LL]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)

class Lc2LambdaPi_DD(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> pi+ Lambda0]cc"
        inputs = [SharedDetachedLcChild_pi,
                  Lambda_DD]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)

class Lc2LambdaK_DD(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> K+ Lambda0]cc"
        inputs = [SharedDetachedLcChild_K,
                  Lambda_DD]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)

# The V0V0 lines now

class D2KS0KS0_2LL(DetachedV0V0Combiner) :
    def __init__(self,name) :
        decay = "D0 -> KS0 KS0"
        inputs = [KS0_LL]
        DetachedV0V0Combiner.__init__(self,name,decay,inputs)

class D2KS0KS0_LLDD(DetachedV0V0Combiner) :
    def __init__(self,name) :
        decay = "D0 -> KS0 KS0"
        inputs = [KS0_LL,KS0_DD]
        DetachedV0V0Combiner.__init__(self,name,decay,inputs,lldd=True)

class D2KS0KS0_2DD(DetachedV0V0Combiner) :
    def __init__(self,name) :
        decay = "D0 -> KS0 KS0"
        inputs = [KS0_DD]
        DetachedV0V0Combiner.__init__(self,name,decay,inputs)

# 

class Dst2D0pi(TagDecay):
    def __init__(self,name,d0) :
        decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"]
        inputs = [d0, SharedSoftTagChild_pi]
        TagDecay.__init__(self,name,decay,inputs)

class D2KsHHH_KSLL(D2HHHKsCombiner) :
    def __init__(self,name, HHH) :
        decay = "[D+ -> KS0 K*(892)+]cc"
        inputs = [HHH,
                  KS0_LL]
        D2HHHKsCombiner.__init__(self,name,decay,inputs)

class D2KsHHH_KSDD(D2HHHKsCombiner) :
    def __init__(self,name, HHH) :
        decay = "[D+ -> KS0 K*(892)+]cc"
        inputs = [HHH,
                  KS0_DD]
        D2HHHKsCombiner.__init__(self,name,decay,inputs)

## D2HH Lines
from Inputs import Hlt2NoPIDsPions, Hlt2NoPIDsPhotons, Hlt2NoPIDsKaons, Hlt2NoPIDsElectrons

class D2PiH0_eeg(D2HH0_3Body_Combiner) :
    def __init__(self,name) :
        decay = "[D+ -> KS0 gamma pi+]cc"
        inputs = [D2HH0_ee_Combiner( 'Conv_Photon'), Hlt2NoPIDsPhotons, Hlt2NoPIDsPions]
        D2HH0_3Body_Combiner.__init__(self, name, decay, inputs)

class D2KH0_eeg(D2HH0_3Body_Combiner) :
    def __init__(self,name) :
        decay = "[D+ -> KS0 gamma K+]cc"
        inputs = [D2HH0_ee_Combiner( 'Conv_Photon'), Hlt2NoPIDsPhotons, Hlt2NoPIDsKaons]
        D2HH0_3Body_Combiner.__init__(self, name, decay, inputs)

class D2PiH0_pipig(D2HH0_4Body_Combiner) :
    def __init__(self,name) :
        decay = "[D+ -> pi+ pi- gamma pi+]cc"
        inputs = [Hlt2NoPIDsPions, Hlt2NoPIDsPhotons]
        D2HH0_4Body_Combiner.__init__(self, name, decay, inputs)

class D2KH0_pipig(D2HH0_4Body_Combiner) :
    def __init__(self,name) :
        decay = "[D+ -> pi+ pi+ gamma K+]cc"
        inputs = [Hlt2NoPIDsPions, Hlt2NoPIDsPhotons, Hlt2NoPIDsKaons]
        D2HH0_4Body_Combiner.__init__(self, name, decay, inputs)
        

class DStar2PiD0_eeg(D2HH0_3Body_Combiner) :
    def __init__(self,name) :
        decay = "[D*(2010)+ -> KS0 gamma pi+]cc"
        inputs = [D2HH0_ee_Combiner( 'Conv_Photon'), Hlt2NoPIDsPhotons, Hlt2NoPIDsPions]
        D2HH0_3Body_Combiner.__init__(self, name, decay, inputs)
        
        
class DStar2PiD0_ee(D2HD0_3Body_Combiner) :
    def __init__(self,name) :
        decay = "[D*(2010)+ -> e+ e- pi+]cc"
        inputs = [Hlt2NoPIDsElectrons, Hlt2NoPIDsPions]
        D2HD0_3Body_Combiner.__init__(self, name, decay, inputs)        

##  the argument 'D02HHKs' defines the dictionary found in Lines.py
##  to be used for the (generic) configurables used by the HHKshCombiner
D02KsPiPi_LL = HHKshCombiner('D02HHKsh', decay="[D0 ->  pi- pi+ KS0]cc",
                   inputs=[SharedKsLL, SharedDetachedDpmChild_pi])
D02KsPiPi_DD = HHKshCombiner('D02HHKsh', decay="[D0 ->  pi- pi+ KS0]cc",
                   inputs=[SharedKsDD, SharedDetachedDpmChild_pi])
D02KsKPi_LL  = HHKshCombiner('D02HHKsh', decay="[D0 ->  K- pi+ KS0]cc",
                   inputs=[SharedKsLL, SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi])
D02KsKPi_DD  = HHKshCombiner('D02HHKsh', decay="[D0 ->  K- pi+ KS0]cc",
                   inputs=[SharedKsDD, SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi])
D02KsKK_LL   = HHKshCombiner('D02HHKsh', decay="[D0 ->  K- K+ KS0]cc",
                   inputs=[SharedKsLL, SharedDetachedDpmChild_K])
D02KsKK_DD   = HHKshCombiner('D02HHKsh', decay="[D0 ->  K- K+ KS0]cc",
                   inputs=[SharedKsDD, SharedDetachedDpmChild_K])


class DetachedD02HHInclCombiner(Hlt2Combiner) : # {
    """
    Displaced combiner for inclusive D*+ line.

    Configuration dictionaries must contain the following keys:
        'Trk_TRCHI2DOF_MAX' : upper limit on TRCHI2DOF of each input particle
        'Trk_PT_MIN'        : lower limit on PT of each input particle
        'Trk_MIPCHI2DV_MIN' : lower limit on MIPCHI2DV(PRIMARY) of each input
        'D0_VCHI2PDOF_MAX'  : upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'D0_BPVVDCHI2_MIN'  : lower limit on BPVVDCHI2 in MotherCut
        'D0_BPVCORRM_MAX'   : upper limit on BPVCORRM in MotherCut
        'TisTosSpec'        : configuration string of the Hlt1 TISTOS filter.

    It is expected that the cuts on input particles will eventually be removed
    when the inputs are made into shared instances of DetachedInParticleFilter.
    """
    def __init__(self, name, decay, inputs, slotName = None) : # {

        inPartCuts = "(TRCHI2DOF< %(Trk_TRCHI2DOF_MAX)s )" \
                     "& (PT> %(Trk_PT_MIN)s)" \
                     "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s )"

        dc = {   'pi+' : inPartCuts
               , 'K+' : inPartCuts
             }

        combCut = "AALL"
        parentCut = "(VFASPF(VCHI2PDOF) < %(D0_VCHI2PDOF_MAX)s)" \
                    "& (BPVVDCHI2 > %(D0_BPVVDCHI2_MIN)s)" \
                    "& (BPVCORRM < %(D0_BPVCORRM_MAX)s)"

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__( self, name, decay, inputs,
                               dependencies = [PV3D('Hlt2')],
                               tistos = 'TisTosSpec',
                               nickname = slotName,
                               DaughtersCuts = dc,
                               CombinationCut = combCut,
                               MotherCut = parentCut,
                               Preambulo = [] )

    # }
# }


class Dstp2D0PiInclCombiner(Hlt2Combiner) : # {
    """
    Combiner for inclusive D*+ line.

    Configuration dictionaries must contain the following keys:
        'Spi_TRCHI2DOF_MAX' : Upper limit of TRCHI2DOF on soft pion
        'Spi_PT_MIN'        : Lower limit of PT on soft pion
        'Dst_VCHI2PDOF_MAX' : Upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'Dst_PT_MIN'        : Lower limit on PT in MotherCut
        'Dst_M_MAX'         : Upper limit on M in MotherCut
        'Dst_D0_DeltaM_MAX' : Upper limit on M - M1 in MotherCut

    It is expected that the cuts on input particles will eventually be removed
    when the inputs are made into shared instances of DetachedInParticleFilter.
    Use of a generic soft pion tagging combiner should be investigated.
    """
    def __init__(self, name, decay, inputs, slotName = None) : # {

        inPartCuts = "(TRCHI2DOF< %(Spi_TRCHI2DOF_MAX)s )" \
                     "& (PT> %(Spi_PT_MIN)s)"

        dc = {  'pi+' : inPartCuts }

        combCut = "AALL"

        parentCut = "(VFASPF(VCHI2PDOF) < %(Dst_VCHI2PDOF_MAX)s)" \
                    "& (PT > %(Dst_PT_MIN)s)" \
                    "& (M < %(Dst_M_MAX)s)" \
                    "& (M - M1 < %(Dst_D0_DeltaM_MAX)s)" \

        Hlt2Combiner.__init__( self, name, decay, inputs,
                               #tistos = 'TisTosSpec',
                               nickname = slotName,
                               DaughtersCuts = dc,
                               CombinationCut = combCut,
                               MotherCut = parentCut,
                               Preambulo = [] )

    # }
# }


## ========================================================================= ##
## Filters for composite particles
## ========================================================================= ##

# Mass filter
## ------------------------------------------------------------------------- ##
class MassFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs, nickname = None, shared = False ):
        cut = "in_range( %(Mass_M_MIN)s , M , %(Mass_M_MAX)s )"
        nickname = name if nickname == None else nickname
        name     = name if not shared       else 'CharmHad%sMass' % name
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,
                                    nickname = nickname , shared = shared )


## ------------------------------------------------------------------------- ##
class BDTFilter( Hlt2ParticleFilter ) : # {
    """
    Filter with a BBDecTreeTool.

    The part of the configuration dictionary that configures the lookup
    table and filter threshold must be passed as constructor argument 'props'
    and must contain the following keys:
        'BDT_Lookup_Filename' : the name of the file that contains the lookup
                                table.  Assumed to be in $PARAMFILESROOT/data/.
        'BDT_Lookup_VarMap'   : the map of nickname:functor strings to define
                                the input variables of the lookup table.
        'BDT_Threshold'       : Minimum response value accepted by the filter.
    """
    def __init__( self, name, inputs, props ) : # {

        ## Configure the DictOfFunctors variable handler
        from HltLine.HltLine import Hlt1Tool
        from Configurables import LoKi__Hybrid__DictOfFunctors
        varHandler = Hlt1Tool( type = LoKi__Hybrid__DictOfFunctors
                               , name = 'VarHandler'
                               , Variables = props['BDT_Lookup_VarMap']
                             )
        from Configurables import BBDecTreeTool
        bdtTool = Hlt1Tool( type = BBDecTreeTool
                            , name = 'DstBBDT'
                            , Threshold = props['BDT_Threshold']
                            , ParamFile = props['BDT_Lookup_Filename']
                            , ParticleDictTool = "%s/%s" % (varHandler.Type.getType(), varHandler.Name )
                            , tools = [varHandler]
                          )
        filtCut = "FILTER('%s/%s')" % (bdtTool.Type.getType(), bdtTool.Name )
        ## The dependence on PV3D('Hlt2') is there assuming that PVs might
        ##   be needed for some of the input variables.
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__( self, name, filtCut, inputs
                                     , dependencies = [ PV3D('Hlt2') ]
                                     , tools = [bdtTool ] )
    # }
# }

