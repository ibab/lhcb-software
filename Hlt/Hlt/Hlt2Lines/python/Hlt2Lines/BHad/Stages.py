##
## derived from CharmHad/Stages.py  160206

## Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Configurables import DaVinci__N4BodyDecays

## ========================================================================= ##
## Global event cuts
## ========================================================================= ##
##  the same as the class of the same name in CharmHad
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
        Hlt2VoidFilter.__init__(self, 'B2HHHH' + name, code, [tracks], shared = True, nickname = 'TrackGEC')


## ========================================================================= ##
## Filters for Input particles
## ========================================================================= ##
## ------------------------------------------------------------------------- ##
from Inputs import Hlt2LoosePions, Hlt2LooseKaons, Hlt2LooseMuons, Hlt2LooseProtons
from Inputs import Hlt2NoPIDsPions, Hlt2NoPIDsKaons, Hlt2NoPIDsProtons

## ------------------------------------------------------------------------- ##
## the same as the class of the same name in CharmHad
class TighterProtonFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs, nickname = None , shared = True  ):
        pidp_cut = "(PIDp > %(PIDp_MIN)s  )"
        deltaPID_cut = "( (PIDp-PIDK) > %(DeltaPID_MIN)s )"
        momentum_cut = "( P > %(P_MIN)s )"
        cut = pidp_cut + " & " + deltaPID_cut + " & " + momentum_cut
        nickname = name if nickname == None else nickname
        name     = name if not shared       else 'B2HHHH%sTighterProton' % name
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,
                                    nickname = nickname , shared = shared )

## ------------------------------------------------------------------------- ##
## the same as the class of the same name in CharmHad
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
## Names of shared particles  begin with B2HHHH to avoid name conflicts
##   with other subdirectories.
## These are all associated with specific combiners and should perhaps be
##   defined closer to that context.
## ------------------------------------------------------------------------- ##
## At least for the moment (160207), we need only protons for B0 --> p,p,pbar,pbar
## mds 160207SharedDetachedB0Child_pi = DetachedInParticleFilter( 'B2HHHHSharedDetachedB0Child_pi', 
## mds 160207                                                      [Hlt2LoosePions], 'PIDK', True )
## mds 160207SharedDetachedB0Child_Tightpi = DetachedInParticleFilter( 'B2HHHHSharedDetachedB0Child_Tightpi', 
## mds 160207                                                           [Hlt2LoosePions], 'PIDK', True )
## mds 160207SharedDetachedB0Child_K = DetachedInParticleFilter( 'B2HHHHSharedDetachedB0Child_K',
## mds 160207                                                     [Hlt2LooseKaons], 'PIDK' )
## mds 160207SharedDetachedB0Child_TightK = DetachedInParticleFilter( 'B2HHHHSharedDetachedB0Child_TightK',
## mds 160207                                                     [Hlt2LooseKaons], 'PIDK' )
SharedDetachedB0Child_Pr = DetachedInParticleFilter('B2HHHHSharedDetachedB0Child_Pr',
                                                   [Hlt2LooseProtons], 'PIDp' )
## use TighterProtonFilter to provide both absolute PIDp and DeltaPID thresholds
SharedDetachedB0Child_TightPr = TighterProtonFilter('B2HHHHSharedDetachedB0Child_TightPr',
                               inputs = [SharedDetachedB0Child_Pr], 
                               nickname = 'TighterProtons',
                               shared = True)


# Mass filter
## ------------------------------------------------------------------------- ##
## Ugh.  I do not think that i approve of this hack.  (P.S.)
## same code as def and class of same names in CharmHad
def refit_pvs_kwargs(reFitPVs) :
    if not reFitPVs :
        return {}
    return {'ReFitPVs' : True,
            'CloneFilteredParticles' : True}

class MassFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs, nickname = None, shared = False, reFitPVs=False ):
        cut = "in_range( %(Mass_M_MIN)s , M , %(Mass_M_MAX)s )"
        nickname = name if nickname == None else nickname
        name     = name if not shared       else 'B2HHHH%sMass' % name
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,
                                    nickname = nickname , shared = shared,
                                    **refit_pvs_kwargs(reFitPVs))


##  mimics DV4BCombiner in CharmHad; only the name has been changed
##  to avoid (potential) confusion
class BV4BCombiner(Hlt2Combiner):
    """
    Combiner for exclusive B0->hhhh lines.
    Explicitly intended for B --> p,p,pbar,pbar where the opening
    angles will be relatively small due to the lower Q-value, so
    the topo triggers are expected to be less efficient. Can probably
    be extended to p,pbar,Lambda,Lambda_bar easily enough.

    Configuration dictionaries must contain the following keys:
        'AM_MIN'        : Minimum mass of the n-body combination
        'AM_MAX'        : Maximum mass of the n-body combination
        'AM_34'         : Mass of the remaining 2 particles
        'AM_4'          : Mass of the last particle
        'ACHI2DOCA_MAX' : Maximum Doca-chi2 of each two-body combination (should be >= VCHI2NDOF)
        'ASUMPT_MIN'    : Lower limit on the sum of the PTs of the daughters
        'VCHI2PDOF_MAX' : Upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'BPVDIRA_MIN'   : Lower limit on DIRA wrt bestPV in MotherCut
        'BPVLTIME_MIN'  : Lower limit on Lifetime wrt bestPV in MotherCut
        'TisTosSpec'    : Configuration string of the Hlt1 TISTOS filter.
        'AMOM_MIN'      : combination minimum  momentum
        'BPT_MIN'       : mother minimum pT
        'BMOM_MIN'      : mother minimum momentum
    """
    def __init__(self, name, decay,inputs, shared = False, nickname = None):
        dc =  {}
        for child in ['pi+','K+','p+'] :
            dc[child] = "(PT > %(Trk_ALL_PT_MIN)s) & (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"
        c12 = (" ( AM < (%(AM_MAX)s - %(AM_34)s) ) " +
               "&( ACHI2DOCA(1,2) < %(ACHI2DOCA_MAX)s ) " +
               "&( ADOCA(1,2) < %(ADOCA_MAX)s ) " )
        c123 =(" ( AM < (%(AM_MAX)s - %(AM_4)s) ) " +
               "&( ACHI2DOCA(1,3) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(2,3) < %(ACHI2DOCA_MAX)s ) " +
               "&( ADOCA(1,3) < %(ADOCA_MAX)s ) " +
               "&( ADOCA(2,3) < %(ADOCA_MAX)s ) " )
        cc =  (" (in_range( %(AM_MIN)s, AM, %(AM_MAX)s )) " +
               "&( (APT1+APT2+APT3+APT4) > %(ASUMPT_MIN)s )" +
               "&( ACHI2DOCA(1,4) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(2,4) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(3,4) < %(ACHI2DOCA_MAX)s ) " +
               "&( ADOCA(1,4) < %(ADOCA_MAX)s ) " +
               "&( ADOCA(2,4) < %(ADOCA_MAX)s ) " +
               "&( ADOCA(3,4) < %(ADOCA_MAX)s ) " +
               "&( AP > %(AMOM_MIN)s )"    )
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > %(BPVDIRA_MIN)s )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )" +
                 " & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s )" +
                 " & (PT > %(BPT_MIN)s )" +
                 " & (P  > %(BMOM_MIN)s )" )
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs, shared = shared,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', combiner = DaVinci__N4BodyDecays, DaughtersCuts = dc,
                              Combination12Cut = c12, Combination123Cut = c123, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [], nickname = nickname)

## Shared instances of BV4BCombiner
## ------------------------------------------------------------------------- ##

B02HHHH_B02PPPbarPbar = BV4BCombiner('B2PPPbarPbar'
                , inputs = [SharedDetachedB0Child_TightPr]
                , decay = "B0 -> p+ p+ p~- p~-"
                , nickname = 'B02HHHH'  ## def in B02HHHHLines.py
                , shared = True)

## Nominal mass window filters for the B0 -> 4h combiners.  Shared for use
## in untagged lines, tagged lines, and other spectroscopy lines.
B02HHHHMass_B02PPPbarPbar   = MassFilter('B2PPPbarPbarFilt'
                , inputs = [B02HHHH_B02PPPbarPbar]
                , nickname = 'B2PPPbarPbarMass'      ## def in B02HHHHLines.py
                , shared = True)


