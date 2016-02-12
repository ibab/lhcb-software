# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter

# Import the used particles
from Inputs import BiKalmanFittedMuons, TTMuons, VeloMuons, DownMuons, LongAssocMuons

# Combination of Tag and Probe to JPsi
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from HltTracking.HltPVs import PV3D
class JPsiCombiner(Hlt2Combiner):
    def __init__(self, mode, nickname, stat):
        cc =    ("(AMAXDOCA('') < %(JPsiDOCA)s)")
        mc =    ("(VFASPF(VCHI2PDOF) < %(JPsiVtxChi2)s)" +
                 " & (PT > %(JPsiPt)s)" +
                 " & (MIPDV(PRIMARY) < %(JPsiMaxIP)s)" +
                 " & (MIPCHI2DV(PRIMARY) < %(JPsiMaxIPchi2)s)" +
                 " & (ADMASS('J/psi(1S)') < %(JPsiMassWin)s)")
        inputs = [TagFilter(mode, nickname), ProbeFilter(mode, nickname, stat)]
        Hlt2Combiner.__init__(self, 'TrackEff'+nickname+str(mode)+stat+'JPsi', "J/psi(1S) -> mu+ mu-", inputs,
                              dependencies = [PV3D('Hlt2')], nickname = nickname,
                              tistos = [], CombinationCut = cc,
                              MotherCut = mc, Preambulo = [], shared = True)
        
# 3Body combiner to check overlap between probe and long track (pion id)
from Configurables import DaVinci__N3BodyDecays as N3Body 
class N3BodyCombiner(Hlt2Combiner):
    def __init__(self, mode, nickname, stat):
        if nickname == 'MuonTT':
       	    overlap = ("(AOVERLAP(1,2, LHCb.LHCbID.TT) > %(OverlapTT)s) & (AOVERLAP(1,2, LHCb.LHCbID.Muon) > %(OverlapMuon)s)") 
        elif nickname == 'VeloMuon':
       	    overlap = ("(AOVERLAP(1,2, LHCb.LHCbID.Velo) > %(OverlapVelo)s) & (AOVERLAP(1,2, LHCb.LHCbID.Muon) > %(OverlapMuon)s)") 
        else:
       	    overlap = ("(AOVERLAP(1,2, LHCb.LHCbID.TT) > %(OverlapTT)s) & ((AOVERLAP(1,2, LHCb.LHCbID.IT) > %(OverlapIT)s) | (AOVERLAP(1,2, LHCb.LHCbID.OT) > %(OverlapOT)s))")
        inputs = [TagFilter(mode, nickname), ProbeFilter(mode, nickname, stat), LongFilter(mode, nickname)]
        decay = "J/psi(1S) -> pi+ mu+ mu-" if mode == 1 else "J/psi(1S) -> pi- mu- mu+"
        Hlt2Combiner.__init__(self, 'TrackEff'+nickname+str(mode)+stat+'N3Body', decay , inputs, combiner = N3Body,
                              dependencies = [PV3D('Hlt2')], nickname = nickname,
                              tistos = [], Combination12Cut = overlap, ParticleCombiners ={"" : "MomentumCombiner"},
                              MotherCut = "ALL", Preambulo = [], shared = True)
        
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
# Tag filter
class TagFilter(Hlt2ParticleFilter):
    def __init__(self, mode, nickname):
        inputs = [BiKalmanFittedMuons]
        charge_cut = "(Q<0)" if mode == 1 else "(Q>0)"
        cut = (charge_cut +
               " & (TRCHI2DOF <%(TagTrChi2)s )" +
               " & (P >%(TagP)s )" +
               " & (PT >%(TagPt)s )" +
               " & (PIDmu >-%(TagMuonID)s )" +
               " & (MIPDV(PRIMARY)>%(TagMinIP)s )" +
               " & (MIPCHI2DV(PRIMARY)>%(TagMinIPchi2)s )")
        Hlt2ParticleFilter.__init__(self, 'TrackEff'+nickname+str(mode)+'Tag', cut, inputs, dependencies = [PV3D('Hlt2')], tistos = 'TisTosSpec', nickname = nickname, shared = True)

# Probe filter
from HltLine.Hlt2Monitoring import Hlt2Monitor,Hlt2MonitorMinMax
class ProbeFilter(Hlt2ParticleFilter):
    def __init__(self, mode, nickname, stat):
        inputs = [TTMuons] if nickname == 'MuonTT' else [VeloMuons] if nickname == 'VeloMuon' else [DownMuons]
        charge_cut = "(Q>0)" if mode == 1 else "(Q<0)"
        stat_cut = " & ((PT >%(StatHighPT)s) | (ETA >%(StatHighETA)s) | (ETA <%(StatLowETA)s))" if stat == 'low' else " & ((PT <%(StatHighPT)s) & (ETA <%(StatHighETA)s) & (ETA >%(StatLowETA)s))"
        cut = (charge_cut + stat_cut +
               " & (TRCHI2DOF <%(ProbeTrChi2)s )" +
               " & (P >%(ProbeP)s )" +
               " & (PT >%(ProbePt)s )")
        Hlt2ParticleFilter.__init__(self, 'TrackEff'+nickname+str(mode)+stat+'Probe', cut, inputs, dependencies = [PV3D('Hlt2')], nickname = nickname, shared = True)


# Long filter
class LongFilter(Hlt2ParticleFilter):
    def __init__(self, mode, nickname):
        inputs = [LongAssocMuons]
        cut = "(Q>0)" if mode == 1 else "(Q<0)"
        Hlt2ParticleFilter.__init__(self, 'TrackEff'+nickname+str(mode)+'Long', cut, inputs, dependencies = [PV3D('Hlt2')], nickname = nickname, shared = True)
