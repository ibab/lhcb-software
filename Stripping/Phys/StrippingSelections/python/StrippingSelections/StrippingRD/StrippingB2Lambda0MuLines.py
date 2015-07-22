"""
B->Lambda0 Mu  reconstruction
"""

__author__ = ['Federico Redi']
__date__ = '08/04/2015'
__version__ = '$Revision: 0.2 $'

__all__ = ('B2Lambda0MuLines',
           '_Bu2LambdaSSMu',
           'default_config')

from StandardParticles import StdLoosePions, StdLooseMuons, StdLooseDownMuons, StdNoPIDsDownPions
from Configurables import TisTosParticleTagger
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from Gaudi.Configuration import *
from PhysSelPython.Wrappers import DataOnDemand, Selection, MergedSelection

default_config = {
    'NAME'        : 'B2Lambda0Mu',
    'WGs'         : ['RD'],
    'BUILDERTYPE' : 'B2Lambda0MuLines',
    'CONFIG'      : { "GEC_nLongTrk"          : 300.  ,#adimensional
                      #Muons
                      "MuonGHOSTPROB"         : 0.5   ,#adimensional
                      "MuonTRCHI2"            : 4.    ,#adimensional
                      "MuonP"                 : 3000. ,#MeV
                      "MuonPT"                : 250.  ,#MeV
                      "MuonPIDK"              : 0.    ,#adimensional
                      "MuonPIDmu"             : 0.    ,#adimensional
                      "MuonPIDp"              : 0.    ,#adimensional
                      "MuonMINIPCHI2"         : 12    ,#adminensional
                      #Lambda Daughter Cuts
                      "Lambda0DaugP"          : 2000. ,#MeV
                      "Lambda0DaugPT"         : 250.  ,#MeV
                      "Lambda0DaugTrackChi2"  : 4.    ,#adimensional
                      "Lambda0DaugMIPChi2"    : 10.   ,#adimensional
                      #Lambda cuts
                      "MajoranaCutFDChi2"   : 100.  ,#adimensional
                      "MajoranaCutM"        : 1500. ,#MeV
                      "Lambda0VertexChi2"     : 10.   ,#adimensional
                      "Lambda0PT"             : 700.  ,#adimensional
                      #B Mother Cuts
                      "BVCHI2DOF"             : 4.    ,#adminensional
                      "BDIRA"                 : 0.99  ,#adminensional
                      "LambdaMuMassLowTight"  : 1500. ,#MeV
                      "XMuMassUpperHigh"      : 6500. ,#MeV
                      'LambdaZ'               : 0.     #mm
                      } ,
    'STREAMS'     : ['Dimuon']
    }

class B2Lambda0MuLines( LineBuilder ) :
    """Definition of B ->Lambda0 Mu (Lambda0->MuPi) stripping"""
    __configuration_keys__ = ("GEC_nLongTrk",
                              "MuonGHOSTPROB",
                              "MuonTRCHI2",
                              "MuonP",
                              "MuonPT",
                              "MuonPIDK",
                              "MuonPIDmu",
                              "MuonPIDp",
                              "MuonMINIPCHI2",
                              "Lambda0DaugP",
                              "Lambda0DaugPT",
                              "Lambda0DaugTrackChi2",
                              "Lambda0DaugMIPChi2",
                              "MajoranaCutFDChi2",
                              "MajoranaCutM",
                              "Lambda0VertexChi2",
                              "Lambda0PT",
                              "BVCHI2DOF",
                              "BDIRA",
                              "LambdaMuMassLowTight",
                              "XMuMassUpperHigh",
                              "LambdaZ"
                              )

    def __init__(self,name,config):
        LineBuilder.__init__(self, name, config)

        self._stdLooseKsLL = DataOnDemand("Phys/StdLooseKsLL/Particles")
        self.GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" % config,
                      "Preambulo": ["from LoKiTracks.decorators import *"]}
        self._muonSel=None
        self._downmuonSel=None
        self._muonFilter()
        self._selMu=None
        self.registerLine(self._LambdaMajoranaSSMu_line())

    def _NominalMuSelection( self ):
        return "(TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonP)s *MeV) &  (PT> %(MuonPT)s* MeV)"\
               "& (TRGHOSTPROB < %(MuonGHOSTPROB)s)"\
               "& (PIDmu-PIDpi> %(MuonPIDmu)s )"\
               "& (PIDmu-PIDp> %(MuonPIDp)s )"\
               "& (PIDmu-PIDK> %(MuonPIDK)s )"\
               "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"

    ######LambdaortMajoranaLine######
    def _LambdaMajoranaSSMu_line( self ):
        return StrippingLine(self._name+'Bu2LambdaSSMuLine', prescale = 1.0,
                             FILTER=self.GECs,
                             algos = [ self._Bu2LambdaSSMu()])

    ######--######
    def _downMuonFilter( self ):
        if self._downmuonSel is not None:
            return self._downmuonSel

        _mu = FilterDesktop( Code = self._NominalMuSelection() % self._config )
        _muSel=Selection("downMuon_for"+self._name,
                         Algorithm=_mu,
                         RequiredSelections = [StdLooseDownMuons])
        self._downmuonSel=_muSel
        return _muSel

    def _muonFilter( self ):
        if self._muonSel is not None:
            return self._muonSel

        _mu = FilterDesktop( Code = self._NominalMuSelection() % self._config )
        _muSel=Selection("Muon_for"+self._name,
                         Algorithm=_mu,
                         RequiredSelections = [StdLooseMuons])
        self._muonSel=_muSel
        return _muSel

    ######--######

    def _LambdaMajoranaMuFilter( self ):
        if self._selMu is not None:
          return self._selMu

        _Lambda = CombineParticles(
            DecayDescriptors = ["[Lambda0 -> mu- pi+]cc"],
            DaughtersCuts   = {"pi+":"(P > %(Lambda0DaugP)s)& (PT > %(Lambda0DaugPT)s)"\
                               "& (TRCHI2DOF < %(Lambda0DaugTrackChi2)s)" \
                               "& (MIPCHI2DV(PRIMARY) > %(Lambda0DaugMIPChi2)s)"   % self._config
                               },
            CombinationCut  = "(ADOCACHI2CUT(25, ''))"% self._config,
            MotherCut       = "( M > %(MajoranaCutM)s*MeV )&( BPVVDCHI2 > %(MajoranaCutFDChi2)s )&( VFASPF(VCHI2/VDOF) < %(Lambda0VertexChi2)s )&( PT > %(Lambda0PT)s*MeV )" % self._config
            )
        _LambdaMajoranaSSMuSel=Selection("LambdaMajoranaSSMu_for"+self._name,
                                      Algorithm=_Lambda,
                                      RequiredSelections = [StdLoosePions, self._muonFilter()])
        _downLambdaMajoranaSSMuSel=Selection("downLambdaMajoranaSSMu_for"+self._name,
                                      Algorithm=_Lambda,
                                      RequiredSelections = [StdNoPIDsDownPions, self._downMuonFilter()])
        self._LambdaMajoranaSSMuSel=_LambdaMajoranaSSMuSel
        self._downLambdaMajoranaSSMuSel=_downLambdaMajoranaSSMuSel
        _selMu = MergedSelection("Selection_"+self._name+"_LambdaMajoranaSSMu",
                                      RequiredSelections = [ _LambdaMajoranaSSMuSel, _downLambdaMajoranaSSMuSel ])
        return _selMu


    ######Bu->LambdaMu SS & OS######
    def _Bu2LambdaSSMu( self ):
        _LambdaSSMu_SSMu = CombineParticles(
            DecayDescriptors = ["[B- -> Lambda0 mu-]cc"],
            CombinationCut = "(AM>%(LambdaMuMassLowTight)s*MeV) & (AM<%(XMuMassUpperHigh)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"\
            "& ( MINTREE((ABSID=='Lambda0'),VFASPF(VZ)) - VFASPF(VZ) > %(LambdaZ)s *mm )" % self._config,
            ReFitPVs = True
            )
        _LambdaSSMu_SSMuSel=Selection("LambdaSSMu_SSMu_for"+self._name,
                              Algorithm=_LambdaSSMu_SSMu,
                              RequiredSelections = [self._muonFilter(), self._LambdaMajoranaMuFilter()])
        return _LambdaSSMu_SSMuSel

# EOF
