# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Inputs import KS0_LL, KS0_DD, Lambda_LL, Lambda_DD

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

# Mass filter
class MassFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        cut = "in_range( %(Mass_M_MIN)s , M , %(Mass_M_MAX)s )"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs)

# A "universal" filter which implements a soft pion tag
class TagDecay(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        dc =    {}
        # OK, a bit of syntax parsing
        DecayRHS = decay.split('->')[1]
        DecayRHS = DecayRHS.rstrip(']cc').rstrip(' ')
        DecayRHS = DecayRHS.lstrip(' ')
        DecayRHS = DecayRHS.split(' ')
        ## ALL should be the default; this should be unneceesary.
        for part in DecayRHS :
            dc[part] = "ALL"
        cc =    ('in_range( %(DeltaM_AM_MIN)s, (AM - AM1), %(DeltaM_AM_MAX)s )')
        mc =    ("(VFASPF(VCHI2PDOF) < %(TagVCHI2PDOF_MAX)s)" +
                 "& in_range( %(DeltaM_MIN)s, (M - M1), %(DeltaM_MAX)s )")
 
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc, 
                              MotherCut = mc, Preambulo = []) 

# The filter for the input particles
class DetachedInParticleFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        cut = ("  (TRCHI2DOF < %(Trk_ALL_TRCHI2DOF_MAX)s )" +
               "& (PT > %(Trk_ALL_PT_MIN)s)" +
               "& (P > %(Trk_ALL_P_MIN)s)" +
               "& (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)")

        from Inputs import Hlt2LoosePions, Hlt2LooseKaons,Hlt2LooseProtons       
 
        pidinfo = {"pi" : {"cut"    : "(PIDK < %(Pion_ALL_PIDK_MAX)s) &",
                           "inputs" : [Hlt2LoosePions]},
                   "K"  : {"cut"    : "(PIDK > %(Kaon_ALL_PIDK_MIN)s) &",
                           "inputs" : [Hlt2LooseKaons]},
                   "p"  : {"cut"    : "(PIDp > %(Proton_ALL_PIDp_MIN)s) &",
                           "inputs" : [Hlt2LooseProtons]}
                  }

        ## This is soooo unsafe.  There has to be an easier and safer way
        ##    to do this.
        cut = pidinfo[name.split('_')[1]]["cut"] + cut
        inputs = pidinfo[name.split('_')[1]]["inputs"]

        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True, 
                                    dependencies = [PV3D('Hlt2')],
                                    UseP2PVRelations = False)

class InParticleFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        cut = ("  (TRCHI2DOF < %(Trk_ALL_TRCHI2DOF_MAX)s )" +
               "& (PT > %(Trk_ALL_PT_MIN)s)" +
               "& (P > %(Trk_ALL_P_MIN)s)" )

        from Inputs import Hlt2LoosePions, Hlt2LooseKaons,Hlt2LooseProtons    
 
        pidinfo = {"pi" : {"cut"    : "(PIDK < %(Pion_ALL_PIDK_MAX)s) &",
                           "inputs" : [Hlt2LoosePions]},
                   "K"  : {"cut"    : "(PIDK > %(Kaon_ALL_PIDK_MIN)s) &",
                           "inputs" : [Hlt2LooseKaons]},
                   "p"  : {"cut"    : "(PIDp > %(Proton_ALL_PIDp_MIN)s) &",
                           "inputs" : [Hlt2LooseProtons]}
                  }   

        ## This is soooo unsafe.  There has to be an easier and safer way
        ##    to do this.
        cut = pidinfo[name.split('_')[1]]["cut"] + cut 
        inputs = pidinfo[name.split('_')[1]]["inputs"]

        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)

class NoPIDInParticleFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        cut = ("  (TRCHI2DOF < %(Trk_ALL_TRCHI2DOF_MAX)s )" +
               "& (PT > %(Trk_ALL_PT_MIN)s)" +
               "& (P > %(Trk_ALL_P_MIN)s)" +
               "& (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)")

        from Inputs import Hlt2NoPIDsPions, Hlt2NoPIDsKaons,Hlt2NoPIDsProtons

        pidinfo = {"pi" : {"cut"    : "",
                           "inputs" : [Hlt2NoPIDsPions]},
                   "K"  : {"cut"    : "",
                           "inputs" : [Hlt2NoPIDsKaons]},
                   "p"  : {"cut"    : "",
                           "inputs" : [Hlt2NoPIDsProtons]}
                  } 

        ## This is soooo unsafe.  There has to be an easier and safer way
        ##    to do this.
        inputs = pidinfo[name.split('_')[1]]["inputs"]

        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)

class SoftTagInParticleFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        cut = ("  (TRCHI2DOF < %(Trk_ALL_TRCHI2DOF_MAX)s )" +
               "& (PT > %(Trk_ALL_PT_MIN)s)" +
               "& (P > %(Trk_ALL_P_MIN)s)" )

        from Inputs import Hlt2NoPIDsPions, Hlt2NoPIDsKaons,Hlt2NoPIDsProtons

        pidinfo = {"pi" : {"cut"    : "", 
                           "inputs" : [Hlt2NoPIDsPions]},
                   "K"  : {"cut"    : "", 
                           "inputs" : [Hlt2NoPIDsKaons]},
                   "p"  : {"cut"    : "", 
                           "inputs" : [Hlt2NoPIDsProtons]}
                  }   

        ## This is soooo unsafe.  There has to be an easier and safer way
        ##    to do this.
        inputs = pidinfo[name.split('_')[1]]["inputs"]

        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)

class DetachedHHHCombiner(Hlt2Combiner):
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
        Hlt2Combiner.__init__(self, "CharmHad" + type(self).__name__, decay, inputs,
                              nickname = name, dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              shared = True, tistos = 'TisTosSpec', DaughtersCuts = dc,
                              CombinationCut = cc, MotherCut = mc, Preambulo = [])

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
    def __init__(self, name, decay,inputs):
        dc =    {'KS0'    : ( "(PT > %(KS0_ALL_PT_MIN)s) &"+
                              "(MIPCHI2DV(PRIMARY) > %(KS0_ALL_MIPCHI2DV_MIN)s)" )
                }
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


## Combiner class for D0 -> h h' decays
class D02HHCombiner(Hlt2Combiner) : # {
    def __init__(self, name, decay, inputs, slotName = None, shared = False) : # {

        incuts = "(TRCHI2DOF< %(Trk_TRCHI2DOF_MAX)s )" \
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
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') " \
                       "< %(Pair_AMINDOCA_MAX)s )"

        parentcuts = "(VFASPF(VCHI2PDOF) < %(D0_VCHI2PDOF_MAX)s)" \
                     "& (BPVVDCHI2> %(D0_BPVVDCHI2_MIN)s )" \
                     "& (BPVDIRA > %(D0_BPVDIRA_MIN)s )"

        from HltTracking.HltPVs import PV3D
        ## The name passed to the base class constructor determines the
        ##   configuration dictionary that is picked up.
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


class D2KPiPi_SS(DetachedHHHCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> K- pi+ pi+]cc"
        kaonsForD2DetachedHHHCombiner = DetachedInParticleFilter("SharedDetachedDpmChild_K")
        pionsForD2DetachedHHHCombiner = DetachedInParticleFilter("SharedDetachedDpmChild_pi")
        inputs = [kaonsForD2DetachedHHHCombiner,pionsForD2DetachedHHHCombiner]
        DetachedHHHCombiner.__init__(self,name,decay,inputs)

class D2KPiPi_OS(DetachedHHHCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> K+ pi- pi+]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedDpmChild_K"),
                  DetachedInParticleFilter("SharedDetachedDpmChild_pi")]
        DetachedHHHCombiner.__init__(self,name,decay,inputs)

class D2KKPi_SS(DetachedHHHCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> K+ K+ pi-]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedDpmChild_K"),
                  DetachedInParticleFilter("SharedDetachedDpmChild_pi")]
        DetachedHHHCombiner.__init__(self,name,decay,inputs)

class D2KKPi_OS(DetachedHHHCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> K- K+ pi+]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedDpmChild_K"),
                  DetachedInParticleFilter("SharedDetachedDpmChild_pi")]
        DetachedHHHCombiner.__init__(self,name,decay,inputs)

class D2PiPiPi(DetachedHHHCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> pi- pi+ pi+]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedDpmChild_pi")]
        DetachedHHHCombiner.__init__(self,name,decay,inputs)

class D2KKK(DetachedHHHCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> K- K+ K+]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedDpmChild_K")]
        DetachedHHHCombiner.__init__(self,name,decay,inputs)

class Lc2KPPi(DetachedHHHCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> K- p+ pi+]cc"
        kaonsForLc2DetachedHHHCombiner = DetachedInParticleFilter("SharedDetachedLcChild_K")
        pionsForLc2DetachedHHHCombiner = DetachedInParticleFilter("SharedDetachedLcChild_pi")
        protonsForLc2DetachedHHHCombiner = DetachedInParticleFilter("SharedDetachedLcChild_p")
        inputs = [kaonsForLc2DetachedHHHCombiner,pionsForLc2DetachedHHHCombiner,protonsForLc2DetachedHHHCombiner]
        DetachedHHHCombiner.__init__(self,name,decay,inputs)

class Lc2KPK(DetachedHHHCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> K- p+ K+]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedLcChild_K"),
                  DetachedInParticleFilter("SharedDetachedLcChild_p")]
        DetachedHHHCombiner.__init__(self,name,decay,inputs)

class Lc2PiPPi(DetachedHHHCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> pi- p+ pi+]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedLcChild_pi"),
                  DetachedInParticleFilter("SharedDetachedLcChild_p")]
        DetachedHHHCombiner.__init__(self,name,decay,inputs)

class Lc2PiPK(DetachedHHHCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> pi- p+ K+]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedLcChild_K"),
                  DetachedInParticleFilter("SharedDetachedLcChild_pi"),
                  DetachedInParticleFilter("SharedDetachedLcChild_p")]
        DetachedHHHCombiner.__init__(self,name,decay,inputs)

# The lifetime unbiased lines now

class D2KPiPi_SS_LTUNB(HHHCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> K- pi+ pi+]cc"
        kaonsForPromptHHHCombiner = InParticleFilter("SharedPromptChild_K")
        pionsForPromptHHHCombiner = InParticleFilter("SharedPromptChild_pi")
        inputs = [kaonsForPromptHHHCombiner,pionsForPromptHHHCombiner]
        HHHCombiner.__init__(self,name,decay,inputs)

class D2KKPi_OS_LTUNB(HHHCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> K- K+ pi+]cc"
        inputs = [InParticleFilter("SharedPromptChild_K"),
                  InParticleFilter("SharedPromptChild_pi")]
        HHHCombiner.__init__(self,name,decay,inputs)

class Lc2KPPi_LTUNB(HHHCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> K- p+ pi+]cc"
        protonsForPromptHHHCombiner = InParticleFilter("SharedPromptChild_p")
        inputs = [InParticleFilter("SharedPromptChild_K"),
                  InParticleFilter("SharedPromptChild_pi"),
                  protonsForPromptHHHCombiner]
        HHHCombiner.__init__(self,name,decay,inputs)

# The PID calib lines now

class Lc2KPPi_PIDCALIB(DetachedHHHCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> K- p+ pi+]cc"
        protonsForLc2KPPiPIDCALIBCombiner = NoPIDInParticleFilter("SharedNoPIDLcChild_p")
        inputs = [DetachedInParticleFilter("SharedDetachedLcChild_K"),
                  DetachedInParticleFilter("SharedDetachedLcChild_pi"),
                  protonsForLc2KPPiPIDCALIBCombiner]
        DetachedHHHCombiner.__init__(self,name,decay,inputs)

# The V0H lines now

class D2KS0Pi_LL(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> pi+ KS0]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedDpmChild_pi"),
                  KS0_LL]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)

class D2KS0K_LL(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> K+ KS0]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedDpmChild_K"),
                  KS0_LL]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)        

class D2KS0Pi_DD(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> pi+ KS0]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedDpmChild_pi"),
                  KS0_DD]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)

class D2KS0K_DD(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[D+ -> K+ KS0]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedDpmChild_K"),
                  KS0_DD]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)

class Lc2LambdaPi_LL(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> pi+ Lambda0]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedLcChild_pi"),
                  Lambda_LL]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)

class Lc2LambdaK_LL(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> K+ Lambda0]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedLcChild_K"),
                  Lambda_LL]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)

class Lc2LambdaPi_DD(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> pi+ Lambda0]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedLcChild_pi"),
                  Lambda_DD]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)

class Lc2LambdaK_DD(DetachedV0HCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> K+ Lambda0]cc"
        inputs = [DetachedInParticleFilter("SharedDetachedLcChild_K"),
                  Lambda_DD]
        DetachedV0HCombiner.__init__(self,name,decay,inputs)

# The V0V0 lines now

class D2KS0KS0_2LL(DetachedV0V0Combiner) :
    def __init__(self,name) :
        decay = "D0 -> KS0 KS0"
        inputs = [KS0_LL]
        DetachedV0V0Combiner.__init__(self,name,decay,inputs)
