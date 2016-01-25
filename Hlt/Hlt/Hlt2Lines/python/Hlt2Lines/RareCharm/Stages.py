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
                    "(P                  > %(XminP)s       ) &" + \
                    "(PT                 > %(XminPT)s      ) &" + \
                    "(TRCHI2DOF          < %(XTrackChi2)s  ) &" + \
                    "(MIPCHI2DV(PRIMARY) > %(XminIPChi2)s  )  "
    # Cuts on the combination 
    d0comb_combcut = \
                   "(AMAXCHILD(PT) > %(XmaxPT)s        ) &" + \
                   "(ADAMASS('D0') < %(DMassWin)s      ) &" + \
                   "(APT           > %(D0MinPT)s       ) &" + \
                   "(AMAXDOCA('')  < %(doca)s          )  "
    # Cuts on the combination for the signals (enlarged mass window) 
    d0comb_combcut_sig = \
                       "(AMAXCHILD(PT) > %(XmaxPT)s           ) &" + \
                       "(ADAMASS('D0') < %(DMassWinSig)s      ) &" + \
                       "(APT           > %(D0MinPT)s          ) &" + \
                       "(AMAXDOCA('')  < %(doca)s             )  "
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
class TwoMuonForD2XXHCombiner(Hlt2Combiner):
    def __init__(self, name):
        from HltTracking.HltPVs import PV3D
        twoMuonDaughterCut = "(TRCHI2DOF< %(Trk_TRCHI2DOF_MAX_mumuX)s )" \
                             "& (PT> %(Trk_PT_MIN_mumuX)s)" \
                             "& (P> %(Trk_P_MIN_mumuX)s)" \
                             "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN_mumuX)s )"
 
        twoMuonCombCut = "(AM<2100)" \
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
                              DaughtersCuts = { "mu+" : twoMuonDaughterCut }, 
                              CombinationCut = twoMuonCombCut,
                              MotherCut = twoMuonMotherCut,
                              Preambulo = [],
                              shared=True)

TwoMuonForD2XXH=TwoMuonForD2XXHCombiner('TwoMuonForD2XXH')

class TwoElectronForD2XXHCombiner(Hlt2Combiner):
    def __init__(self, name):
        from HltTracking.HltPVs import PV3D
        twoElectronDaughterCut = "(TRCHI2DOF< %(Trk_TRCHI2DOF_MAX_eeX)s )" \
                             "& (PT> %(Trk_PT_MIN_eeX)s)" \
                             "& (P> %(Trk_P_MIN_eeX)s)" \
                             "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN_eeX)s )"
 
        twoElectronCombCut = "(AM<2100)" \
                         "& ((APT1+APT2)> %(Pair_SumAPT_MIN_eeX)s)" \
                         "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(Pair_AMINDOCA_MAX_eeX)s )" \
                         "& (AALLSAMEBPV)"

        twoElectronMotherCut = "(BPVVD> %(Pair_BPVVD_MIN_eeX)s )" \
                           "& (BPVCORRM < %(Pair_BPVCORRM_MAX_eeX)s)" \
                           "& (BPVVDCHI2> %(Pair_BPVVDCHI2_MIN_eeX)s )"
        #First stage - Combine 2 Body with pt > 500MeV        
        
        inputs = [Hlt2Electrons]
        Hlt2Combiner.__init__(self, name, ["J/psi(1S) -> e+ e-","phi(1020) -> e+ e+", "rho(770)0 -> e- e-"],
                              inputs,
                              dependencies = [PV3D('Hlt2')],
                              DaughtersCuts = { "e+" : twoElectronDaughterCut}, 
                              CombinationCut = twoElectronCombCut,
                              MotherCut = twoElectronMotherCut,
                              Preambulo = [],
                              shared=True)

TwoElectronForD2XXH=TwoElectronForD2XXHCombiner('TwoElectronForD2XXH')

class TwoMuElForD2XXHCombiner(Hlt2Combiner):
    def __init__(self, name):
        from HltTracking.HltPVs import PV3D
        twoMuElDaughterCut = "(TRCHI2DOF< %(Trk_TRCHI2DOF_MAX_mueX)s )" \
                             "& (PT> %(Trk_PT_MIN_mueX)s)" \
                             "& (P> %(Trk_P_MIN_mueX)s)" \
                             "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN_mueX)s )"
 
        twoMuElCombCut = "(AM<2100)" \
                         "& ((APT1+APT2)> %(Pair_SumAPT_MIN_mueX)s)" \
                         "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(Pair_AMINDOCA_MAX_mueX)s )" \
                         "& (AALLSAMEBPV)"

        twoMuElMotherCut = "(BPVVD> %(Pair_BPVVD_MIN_mueX)s )" \
                           "& (BPVCORRM < %(Pair_BPVCORRM_MAX_mueX)s)" \
                           "& (BPVVDCHI2> %(Pair_BPVVDCHI2_MIN_mueX)s )"
        #First stage - Combine 2 Body with pt > 500MeV        
        
        inputs = [Hlt2Muons,Hlt2Electrons]
        Hlt2Combiner.__init__(self, name, ["J/psi(1S) -> mu+ e-","phi(1020) -> e+ mu-","rho(770)0 -> mu+ e+", "eta -> mu- e-"],
                              inputs,
                              dependencies = [PV3D('Hlt2')],
                              DaughtersCuts = { "mu+" : twoMuElDaughterCut, "e-" : twoMuElDaughterCut}, 
                              CombinationCut = twoMuElCombCut,
                              MotherCut = twoMuElMotherCut,
                              Preambulo = [],
                              shared=True)

TwoMuElForD2XXH=TwoMuElForD2XXHCombiner('TwoMuElForD2XXH')



#Particle filters
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
class InPartFilterHXX(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        from HltTracking.HltPVs import PV3D
        cut = "(TRCHI2DOF< %(TrkChi2_Hmumu)s )" \
                 "& (PT> %(TrkPt_Hmumu)s)" \
                 "& (P> %(TrkP_Hmumu)s)" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_Hmumu)s )"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, dependencies=[PV3D('Hlt2')], shared=True)

#Filtered tracks for threebody
InPartFilterHXX_Kaons = InPartFilterHXX("DHXXKaons",[Hlt2Kaons])
InPartFilterHXX_Pions = InPartFilterHXX("DHXXPions",[Hlt2Pions])
InPartFilterHXX_Protons = InPartFilterHXX("LcPXXProtons",[Hlt2Protons])


class InPartFilterHHXX(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        from HltTracking.HltPVs import PV3D
        cut = "(TRCHI2DOF< %(TrkChi2_HHmumu)s )" \
                 "& (PT> %(TrkPt_HHmumu)s)" \
                 "& (P> %(TrkP_HHmumu)s)" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_HHmumu)s )"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, dependencies=[PV3D('Hlt2')], shared=True)

#Filtered tracks for fourbody
InPartFilterHHXX_Kaons = InPartFilterHHXX("DHHXXKaons",[Hlt2Kaons])
InPartFilterHHXX_Pions = InPartFilterHHXX("DHHXXPions",[Hlt2Pions])

#D2HMUMU COMBINER
class D2HXXCombiner(Hlt2Combiner):
    def __init__(self, name, decayDesc, inputSeq):
        from HltTracking.HltPVs import PV3D
        masscut = "in_range( %(WideMass_M_MIN_Hmumu)s , M , %(WideMass_M_MAX_Hmumu)s )"

        combcuts = "(AM<2100)" \
                   "& (AMAXCHILD(PT) > %(TrkPtMAX_Hmumu)s) "  \
                   "& ((APT1+APT2+APT3) > %(DSumPt_Hmumu)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_Hmumu)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_Hmumu)s)" \
                   "& (AALLSAMEBPV)"
        mothercuts = masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_Hmumu)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_Hmumu)s)" \
                     "& (BPVDIRA                 > %(DDira_Hmumu)s         ) " \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_Hmumu)s )" \
                     "& ( SUMTREE( ( (ID=='K+') | (ID=='K-') | (ID=='pi+') | (ID=='pi-') | (ID=='mu+') | (ID=='mu-')  | (ID=='e+') | (ID=='e-') ), sqrt(BPVIPCHI2()) ) > %(TrkPVIPChi2MAX_Hmumu)s)"\
                     "& (BPVIPCHI2() < %(DIPChi2_Hmumu)s )"
        Hlt2Combiner.__init__(self, name, decayDesc, inputSeq,
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = combcuts,
                              MotherCut = mothercuts,
                              Preambulo = [],
                              shared=True)


#D2H_Ele_MUMU COMBINER
class D2HXX_Ele_Combiner(Hlt2Combiner):
    def __init__(self, name, decayDesc, inputSeq):
        from HltTracking.HltPVs import PV3D
        masscut = "in_range( %(WideMass_M_MIN_Hmumu)s , M , %(WideMass_M_MAX_Hmumu)s )"

        combcuts = "(AM<2100)" \
                   "& (AMAXCHILD(PT) > %(TrkPtMAX_Hmumu)s) "  \
                   "& ((APT1+APT2+APT3) > %(DSumPt_Hmumu)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_Hmumu)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_Hmumu)s)" \
                   "& (AALLSAMEBPV)"
        mothercuts = masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_Hmumu)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_Hmumu)s)" \
                     "& (BPVDIRA                 > %(DDira_Hmumu)s         ) " \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_XeeORmue)s )" \
                     "& ( SUMTREE( ( (ID=='K+') | (ID=='K-') | (ID=='pi+') | (ID=='pi-') | (ID=='mu+') | (ID=='mu-')  | (ID=='e+') | (ID=='e-') ), sqrt(BPVIPCHI2()) ) > %(TrkPVIPChi2MAX_Hmumu)s)"\
                     "& (BPVIPCHI2() < %(DIPChi2_Hmumu)s )"
        Hlt2Combiner.__init__(self, name, decayDesc, inputSeq,
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = combcuts,
                              MotherCut = mothercuts,
                              Preambulo = [],
                              shared=True)



D2PiMuMuComb = D2HXXCombiner("D2PiMuMuComb",[ "D+ -> J/psi(1S) pi+", "D- -> J/psi(1S) pi-"],[InPartFilterHXX_Pions,TwoMuonForD2XXH])
D2KMuMuComb = D2HXXCombiner("D2KMuMuComb",[ "D+ -> J/psi(1S) K+", "D- -> J/psi(1S) K-"],[InPartFilterHXX_Kaons,TwoMuonForD2XXH])
D2PiMuMuSSComb = D2HXXCombiner("D2PiMuMuSSComb", ["D+ -> phi(1020) pi-", "D- -> rho(770)0 pi+"],[InPartFilterHXX_Pions,TwoMuonForD2XXH])
D2KMuMuSSComb = D2HXXCombiner("D2KMuMuSSComb", ["D+ -> phi(1020) K-", "D- -> rho(770)0 K+"],[InPartFilterHXX_Kaons,TwoMuonForD2XXH])

D2PieeComb = D2HXX_Ele_Combiner("D2PieeComb",[ "D+ -> J/psi(1S) pi+", "D- -> J/psi(1S) pi-"],[InPartFilterHXX_Pions,TwoElectronForD2XXH])
D2PieeSSComb = D2HXX_Ele_Combiner("D2PieeSSComb",["D+ -> phi(1020) pi-", "D- -> rho(770)0 pi+"],[InPartFilterHXX_Pions,TwoElectronForD2XXH])
D2KeeComb = D2HXX_Ele_Combiner("D2KeeComb",[ "D+ -> J/psi(1S) K+", "D- -> J/psi(1S) K-"],[InPartFilterHXX_Kaons,TwoElectronForD2XXH])
D2KeeSSComb = D2HXX_Ele_Combiner("D2KeeSSComb",["D+ -> phi(1020) K-", "D- -> rho(770)0 K+"],[InPartFilterHXX_Kaons,TwoElectronForD2XXH])

D2PiMueComb = D2HXX_Ele_Combiner("D2PiMueComb",[ "D+ -> J/psi(1S) pi+", "D- -> J/psi(1S) pi-","D+ -> phi(1020) pi+", "D- -> phi(1020) pi-"],[InPartFilterHXX_Pions,TwoMuElForD2XXH])
D2PiMueSSComb = D2HXX_Ele_Combiner("D2PiMueSSComb",[ "D+ -> rho(770)0 pi-","D- -> eta pi+"],[InPartFilterHXX_Pions,TwoMuElForD2XXH])
D2KMueComb = D2HXX_Ele_Combiner("D2KMueComb",[ "D+ -> J/psi(1S) K+", "D- -> J/psi(1S) K-","D+ -> phi(1020) K+", "D- -> phi(1020) K-"],[InPartFilterHXX_Kaons,TwoMuElForD2XXH])
D2KMueSSComb = D2HXX_Ele_Combiner("D2KMueSSComb",["D+ -> rho(770)0 K-","D- -> eta K+"],[InPartFilterHXX_Kaons,TwoMuElForD2XXH])


#Lc2PXX COMBINER
class Lc2PXXCombiner(Hlt2Combiner):
    def __init__(self, name, decayDesc, inputSeq):
        from HltTracking.HltPVs import PV3D

        Lambda_c_masscut = "in_range( %(Lambda_c_WideMass_M_MIN)s , M , %(Lambda_c_WideMass_M_MAX)s )"

        combcuts = "(AM<2400)" \
                   "& (AMAXCHILD(PT) > %(TrkPtMAX_Hmumu)s) "  \
                   "& ((APT1+APT2+APT3) > %(DSumPt_Hmumu)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_Hmumu)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_Hmumu)s)" \
                   "& (AALLSAMEBPV)"
        mothercuts = Lambda_c_masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_Hmumu)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_Hmumu)s)" \
                     "& (BPVDIRA                 > %(DDira_Hmumu)s         ) " \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_Hmumu)s )" \
                     "& ( SUMTREE( ( (ID=='p+') | (ID=='p~-') | (ID=='pi+') | (ID=='pi-') | (ID=='mu+') | (ID=='mu-')  | (ID=='e+') | (ID=='e-') ), sqrt(BPVIPCHI2()) ) > %(TrkPVIPChi2MAX_Hmumu)s)"\
                     "& (BPVIPCHI2() < %(DIPChi2_Hmumu)s )"
        Hlt2Combiner.__init__(self, name, decayDesc, inputSeq,
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = combcuts,
                              MotherCut = mothercuts,
                              Preambulo = [],
                              shared=True)


#Lc2PXX_Ele COMBINER
class Lc2PXX_Ele_Combiner(Hlt2Combiner):
    def __init__(self, name, decayDesc, inputSeq):
        from HltTracking.HltPVs import PV3D

        Lambda_c_masscut = "in_range( %(Lambda_c_WideMass_M_MIN)s , M , %(Lambda_c_WideMass_M_MAX)s )"

        combcuts = "(AM<2400)" \
                   "& (AMAXCHILD(PT) > %(TrkPtMAX_Hmumu)s) "  \
                   "& ((APT1+APT2+APT3) > %(DSumPt_Hmumu)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_Hmumu)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_Hmumu)s)" \
                   "& (AALLSAMEBPV)"
        mothercuts = Lambda_c_masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_Hmumu)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_Hmumu)s)" \
                     "& (BPVDIRA                 > %(DDira_Hmumu)s         ) " \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_XeeORmue)s )" \
                     "& ( SUMTREE( ( (ID=='p+') | (ID=='p~-') | (ID=='pi+') | (ID=='pi-') | (ID=='mu+') | (ID=='mu-')  | (ID=='e+') | (ID=='e-') ), sqrt(BPVIPCHI2()) ) > %(TrkPVIPChi2MAX_Hmumu)s)"\
                     "& (BPVIPCHI2() < %(DIPChi2_Hmumu)s )"
        Hlt2Combiner.__init__(self, name, decayDesc, inputSeq,
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = combcuts,
                              MotherCut = mothercuts,
                              Preambulo = [],
                              shared=True)



Lc2PMuMuComb = Lc2PXXCombiner("Lc2PMuMuComb",["Lambda_c+ -> J/psi(1S) p+" , "Lambda_c~- -> J/psi(1S) p~-"],[InPartFilterHXX_Protons,TwoMuonForD2XXH])
Lc2PMuMuSSComb = Lc2PXXCombiner("Lc2PMuMuSSComb", ["Lambda_c+ -> phi(1020) p~-" , "Lambda_c~- ->  rho(770)0 p+"],[InPartFilterHXX_Protons,TwoMuonForD2XXH])

Lc2PMueComb = Lc2PXX_Ele_Combiner("Lc2PMueComb",["Lambda_c+ -> J/psi(1S) p+" , "Lambda_c~- -> J/psi(1S) p~-","Lambda_c+ -> phi(1020) p+" , "Lambda_c~- -> phi(1020) p~-"],[InPartFilterHXX_Protons,TwoMuElForD2XXH])
Lc2PeeComb = Lc2PXX_Ele_Combiner("Lc2PeeComb",["Lambda_c+ -> J/psi(1S) p+" , "Lambda_c~- -> J/psi(1S) p~-"],[InPartFilterHXX_Protons,TwoElectronForD2XXH])


#D02HHMUMU COMBINER
class D02HHXXCombiner(Hlt2Combiner):
    def __init__(self, name, decayDesc, inputSeq):
        from HltTracking.HltPVs import PV3D
       
  #      massmin = min(float("%(Sig_M_MIN_HHmumu)s"), float("%(WideMass_M_MIN_HHmumu)s"))
 #       massmax = max(float("%(Sig_M_MAX_HHmumu)s"), float("%(WideMass_M_MAX_HHmumu)s"))

#        masscut = "in_range(%s,  M, %s)" % (massmin, massmax)
        masscut = "in_range( %(WideMass_M_MIN_HHmumu)s , M , %(WideMass_M_MAX_HHmumu)s )"
        combcuts = "(AM<2100)" \
                   "& (AMAXCHILD(PT) > %(TrkPtMAX_HHmumu)s) "  \
                   "& ((APT1+APT2+APT3+APT4) > %(DSumPt_HHmumu)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_HHmumu)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_HHmumu)s)" \
                   "& (AALLSAMEBPV)"
        mothercuts = masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_HHmumu)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_HHmumu)s)" \
                     "& (BPVDIRA                 > %(DDira_HHmumu)s         ) " \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_HHmumu)s )" \
                     "& ( SUMTREE( ( (ID=='K+') | (ID=='K-') | (ID=='pi+') | (ID=='pi-') | (ID=='mu+') | (ID=='mu-')  | (ID=='e+') | (ID=='e-') ), sqrt(BPVIPCHI2()) ) > %(TrkPVIPChi2MAX_HHmumu)s)"\
                     "& (BPVIPCHI2() < %(DIPChi2_HHmumu)s )"
                     
        Hlt2Combiner.__init__(self, name, decayDesc, inputSeq,
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = combcuts,
                              MotherCut = mothercuts,
                              Preambulo = [],
                              shared=True)


class D02HHXX_Ele_Combiner(Hlt2Combiner):
    def __init__(self, name, decayDesc, inputSeq):
        from HltTracking.HltPVs import PV3D
       
  #      massmin = min(float("%(Sig_M_MIN_HHmumu)s"), float("%(WideMass_M_MIN_HHmumu)s"))
 #       massmax = max(float("%(Sig_M_MAX_HHmumu)s"), float("%(WideMass_M_MAX_HHmumu)s"))

#        masscut = "in_range(%s,  M, %s)" % (massmin, massmax)
        masscut = "in_range( %(WideMass_M_MIN_HHmumu)s , M , %(WideMass_M_MAX_HHmumu)s )"
        combcuts = "(AM<2100)" \
                   "& (AMAXCHILD(PT) > %(TrkPtMAX_HHmumu)s) "  \
                   "& (AMINCHILD( MIPCHI2DV(PRIMARY) ) > %(TrkPVIPChi2_XeeORmue)s  ) "  \
                   "& ((APT1+APT2+APT3+APT4) > %(DSumPt_HHmumu)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_HHmumu)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_HHmumu)s)" \
                   "& (AALLSAMEBPV)"
        mothercuts = masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_HHmumu)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_HHmumu)s)" \
                     "& (BPVDIRA                 > %(DDira_HHmumu)s         ) " \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_XeeORmue)s )" \
                     "& ( SUMTREE( ( (ID=='K+') | (ID=='K-') | (ID=='pi+') | (ID=='pi-') | (ID=='mu+') | (ID=='mu-')  | (ID=='e+') | (ID=='e-') ), sqrt(BPVIPCHI2()) ) > %(TrkPVIPChi2MAX_HHmumu)s)"\
                     "& (BPVIPCHI2() < %(DIPChi2_HHmumu)s )"
                     
        Hlt2Combiner.__init__(self, name, decayDesc, inputSeq,
                              dependencies = [PV3D('Hlt2')],
                              CombinationCut = combcuts,
                              MotherCut = mothercuts,
                              Preambulo = [],
                              shared=True)



D02PiPiMuMuComb = D02HHXXCombiner("D02PiPiMuMuComb","D0 -> J/psi(1S) pi+ pi-",[InPartFilterHHXX_Pions,TwoMuonForD2XXH])
D02KKMuMuComb = D02HHXXCombiner("D02KKMuMuComb","D0 -> J/psi(1S) K+ K-",[InPartFilterHHXX_Kaons,TwoMuonForD2XXH])
D02KPiMuMuComb = D02HHXXCombiner("D02KPiMuMuComb",["D0 -> J/psi(1S) K+ pi-","D0 -> J/psi(1S) K- pi+"],[InPartFilterHHXX_Pions,InPartFilterHHXX_Kaons,TwoMuonForD2XXH])
D02KPiMuMuSSComb = D02HHXXCombiner("D02KPiMuMuSSComb",["D0 -> phi(1020) K- pi-","D0 -> rho(770)0  K+ pi+"],[InPartFilterHHXX_Pions,InPartFilterHHXX_Kaons,TwoMuonForD2XXH])

D02PiPiMueComb = D02HHXX_Ele_Combiner("D02PiPiMueComb",["D0 -> J/psi(1S) pi+ pi-","D0 -> phi(1020) pi+ pi-"],[InPartFilterHHXX_Pions,TwoMuElForD2XXH])
D02KKMueComb = D02HHXX_Ele_Combiner("D02KKMueComb",["D0 -> J/psi(1S) K+ K-","D0 -> phi(1020) K+ K-"],[InPartFilterHHXX_Kaons,TwoMuElForD2XXH])
D02KPiMueComb = D02HHXX_Ele_Combiner("D02KPiMueComb",["D0 -> J/psi(1S) K+ pi-","D0 -> J/psi(1S) K- pi+","D0 -> phi(1020) K+ pi-","D0 -> phi(1020) K- pi+"],[InPartFilterHHXX_Pions,InPartFilterHHXX_Kaons,TwoMuElForD2XXH])

D02PiPieeComb = D02HHXX_Ele_Combiner("D02PiPieeComb","D0 -> J/psi(1S) pi+ pi-",[InPartFilterHHXX_Pions,TwoElectronForD2XXH])
D02KKeeComb = D02HHXX_Ele_Combiner("D02KKeeComb","D0 -> J/psi(1S) K+ K-",[InPartFilterHHXX_Kaons,TwoElectronForD2XXH])
D02KPieeComb = D02HHXX_Ele_Combiner("D02KPieeComb",["D0 -> J/psi(1S) K+ pi-","D0 -> J/psi(1S) K- pi+"],[InPartFilterHHXX_Pions,InPartFilterHHXX_Kaons,TwoElectronForD2XXH])





#D2HMUMU FILTER
class D2HXXFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        from HltTracking.HltPVs import PV3D
        cut  = "in_range(%(Sig_M_MIN_Hmumu)s, M, %(Sig_M_MAX_Hmumu)s)"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, dependencies=[PV3D('Hlt2')])

D2PiMuMuFilter = D2HXXFilter("D2PiMuMuFilter",[D2PiMuMuComb])
D2KMuMuFilter = D2HXXFilter("D2KMuMuFilter",[D2KMuMuComb])
D2PiMuMuSSFilter = D2HXXFilter("D2PiMuMuSSFilter",[D2PiMuMuSSComb])
D2KMuMuSSFilter = D2HXXFilter("D2KMuMuSSFilter",[D2KMuMuSSComb])

D2PiMueFilter = D2HXXFilter("D2PiMueFilter",[D2PiMueComb])
D2KMueFilter = D2HXXFilter("D2KMueFilter",[D2KMueComb])
D2PiMueSSFilter = D2HXXFilter("D2PiMueSSFilter",[D2PiMueSSComb])
D2KMueSSFilter = D2HXXFilter("D2KMueSSFilter",[D2KMueSSComb])

D2PieeFilter = D2HXXFilter("D2PieeFilter",[D2PieeComb])
D2KeeFilter = D2HXXFilter("D2KeeFilter",[D2KeeComb])
D2PieeSSFilter = D2HXXFilter("D2PieeSSFilter",[D2PieeSSComb])
D2KeeSSFilter = D2HXXFilter("D2KeeSSFilter",[D2KeeSSComb])

#Lc2PXX FILTER
class Lc2PXXFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        from HltTracking.HltPVs import PV3D
        cut  = "in_range(%(Lambda_c_Sig_M_MIN)s, M, %(Lambda_c_Sig_M_MAX)s)"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, dependencies=[PV3D('Hlt2')])

Lc2PMuMuSSFilter = Lc2PXXFilter("Lc2PMuMuSSFilter",[Lc2PMuMuSSComb])
Lc2PMuMuFilter = Lc2PXXFilter("Lc2PMuMuFilter",[Lc2PMuMuComb])
Lc2PMueFilter = Lc2PXXFilter("Lc2PMueFilter",[Lc2PMueComb])

Lc2PeeFilter = Lc2PXXFilter("Lc2PeeFilter",[Lc2PeeComb])

#D02HHXX FILTER
class D02HHXXFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs):
        from HltTracking.HltPVs import PV3D
        cut  = "in_range(%(Sig_M_MIN_HHmumu)s, M, %(Sig_M_MAX_HHmumu)s)"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, dependencies=[PV3D('Hlt2')])


D02PiPiMuMuFilter = D02HHXXFilter("D02PiPiMuMuFilter",[D02PiPiMuMuComb])
D02KKMuMuFilter = D02HHXXFilter("D02KKMuMuFilter",[D02KKMuMuComb])
D02KPiMuMuFilter = D02HHXXFilter("D02KPiMuMuFilter",[D02KPiMuMuComb])

D02PiPiMueFilter = D02HHXXFilter("D02PiPiMueFilter",[D02PiPiMueComb])
D02KKMueFilter = D02HHXXFilter("D02KKMueFilter",[D02KKMueComb])
D02KPiMueFilter = D02HHXXFilter("D02KPiMueFilter",[D02KPiMueComb])

D02PiPieeFilter = D02HHXXFilter("D02PiPieeFilter",[D02PiPieeComb])
D02KKeeFilter = D02HHXXFilter("D02KKeeFilter",[D02KKeeComb])
D02KPieeFilter = D02HHXXFilter("D02KPieeFilter",[D02KPieeComb])

D02KPiMuMuSSFilter = D02HHXXFilter("D02KPiMuMuSSFilter",[D02KPiMuMuSSComb])
