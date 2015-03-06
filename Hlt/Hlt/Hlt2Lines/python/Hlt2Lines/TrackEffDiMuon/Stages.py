__author__  = 'Michael Kolpin'
__date__    = '$Date: 2015-03-03$'
__version__ = '$Revision: 0.0 $'

# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter

# Import the used particles
from Inputs import BiKalmanFittedMuons, TTMuons, VeloMuons, DownMuons, LongAssocMuons

# Combination of Tag and Probe to JPsi
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
class JPsiCombiner(Hlt2Combiner):
    def __init__(self, name, mode, nickname):
        cc =    ("(AMAXDOCA('') < %(JPsiDOCA)s)")
        mc =    ("(VFASPF(VCHI2PDOF) < %(JPsiVtxChi2)s)" +
                 " & (PT > %(JPsiPt)s)" +
                 " & (MIPDV(PRIMARY) < %(JPsiMaxIP)s)" +
                 " & (ADMASS('J/psi(1S)') < %(JPsiMassWin)s)")
        inputs = [TagFilter('Tag1', mode, nickname), ProbeFilter('Probe1', mode, nickname)]
        Hlt2Combiner.__init__(self, name, "J/psi(1S) -> mu+ mu-", inputs,
                              dependencies = [], nickname = nickname,
                              tistos = [], CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])
        
# 3Body combiner to check overlap between probe and long track (pion id)
from Configurables import DaVinci__N3BodyDecays as N3Body 
class N3BodyCombiner(Hlt2Combiner):
    def __init__(self, name, mode, nickname):
        if nickname == 'MuonTT':
       	    overlap = ("(AOVERLAP(1,2, LHCb.LHCbID.TT) > %(OverlapTT)s) & (AOVERLAP(1,2, LHCb.LHCbID.Muon) > %(OverlapMuon)s)") 
        elif nickname == 'VeloMuon':
       	    overlap = ("(AOVERLAP(1,2, LHCb.LHCbID.Velo) > %(OverlapVelo)s) & (AOVERLAP(1,2, LHCb.LHCbID.Muon) > %(OverlapMuon)s)") 
        else:
       	    overlap = ("(AOVERLAP(1,2, LHCb.LHCbID.TT) > %(OverlapTT)s) & ((AOVERLAP(1,2, LHCb.LHCbID.IT) > %(OverlapIT)s) | (AOVERLAP(1,2, LHCb.LHCbID.OT) > %(OverlapOT)s))")
        inputs = [TagFilter('Tag2', mode, nickname), ProbeFilter('Probe2', mode, nickname), LongFilter('Long', mode, nickname)]
        decay = "J/psi(1S) -> pi+ mu+ mu-" if mode == 1 else "J/psi(1S) -> pi- mu- mu+"
        Hlt2Combiner.__init__(self, name, decay , inputs, combiner = N3Body,
                              dependencies = [], nickname = nickname,
                              tistos = [], Combination12Cut = overlap, ParticleCombiners ={"" : "MomentumCombiner"},
                              MotherCut = "ALL", Preambulo = [])
        
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
# Tag filter
class TagFilter(Hlt2ParticleFilter):
    def __init__(self, name, mode, nickname):
        inputs = [BiKalmanFittedMuons]
        charge_cut = "(Q<0)" if mode == 1 else "(Q>0)"
        cut = (charge_cut +
               " & (TRCHI2DOF <%(TagTrChi2)s )" +
               " & (P >%(TagP)s )" +
               " & (PT >%(TagPt)s )" +
               " & (PIDmu >-%(TagMuonID)s )" +
               " & (MIPDV(PRIMARY)>%(TagMinIP)s )")
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, tistos = 'TisTosSpec', nickname = nickname)

# Probe filter
class ProbeFilter(Hlt2ParticleFilter):
    def __init__(self, name, mode, nickname):
        inputs = [TTMuons] if nickname == 'MuonTT' else [VeloMuons] if nickname == 'VeloMuon' else [DownMuons]
        charge_cut = "(Q>0)" if mode == 1 else "(Q<0)"
        cut = (charge_cut +
               " & (TRCHI2DOF <%(ProbeTrChi2)s )" +
               " & (P >%(ProbeP)s )" +
               " & (PT >%(ProbePt)s )")
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, nickname = nickname)


# Long filter
class LongFilter(Hlt2ParticleFilter):
    def __init__(self, name, mode, nickname):
        inputs = [LongAssocMuons]
        cut = "(Q>0)" if mode == 1 else "(Q<0)"
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, nickname = nickname)
