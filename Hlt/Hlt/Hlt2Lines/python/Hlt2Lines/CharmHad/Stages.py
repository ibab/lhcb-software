# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter

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
        for part in DecayRHS :
            dc[part] = "ALL"
        cc =    ('  ((AM - AM1) < %(DeltaM_AM_MAX)s)' +
                 '& ((AM1 - AM) > %(DeltaM_AM_MIN)s)' )
        mc =    ("(VFASPF(VCHI2PDOF) < %(TagVCHI2PDOF_MAX)s)" +
                 "& ((M - M1) < %(DeltaM_MAX)s)" +
                 "& ((M1 - M) > %(DeltaM_MIN)s)")  
 
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

        inputs = pidinfo[name.split('_')[1]]["inputs"]

        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)

class DetachedHHHCombiner(Hlt2Combiner):
    def __init__(self, name, decay,inputs):
        if name.find('PIDCALIB')>-1 :
            dc =    {'pi+' : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)",
                     'K+'  : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)",
                     'p+'  : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"}
        else :
            dc =    {'pi+' : "ALL",
                     'K+'  : "ALL",
                     'p+'  : "ALL"}
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

class HHHCombiner(Hlt2Combiner):
    def __init__(self, name, decay,inputs):
        dc =    {'pi+' : "ALL",
                 'K+'  : "ALL",
                 'p+'  : "ALL"}
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

class Lc2KPPi_PIDCALIB(DetachedHHHCombiner) :
    def __init__(self,name) :
        decay = "[Lambda_c+ -> K- p+ pi+]cc"
        protonsForLc2KPPiPIDCALIBCombiner = NoPIDInParticleFilter("SharedNoPIDLcChild_p")
        inputs = [DetachedInParticleFilter("SharedDetachedLcChild_K"),
                  DetachedInParticleFilter("SharedDetachedLcChild_pi"),
                  protonsForLc2KPPiPIDCALIBCombiner]
        DetachedHHHCombiner.__init__(self,name,decay,inputs)
