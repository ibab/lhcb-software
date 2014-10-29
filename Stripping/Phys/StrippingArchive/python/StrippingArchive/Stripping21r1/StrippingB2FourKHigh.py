'''
Bs->quasi 2-body selection
Note: This script borrows heavily from that constructed by
Fred Blanc in the B2CharmlessQuasi2Body selection

Designed to look for 4 Kaon final states with a high resonance mass
'''

__author__ = ['Sean Benson']
__date__ = '17/08/2014'
__version__ = '3.0'

__all__ = ( 'Bs2Q2Body4KConf',
            'mkDiTrackList',
            'mkBs2Q2B4K' )

default_config = {
    'NAME'        : 'Bs2Q2Body4K',
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Bs2Q2Body4KConf',
    'CONFIG'      : {'Q2BPrescale'     : 1.,
                     'Q2BResMinPT'     : 900.,
                     'Q2BResMinP'      : 1.,
                     'Q2BResMinMass'   : 990.,
                     'Q2BResMaxMass'   : 2500.,
                     'Q2BResVtxChiDOF' : 9.,
                     'Q2BBMinM'        : 5200.,
                     'Q2BBMaxM'        : 5500.,
                     'Q2BBVtxChi2DOF'  : 9.,
                     'Q2BIPCHI2' : 20
                     },
    'STREAMS'     : ['Bhadron']
    }
    

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles, FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseKaons

name = "Bs2Q2Body4K"

class Bs2Q2Body4KConf(LineBuilder) :
    """
    Builder for Bs2Q2Body4K
    """

    __configuration_keys__ = ( 'Q2BPrescale',
                               'Q2BResMinPT',
                               'Q2BResMinP',
                               'Q2BResMinMass',
                               'Q2BResMaxMass',
                               'Q2BResVtxChiDOF',
                               'Q2BBMinM',
                               'Q2BBMaxM',
                               'Q2BBVtxChi2DOF',
                               'Q2BIPCHI2')

    def __init__(self, name, config) :
	self.name = name
        LineBuilder.__init__(self, name, config)

        _trkFilter = FilterDesktop(Code = "(TRGHOSTPROB < 0.4) & (PT>500.*MeV) & (TRCHI2DOF < 4.0) & (MIPCHI2DV(PRIMARY) > 4.0)")
        self.hltFilter = "( HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2Topo[234]BodyBBDTDecision') )"
        self.TrackList = Selection( 'TrackList' + self.name,
                                    Algorithm = _trkFilter,
                                    RequiredSelections = [StdLooseKaons])

        self.DiTrackList = mkDiTrackList( name="DiTracks" + self.name,
                                            trkList=self.TrackList,
                                            MinPTCut = config['Q2BResMinPT'],
                                            MinPCut = config['Q2BResMinP'],
                                            MinMassCut = config['Q2BResMinMass'],
                                            MaxMassCut = config['Q2BResMaxMass'],
                                            VtxChi2DOFCut = config['Q2BResVtxChiDOF'] )

        Bs2Q2BName = self.name
        self.B2CharmlessQ2B4K = mkBs2Q2B4K( Bs2Q2BName,
                                               diTrkList=self.DiTrackList,
                                               MinMassCut = config['Q2BBMinM'],
                                               MaxMassCut = config['Q2BBMaxM'],
                                               VtxChi2DOFCut = config['Q2BBVtxChi2DOF'],
                                              BIPchi2Cut = config['Q2BIPCHI2'])

        self.Q2B4KLine = StrippingLine( Bs2Q2BName+"Line",
                                         prescale = config['Q2BPrescale'],
                                         HLT = self.hltFilter,
                                         selection = self.B2CharmlessQ2B4K, EnableFlavourTagging = True,
                                         RelatedInfoTools = [ { "Type"         : "RelInfoConeVariables",
                                                                "ConeAngle"    : 0.8,
                                                                "Variables"    : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                                "TopSelection" : self.TrackList,
                                                                "Location"     : 'P2ConeVar08' },
                                                              { "Type"         : "RelInfoConeVariables",
                                                                "ConeAngle"    : 1.0,
                                                                "Variables"    : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                                "TopSelection" : self.TrackList,
                                                                "Location"     : 'P2ConeVar10' },
                                                              { "Type"         : "RelInfoConeVariables",
                                                                "ConeAngle"    : 1.3,
                                                                "Variables"    : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                                "TopSelection" : self.TrackList,
                                                                "Location"     : 'P2ConeVar13' },
                                                              { "Type"         : "RelInfoConeVariables",
                                                                "ConeAngle"    : 1.7,
                                                                "Variables"    : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                                "TopSelection" : self.TrackList,
                                                                "Location"     : 'P2ConeVar17' },
                                                              { "Type"         : "RelInfoVertexIsolation",
                                                                "Location"     : "VertexIsoInfo" } ] )

        self.registerLine(self.Q2B4KLine)

def mkDiTrackList( name,
                     trkList,
                     MinPTCut,
                     MinPCut,
                     MinMassCut,
                     MaxMassCut,
                     VtxChi2DOFCut ) :
    """
    Di-track selection
    """
    _diTrackPreVertexCuts = "(APT> %(MinPTCut)s *MeV) & (AP> %(MinPCut)s *GeV) & in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" % locals()
    _diTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s)" % locals()
    
    _combineDiTrack = CombineParticles( DecayDescriptor="phi(1020) -> K+ K-",
					CombinationCut = _diTrackPreVertexCuts,
                                        MotherCut = _diTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineDiTrack,
                     RequiredSelections = [ trkList ] )

def mkBs2Q2B4K( name,
                  diTrkList,
                  MinMassCut,
                  MaxMassCut,
                  VtxChi2DOFCut,
                 BIPchi2Cut) :
    """
    Charmless Q2B selection
    """
    _B2Q2BPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" % locals()
    _B2Q2BPostVertexCuts = "(BPVDIRA > 0.9995) & (MIPCHI2DV(PRIMARY) < %(BIPchi2Cut)s) & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s )" % locals()

    _combineB2Q2B = CombineParticles( DecayDescriptor="B_s0 -> phi(1020) phi(1020)",
                                      MotherCut = _B2Q2BPostVertexCuts,
                                      CombinationCut = _B2Q2BPreVertexCuts )


    return Selection(name,
                     Algorithm = _combineB2Q2B,
                     RequiredSelections = [ diTrkList ] )
