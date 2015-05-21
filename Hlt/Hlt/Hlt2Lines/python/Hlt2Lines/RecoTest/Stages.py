# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Stage import Hlt2Stage
from HltTracking.HltPVs import PV3D

from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Inputs import Pions,DownPions,Photons,TrackFittedDiMuon

class CreatePions(Hlt2ParticleFilter):
    def __init__(self,name):
        code = ("(PT >  %(Pt)s)")
        inputs = [ Pions, DownPions, Photons ]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, dependencies = [PV3D('Hlt2')])

class JpsiReFitPVsFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(ADMASS('J/psi(1S)') < %(MassWindow)s) " +
                "& (PT > %(Pt)s) " +
                "& (MAXTREE('mu-' == ABSID,TRCHI2DOF) < %(TrChi2Tight)s )" +
                "& (MINTREE('mu-' == ABSID,PT) > %(MuPt)s) " +
                "& (VFASPF(VCHI2PDOF) < %(VertexChi2)s )")
        from HltLine.Hlt2Monitoring import Hlt2Monitor, Hlt2MonitorMinMax
        args = {'PreMonitor'  : Hlt2Monitor("M", "M(#mu#mu)", 3097, 200, 'M_in',  nbins = 25),
                'PostMonitor' : Hlt2Monitor("M", "M(#mu#mu)", 3097, 200, 'M_out', nbins = 25)
                                + " & " + Hlt2MonitorMinMax("PT","PT(#mu#mu)", 0, 10000, 'JPsiPT_out', nbins = 100)
                                + " & " + Hlt2MonitorMinMax("MINTREE('mu-' == ABSID, PT)", "MINTREE(mu-==ABSID, PT)", 0, 10000, 'MuPT_out', nbins = 100)
                                + " & " + Hlt2MonitorMinMax("VFASPF(VCHI2PDOF)", "VFASPF(VCHI2PDOF)", 0, 25, 'JPsiVeterxChi2_out', nbins = 100),
                                'ReFitPVs':True,
                                'CloneFilteredParticles':True}
        inputs = [TrackFittedDiMuon]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, shared = False, **args)

