###
#  @author F. Dettori Francesco.Dettori@cern.ch
#          A. Contu   Andrea.Contu@cern.ch
#          B. Viaud   viaud@lal.in2p3.fr
#  @date 2015-03-07
#
#  Please contact the abovementioned responsibles before editing this file
#
##

# Combining tracks for D-> x x  for the rare charm decays and their control channels
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Inputs import Hlt2Muons, Hlt2Pions, Hlt2Kaons, Hlt2Protons, Hlt2Electrons

class D2xxCombiner(Hlt2Combiner):
    # Common cuts
    # Cuts on the daughters
    d0comb_childcut = \
                    "(P                  > %(XminP)s  * MeV) &" + \
                    "(PT                 > %(XminPT)s * MeV) &" + \
                    "(TRCHI2DOF          < %(XTrackChi2)s  ) &" + \
                    "(MIPCHI2DV(PRIMARY) > %(XminIPChi2)s  )  "
    # Cuts on the combination 
    d0comb_combcut = \
                   "(AMAXCHILD(PT) > %(XmaxPT)s   * MeV) &" + \
                   "(ADAMASS('D0') < %(DMassWin)s * MeV) &" + \
                   "(APT           > %(D0MinPT)s       ) &" + \
                   "(AMAXDOCA('')  < %(doca)s     * mm )  "
    # Cuts on the combination for the signals (enlarged mass window) 
    d0comb_combcut_sig = \
                       "(AMAXCHILD(PT) > %(XmaxPT)s      * MeV) &" + \
                       "(ADAMASS('D0') < %(DMassWinSig)s * MeV) &" + \
                       "(APT           > %(D0MinPT)s          ) &" + \
                       "(AMAXDOCA('')  < %(doca)s        * mm )  "
    # Cuts on the mother 
    d0comb_d0cut = \
                 "INGENERATION(BPVIPCHI2() > %(XmaxIPChi2)s, 1 ) &" + \
                 "(VFASPF(VCHI2PDOF)       < %(DVChi2)s        ) &" + \
                 "(BPVDIRA                 > %(DDira)s         ) &" + \
                 "(BPVVDCHI2               > %(DMinFlightChi2)s) &" + \
                 "(MIPCHI2DV(PRIMARY)      < %(DMaxIPChi2)s    )  "
    
    
    def __init__(self, name,xplus = "mu", xminus = "mu", version = "signal"):
        from HltTracking.HltPVs import PV3D
        # Dictionary of the inputs 
        inputlist = {
            "mu" : Hlt2Muons,
            "pi" : Hlt2Pions,
            "K" : Hlt2Kaons,
            "p" : Hlt2Protons,
            "e" : Hlt2Electrons,
            }
        # Current input 
        inputs = [ inputlist[xminus] ]  if (xminus == xplus ) else [ inputlist[xminus] , inputlist[xplus]]
        # dictionary of the daughter cuts 
        d0comb_childcut_list =\
                             ( { xplus+'+'  : self.d0comb_childcut } if ( xplus == xminus ) else \
                               { xplus+'+'  : self.d0comb_childcut,
                                 xminus+'+' : self.d0comb_childcut}
                               )
        # Decay descriptor 
        decay = "D0 -> "+xplus+"+ "+xminus+"-"    if (xplus == xminus)  else   "[D0 -> "+xplus+"+ "+xminus+"- ]cc"
        # Combiner 
        Hlt2Combiner.__init__(self, name, decay ,
                              inputs,
                              dependencies = [ PV3D('Hlt2') ],
                              #tistos = 'TisTosSpec',
                              DaughtersCuts = d0comb_childcut_list,
                              CombinationCut = self.d0comb_combcut_sig if version =="signal" else self.d0comb_combcut ,
                              MotherCut = self.d0comb_d0cut,
                              Preambulo = [])


# Combiners
D02mumuComb   = D2xxCombiner("MuMu")
D02pipiComb   = D2xxCombiner("PiPi", xplus = 'pi', xminus = 'pi', version = "")
D02kpiComb   = D2xxCombiner("KPi", xplus = 'K', xminus = 'pi', version = "")
D02kmuComb   = D2xxCombiner("KMu", xplus = 'K', xminus = 'mu', version = "")
D02emuComb   = D2xxCombiner("EMu", xplus = 'e', xminus = 'mu', version = "")

#D02pipiComb   = D02mumuComb.clone("PiPi", xplus = 'pi', xminus = 'pi', decay = "D0 -> pi+ pi-", CombinationCut = D02mumuComb.d0comb_combcut )
                             


#Multi body


#Dimuons both for three and four body
class TwoMuonForD2MuMuXCombiner(Hlt2Combiner):
    def __init__(self, name):
        from HltTracking.HltPVs import PV3D
        twoMuonDaughterCut = "(TRCHI2DOF< %(Trk_TRCHI2DOF_MAX_mumuX)s )" \
                             "& (PT> %(Trk_PT_MIN_mumuX)s)" \
                             "& (P> %(Trk_P_MIN_mumuX)s)" \
                             "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN_mumuX)s )"
 
        twoMuonCombCut = "(AM<2100*MeV)" \
                         "& ((APT1+APT2)> %(Pair_SumAPT_MIN_mumuX)s)" \
                         "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(Pair_AMINDOCA_MAX_mumuX)s )" \
                         "& (AALLSAMEBPV)"

        twoMuonMotherCut = "(BPVVD> %(Pair_BPVVD_MIN_mumuX)s )" \
                           "& (BPVCORRM < %(Pair_BPVCORRM_MAX_mumuX)s)" \
                           "& (BPVVDCHI2> %(Pair_BPVVDCHI2_MIN_mumuX)s )"
        #First stage - Combine 2 Body with pt > 500MeV        
        
        inputs = [Hlt2Muons]
        Hlt2Combiner.__init__(self, name, ["J/psi(1S) -> mu+ mu-","phi(1020) -> mu+ mu+", " rho(770)0 -> mu- mu-"], 
                              inputs,
                              dependencies = [PV3D('Hlt2')],
                              DaughtersCuts = { "mu+" : twoMuonDaughterCut}, 
                              CombinationCut = twoMuonCombCut,
                              MotherCut = twoMuonMotherCut,
                              Preambulo = [],
                              shared=True)

TwoMuonForD2MuMuX=TwoMuonForD2MuMuXCombiner('TwoMuonForD2MuMuX')


#Particle filters
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
class InPartFilterHMuMu(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        from HltTracking.HltPVs import PV3D
        cut = "(TRCHI2DOF< %(TrkChi2_Hmumu)s )" \
                 "& (PT> %(TrkPt_Hmumu)s)" \
                 "& (P> %(TrkP_Hmumu)s)" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_Hmumu)s )"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, dependencies=[PV3D('Hlt2'),TwoMuonForD2MuMuX], shared=True)

#Filtered tracks for threebody
InPartFilterHMuMu_Kaons = InPartFilterHMuMu("DHMuMuKaons",[Hlt2Kaons])
InPartFilterHMuMu_Pions = InPartFilterHMuMu("DHMuMuPions",[Hlt2Pions])
InPartFilterHMuMu_Protons = InPartFilterHMuMu("LcPMuMuProtons",[Hlt2Protons])

class InPartFilterHHMuMu(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        from HltTracking.HltPVs import PV3D
        cut = "(TRCHI2DOF< %(TrkChi2_HHmumu)s )" \
                 "& (PT> %(TrkPt_HHmumu)s)" \
                 "& (P> %(TrkP_HHmumu)s)" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_HHmumu)s )"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, dependencies=[PV3D('Hlt2'),TwoMuonForD2MuMuX], shared=True)

#Filtered tracks for fourbody
InPartFilterHHMuMu_Kaons = InPartFilterHHMuMu("DHHMuMuKaons",[Hlt2Kaons])
InPartFilterHHMuMu_Pions = InPartFilterHHMuMu("DHHMuMuPions",[Hlt2Pions])

#D2HMUMU COMBINER
class D2HMuMuCombiner(Hlt2Combiner):
    def __init__(self, name, decayDesc, inputSeq):
        from HltTracking.HltPVs import PV3D
        masscut = "in_range( %(WideMass_M_MIN_Hmumu)s , M , %(WideMass_M_MAX_Hmumu)s )"

        combcuts = "(AM<2100*MeV)" \
                   "& (AMAXCHILD(PT) > %(TrkPtMAX_Hmumu)s      * MeV) "  \
                   "& ((APT1+APT2+APT3) > %(DSumPt_Hmumu)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_Hmumu)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_Hmumu)s)" \
                   "& (AALLSAMEBPV)"
        mothercuts = masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_Hmumu)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_Hmumu)s)" \
                     "& (BPVDIRA                 > %(DDira_Hmumu)s         ) " \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_Hmumu)s )" \
                     "& ( SUMTREE( ( (ID=='K+') | (ID=='K-') | (ID=='pi+') | (ID=='pi-') | (ID=='mu+') | (ID=='mu-') ), sqrt(BPVIPCHI2()) ) > %(TrkPVIPChi2MAX_Hmumu)s)"\
                     "& (BPVIPCHI2() < %(DIPChi2_Hmumu)s )"
        Hlt2Combiner.__init__(self, name, decayDesc, inputSeq,
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = combcuts,
                              MotherCut = mothercuts,
                              Preambulo = [],
                              shared=True)

D2PiMuMuComb = D2HMuMuCombiner("D2PiMuMuComb",[ "D+ -> J/psi(1S) pi+", "D- -> J/psi(1S) pi-"],[InPartFilterHMuMu_Pions,TwoMuonForD2MuMuX])
D2KMuMuComb = D2HMuMuCombiner("D2KMuMuComb",[ "D+ -> J/psi(1S) K+", "D- -> J/psi(1S) K-"],[InPartFilterHMuMu_Kaons,TwoMuonForD2MuMuX])
D2PiMuMuSSComb = D2HMuMuCombiner("D2PiMuMuSSComb", ["D+ -> phi(1020) pi-", "D- -> rho(770)0 pi+"],[InPartFilterHMuMu_Pions,TwoMuonForD2MuMuX])
D2KMuMuSSComb = D2HMuMuCombiner("D2KMuMuSSComb", ["D+ -> phi(1020) K-", "D- -> rho(770)0 K+"],[InPartFilterHMuMu_Kaons,TwoMuonForD2MuMuX])

#Lc2PMUMU COMBINER
class Lc2PMuMuCombiner(Hlt2Combiner):
    def __init__(self, name, decayDesc, inputSeq):
        from HltTracking.HltPVs import PV3D

        Lambda_c_masscut = "in_range( %(Lambda_c_WideMass_M_MIN)s , M , %(Lambda_c_WideMass_M_MAX)s )"

        combcuts = "(AM<2400*MeV)" \
                   "& (AMAXCHILD(PT) > %(TrkPtMAX_Hmumu)s      * MeV) "  \
                   "& ((APT1+APT2+APT3) > %(DSumPt_Hmumu)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_Hmumu)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_Hmumu)s)" \
                   "& (AALLSAMEBPV)"
        mothercuts = Lambda_c_masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_Hmumu)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_Hmumu)s)" \
                     "& (BPVDIRA                 > %(DDira_Hmumu)s         ) " \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_Hmumu)s )" \
                     "& ( SUMTREE( ( (ID=='p+') | (ID=='p~-') | (ID=='pi+') | (ID=='pi-') | (ID=='mu+') | (ID=='mu-') ), sqrt(BPVIPCHI2()) ) > %(TrkPVIPChi2MAX_Hmumu)s)"\
                     "& (BPVIPCHI2() < %(DIPChi2_Hmumu)s )"
        Hlt2Combiner.__init__(self, name, decayDesc, inputSeq,
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = combcuts,
                              MotherCut = mothercuts,
                              Preambulo = [],
                              shared=True)

Lc2PMuMuComb = Lc2PMuMuCombiner("Lc2PMuMuComb",["Lambda_c+ -> J/psi(1S) p+" , "Lambda_c~- -> J/psi(1S) p~-"],[InPartFilterHMuMu_Protons,TwoMuonForD2MuMuX])
Lc2PMuMuSSComb = Lc2PMuMuCombiner("Lc2PMuMuSSComb", ["Lambda_c+ -> phi(1020) p~-" , "Lambda_c~- ->  rho(770)0 p+"],[InPartFilterHMuMu_Protons,TwoMuonForD2MuMuX])


#D02HHMUMU COMBINER
class D02HHMuMuCombiner(Hlt2Combiner):
    def __init__(self, name, decayDesc, inputSeq):
        from HltTracking.HltPVs import PV3D
       
  #      massmin = min(float("%(Sig_M_MIN_HHmumu)s"), float("%(WideMass_M_MIN_HHmumu)s"))
 #       massmax = max(float("%(Sig_M_MAX_HHmumu)s"), float("%(WideMass_M_MAX_HHmumu)s"))

#        masscut = "in_range(%s,  M, %s)" % (massmin, massmax)
        masscut = "in_range( %(WideMass_M_MIN_HHmumu)s , M , %(WideMass_M_MAX_HHmumu)s )"
        combcuts = "(AM<2100*MeV)" \
                   "& (AMAXCHILD(PT) > %(TrkPtMAX_HHmumu)s      * MeV) "  \
                   "& ((APT1+APT2+APT3+APT4) > %(DSumPt_HHmumu)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_HHmumu)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_HHmumu)s)" \
                   "& (AALLSAMEBPV)"
        mothercuts = masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_HHmumu)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_HHmumu)s)" \
                     "& (BPVDIRA                 > %(DDira_HHmumu)s         ) " \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_HHmumu)s )" \
                     "& ( SUMTREE( ( (ID=='K+') | (ID=='K-') | (ID=='pi+') | (ID=='pi-') | (ID=='mu+') | (ID=='mu-') ), sqrt(BPVIPCHI2()) ) > %(TrkPVIPChi2MAX_HHmumu)s)"\
                     "& (BPVIPCHI2() < %(DIPChi2_HHmumu)s )"
                     
        Hlt2Combiner.__init__(self, name, decayDesc, inputSeq,
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = combcuts,
                              MotherCut = mothercuts,
                              Preambulo = [],
                              shared=True)

D02PiPiMuMuComb = D02HHMuMuCombiner("D02PiPiMuMuComb","D0 -> J/psi(1S) pi+ pi-",[InPartFilterHHMuMu_Pions,TwoMuonForD2MuMuX])
D02KKMuMuComb = D02HHMuMuCombiner("D02KKMuMuComb","D0 -> J/psi(1S) K+ K-",[InPartFilterHHMuMu_Kaons,TwoMuonForD2MuMuX])
D02KPiMuMuComb = D02HHMuMuCombiner("D02KPiMuMuComb",["D0 -> J/psi(1S) K+ pi-","D0 -> J/psi(1S) K- pi+"],[InPartFilterHHMuMu_Pions,InPartFilterHHMuMu_Kaons,TwoMuonForD2MuMuX])


#D2HMUMU FILTER
class D2HMuMuFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        from HltTracking.HltPVs import PV3D
        cut  = "in_range(%(Sig_M_MIN_Hmumu)s, M, %(Sig_M_MAX_Hmumu)s)"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, dependencies=[PV3D('Hlt2')])

D2PiMuMuFilter = D2HMuMuFilter("D2PiMuMuFilter",[D2PiMuMuComb])
D2KMuMuFilter = D2HMuMuFilter("D2KMuMuFilter",[D2KMuMuComb])
D2PiMuMuSSFilter = D2HMuMuFilter("D2PiMuMuSSFilter",[D2PiMuMuSSComb])
D2KMuMuSSFilter = D2HMuMuFilter("D2KMuMuSSFilter",[D2KMuMuSSComb])

#Lc2PMUMU FILTER
class Lc2PMuMuFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        from HltTracking.HltPVs import PV3D
        cut  = "in_range(%(Lambda_c_Sig_M_MIN)s, M, %(Lambda_c_Sig_M_MAX)s)"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, dependencies=[PV3D('Hlt2')])

Lc2PMuMuSSFilter = Lc2PMuMuFilter("Lc2PMuMuSSFilter",[Lc2PMuMuSSComb])
Lc2PMuMuFilter = Lc2PMuMuFilter("Lc2PMuMuFilter",[Lc2PMuMuComb])

#D02HHMUMU FILTER
class D02HHMuMuFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        from HltTracking.HltPVs import PV3D
        cut  = "in_range(%(Sig_M_MIN_HHmumu)s, M, %(Sig_M_MAX_HHmumu)s)"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, dependencies=[PV3D('Hlt2')])


D02PiPiMuMuFilter = D02HHMuMuFilter("D02PiPiMuMuFilter",[D02PiPiMuMuComb])
D02KKMuMuFilter = D02HHMuMuFilter("D02KKMuMuFilter",[D02KKMuMuComb])
D02KPiMuMuFilter = D02HHMuMuFilter("D02KPiMuMuFilter",[D02KPiMuMuComb])




