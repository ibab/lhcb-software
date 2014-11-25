'''
Bs->quasi 2-body selection
Note: This script borrows heavily from that constructed by
Fred Blanc in the B2CharmlessQuasi2Body selection

Designed to look for KKhh final states with KK around the
phi mass
'''

__author__ = ['Sean Benson']
__date__ = '17/08/2014'
__version__ = '3.0'

__all__ = ( 'BsPhiRhoConf',
            'mkDiTrackList',
            'mkKKTrackList',
            'mkBs2PRKKhh' )

default_config = {
    'NAME'        : 'BsPhiRho',
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'BsPhiRhoConf',
    'CONFIG'      : {'PRPrescale'     : 1.,
                     'PRResMinPT'     : 1200.,
                     'PRResMinP'      : 3.,
                     'PRResMinMass'   : 0.,
                     'PRResMaxMass'   : 4000.,
                     'PRResVtxChiDOF' : 9.,
                     'PRBMinM'        : 4800.,
                     'PRBMaxM'        : 5600.,
                     'PRPhiWindow'    : 20.,
                     'PRBVtxChi2DOF'  : 9.,
                     'PRIPCHI2' : 20
                     },
    'STREAMS'     : ['Bhadron']
    }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles, FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions

name = "BsPhiRho"

class BsPhiRhoConf(LineBuilder) :
    """
    Builder for BsPhiRho
    """

    __configuration_keys__ = ( 'PRPrescale',
                               'PRResMinPT',
                               'PRResMinP',
                               'PRResMinMass',
                               'PRResMaxMass',
                               'PRResVtxChiDOF',
                               'PRBMinM',
                               'PRBMaxM',
                               'PRPhiWindow',
                               'PRBVtxChi2DOF',
                               'PRIPCHI2')

    def __init__(self, name, config) :
	self.name = name
        LineBuilder.__init__(self, name, config)

        self.hltFilter = "( HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2Topo[234]BodyBBDTDecision') )"
        _trkFilter = FilterDesktop(Code = "(TRGHOSTPROB < 0.45) & (PT>600.*MeV) & (TRCHI2DOF < 3.5) & (MIPCHI2DV(PRIMARY) > 4.5)")
        
	self.TrackListhh = Selection( 'TrackList' + self.name,
                                    Algorithm = _trkFilter,
                                    RequiredSelections = [StdNoPIDsPions])

        self.DiTrackList = mkDiTrackList( name="DiTracksForCharmlessB" + self.name,
                                            trkList=self.TrackListhh,
                                            MinPTCut = config['PRResMinPT'],
                                            MinPCut = config['PRResMinP'],
                                            MinMassCut = config['PRResMinMass'],
                                            MaxMassCut = config['PRResMaxMass'],
                                            VtxChi2DOFCut = config['PRResVtxChiDOF'] )

        self.KKTrackList = mkKKTrackList( name="KKTracksForCharmlessB"+ self.name,
                                            MinPTCut = config['PRResMinPT'],
                                            MinPCut = config['PRResMinP'],
                                            PhiWindowMassCut = config['PRPhiWindow'] )

	Bs2PRName = self.name
        self.B2CharmlessPRKKhh = mkBs2PRKKhh( Bs2PRName,
                                               diTrkList=self.DiTrackList,
                                               diTrkListKK=self.KKTrackList,
                                               MinMassCut = config['PRBMinM'],
                                               MaxMassCut = config['PRBMaxM'],
                                               VtxChi2DOFCut = config['PRBVtxChi2DOF'],
                                              BIPchi2Cut = config['PRIPCHI2'])

        self.PRKKhhLine = StrippingLine( Bs2PRName+"Line",
                                         prescale = config['PRPrescale'],
                                         HLT = self.hltFilter,
                                         selection = self.B2CharmlessPRKKhh, EnableFlavourTagging = True,
                                         RelatedInfoTools = [ { "Type"         : "RelInfoConeVariables",
                                                                "ConeAngle"    : 0.8,
                                                                "Variables"    : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                                "TopSelection" : self.TrackListhh,
                                                                "Location"     : 'P2ConeVar08' },
                                                              { "Type"         : "RelInfoConeVariables",
                                                                "ConeAngle"    : 1.0,
                                                                "Variables"    : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                                "TopSelection" : self.TrackListhh,
                                                                "Location"     : 'P2ConeVar10' },
                                                              { "Type"         : "RelInfoConeVariables",
                                                                "ConeAngle"    : 1.3,
                                                                "Variables"    : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                                "TopSelection" : self.TrackListhh,
                                                                "Location"     : 'P2ConeVar13' },
                                                              { "Type"         : "RelInfoConeVariables",
                                                                "ConeAngle"    : 1.7,
                                                                "Variables"    : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                                "TopSelection" : self.TrackListhh,
                                                                "Location"     : 'P2ConeVar17' },
                                                              { "Type"         : "RelInfoVertexIsolation",
                                                                "Location"     : "VertexIsoInfo" } ] )

        self.registerLine(self.PRKKhhLine)

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
    _diTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s )"% locals()
    
    _combineDiTrack = CombineParticles( DecayDescriptor="rho(770)0 -> pi+ pi-",
					CombinationCut = _diTrackPreVertexCuts,
                                         MotherCut = _diTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineDiTrack,
                     RequiredSelections = [ trkList ] )

def mkKKTrackList( name,
                     MinPTCut,
                     MinPCut,
                     PhiWindowMassCut ) :
    """
    KK selection
    """

    _code = "(ADMASS('phi(1020)')< %(PhiWindowMassCut)s *MeV) & (PT> %(MinPTCut)s *MeV) & (P> %(MinPCut)s *GeV)" % locals()
    _stdPhi = DataOnDemand(Location="Phys/StdTightPhi2KK/Particles")
    _phiFilter = FilterDesktop(Code = _code)

    return Selection (name,
                      Algorithm = _phiFilter,
                      RequiredSelections = [_stdPhi])

def mkBs2PRKKhh( name,
                  diTrkList,
                  diTrkListKK,
                  MinMassCut,
                  MaxMassCut,
                  VtxChi2DOFCut,
                 BIPchi2Cut) :
    """
    Bs to KKhh selection
    """
    _B2PRPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" % locals()
    _B2PRPostVertexCuts = "(BPVDIRA > 0.9999) & (MIPCHI2DV(PRIMARY) < %(BIPchi2Cut)s) & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s )" % locals()

    _combineB2PR = CombineParticles( DecayDescriptor="B0 -> phi(1020) rho(770)0",
                                      MotherCut = _B2PRPostVertexCuts,
                                      CombinationCut = _B2PRPreVertexCuts )


    return Selection(name,
                     Algorithm = _combineB2PR,
                     RequiredSelections = [ diTrkList, diTrkListKK ])
