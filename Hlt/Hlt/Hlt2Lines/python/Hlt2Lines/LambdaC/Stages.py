# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter

linePrefix = ''

class TrackGEC(Hlt2VoidFilter):
    def __init__(self, name):
        from Configurables import LoKi__Hybrid__CoreFactory as Factory
        modules =  Factory().Modules
        for i in [ 'LoKiTrigger.decorators' ] : 
            if i not in modules : modules.append(i)
                                                           
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking as Hlt2LongTracking
        tracks = Hlt2LongTracking().hlt2PrepareTracks()
        code = ("CONTAINS('%s')" % tracks.outputSelection()) + " < %(NTRACK_MAX)s"
        Hlt2VoidFilter.__init__(self, linePrefix + 'Track', 'GEC', code, [tracks])

from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
class LambdaC2KPPiCombiner(Hlt2Combiner):
    def __init__(self, name):
        piK_cut = ("(TRCHI2DOF < %(Trk_TRCHI2DOF_MAX)s )" +
                   "& (PT > %(Trk_PT_MIN)s)" +
                   "& (MIPCHI2DV(PRIMARY) > %(Trk_MIPCHI2DV_MIN)s)")
        p_cut = ("(TRCHI2DOF < %(Trk_TRCHI2DOF_MAX)s )" +
                 "& (PT > %(P_PT_MIN)s)" +
                 "& (P > %(P_P_MIN)s)" +
                 "& (MIPCHI2DV(PRIMARY) > %(Trk_MIPCHI2DV_MIN)s)")
        cuts = dict(DaughtersCuts  = {'pi+' : piK_cut, 'K+' : piK_cut, 'p+' : p_cut },
                    CombinationCut = ("(in_range( %(Lc_AM_MIN)s, AM, %(Lc_AM_MAX)s ))" +
                                      "& ((APT1+APT2+APT3) > %(Lc_ASUMPT_MIN)s )" +
                                      "& (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Trk_1of3_MIPCHI2DV_MIN)s))"),
                     MotherCut =     ("(VFASPF(VCHI2PDOF) < %(Lc_VCHI2PDOF_MAX)s)" +
                                      "& (BPVDIRA > %(Lc_BPVDIRA_MIN)s )" +
                                      "& (BPVVDR < %(Lc_BPVVDR_MAX)s)" +
                                      "& (BPVVDCHI2 > %(Lc_BPVVDCHI2_MIN)s )" +
                                      "& (BPVLTIME() < %(Lc_BPVLTIME_MAX)s )"))
        from HltTracking.HltPVs import PV3D
        from Inputs import Hlt2LoosePions, Hlt2LooseKaons, Hlt2LooseProtons
        inputs = [Hlt2LooseKaons, Hlt2LooseProtons, Hlt2LoosePions]
        Hlt2Combiner.__init__(self, linePrefix, 'KPPi', "[Lambda_c+ -> K- p+ pi+]cc", cuts, inputs,
                              dependencies = [TrackGEC(linePrefix + 'TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec')

# Combiners
from Inputs import Hlt2LoosePions, Hlt2LooseKaons, Hlt2LooseProtons
LambdaC2KPPi  = LambdaC2KPPiCombiner('KPPi')
LambdaC2KPK   = LambdaC2KPPi.clone('KPK', decay = "[Lambda_c+ -> K- p+ K+]cc",
                                   inputs = [Hlt2LooseKaons, Hlt2LooseProtons])
LambdaC2PiPPi = LambdaC2KPPi.clone('PiPPi', decay = "[Lambda_c+ -> pi- p+ pi+]cc",
                                   inputs = [Hlt2LoosePions, Hlt2LooseProtons])
LambdaC2PiPK  = LambdaC2KPPi.clone('PiPK', decay = "[Lambda_c+ -> pi- p+ K+]cc")
        
# Mass filter
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
class LambdaCMassFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        cut = "in_range( %(SigMass_M_MIN)s, M, %(SigMass_M_MAX)s )"
        Hlt2ParticleFilter.__init__(self, linePrefix, name, cut, inputs)

# Final selections
LC2KPPi  = LambdaCMassFilter("KPPi",  inputs = [LambdaC2KPPi])
LC2KPK   = LambdaCMassFilter("KPK",   inputs = [LambdaC2KPK])
LC2PiPPi = LambdaCMassFilter("PiPPi", inputs = [LambdaC2KPPi])
LC2PiPK  = LambdaCMassFilter("PiPK",  inputs = [LambdaC2PiPK])
