###
#  @author F. Redi Federico.Redi@cern.ch
#  @date 2016-02-10
#
#  Please contact the abovementioned responsibles before editing this file
#
##
from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter

from HltTracking.HltPVs import PV3D
from Inputs import Hlt2Muons, Hlt2DownMuons, Hlt2Pions, Hlt2Kaons, Hlt2Protons, Hlt2DownPions

class LambdaMuPiCombiner(Hlt2Combiner):
    def __init__(self, name, inputs):

        daughters_cuts = { "mu-" : ("(TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonP)s *MeV) &  (PT> %(MuonPT)s* MeV)" +
                                    "& (TRGHOSTPROB < %(MuonGHOSTPROB)s)" +
                                    "& (PIDmu-PIDpi> %(MuonPIDmu)s )" +
                                    "& (PIDmu-PIDp> %(MuonPIDp)s )" +
                                    "& (PIDmu-PIDK> %(MuonPIDK)s )" +
                                    "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"),
                           "pi+" : ("(P > %(Lambda0DaugP)s)& (PT > %(Lambda0DaugPT)s)" +
                                    "& (TRCHI2DOF < %(Lambda0DaugTrackChi2)s)" +
                                    "& (MIPCHI2DV(PRIMARY) > %(Lambda0DaugMIPChi2)s)")
                           }

        combination_cuts = "(ADOCACHI2CUT(25, ''))"

        mother_cuts =  "( M > %(MajoranaCutM)s*MeV )&( BPVVDCHI2 > %(MajoranaCutFDChi2)s )&( VFASPF(VCHI2/VDOF) < %(Lambda0VertexChi2)s )&( PT > %(Lambda0PT)s*MeV )"

        decay = "[Lambda0 -> mu- pi+]cc"
        Hlt2Combiner.__init__(self, name, decay ,
                              inputs, nickname = 'LambdaMuPi',
                              dependencies = [ PV3D('Hlt2') ],
                              DaughtersCuts = daughters_cuts,
                              CombinationCut = combination_cuts,
                              MotherCut = mother_cuts,
                              Preambulo = [])

LambdaMuPiLL = LambdaMuPiCombiner('LambdaMuPiLL', [Hlt2Muons, Hlt2Pions])
LambdaMuPiDD = LambdaMuPiCombiner('LambdaMuPiDD', [Hlt2DownMuons, Hlt2DownPions])

class BLambdaMuCombiner(Hlt2Combiner):
    def __init__(self, name, inputs):
        combination_cuts = "(AM>%(LambdaMuMassLowTight)s*MeV) & (AM<%(XMuMassUpperHigh)s*MeV)"

        mother_cuts = ("(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)" +
                       "& ( MINTREE((ABSID=='Lambda0'),VFASPF(VZ)) - VFASPF(VZ) > %(LambdaZ)s *mm )")

        decay = "[B- -> Lambda0 mu-]cc"
        Hlt2Combiner.__init__(self, name, decay ,
                              inputs, nickname = 'BLambdaMu',
                              dependencies = [ PV3D('Hlt2') ],
                              # DaughtersCuts = daughters_cuts,
                              CombinationCut = combination_cuts,
                              MotherCut = mother_cuts,
                              Preambulo = [])

BLambdaMuLL = BLambdaMuCombiner("BLL", [Hlt2Muons, LambdaMuPiLL])
BLambdaMuDD = BLambdaMuCombiner("BDD", [Hlt2Muons, LambdaMuPiDD])
