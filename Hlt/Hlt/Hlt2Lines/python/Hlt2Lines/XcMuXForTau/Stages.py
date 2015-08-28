###
#  @author G. Ciezarek gregory.max.ciezarek@cern.ch
#          B. Hamilton   
#  @date 2015-28-08
#
#  Please contact the abovementioned responsibles before editing this file
#
##


from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Inputs import Hlt2Muons, Hlt2Pions, Hlt2Kaons, Hlt2Protons
from Hlt2Lines.Utilities.Hlt2MergedStage import Hlt2MergedStage
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
class MuonFilter(Hlt2ParticleFilter):
    def __init__(self, name, nickname, inputs):
        cut = ("(MIPCHI2DV(PRIMARY) > %(Mu_IPCHI2)s)")
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,nickname = nickname, dependencies=[PV3D('Hlt2')],shared=True)


filteredMuons = MuonFilter('XcMuXForTauFilteredMuons','Muon', [Hlt2Muons])
filteredPions = MuonFilter('XcMuXForTauFilteredPions','Muon', [Hlt2Pions])

class DTrackFilter(Hlt2ParticleFilter):
    def __init__(self, name, nickname, inputs,hadron):
        dc = "(MIPCHI2DV(PRIMARY) > %(DTrk_ALL_MIPCHI2DV_MIN)s)"\
                        "& (PT > %(DTrk_ALL_PT_MIN)s)" \
                        "& (P > %(DTrk_ALL_P_MIN)s)" 
        if (hadron=='pi'):
            dc = dc + "& (PIDK < %(Pi_PIDK)s)"
        if (hadron=='K'):
            dc = dc + "& (PIDK > %(K_PIDK)s)"
        if (hadron=='p'):
            dc = dc + "& (PIDp > %(P_PIDp)s)"
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, dc, inputs,nickname = nickname, dependencies=[PV3D('Hlt2')],shared=True)


filteredDPions = DTrackFilter('XcMuXForTauFilteredDPions','XcDaughter', [Hlt2Pions],'pi')
filteredDKaons = DTrackFilter('XcMuXForTauFilteredDKaons','XcDaughter', [Hlt2Kaons],'K')
filteredDProtons = DTrackFilter('XcMuXForTauFilteredDProtons','XcDaughter', [Hlt2Protons],'p')

                                         
class D2XCombiner(Hlt2Combiner):
    
    def __init__(self, name, nickname, decayDesc, inputs, masswindow):
        from HltTracking.HltPVs import PV3D
        masscut = ''
        if masswindow == 'D':
            masscut = "in_range(%(D_AM_MIN)s,  AM, %(D_AM_MAX)s)"
        if masswindow == 'Ds':
            masscut = "in_range(%(Ds_AM_MIN)s,  AM, %(Ds_AM_MAX)s)"
        if masswindow == 'Lc':
            masscut = "(in_range(%(Lc_AM_MIN)s,  AM, %(Lc_AM_MAX)s))"
        combcuts = masscut + " &((APT1 > %(MIN_D_CHILDPT)s) " \
            "| (APT2 > %(MIN_D_CHILDPT)s)" \
            "| (APT3 > %(MIN_D_CHILDPT)s)" \
            "| (APT4 > %(MIN_D_CHILDPT)s))" \
            "& (APT > %(D0_PT_MIN)s)" \
            "& (AMINDOCA('') " \
            "< %(Pair_AMINDOCA_MAX)s )"\
            "& ((APT1+APT2+APT3+APT4) > %(D_SUMPT)s) "    

        parentcuts = "(VFASPF(VCHI2PDOF) < %(D0_VCHI2PDOF_MAX)s)" \
            "& (BPVVDCHI2> %(D0_BPVVDCHI2_MIN)s )" \
            "& (BPVDIRA > %(D0_BPVDIRA_MIN)s )"
                     
        Hlt2Combiner.__init__(self, name,  decayDesc,
                              inputs = inputs,
                              dependencies = [ PV3D('Hlt2') ],
                              nickname = nickname,
                              CombinationCut = combcuts,
                              MotherCut = parentcuts,
                              Preambulo = [],
                              shared=True)
    
# Combiners
D02kpiComb   = D2XCombiner("XcMuXForTauKPi","Xc","[D0 -> K- pi+]cc",[filteredDPions, filteredDKaons],"D")
D02k3piComb   = D2XCombiner("XcMuXForTauK3Pi","Xc","[D0 -> K- pi+ pi- pi+]cc",[filteredDPions, filteredDKaons],"D")
Dplus2kpipiComb   = D2XCombiner("XcMuXForTauKPiPi","Xc","[D+ -> K- pi+ pi+]cc",[filteredDPions, filteredDKaons],"D")
Ds2kkpiComb   = D2XCombiner("XcMuXForTauKKPi","Xc","[D+ -> K- K+ pi+]cc",[filteredDPions, filteredDKaons],"Ds")
Lc2pkpiComb   = D2XCombiner("XcMuXForTauPKPi","Xc","[Lambda_c+ -> p+ K- pi+]cc",[filteredDPions, filteredDKaons,filteredDProtons],"Lc")

# Merge Combiners
MergeAllXc = Hlt2MergedStage('MergedXc', inputs = [D02kpiComb,D02k3piComb,Dplus2kpipiComb,Ds2kkpiComb,Lc2pkpiComb],shared=True)


class B2XMuCombiner(Hlt2Combiner):
    def __init__(self, name, nickname, decayDesc, inputSeq):
        from HltTracking.HltPVs import PV3D
        masscut = "(M < 10500)"

        combcuts = "(AM<11000)"  \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(XcMu_DOCA_MAX)s)" 
        mothercuts = masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(XcMu_VTXCHI2)s) " \
                     "& (BPVDIRA                 > %(XcMu_DIRA)s         ) " \
                     "& (BPVVDCHI2> %(XcMu_FDCHI2)s )" 
        Hlt2Combiner.__init__(self, name, decayDesc, inputSeq,
                              dependencies = [PV3D('Hlt2')],
                              nickname = nickname,
                              CombinationCut = combcuts,
                              MotherCut = mothercuts,
                              Preambulo = [],
                              shared=True)


B2XcMuComb = B2XMuCombiner("B2XcMuComb","B",[ "[B+ -> D0 mu+]cc", "[B+ -> D0 mu-]cc","[B+ -> D- mu+]cc", "[B+ -> D- mu-]cc","[B+ -> Lambda_c~- mu+]cc", "[B+ -> Lambda_c~- mu-]cc"],[filteredMuons,MergeAllXc])
B2XcFakeMuComb = B2XMuCombiner("B2XcFakeMuComb","B",[ "[B+ -> D0 pi+]cc", "[B+ -> D0 pi-]cc","[B+ -> D- pi+]cc", "[B+ -> D- pi-]cc","[B+ -> Lambda_c~- pi+]cc", "[B+ -> Lambda_c~- pi-]cc"],[filteredPions,MergeAllXc])

