# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter


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

# Mass filters
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
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
        hhCc =  ("   (AM<1450*MeV)" +
                 " & (APT > %(KshhTFHHPtLL)s )" +
                 " & (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Trk_1OF3_MIPCHI2DV_MIN)s)) " +
                 " & (ANUM(MIPCHI2DV(PRIMARY) > %(Trk_2OF3_MIPCHI2DV_MIN)s) >= 2)" +
                 " & (AALLSAMEBPV)" )
## mother cuts for the HH pair
        hhMc =  ("   (VFASPF(VCHI2PDOF) < %(KshhTFHHVtxChi2UL)s)" +
                 " & (BPVVD > %(KshhTFHHFDLL)s )" )

## get primary vertices here as they are needed in building the HH pairs.
        from HltTracking.HltPVs import PV3D

        inputs = [Hlt2LoosePions]
        Hlt2Combiner.__init__(self, 'FakeKstarPimPip',"[K*(892)0 -> pi- pi+]cc", inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              DaughtersCuts = hhDc, CombinationCut = hhCc,
                              MotherCut = hhMc, Preambulo = [])

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

        Hlt2Combiner.__init__(self, 'Dz2KshPimPip_LL', "[D0 -> K*(892)0 KS0]cc", inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec',  CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

# Combiners
from Inputs import Hlt2LoosePions, Hlt2LooseKaons, Hlt2KsLL, Hlt2KsDD
localKsLL =  KsFilter_LL('HHKsLL',inputs=[Hlt2KsLL])
localKsDD =  KsFilter_DD('HHKsDD',inputs=[Hlt2KsDD])
localPiPi =  FakeKstarHmHp('HHKsPiPi',decay="[K*(892)0 -> pi- pi+]cc", inputs=[Hlt2LoosePions])
localKPi  =  FakeKstarHmHp('HHKsKPi', decay="[K*(892)0 -> K- pi+]cc",  inputs=[Hlt2LoosePions,Hlt2LooseKaons])
localKK  =  FakeKstarHmHp('HHKsKPi',decay="[K*(892)0 -> K- K+]cc", inputs=[Hlt2LooseKaons])
Dz2KshPimPip_LL = HHKsCombiner('Dz2KshPimPip_LL',inputs=[localKsLL, localPiPi])
Dz2KshPimPip_DD = HHKsCombiner('Dz2KshPimPip_DD',inputs=[localKsDD, localPiPi])
Dz2KshKPi_LL =    HHKsCombiner('Dz2KshKPi_LL',   inputs=[localKsLL, localKPi])
Dz2KshKPi_DD =    HHKsCombiner('Dz2KshKPi_DD',   inputs=[localKsDD, localKPi])
Dz2KshKK_LL =     HHKsCombiner('Dz2KshKK_LL',    inputs=[localKsLL, localKK])
Dz2KshKK_DD =     HHKsCombiner('Dz2KshKK_DD',    inputs=[localKsDD, localKK])

