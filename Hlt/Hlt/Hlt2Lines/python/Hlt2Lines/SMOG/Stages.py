## Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Configurables import DaVinci__N4BodyDecays
from Configurables import DaVinci__N5BodyDecays

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
        Hlt2VoidFilter.__init__(self, 'SMOG' + name, code, [tracks], shared = True, nickname = 'TrackGEC')


## ========================================================================= ##
## Filters for Input particles
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

## ------------------------------------------------------------------------- ##
from Inputs import Hlt2LoosePions, Hlt2LooseKaons, Hlt2LooseProtons
from Inputs import Hlt2NoPIDsPions, Hlt2NoPIDsKaons, Hlt2NoPIDsProtons
## ------------------------------------------------------------------------- ##
class TighterProtonFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs, nickname = None , shared = True  ):
        pidp_cut = "(PIDp > %(PIDp_MIN)s  )"
        deltaPID_cut = "( (PIDp-PIDK) > %(DeltaPID_MIN)s )"
        momentum_cut = "( P > %(P_MIN)s )"
        cut = pidp_cut + " & " + deltaPID_cut + " & " + momentum_cut
        nickname = name if nickname == None else nickname
        name     = name if not shared       else 'SMOG%sTighterProton' % name
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,
                                    nickname = nickname , shared = shared )

##  --------------

## Shared instances of DetachedInParticleFilter
## Names of shared particles  begin with SMOG to avoid name conflicts
##   with other subdirectories.
## These are all associated with specific combiners and should perhaps be
##   defined closer to that context.
## ------------------------------------------------------------------------- ##
SharedDpmChild_pi = InParticleFilter( 'SMOGSharedDpmChild_pi', 
                                                      [Hlt2LoosePions], 'PIDK', True )
SharedDpmChild_Tightpi = InParticleFilter( 'SMOGSharedDpmChild_Tightpi', 
                                                           [Hlt2LoosePions], 'PIDK', True )
SharedDpmChild_K = InParticleFilter( 'SMOGSharedDpmChild_K',
                                                     [Hlt2LooseKaons], 'PIDK' )
SharedDpmChild_TightK = InParticleFilter( 'SMOGSharedDpmChild_TightK',
                                                     [Hlt2LooseKaons], 'PIDK' )
SharedLcChild_pi = InParticleFilter('SMOGSharedLcChild_pi',
                                                    [Hlt2LoosePions], 'PIDK', True )
SharedLcChild_K = InParticleFilter( 'SMOGSharedLcChild_K',
                                                    [Hlt2LooseKaons], 'PIDK' )
SharedLcChild_p = InParticleFilter('SMOGSharedLcChild_p',
                                                   [Hlt2LooseProtons], 'PIDp' )
SharedTighterLcChild_p = TighterProtonFilter('SharedLcChild',
                               inputs = [SharedLcChild_p], 
                               nickname = 'TighterProtons',
                               shared = True)

## ========================================================================= ##
## Filters for composite particles
## ========================================================================= ##

# Mass filter
class MassFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs, nickname = None, shared = False, reFitPVs=False ):
        cut = "in_range( %(Mass_M_MIN)s , M , %(Mass_M_MAX)s )"
        nickname = name if nickname == None else nickname
        name     = name if not shared       else 'SMOG%sMass' % name
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,
                                    nickname = nickname , shared = shared )


## ========================================================================= ##
## 2-body Combiner
## ========================================================================= ##
class HHCombiner(Hlt2Combiner) : # {
    """
    Combiner for 2 basic track-based particles.  
    Should use no cuts that require knowledge of the PV.
    Always creates a shared instance of the filter.

    Configuration dictionaries must contain the following keys:
        ## P.S. -- If filtered input particles are used, i would prefer that
        ##         all filtering that applies to every charged input be done
        ##         in those filters to prevent inadvertent configuration
        ##         mismatches.
        'Trk_ALL_PT_MIN'        :  lower limit on PT for all inputs
        'Trk_ALL_P_MIN'         :  lower limit on P for all inputs

        'Comb_AM_MIN'
        'Comb_AM_MAX'       : lower and upper limits on AM in CombinationCut
        'Trk_Max_APT_MIN'   : lower limit on largest product PT (APT1 or APT2)
        'Pair_AMINDOCA_MAX' : upper limit on AMINDOCA('') in CombinationCut
        'D0_VCHI2PDOF_MAX'  : upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'TisTosSpec'        : configuration string of the Hlt1 TISTOS filter.
    """
    def __init__(self, name, decay, inputs, nickname = None, shared = False, **kwargs) : # {
        '''**kwargs can be anything accepted by the Hlt2Combiner constructor, eg, to enable PV refitting use
        ReFitPVs = True.'''
        dc = { }
        for child in ['pi+','K+','p+'] :
            dc[child] = "(PT > %(Trk_ALL_PT_MIN)s)" \
                        "& (P > %(Trk_ALL_P_MIN)s)" 

        ## Assume that the wide mass range is wider than the narrow range.
        combcuts = "in_range(%(Comb_AM_MIN)s,  AM, %(Comb_AM_MAX)s)" \
                   "& ((APT1 > %(Trk_Max_APT_MIN)s) " \
                       "| (APT2 > %(Trk_Max_APT_MIN)s))" \
                   "& (AMINDOCA('') " \
                       "< %(Pair_AMINDOCA_MAX)s )"

        parentcuts = "(VFASPF(VCHI2PDOF) < %(D0_VCHI2PDOF_MAX)s)" 

        name = name if not shared else 'SMOG' + name
        Hlt2Combiner.__init__( self, name, decay, inputs,
                               dependencies = [TrackGEC('TrackGEC')],
                               tistos = 'TisTosSpec',
                               nickname = nickname,
                               shared = shared,
                               DaughtersCuts = dc,
                               CombinationCut = combcuts,
                               MotherCut = parentcuts,
                               Preambulo = [],
                               **kwargs)
    # }
# }

## ========================================================================= ##
## 3-body Combiners
## ========================================================================= ##

# ------------------------------------------------------------------------- ##
class HHHCombiner(Hlt2Combiner) : # {
    """
    Combiner for 3 basic track-based particles.  
    Should use no cuts that require knowledge of the PV.
    Always creates a shared instance of the filter.

    Configuration dictionaries must contain the following keys:
        'Trk_1OF3_PT_MIN'
        'Trk_2OF3_PT_MIN'
        'Trk_ALL_PT_MIN'        : tiered lower limits on product PT.
                                  All 3 must pass the ALL threshold.
                                  At least 2 must pass the 2OF3 threshold.
                                  At least 1 must pass the 1OF3 threshold.
        'AM_MIN'
        'AM_MAX'                : lower and upper limits on AM in CombinationCut
        'ASUMPT_MIN'            : lower limit on APT1+APT2+APT3 in CombinationCut
        'VCHI2PDOF_MAX'         : upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'TisTosSpec'            : configuration string of the Hlt1 TISTOS filter.
    """
    def __init__(self, name, decay, inputs, nickname = None) : # {
        dc =    {}
        for child in ['pi+','K+','p+'] :
            dc[child] = "(PT > %(Trk_ALL_PT_MIN)s)"
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(ASUMPT_MIN)s )" +
                 " & (AHASCHILD(PT > %(Trk_1OF3_PT_MIN)s))"+
                 " & (ANUM(PT > %(Trk_2OF3_PT_MIN)s) >= 2)")
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)")

        nickname = name if nickname == None else nickname
        
        Hlt2Combiner.__init__(self, "SMOG" + name, decay, inputs,
                              nickname = nickname, dependencies = [TrackGEC('TrackGEC')],
                              shared = True, tistos = 'TisTosSpec', DaughtersCuts = dc,
                              CombinationCut = cc, MotherCut = mc, Preambulo = [])
    # }
# }


## Shared instances of HHHCombiner
## ------------------------------------------------------------------------- ##
DpToKmPipPip = HHHCombiner( 'DpToKmPipPip'
        , decay = "[D+ -> K- pi+ pi+]cc"
        , inputs = [ SharedDpmChild_K, SharedDpmChild_pi ]
        , nickname = 'Dpm2HHH' )   ## 'Dpm2HHH' def in Lines.py

DspToKmKpPim = HHHCombiner( 'DspToKmKpPim'
        , decay = "[D_s+ -> K- K+ pi+]cc"
        , inputs = [ SharedDpmChild_K, SharedDpmChild_pi ]
        , nickname = 'Ds2HHH' )    ## 'Ds2HHH' def in Lines.py

LcpToKmPpPip = HHHCombiner( 'LcpToKmPpPip'
        , decay = "[Lambda_c+ -> K- p+ pi+]cc"
        , inputs = [ SharedTighterLcChild_p, SharedLcChild_K,
                     SharedLcChild_pi ]
        , nickname = 'Lc2HHH' ) ## 'Lc2HHH' defined in Lines.py

D02KPi = HHCombiner('D02KPi'
        , decay = "[D0 -> K- pi+]cc"
        , inputs = [ SharedDpmChild_K, SharedDpmChild_pi ]
        , nickname = 'D02HH',shared=True ) ## 'D02HH' defined in Lines.py


