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
        Hlt2VoidFilter.__init__(self, name, code, [tracks])

# Mass filter
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
class MassFilterD(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        cut = "in_range( %(D_Mass_M_MIN)s , M , %(D_Mass_M_MAX)s )"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs)

class MassFilterLc(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        cut = "in_range( %(Lc_Mass_M_MIN)s , M , %(Lc_Mass_M_MAX)s )"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs)

# The class that creates the Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
class HHHCombiner(Hlt2Combiner):
    def __init__(self, name):
        piK_cut =  ("(TRCHI2DOF < %(Trk_TRCHI2DOF_MAX)s )" +
                   "& (PT > %(Trk_PT_MIN)s)" +
                   "& (P > %(Trk_P_MIN)s)" +
                   "& (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)")
        pi_cut = ("(PIDK < %(Pion_PIDK_MAX)s) &"+piK_cut)
        K_cut  = ("(PIDK > %(Kaon_PIDK_MIN)s) &"+piK_cut)
        p_cut  = ("(PIDp > %(Proton_PIDp_MIN)s) &"+piK_cut)
        dc =    {'pi+' : pi_cut,
                 'K+'  : K_cut,
                 'p+'  : p_cut}
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(D_ASUMPT_MIN)s )" +
                 " & (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Trk_1OF3_MIPCHI2DV_MIN)s))"+
                 " & (ANUM(MIPCHI2DV(PRIMARY) > %(Trk_2OF3_MIPCHI2DV_MIN)s) >= 2)")
        mc =    ("(VFASPF(VCHI2PDOF) < %(D_VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > %(D_BPVDIRA_MIN)s )" +
                 " & (BPVVDCHI2 > %(D_BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(D_BPVLTIME_MIN)s )")
        from HltTracking.HltPVs import PV3D
        from Inputs import Hlt2LoosePions, Hlt2LooseKaons,Hlt2LooseProtons
        inputs = [Hlt2LooseKaons, Hlt2LoosePions]
        Hlt2Combiner.__init__(self, 'KPiPi_SS', "[D+ -> K- pi+ pi+]cc", inputs,
                              dependencies = [TrackGEC('GEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

# Combiners
from Inputs import Hlt2LoosePions, Hlt2LooseKaons,Hlt2LooseProtons
D2KPiPi_SS  = HHHCombiner('KPiPi_SS')
D2KPiPi_OS  = D2KPiPi_SS.clone('KPiPi_OS', decay = "[D+ -> K+ pi+ pi-]cc",
                                           inputs = [Hlt2LooseKaons, Hlt2LoosePions])
D2KKPi_SS   = D2KPiPi_SS.clone('KKPi_SS',  decay = "[D+ -> K+ K+ pi-]cc",
                                           inputs = [Hlt2LooseKaons, Hlt2LoosePions])
D2KKPi_OS   = D2KPiPi_SS.clone('KKPi_OS',  decay = "[D+ -> K+ K- pi+]cc",
                                           inputs = [Hlt2LooseKaons, Hlt2LoosePions])
D2PiPiPi    = D2KPiPi_SS.clone('PiPiPi',   decay = "[D+ -> pi+ pi+ pi-]cc",
                                                inputs = [Hlt2LoosePions])
D2KKK       = D2KPiPi_SS.clone('KKK',      decay = "[D+ -> K+ K+ K-]cc",
                                                inputs = [Hlt2LooseKaons])
#
Lc2KPPi     = D2KPiPi_SS.clone('KPPi',     decay = "[Lambda_c+ -> K- p+ pi+]cc",
                                           inputs = [Hlt2LooseKaons, Hlt2LoosePions, Hlt2LooseProtons])     
Lc2KPK      = D2KPiPi_SS.clone('KPK',      decay = "[Lambda_c+ -> K- p+ K+]cc",
                                           inputs = [Hlt2LooseKaons, Hlt2LooseProtons])   
Lc2PiPPi    = D2KPiPi_SS.clone('PiPPi',    decay = "[Lambda_c+ -> pi- p+ pi+]cc",
                                           inputs = [Hlt2LoosePions, Hlt2LooseProtons])   
Lc2PiPK     = D2KPiPi_SS.clone('PiPK',     decay = "[Lambda_c+ -> pi- p+ K+]cc",
                                           inputs = [Hlt2LooseKaons, Hlt2LoosePions, Hlt2LooseProtons])   


