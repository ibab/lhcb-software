##
#  @author F. Dettori  Francesco.Dettori@cern.ch
# 
#  Please contact the responsible before editing this file
#
##

# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter

from HltTracking.HltPVs import PV3D
from Inputs import Hlt2Muons

# Single muon filter (just cut on PT and track chi2)
# Common input to every other line
class SingleMuonFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(PT> %(Pt)s) & (TRCHI2DOF<%(TrChi2)s)")
        inputs = [ Hlt2Muons ]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, shared = True)

# Detached single muon filter (now cut on IP or IPCHI2)
class DetachedSingleMuonFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(MIPDV(PRIMARY)>%(IP)s) & (MIPCHI2DV(PRIMARY)>%(IPChi2)s)")
        inputs = [ SingleMuonFilter("SharedSingleMuon") ]
        Hlt2ParticleFilter.__init__(self, name, code, inputs,
                                    dependencies = [PV3D('Hlt2')],
                                    UseP2PVRelations = False)

# Just cut on PT (for prompt high pt) 
class SingleMuonHighPTFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(PT> %(HighPt)s)")
        inputs = [ SingleMuonFilter("SharedSingleMuon") ]
        Hlt2ParticleFilter.__init__(self, name, code, inputs,
                                    dependencies = [PV3D('Hlt2')],
                                    UseP2PVRelations = False)
        
        

# Single muon for Rare Decay B+-> mu+ nu
class SingleMuonRareFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        code = ("(MIPCHI2DV(PRIMARY)>%(RareIPChi2)s)"+
               "& (PT> %(RarePtMin)s) & (PT < %(RarePtMax)s) & ( P < %(RarePMax)s )"+
               "& (TRCHI2DOF<%(RareTrChi2Tight)s)"+
                "& (PIDmu >%(muID)s)")
              
        inputs = [ SingleMuonFilter("SharedSingleMuon") ]
        Hlt2ParticleFilter.__init__(self, name, code, inputs,
                                    dependencies = [PV3D('Hlt2')],
                                    UseP2PVRelations = False)
        



