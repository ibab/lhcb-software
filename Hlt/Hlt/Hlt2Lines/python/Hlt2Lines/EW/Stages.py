from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Inputs import TrackFittedDiMuon
from Inputs import BiKalmanFittedMuons
from Inputs import TrackFittedDiElectronFromL0
from Inputs import BiKalmanFittedElectronsFromL0
from HltTracking.HltPVs import PV3D
from HltLine.HltDecodeRaw import DecodeL0CALO

class DiMuonZFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(MM > %(MinMass)s) & (PT > %(Pt)s)")
        from HltLine.Hlt2Monitoring import Hlt2Monitor, Hlt2MonitorMinMax
        args = {'PreMonitor'  : Hlt2Monitor("M", "M(#mu#mu)",  3097,   200, 'M_in',  nbins = 25),
                'PostMonitor' : Hlt2Monitor("M", "M(#mu#mu)", 85000, 95000, 'M_out', nbins=25)}
        inputs = [TrackFittedDiMuon]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, **args)

class DiMuonDY1Filter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(MM > %(MinMass)s) & (PT > %(Pt)s) " +
                "& ~in_range(%(MinExcMass)s, MM, %(MaxExcMass)s) " +
                "& (MAXTREE('mu-' == ABSID, TRCHI2DOF) < %(TkChi2)s) " +
                "& (MINTREE('mu-' == ABSID, PT) > %(MuPt)s")
        inputs = [TrackFittedDiMuon]
        Hlt2ParticleFilter.__init__(self, name, code, inputs)

class DiMuonDY2Filter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(MM > %(MinMass)s) & (PT > %(Pt)s) " +
                "& (MAXTREE('mu-' == ABSID, TRCHI2DOF) < %(TkChi2)s) " +
                "& (MINTREE('mu-' == ABSID, PT) > %(MuPt)s")
        inputs = [TrackFittedDiMuon]
        Hlt2ParticleFilter.__init__(self, name, code, inputs)

class DiMuonDY3Filter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(MM > %(MinMass)s) & (PT > %(Pt)s) " +
                "& (MAXTREE('mu-' == ABSID, TRCHI2DOF) < %(TkChi2)s)")
        inputs = [TrackFittedDiMuon]
        Hlt2ParticleFilter.__init__(self, name, code, inputs)

class DiMuonDY4Filter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(MM > %(MinMass)s) & (PT > %(Pt)s) " +
                "& (MAXTREE('mu-' == ABSID, TRCHI2DOF) < %(TkChi2)s)")
        inputs = [TrackFittedDiMuon]
        Hlt2ParticleFilter.__init__(self, name, code, inputs)

class SingleMuonLowPtFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(PT > %(Pt)s) & (TRCHI2DOF < %(TkChi2)s)")
        inputs = [BiKalmanFittedMuons]
        Hlt2ParticleFilter.__init__(self, name, code, inputs)

class SingleMuonHighPtFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(PT > %(Pt)s)")
        inputs = [BiKalmanFittedMuons]
        Hlt2ParticleFilter.__init__(self, name, code, inputs)

class SingleMuonVHighPtFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(PT > %(Pt)s)")
        inputs = [BiKalmanFittedMuons]
        Hlt2ParticleFilter.__init__(self, name, code, inputs)

class DiElectronDYFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(MINTREE('e+' == ABSID, PT) > %(ElecPt)s) " +
                "& (MAXTREE('e+' == ABSID, TRCHI2DOF) < %(ElecTkChi2)s) " +
                "& (MINTREE('e+' == ABSID, PPINFO(LHCb.ProtoParticle.CaloPrsE, 0)) > %(PrsMin)s) " +
                "& (MINTREE('e+' == ABSID, PPINFO(LHCb.ProtoParticle.CaloEcalE, 0)/P) > %(EcalMin)s) " +
                "& (MAXTREE('e+' == ABSID, PPINFO(LHCb.ProtoParticle.CaloHcalE, 99999)/P) < %(HcalMax)s) " +
                "& (in_range(%(MinMass)s, MM, %(MaxMass)s)) " +
                "& (VFASPF(VCHI2PDOF) < %(VtxChi2)s) " +
                "& (PT > %(Pt)s)")
        inputs = [TrackFittedDiElectronFromL0]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, dependencies = [DecodeL0CALO])

class DiElectronHighMassFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(MINTREE('e+' == ABSID, PT) > %(ElecPt)s) " +
                "& (MINTREE('e+' == ABSID, PPINFO(LHCb.ProtoParticle.CaloPrsE, 0)) > %(PrsMin)s) " +
                "& (MINTREE('e+' == ABSID, PPINFO(LHCb.ProtoParticle.CaloEcalE, 0)/P) > %(EcalMin)s) " +
                "& (MAXTREE('e+' == ABSID, PPINFO(LHCb.ProtoParticle.CaloHcalE, 99999)/P) < %(HcalMax)s) " +
                "& (MAXTREE('e+' == ABSID, TRCHI2DOF) < %(TkChi2)s) " +
                "& (MM > %(MinMass)s) " +
                "& (VFASPF(VCHI2PDOF) < %(VtxChi2)s)" +
                "& (PT > %(Pt)s)")
        inputs = [TrackFittedDiElectronFromL0]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, dependencies = [DecodeL0CALO])

class SingleTFElectronFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(PT > %(Pt)s) " +
                "& (PPINFO(LHCb.ProtoParticle.CaloPrsE, 0) > %(PrsMin)s) " +
                "& (PPINFO(LHCb.ProtoParticle.CaloEcalE, 0)/P > %(EcalMin)s) " +
                "& (PPINFO(LHCb.ProtoParticle.CaloHcalE, 99999)/P < %(HcalMax)s) " +
                "& (MIPDV(PRIMARY) > %(IP)s) " +
                "& (MIPCHI2DV(PRIMARY) > %(IPChi2)s) " +
                "& (TRCHI2DOF < %(TkChi2)s)")
        inputs = [BiKalmanFittedElectronsFromL0]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, dependencies = [DecodeL0CALO, PV3D('HLT2')])

class SingleTFElectronLowPtFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(PT > %(Pt)s) " +
                "& (PPINFO(LHCb.ProtoParticle.CaloPrsE, 0) > %(PrsMin)s) " +
                "& (PPINFO(LHCb.ProtoParticle.CaloEcalE, 0)/P > %(EcalMin)s) " +
                "& (PPINFO(LHCb.ProtoParticle.CaloHcalE, 99999)/P < %(HcalMax)s) " +
                "& (TRCHI2DOF < %(TkChi2)s)")
        inputs = [BiKalmanFittedElectronsFromL0]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, dependencies = [DecodeL0CALO])

class SingleTFElectronHighPtFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(PT > %(Pt)s) " +
                "& (PPINFO(LHCb.ProtoParticle.CaloPrsE, 0) > %(PrsMin)s) " +
                "& (PPINFO(LHCb.ProtoParticle.CaloEcalE, 0)/P > %(EcalMin)s) " +
                "& (PPINFO(LHCb.ProtoParticle.CaloHcalE, 99999)/P < %(HcalMax)s) " +
                "& (TRCHI2DOF < %(TkChi2)s)")
        inputs = [BiKalmanFittedElectronsFromL0]
        Hlt2ParticleFilter.__init__(self,name, code, inputs, dependencies = [DecodeL0CALO])

class SingleTFElectronVHighPtFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(PT > %(Pt)s) " +
                "& (PPINFO(LHCb.ProtoParticle.CaloPrsE, 0) > %(PrsMin)s) " +
                "& (PPINFO(LHCb.ProtoParticle.CaloEcalE, 0)/P > %(EcalMin)s) " +
                "& (PPINFO(LHCb.ProtoParticle.CaloHcalE, 99999)/P < %(HcalMax)s) " +
                "& (TRCHI2DOF < %(TkChi2)s)")
        inputs = [BiKalmanFittedElectronsFromL0]
        Hlt2ParticleFilter.__init__(self,name, code, inputs, dependencies = [DecodeL0CALO])
