# Each stage must specify its own inputs
linePrefix=''

# The GEC
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
class TrackGEC(Hlt2VoidFilter):
    def __init__(self, name):
        from Configurables import LoKi__Hybrid__CoreFactory as Factory
        modules =  Factory().Modules
        for i in [ 'LoKiTrigger.decorators' ] : 
            if i not in modules : modules.append(i)

        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking 
        velotracks = Hlt2BiKalmanFittedForwardTracking().hlt2VeloTracking()
        VT = velotracks.outputSelection()
        code = ("CONTAINS('%s')" % VT) + (" < %(nVeloTracksmax)s") + ("TrNUM('%s', TrBACKWARD)" % VT) + (" < %(nBackTracksmax)s")
        Hlt2VoidFilter.__init__(self, linePrefix + 'Track', 'GEC', code, [velotracks])

# create filters for pions, kaons and protons
# NB: The name of this object must be of the form:
# <text string>_<particle type>
# e.g. D2KPiInputs_K or D2KPiInputs_pi
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
class InFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        allTracksCut = ("(PT > %(H_PTmin)s)" +
                        "(P > %(H_Pmin)s)" +
                        "(TRCHI2DOF < %(H_TrkChi2max)s)")

        from Inputs import Hlt2LoosePions, Hlt2LooseKaons,Hlt2LooseProtons       
 
        inputCuts = {"pi" : {"cut"    : allTracksCut,
                             "inputs" : [Hlt2LoosePions]},
                     "K"  : {"cut"    : allTracksCut+"&(PIDK > %(K_PIDKmin)s)",
                             "inputs" : [Hlt2LooseKaons]},
                     "p"  : {"cut"    : allTracksCut+"&(PIDp > %(P_PIDpmin)s)",
                             "inputs" : [Hlt2LooseProtons]}
                    }

        cut    = inputCuts[name.split('_')[1]]["cut"]
        inputs = inputCuts[name.split('_')[1]]["inputs"]
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)

from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
# The class that creates the leptonic combiner
class ElectronCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        dc = {'e+'   : "(PT > %(e_PTmin)s)",
              'e-'   : "(PT > %(e_PTmin)s)"}
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC(linePrefix + 'TrackGEC')],
                              DaughtersCuts = dc, Preambulo = []);

class ElectronCombiner_MinKinBias(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        dc = {'e+'   : "(PT > %(e_PTmin)s)",
              'e-'   : "(PT > %(e_PTmin)s)"}
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              DaughtersCuts = dc, Preambulo = []);

class MuonCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        dc = {'mu+'   : "(PT > %(mu_PTmin)s)",
              'mu-'   : "(PT > %(mu_PTmin)s)"}
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC(linePrefix + 'TrackGEC')],
                              DaughtersCuts = dc, Preambulo = []);

# The class that creates the hadronic Hlt2Combiners
class HadronicCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        cc = ("(APT > %(APTmin)s)"+
              "(APT < %(APTmax)s)"+
              "(AP  > %(APmin)s)"+
              "(ADOCAMAX() < %(ADOCAmax)s)"+
              "(in_range( %(AMmin)s, AM, %(AMmax)s ))")
        mc =    ("(VFASPF(VCHI2PDOF) < %(VtxChi2DoFmax)s)")
        
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC(linePrefix + 'TrackGEC')],
                              CombinationCut = cc, MotherCut = mc, Preambulo = [])
class HadronicCombiner_MinKinBias(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        cc = ("(APT > %(APTmin)s)"+
              "(APT < %(APTmax)s)")
        mc =    ("ALL")
        
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              CombinationCut = cc, MotherCut = mc, Preambulo = [])


##############################
# HADRONIC FILTERS
##############################
# Lambda -> p pi
class LowMultL2pPiFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = "[Lambda0 -> p+ pi-]cc",
        inputs  = [InFilter("SharedInFilter_p"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

class LowMultL2pPiWSFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = "[D0 -> p+ pi+]cc",
        inputs  = [InFilter("SharedInFilter_p"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

# D0 -> K pi
class LowMultD2KPiFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = "[D0 -> K- pi+]cc",
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

class LowMultD2KPiWSFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = "[D0 -> K+ pi+]cc",
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

# D(s)+ -> 3h
class LowMultD2KPiPiFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = "[D+ -> K- pi+ pi+]cc",
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

class LowMultD2KPiPiWSFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = "[D+ -> K+ pi+ pi+]cc",
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

class LowMultD2KKPiFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = "[D+ -> K- K+ pi+]cc",
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

class LowMultD2KKPiWSFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = "[D+ -> K+ K+ pi+]cc",
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

# D -> K3pi
class LowMultD2K3PiFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = "[D0 -> K- pi+ pi+ pi+]cc",
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

class LowMultD2K3PiWSFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = ["[D0 -> K+ pi+ pi+ pi+]cc","[D0 -> K+ pi+ pi+ pi-]cc","[D0 -> K+ pi- pi- pi-]cc"],
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

# chic -> hh
class LowMultChiC2HHFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = ["chi_c1(1P) -> K+ K-", "chi_c1(1P) -> pi+ pi-"],
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

class LowMultChiC2HHWSFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = ["[chi_c1(1P) -> K+ K+]cc", "[chi_c1(1P) -> pi+ pi+]cc"],
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

# chic -> hhhh
class LowMultChiC2HHHHFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = ["chi_c1(1P) -> K+ K- K+ K-", "chi_c1(1P) -> K+ K- pi+ pi-", 
                   "chi_c1(1P) -> K+ K+ pi- pi-", "chi_c1(1P) -> pi+ pi+ pi- pi-"],
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

class LowMultChiC2HHHHWSFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = ["[chi_c1(1P) -> K+ K+ pi+ pi+]cc", "[chi_c1(1P) -> K+ K+ pi+ pi-]cc",
                   "[chi_c1(1P) -> K+ K- pi+ pi+]cc", "[chi_c1(1P) -> K+ K+ K+ K+]cc",
                   "[chi_c1(1P) -> K+ K+ K+ K-]cc", "[chi_c1(1P) -> pi+ pi+ pi+ pi+]cc",
                   "[chi_c1(1P) -> pi+ pi+ pi+ pi-]cc" ],
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")]
        HadronicCombiner.__init__(self,name,decay,inputs)

# chic -> ppbar
class LowMultChiC2PPFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = "chi_c1(1P) -> p+ p~-",
        inputs  = [InFilter("SharedInFilter_p")],
        HadronicCombiner.__init__(self,name,decay,inputs)

class LowMultChiC2PPWSFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = "[chi_c1(1P) -> p+ p+]cc",
        inputs  = [InFilter("SharedInFilter_p")],
        HadronicCombiner.__init__(self,name,decay,inputs)

# LMR -> hh
class LowMultLMR2HHFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = ["phi(1020) -> K+ K-","[phi(1020) -> K+ pi-]cc","phi(1020) -> pi+ pi-","phi(1020) -> p+ p~-"],
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

class LowMultLMR2HHWSFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = ["[phi(1020) -> K+ K+]cc","[phi(1020) -> K+ pi+]cc","[phi(1020) -> pi+ pi+]","[phi(1020) -> p+ p+]cc"],
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

# LMR -> hhhh
class LowMultLMR2HHHHFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = ["phi(1020) -> pi+ pi- pi+ pi-","phi(1020) -> K+ K- K+ K-","phi(1020) -> K+ K- pi+ pi-"],
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

class LowMultLMR2HHHHWSFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = ["[phi(1020) -> K+ K+ K+ K+]cc", "[phi(1020) -> K+ K+ K+ K-]cc",
                   "[phi(1020) -> K+ K+ pi+ pi+]cc", "[phi(1020) -> K+ K+ pi+ pi-]cc",
                   "[phi(1020) -> K+ K- pi+ pi+]cc", "[phi(1020) -> pi+ pi+ pi+ pi-]cc",
                   "[phi(1020) -> pi+ pi+ pi+ pi+]cc"]
        inputs  = [InFilter("SharedInFilter_K"), InFilter("SharedInFilter_pi")],
        HadronicCombiner.__init__(self,name,decay,inputs)

class LowMultMinKinBiasHadronFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = ["phi(1020) -> K+ K-","[phi(1020) -> K+ pi+]cc"],
        from Inputs import Hlt2NoPIDsKaons
        inputs  = [Hlt2NoPIDsKaons]
        HadronicCombiner_MinKinBias.__init__(self,name,decay,inputs)

##############################
# MUON FILTERS
##############################
class LowMultDiMuonFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = ["J/psi(1S) -> mu+ mu-","J/psi(1S) -> mu+ mu+","J/psi(1S) -> mu- mu-"]
        from Inputs import Hlt2LooseMuons
        inputs  = [Hlt2LooseMuons]
        MuonCombiner.__init__(self,name,decay,inputs)

class LowMultMuonFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        cut = "(PT > %(mu_PT)s)";
        from Inputs import Hlt2LooseMuons
        inputs  = [Hlt2LooseMuons]
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,
                                    dependencies = [TrackGEC(linePrefix + 'TrackGEC')])

##############################
# PHOTON FILTERS
##############################
class LowMultPhotonFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        cut = "(PT > %(gamma_PTmin)s)";
        from Inputs import MergedPi0sFromL0, ResolvedPi0sFromL0, AllPi0sFromL0, BiKalmanFittedPhotonsFromL0Low
        inputs = [MergedPi0sFromL0, ResolvedPi0sFromL0, AllPi0sFromL0, BiKalmanFittedPhotonsFromL0Low]
        Hlt2ParticleFilter.__init__(self, name, cut, inputs)

##############################
# ELECTRON FILTERS
##############################
class LowMultElectronFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = ["J/psi(1S) -> e+ e-","J/psi(1S) -> e+ e+","J/psi(1S) -> e- e-"]
        from Inputs import BiKalmanFittedElectronsFromL0
        inputs  = [BiKalmanFittedElectronsFromL0]
        ElectronCombiner.__init__(self,name,decay,inputs,
                                  dependencies = [TrackGEC(linePrefix + 'TrackGEC')])

class LowMultMinKinBiasElectronFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        decay   = ["J/psi(1S) -> e+ e-","J/psi(1S) -> e+ e+","J/psi(1S) -> e- e-"]
        from Inputs import BiKalmanFittedElectronsFromL0
        inputs  = [BiKalmanFittedElectronsFromL0]
        ElectronCombiner_MinKinBias.__init__(self,name,decay,inputs)
