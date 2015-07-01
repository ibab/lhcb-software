from HltLine.HltDecodeRaw import DecodeL0CALO
# The GEC
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
class TrackGEC(Hlt2VoidFilter):
    def __init__(self, name):
#        from Configurables import LoKi__Hybrid__CoreFactory as Factory
#        modules =  Factory().Modules
#        for i in [ 'LoKiTrigger.decorators' ] :
#            if i not in modules : modules.append(i)

        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        velotracks = Hlt2BiKalmanFittedForwardTracking().hlt2VeloTracking()
        VT = velotracks.outputSelection()
        code = ("(CONTAINS('%s')" % VT) + (" < %(nVeloTracksmax)s)") + " & " + ("(TrNUM('%s', TrBACKWARD)" % VT) + (" < %(nBackTracksmax)s)")
        Hlt2VoidFilter.__init__(self, "CEPTrackGEC_"+name, code, [velotracks], nickname = name, shared = True)

################################
# create track filters for
# pions, kaons and protons
################################
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
class InHadronFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        cut          =  ("(PT > %(H_PTmin)s)" +
                       "& (P > %(H_Pmin)s)"  +
                       "& (TRCHI2DOF < %(H_TrkChi2max)s)")
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)
# Global hadronic inputs
from Inputs import Hlt2NoPIDsPions, Hlt2LooseKaons,Hlt2LooseProtons
SharedChild_pi = InHadronFilter( 'LowMultSharedChild_pi',
                                 [Hlt2NoPIDsPions] );
SharedChild_K  = InHadronFilter( 'LowMultSharedChild_K',
                                 [Hlt2LooseKaons] )
SharedChild_p  = InHadronFilter( 'LowMultSharedChild_p',
                                 [Hlt2LooseProtons])

from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
# The class that creates the leptonic combiner
class ElectronCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        dc = {'e+'   : "(PT > %(e_PTmin)s)",
              'e-'   : "(PT > %(e_PTmin)s)"}
        mc = "ALL"
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC(name)] ,
                              DaughtersCuts = dc, MotherCut = mc, Preambulo = []);

class MuonCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        dc = {'mu+'   : "(PT > %(mu_PTmin)s)",
              'mu-'   : "(PT > %(mu_PTmin)s)"}
        cc = ("(AM > %(AMmin)s)")
        mc = "ALL"
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC(name)],
                              DaughtersCuts = dc, CombinationCut = cc, MotherCut = mc, Preambulo = []);

# The class that creates the photon Hlt2Combiners
class PhotonCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        dc = {'gamma'   : "(PT > %(gamma_PTmin)s)"}
        mc = ("( (M > %(digamma_AMmin)s) & (M < %(digamma_AMmax)s) )")
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC(name)],
                              DaughtersCuts = dc,
                              ParticleCombiners={ '' : 'ParticleAdder'},
                              MotherCut = mc );

# The class that creates the hadronic Hlt2Combiners
class HadronicCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs, pidcut={}):
        dc = pidcut
        cc = ("(APT > %(APTmin)s)"+
              "& (APT < %(APTmax)s)"+
              "& (AP  > %(APmin)s)"+
              "& (ADOCAMAX('') < %(ADOCAmax)s)"+
              "& (in_range( %(AMmin)s, AM, %(AMmax)s ))")
        mc =    ("(VFASPF(VCHI2PDOF) < %(VtxChi2DoFmax)s)")
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC(name)],
                              DaughtersCuts = dc,
                              CombinationCut = cc, MotherCut = mc, Preambulo = [])

class HadronicCombiner_noTrFilt(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        dc = {'K+'    : "(PT > %(PTmin)s)",
              'K-'    : "(PT > %(PTmin)s)"}
        mc =    ("ALL")
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              DaughtersCuts = dc, MotherCut = mc, Preambulo = [])


##############################
# HADRONIC FILTERS
##############################
# Lambda -> p pi
class LowMultL2pPiFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'p+' : "(PIDp > %(P_PIDpmin)s)", 'p~-' : "(PIDp > %(P_PIDpmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = "[Lambda0 -> p+ pi-]cc"
        inputs  = [SharedChild_p, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

class LowMultL2pPiWSFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'p+' : "(PIDp > %(P_PIDpmin)s)", 'p~-' : "(PIDp > %(P_PIDpmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = "[D0 -> p+ pi+]cc"
        inputs  = [SharedChild_p, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

# D0 -> K pi
class LowMultD2KPiFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = "[D0 -> K- pi+]cc"
        inputs  = [SharedChild_K, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

class LowMultD2KPiWSFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = "[D0 -> K+ pi+]cc"
        inputs  = [SharedChild_K, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

# D(s)+ -> 3h
class LowMultD2KPiPiFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = "[D+ -> K- pi+ pi+]cc"
        inputs  = [SharedChild_K, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

class LowMultD2KPiPiWSFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = "[D+ -> K+ pi+ pi+]cc"
        inputs  = [SharedChild_K, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

class LowMultD2KKPiFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = "[D+ -> K- K+ pi+]cc"
        inputs  = [SharedChild_K, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

class LowMultD2KKPiWSFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = "[D+ -> K+ K+ pi+]cc"
        inputs  = [SharedChild_K, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

# D -> K3pi
class LowMultD2K3PiFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = "[D0 -> K- pi+ pi+ pi+]cc"
        inputs  = [SharedChild_K, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

class LowMultD2K3PiWSFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = ["[D0 -> K+ pi+ pi+ pi+]cc","[D0 -> K+ pi+ pi+ pi-]cc","[D0 -> K+ pi- pi- pi-]cc"]
        inputs  = [SharedChild_K, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

# chic -> hh
class LowMultChiC2HHFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = ["chi_c1(1P) -> K+ K-", "chi_c1(1P) -> pi+ pi-"]
        inputs  = [SharedChild_K, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

class LowMultChiC2HHWSFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = ["[chi_c1(1P) -> K+ K+]cc", "[chi_c1(1P) -> pi+ pi+]cc"]
        inputs  = [SharedChild_K, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

# chic -> hhhh
class LowMultChiC2HHHHFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = ["chi_c1(1P) -> K+ K- K+ K-", "chi_c1(1P) -> K+ K- pi+ pi-",
                   "chi_c1(1P) -> K+ K+ pi- pi-", "chi_c1(1P) -> pi+ pi+ pi- pi-"]
        inputs  = [SharedChild_K, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

class LowMultChiC2HHHHWSFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = ["[chi_c1(1P) -> K+ K+ pi+ pi+]cc", "[chi_c1(1P) -> K+ K+ pi+ pi-]cc",
                   "[chi_c1(1P) -> K+ K- pi+ pi+]cc", "[chi_c1(1P) -> K+ K+ K+ K+]cc",
                   "[chi_c1(1P) -> K+ K+ K+ K-]cc", "[chi_c1(1P) -> pi+ pi+ pi+ pi+]cc",
                   "[chi_c1(1P) -> pi+ pi+ pi+ pi-]cc" ]
        inputs  = [SharedChild_K, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

# chic -> ppbar
class LowMultChiC2PPFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'p+' : "(PIDp > %(P_PIDpmin)s)", 'p~-' : "(PIDp > %(P_PIDpmin)s)"}
        decay   = "chi_c1(1P) -> p+ p~-"
        inputs  = [SharedChild_p]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

class LowMultChiC2PPWSFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'p+' : "(PIDp > %(P_PIDpmin)s)", 'p~-' : "(PIDp > %(P_PIDpmin)s)"}
        decay   = "[chi_c1(1P) -> p+ p+]cc"
        inputs  = [SharedChild_p]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

# LMR -> hh
class LowMultLMR2HHFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)",
                   'p+' : "(PIDp > %(P_PIDpmin)s)", 'p~-' : "(PIDp > %(P_PIDpmin)s)"}
        decay   = ["chi_c1(1P) -> K+ K-","[chi_c1(1P) -> K+ pi-]cc","chi_c1(1P) -> pi+ pi-","chi_c1(1P) -> p+ p~-"]
        inputs  = [SharedChild_K, SharedChild_pi, SharedChild_p]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

class LowMultLMR2HHWSFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)",
                   'p+' : "(PIDp > %(P_PIDpmin)s)", 'p~-' : "(PIDp > %(P_PIDpmin)s)"}
        decay   = ["[chi_c1(1P) -> K+ K+]cc","[chi_c1(1P) -> K+ pi+]cc","[chi_c1(1P) -> pi+ pi+]cc","[chi_c1(1P) -> p+ p+]cc"]
        inputs  = [SharedChild_K, SharedChild_pi, SharedChild_p]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

# LMR -> hhhh
class LowMultLMR2HHHHFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = ["chi_c1(1P) -> pi+ pi- pi+ pi-","chi_c1(1P) -> K+ K- K+ K-","chi_c1(1P) -> K+ K- pi+ pi-"]
        inputs  = [SharedChild_K, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

class LowMultLMR2HHHHWSFilter(HadronicCombiner):
    def __init__(self, name):
        pidcut  = {'K+' : "(PIDK > %(K_PIDKmin)s)", 'K-'  : "(PIDK > %(K_PIDKmin)s)",
                   'pi+': "(PIDK < %(Pi_PIDKmax)s)",'pi-' : "(PIDK < %(Pi_PIDKmax)s)"}
        decay   = ["[chi_c1(1P) -> K+ K+ K+ K+]cc", "[chi_c1(1P) -> K+ K+ K+ K-]cc",
                   "[chi_c1(1P) -> K+ K+ pi+ pi+]cc", "[chi_c1(1P) -> K+ K+ pi+ pi-]cc",
                   "[chi_c1(1P) -> K+ K- pi+ pi+]cc", "[chi_c1(1P) -> pi+ pi+ pi+ pi-]cc",
                   "[chi_c1(1P) -> pi+ pi+ pi+ pi+]cc"]
        inputs  = [SharedChild_K, SharedChild_pi]
        HadronicCombiner.__init__(self,name,decay,inputs,pidcut)

class LowMultHadron_noTrFiltFilter(HadronicCombiner_noTrFilt):
    def __init__(self, name):
        decay   = "phi(1020) -> K+ K-"
        from Inputs import Hlt2NoPIDsKaons
        inputs  = [Hlt2NoPIDsKaons]
        HadronicCombiner_noTrFilt.__init__(self,name,decay,inputs)

##############################
# MUON FILTERS
##############################
class LowMultDiMuonFilter(MuonCombiner):
    def __init__(self, name):
        decay   = ["J/psi(1S) -> mu+ mu-","J/psi(1S) -> mu+ mu+","J/psi(1S) -> mu- mu-"]
        from Inputs import Hlt2LooseMuons
        inputs  = [Hlt2LooseMuons]
        MuonCombiner.__init__(self,name,decay,inputs)

class LowMultMuonFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        cut = "(PT > %(mu_PTmin)s)"
        from Inputs import Hlt2LooseMuons
        inputs  = [Hlt2LooseMuons]
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,
                                    dependencies = [TrackGEC(name)])

##############################
# PHOTON FILTERS
##############################
class LowMultDiPhotonFilter(PhotonCombiner):
    def __init__(self, name):
        decay = "chi_c1(1P) -> gamma gamma"
        from Inputs import Hlt2LoosePhotons
        inputs = [Hlt2LoosePhotons]
        PhotonCombiner.__init__(self,name,decay,inputs)

class LowMultPi0Filter(Hlt2ParticleFilter):
    def __init__(self, name):
        cut = "(PT > %(pi0_PTmin)s)"
        from Inputs import MergedPi0s, ResolvedPi0s
        inputs = [MergedPi0s, ResolvedPi0s]
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,
                                    dependencies = [TrackGEC(name)])

##############################
# ELECTRON FILTERS
##############################
class LowMultDiElectronFilter(ElectronCombiner):
    def __init__(self, name):
        decay   = ["J/psi(1S) -> e+ e-","J/psi(1S) -> e+ e+","J/psi(1S) -> e- e-"]
        from Inputs import Hlt2LooseElectrons
        inputs  = [Hlt2LooseElectrons]
        ElectronCombiner.__init__(self,name,decay,inputs)
