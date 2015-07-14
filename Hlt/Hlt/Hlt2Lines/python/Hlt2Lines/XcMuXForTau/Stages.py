###
#  @author G. Ciezarek gregory.max.ciezarek@cern.ch
#          B. Hamilton   
#  @date 2015-14-07
#
#  Please contact the abovementioned responsibles before editing this file
#
##


from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Inputs import Hlt2Muons, Hlt2Pions, Hlt2Kaons, Hlt2Protons

from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
class MuonFilter(Hlt2ParticleFilter):
    """Filter the Hlt1 TOS track"""
    def __init__(self, name, inputs):
        cut = ("(MIPCHI2DV(PRIMARY) > %(Mu_IPCHI2)s)")
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, dependencies=[PV3D('Hlt2')])


filteredMuons = MuonFilter('FilteredMuons', [Hlt2Muons])
filteredPions = MuonFilter('FilteredPions', [Hlt2Pions])

                                         
class D2XCombiner(Hlt2Combiner):
    
    def __init__(self, name,decayDesc,masswindow):
        from HltTracking.HltPVs import PV3D
        dc = { }
        for child in ['pi+','K+','p+'] :
            dc[child] = "(PT > %(DTrk_ALL_PT_MIN)s)" \
                        "& (P > %(DTrk_ALL_P_MIN)s)" \
                        "& (MIPCHI2DV(PRIMARY) > %(DTrk_ALL_MIPCHI2DV_MIN)s)"
    dc['pi+'] = dc['pi+'] + "& (PIDK < %(Pi_PIDK)s)"
    dc['K+'] = dc['K+'] + "& (PIDK > %(K_PIDK)s)"
    dc['p+'] = dc['p+'] + "& (PIDp > %(P_PIDp)s)"
        masscut = ''
        if masswindow == 'D':
          masscut = "in_range(%(D_AM_MIN)s,  AM, %(D_AM_MAX)s)"
        if masswindow == 'Ds':
          masscut = "in_range(%(Ds_AM_MIN)s,  AM, %(Ds_AM_MAX)s)"
        if masswindow == 'Lc':
          masscut = "(in_range(%(Lc_AM_MIN)s,  AM, %(Lc_AM_MAX)s))"
        combcuts = masscut + "& ((APT1 > %(MIN_D_CHILDPT)s) " \
                       "| (APT2 > %(MIN_D_CHILDPT)s)" \
                       "| (APT3 > %(MIN_D_CHILDPT)s)" \
                       "| (APT4 > %(MIN_D_CHILDPT)s))" \
                   "& (APT > %(D0_PT_MIN)s)" \
                   "& (AMINDOCA('') " \
                       "< %(Pair_AMINDOCA_MAX)s )"\
                   "& ((APT1+APT2+APT3+APT4) > %(D_SUMPT)s)" 

        parentcuts = "(VFASPF(VCHI2PDOF) < %(D0_VCHI2PDOF_MAX)s)" \
                     "& (BPVVDCHI2> %(D0_BPVVDCHI2_MIN)s )" \
                     "& (BPVDIRA > %(D0_BPVDIRA_MIN)s )"
                     
    Hlt2Combiner.__init__(self, name, decayDesc,
			      [Hlt2Pions, Hlt2Kaons,Hlt2Protons],
                              dependencies = [ PV3D('Hlt2') ],
                              #tistos = 'TisTosSpec',
                              DaughtersCuts = dc,
                              CombinationCut = combcuts,
                              MotherCut = parentcuts,
                              Preambulo = [])

# Combiners
D02kpiComb   = D2XCombiner("KPi","[D0 -> K- pi+]cc","D")
D02k3piComb   = D2XCombiner("K3Pi","[D0 -> K- pi+ pi- pi+]cc","D")
Dplus2kpipiComb   = D2XCombiner("KPiPi","[D+ -> K- pi+ pi+]cc","D")
Ds2kkpiComb   = D2XCombiner("KKPi","[D+ -> K- K+ pi+]cc","Ds")
Lc2pkpiComb   = D2XCombiner("PKPi","[Lambda_c+ -> p+ K- pi+]cc","Ds")

class B2XMuCombiner(Hlt2Combiner):
    def __init__(self, name, decayDesc, inputSeq):
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
                              CombinationCut = combcuts,
                              MotherCut = mothercuts,
                              Preambulo = [],
                              shared=True)


B2D0KPiMuComb = B2XMuCombiner("B2D0KPiMuComb",[ "B+ -> D0 mu+", "B+ -> D0 mu-"],[D02kpiComb,filteredMuons])
B2D0K3PiMuComb = B2XMuCombiner("B2D0K3PiMuComb",[ "B+ -> D0 mu+", "B+ -> D0 mu-"],[D02k3piComb,filteredMuons])
B2DplusKPiPiMuComb = B2XMuCombiner("B2DplusKPiPiMuComb",[ "B+ -> D- mu+", "B+ -> D- mu-"],[Dplus2kpipiComb,filteredMuons])
B2DsKKPiMuComb = B2XMuCombiner("B2DsKKPiMuComb",[ "B+ -> D- mu+", "B+ -> D- mu-"],[Ds2kkpiComb,filteredMuons])
B2LcPKPiMuComb = B2XMuCombiner("B2LcPKPiMuComb",[ "B+ -> Lambda_c~- mu+", "B+ -> Lambda_c~- mu-"],[Lc2pkpiComb,filteredMuons])

B2D0KPiFakeMuComb = B2XMuCombiner("B2D0KPiFakeMuComb",[ "B+ -> D0 pi+", "B+ -> D0 pi-"],[D02kpiComb,filteredPions])
B2D0K3PiFakeMuComb = B2XMuCombiner("B2D0K3PiFakeMuComb",[ "B+ -> D0 pi+", "B+ -> D0 pi-"],[D02k3piComb,filteredPions])
B2DplusKPiPiFakeMuComb = B2XMuCombiner("B2DplusKPiPiFakeMuComb",[ "B+ -> D- pi+", "B+ -> D- pi-"],[Dplus2kpipiComb,filteredPions])
B2DsKKPiFakeMuComb = B2XMuCombiner("B2DsKKPiFakeMuComb",[ "B+ -> D- pi+", "B+ -> D- pi-"],[Ds2kkpiComb,filteredPions])
B2LcPKPiFakeMuComb = B2XMuCombiner("B2LcPKPiFakeMuComb",[ "B+ -> Lambda_c~- pi+", "B+ -> Lambda_c~- pi-"],[Lc2pkpiComb,filteredPions])

