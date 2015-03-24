# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter

from Configurables import DaVinci__N3BodyDecays as N3BodyDecays

# The GEC
class TrackGEC(Hlt2VoidFilter):
    def __init__(self, name):
        from Configurables import LoKi__Hybrid__CoreFactory as Factory
        modules =  Factory().Modules
        for i in [ 'LoKiTrigger.decorators' ] :
          if i not in modules : modules.append(i)

        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking as Hlt2LongTracking
        tracks = Hlt2LongTracking().hlt2PrepareTracks()
        code = ("CONTAINS('%s')" % tracks.outputSelection()) + " < %(NTRACK_MAX)s"
        Hlt2VoidFilter.__init__(self, 'TrackGEC', code, [tracks], shared = True)

## some filters copied from CharmHad/Stages.py  150319


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
    name of the PID functor on which to cut and the configuration dictionary
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
    name of the PID functor on which to cut and the configuration dictionary
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
SharedDetachedCFChild_pi = DetachedInParticleFilter( 'SharedDetachedCFChild_pi', [Hlt2LoosePions], 'PIDK', True )
SharedDetachedCFChild_K  = DetachedInParticleFilter( 'SharedDetachedCFChild_K',
                                                     [Hlt2LooseKaons], 'PIDK' )

SharedDetachedSCSChild_pi = DetachedInParticleFilter( 'SharedDetachedSCSChild_pi', [Hlt2LoosePions], 'PIDK', True )
SharedDetachedSCSChild_K  = DetachedInParticleFilter( 'SharedDetachedSCSChild_K',
                                                     [Hlt2LooseKaons], 'PIDK' )

SharedDetachedDCSChild_pi = DetachedInParticleFilter( 'SharedDetachedDCSChild_pi', [Hlt2LoosePions], 'PIDK', True )
SharedDetachedDCSChild_K  = DetachedInParticleFilter( 'SharedDetachedDCSChild_K',
                                                     [Hlt2LooseKaons], 'PIDK' )

SharedNoPIDDetachedD0Child_pi = DetachedInParticleFilter( 'SharedNoPIDDetachedD0Child_pi', [Hlt2NoPIDsPions] )
SharedNoPIDDetachedD0Child_K = DetachedInParticleFilter( 'SharedNoPIDDetachedD0Child_K', [Hlt2NoPIDsKaons] )
SharedNoPIDDetachedLcChild_p = DetachedInParticleFilter( 'SharedNoPIDDetachedLcChild_p', [Hlt2NoPIDsProtons] )



##  end of filters copied from CharmHad/Stages.py  150319

# Mass filters
class MassFilterD_LL(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        cut = "in_range( %(Dz2HHKs_Mass_LL_MIN)s , M , %(Dz2HHKs_Mass_LL_MAX)s )"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)

class MassFilterD_DD(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        cut = "in_range( %(Dz2HHKs_Mass_DD_MIN)s , M , %(Dz2HHKs_Mass_DD_MAX)s )"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)

class KsFilter_LL(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        cut =( " (ADMASS('KS0')<%(KshhTFKsLLMothSymMassWin)s) " +
               " & (BPVVDCHI2 > %(KshhTFKsLLVtxPVDispChi2LL)s)" +
               " & (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(KshhTFKsLLBpvlTimeLL)s * ps)" +
               " & (VFASPF(VCHI2)<%(KshhTFKsLLVtxChi2UL)s)" +
               " & (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhTFKsLLTrkPVIPChi2LL)s ),1))" +
               " & (CHILDCUT((MIPCHI2DV(PRIMARY) > %(KshhTFKsLLTrkPVIPChi2LL)s ),2))" +
               " & ((CHILDCUT((PT> %(KshhTFKsLLTrkPtLL)s ) ,1)) or (CHILDCUT((PT> %(KshhTFKsLLTrkPtLL) s ),2)))" +
               " & (CHILDCUT((TRCHI2DOF < %(KshhTFKsLLTrkChi2UL)s),1))" )
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)


class KsFilter_DD(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        cut =( "   (ADMASS('KS0')<%(KshhTFKsDDSymMassWin)s) " +
               " & (BPVVDZ > %(KshhTFKsDDVtxPVDispZLL)s )" +
               " & ((CHILDCUT((PT> %(KshhTFKsDDTrkPtLL)s ) ,1)) or (CHILDCUT((PT> %(KshhTFKsDDTrkPtLL) s ),2)))" +
               " & (VFASPF(VCHI2)<%(KshhTFKsDDVtxChi2UL)s) " )
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)


# The class that creates Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner

## first create a class for the two "other" hadron tracks in D0 --> HHKs
class FakeKstarHmHp(Hlt2Combiner):
    def __init__(self, name, decay, inputs):

##  first, look at the HH in HHKs
##  define some generic cuts for HH pairs and get the methods to be used

        piK_cut =  ("(TRCHI2DOF < %(Trk_TRCHI2DOF_MAX)s )" +
                   "& (PT > %(Trk_PT_MIN)s)" +
                   "& (P > %(Trk_P_MIN)s)" +
                   "& (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)")
        pi_cut = ("(PIDK < %(Pion_PIDK_MAX)s) &"+piK_cut)
        K_cut  = ("(PIDK > %(Kaon_PIDK_MIN)s) &"+piK_cut)

## daughter cuts (that is, cuts for the individual hadron tracks)
        hhDc =    {'pi+' : pi_cut,
                   'K+'  : K_cut}

## combination cuts for the HH pairs
        hhCc =  ("(AM<%(HHMass_MAX)s)" +
                 " & (APT > %(KshhTFHHPtLL)s )" +
                 " & (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Trk_1OF3_MIPCHI2DV_MIN)s)) " +
                 " & (ANUM(MIPCHI2DV(PRIMARY) > %(Trk_2OF3_MIPCHI2DV_MIN)s) >= 2)" +
                 " & (AALLSAMEBPV)" +
                 " & ACUTDOCA(%(MaxDOCA)s,'')" )
## mother cuts for the HH pair
        hhMc =  ("   (VFASPF(VCHI2PDOF) < %(KshhTFHHVtxChi2UL)s)" +
                 " & (BPVVD > %(KshhTFHHFDLL)s )" )

## get primary vertices here as they are needed in building the HH pairs.
        from HltTracking.HltPVs import PV3D

        inputs = [Hlt2LoosePions]

## remove the daughter cuts as we are now using SharedDetached...  daughters
        Hlt2Combiner.__init__(self, 'FakeKstarPiPi',decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              CombinationCut = hhCc,
                              MotherCut = hhMc, Preambulo = [])

class HHKS0Combiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):

## HHKs combination cuts
        cc =    ("   (in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(D_ASUMPT_MIN)s )" +
                 " & (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Trk_1OF3_MIPCHI2DV_MIN)s))"+
                 " & (ANUM(MIPCHI2DV(PRIMARY) > %(Trk_2OF3_MIPCHI2DV_MIN)s) >= 2)")
## HHKs mother cuts
        mc =    ("(VFASPF(VCHI2PDOF) < %(D_VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > %(D_BPVDIRA_MIN)s )" +
                 " & (BPVVDCHI2 > %(D_BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(D_BPVLTIME_MIN)s )")

        c12Cut = ("(AM <  %(HHMass_MAX)s)" +
                  " & (ADOCAMAX('') < %(MaxDOCA)s)")


        from HltTracking.HltPVs import PV3D

        inputs=[localKsLL, Hlt2LoosePions]

        Hlt2Combiner.__init__(self, 'HHKs', decay, inputs,
                     dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                     tistos = 'TisTosSpec', combiner = N3BodyDecays,
                     Combination12Cut =  c12Cut,
                     CombinationCut = cc,
                     MotherCut = mc, Preambulo = [])

## orig                     DaughtersCuts = dc, CombinationCut = cc,

class HHKsCombiner(Hlt2Combiner):
    def __init__(self, name, inputs):

## There are no HHKs daughter cuts as the equivalents have already been applied in building the
## HH (FakeKstarHmHp) candidates and the Kshort candidates. 

## HHKs combination cuts
        cc =    ("   (in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2) > %(D_ASUMPT_MIN)s )" +
                 " & (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Trk_1OF3_MIPCHI2DV_MIN)s))"+
                 " & (ANUM(MIPCHI2DV(PRIMARY) > %(Trk_2OF3_MIPCHI2DV_MIN)s) >= 2)")
## HHKs mother cuts
        mc =    ("(VFASPF(VCHI2PDOF) < %(D_VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > %(D_BPVDIRA_MIN)s )" +
                 " & (BPVVDCHI2 > %(D_BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(D_BPVLTIME_MIN)s )")
        from HltTracking.HltPVs import PV3D

        Hlt2Combiner.__init__(self, 'Dz2KshPiPi_LL', "[D0 -> K*(892)0 KS0]cc", inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec',  CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

# Combiners
from Inputs import Hlt2LoosePions, Hlt2LooseKaons, Hlt2KsLL, Hlt2KsDD
localKsLL =  KsFilter_LL('HHKsLL',inputs=[Hlt2KsLL])
localKsDD =  KsFilter_DD('HHKsDD',inputs=[Hlt2KsDD])
localPiPi =  FakeKstarHmHp('HHKsPiPi',decay="[K*(892)0 -> pi- pi+]cc", inputs=[SharedDetachedCFChild_pi])
localKPi  =  FakeKstarHmHp('HHKsKPi', decay="[K*(892)0 -> K- pi+]cc",  
             inputs=[SharedDetachedSCSChild_pi,SharedDetachedSCSChild_K])
##  Ksh,K,K is not SCS but it is phase-space suppressed.So use the SCS PID to suppress rate??
localKK  =  FakeKstarHmHp('HHKsKPi',decay="[K*(892)0 -> K- K+]cc", inputs=[SharedDetachedSCSChild_K])
Dz2KshPiPi_LL = HHKsCombiner('Dz2KshPiPi_LL',inputs=[localKsLL, localPiPi])
Dz2KshPiPi_DD = HHKsCombiner('Dz2KshPiPi_DD',inputs=[localKsDD, localPiPi])
Dz2KshKPi_LL =    HHKsCombiner('Dz2KshKPi_LL',   inputs=[localKsLL, localKPi])
Dz2KshKPi_DD =    HHKsCombiner('Dz2KshKPi_DD',   inputs=[localKsDD, localKPi])
Dz2KshKK_LL =     HHKsCombiner('Dz2KshKK_LL',    inputs=[localKsLL, localKK])
Dz2KshKK_DD =     HHKsCombiner('Dz2KshKK_DD',    inputs=[localKsDD, localKK])

D02KshPiPi_LL  =  HHKS0Combiner('D02KshPiPi_LL', decay="[D0 ->  pi- pi+ KS0]cc",
                   inputs=[localKsLL, SharedDetachedCFChild_pi])
D02KshPiPi_DD  =  HHKS0Combiner('D02KshPiPi_DD', decay="[D0 ->  pi- pi+ KS0]cc",
                   inputs=[localKsDD, SharedDetachedCFChild_pi])
D02KshKPi_LL   =  HHKS0Combiner('D02KshKPi_LL', decay="[D0 ->  K- pi+ KS0]cc", 
                   inputs=[localKsLL, SharedDetachedSCSChild_K, SharedDetachedSCSChild_pi])
D02KshKPi_DD   =  HHKS0Combiner('D02KshKPi_DD', decay="[D0 ->  K- pi+ KS0]cc", 
                   inputs=[localKsDD, SharedDetachedSCSChild_K, SharedDetachedSCSChild_pi])
D02KshKK_LL    =  HHKS0Combiner('D02KshKK_LL', decay="[D0 ->  K- K+ KS0]cc", 
                   inputs=[localKsLL, SharedDetachedSCSChild_K])
D02KshKK_DD    =  HHKS0Combiner('D02KshKK_DD', decay="[D0 ->  K- K+ KS0]cc", 
                   inputs=[localKsDD, SharedDetachedSCSChild_K])
