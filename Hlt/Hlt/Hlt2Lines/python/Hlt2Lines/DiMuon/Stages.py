# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from LoKiTrigger.decorators import *


from HltTracking.HltPVs import PV3D
from Inputs import TrackFittedDiMuon

class DiMuonFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(MM > %(MinMass)s) & (PT> %(Pt)s)" +
                " & (MINTREE('mu-' == ABSID, PT) > %(MuPt)s)" +
                " & (VFASPF(VCHI2PDOF)<%(VertexChi2)s )" +
                " & (MAXTREE('mu-' == ABSID, TRCHI2DOF) < %(TrChi2Tight)s)")
        inputs = [TrackFittedDiMuon]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, shared = True)

class DetachedDiMuonFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(MINTREE('mu-' == ABSID, MIPCHI2DV(PRIMARY)) > %(IPChi2)s)" +
                " & (BPVDLS > %(DLS)s)")
        inputs = [DiMuonFilter('DiMuon')]
        Hlt2ParticleFilter.__init__(self, name, code, inputs,
                                    dependencies = [PV3D('Hlt2')],
                                    UseP2PVRelations = False)

class DetachedDiMuonHeavyFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(MM > %(MinMass)s) & (PT> %(Pt)s)" +
                " & (MINTREE('mu-' == ABSID, PT) > %(MuPt)s)" +
                " & (VFASPF(VCHI2PDOF)<%(VertexChi2)s )" +
                " & (MAXTREE('mu-' == ABSID, TRCHI2DOF) < %(TrChi2Tight)s)" +
                " & (MINTREE('mu-' == ABSID, MIPCHI2DV(PRIMARY)) > %(IPChi2)s)" +
                " & (BPVDLS > %(DLS)s)")
        inputs = [TrackFittedDiMuon]
        Hlt2ParticleFilter.__init__(self, name, code, inputs,
                                    dependencies = [PV3D('Hlt2')],
                                    UseP2PVRelations = False)

class SoftDiMuonFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(MINTREE('mu-' == ABSID, MIPDV(PRIMARY)) > %(IP)s)" +
                " & (CHILDCUT((TRGHOSTPROB < %(TRACK_TRGHOSTPROB_MAX)s),1))"
                " & (CHILDCUT((TRGHOSTPROB < %(TRACK_TRGHOSTPROB_MAX)s),2))"
                " & (TRCUT (TrIDC('isTT') > %(TTHits)s))" +
                " & (MINTREE('mu-' == ABSID, MIPCHI2DV(PRIMARY)) > %(IPChi2Min)s)" +
                " & (MAXTREE('mu-' == ABSID, MIPCHI2DV(PRIMARY)) < %(IPChi2Max)s)" +
                " & ( VFASPF (sqrt(VX*VX+VY*VY)) > %(Rho)s) " +
                " & ( VFASPF ( VZ ) > %(SVZ)s) " +
                " & ((MIPDV(PRIMARY)/BPVVDZ)< %(MaxIpDistRatio)s)"+
                " & (MM < %(MaxMass)s)" +
                " & (VFASPF(VCHI2PDOF)<%(VertexChi2)s )" +
                " & (DOCAMAX < %(doca)s)" +
                " & (BPVVDZ > %(MinVDZ)s) " +
                "&  (BPVDIRA > %(MinBPVDira)s) " +
                "& (PCUTA (ALV (1,2) < %(cosAngle)s))"  
                )
        inputs = [TrackFittedDiMuon]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, shared = True)


class JpsiFilter(Hlt2ParticleFilter):
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
                                + " & " + Hlt2MonitorMinMax("VFASPF(VCHI2PDOF)", "VFASPF(VCHI2PDOF)", 0, 25, 'JPsiVeterxChi2_out', nbins = 100)}
        inputs = [TrackFittedDiMuon]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, shared = True, **args)

class DetachedJpsiFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = "(BPVDLS > %(DLS)s)"
        from HltLine.Hlt2Monitoring import Hlt2Monitor, Hlt2MonitorMinMax
        args = {'UseP2PVRelations' : False,
                'PostMonitor' : Hlt2Monitor("M", "M(#mu#mu)", 3097, 200, 'M_out', nbins = 25)}
        inputs = [JpsiFilter('JPsi')]
        Hlt2ParticleFilter.__init__(self, name, code, inputs,
                                    dependencies = [PV3D('Hlt2')],
                                    **args)
                        
class Psi2SFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(ADMASS(3686.09) < %(MassWindow)s) " +
                "& (MAXTREE('mu-' == ABSID, TRCHI2DOF) < %(TrChi2Tight)s) " +
                "& (PT > %(Pt)s) " +
                "& (MINTREE('mu-' == ABSID, PT) > %(MuPt)s) " +
                "& (VFASPF(VCHI2PDOF) < %(VertexChi2)s )")
        
        from HltLine.Hlt2Monitoring import Hlt2Monitor, Hlt2MonitorMinMax
        args = {'PreMonitor'  : Hlt2Monitor("M", "M(#mu#mu)", 3097, 200, 'M_in',  nbins = 25),
                'PostMonitor' : Hlt2Monitor("M", "M(#mu#mu)", 3686, 200, 'M_out', nbins = 25)}
        inputs = [TrackFittedDiMuon]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, shared = True, **args)

class DetachedPsi2SFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = "BPVDLS > %(DLS)s"
        from HltLine.Hlt2Monitoring import Hlt2Monitor, Hlt2MonitorMinMax
        args = {'UseP2PVRelations' : False,
                'PostMonitor' : Hlt2Monitor("M", "M(#mu#mu)", 3686, 200, 'M_out', nbins = 25)}
        inputs = [Psi2SFilter('Psi2S')]
        Hlt2ParticleFilter.__init__(self, name, code, inputs,
                                    dependencies = [PV3D('Hlt2')],**args)

class BFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(MM > %(MinMass)s) " +
                "& (MAXTREE('mu-' == ABSID, TRCHI2DOF) < %(TrChi2Tight)s) " +
                "& (VFASPF(VCHI2PDOF) < %(VertexChi2)s) ")

        from HltLine.Hlt2Monitoring import Hlt2Monitor, Hlt2MonitorMinMax
        args = {'PreMonitor'  : Hlt2Monitor("M", "M(#mu#mu)", 3097, 200, 'M_in',  nbins = 25),
                'PostMonitor' : Hlt2Monitor("M", "M(#mu#mu)", 5300, 700, 'M_out', nbins=25)}
        inputs = [TrackFittedDiMuon]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, **args)

class ZFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = "(MM>%(MinMass)s) & (PT>%(Pt)s)"

        from HltLine.Hlt2Monitoring import Hlt2Monitor, Hlt2MonitorMinMax
        args = {'PreMonitor'  : Hlt2Monitor("M", "M(#mu#mu)",  3097,   200, 'M_in',  nbins = 25),
                'PostMonitor' : Hlt2Monitor("M", "M(#mu#mu)", 90000, 10000, 'M_out', nbins=25)}
        inputs = [TrackFittedDiMuon]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, **args)
