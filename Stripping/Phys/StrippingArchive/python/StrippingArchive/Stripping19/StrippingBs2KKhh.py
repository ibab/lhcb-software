'''
Bs->quasi 2-body selection
Note: This script borrows heavily from that constructed by
Fred Blanc in the B2CharmlessQuasi2Body selection

Designed to look for KKhh final states with KK around the
phi mass
'''

__author__ = ['Sean Benson']
__date__ = '14/03/2011'
__version__ = '1.0'

__all__ = ( 'BsPhiRhoConf',
            'mkDiTrackList',
            'mkKKTrackList',
            'mkBs2PRKKhh' )

config_params = {'PRPrescale'     : 1.,
                 'PRResMinPT'     : 900.,
                 'PRResMinP'      : 1.,
                 'PRResMaxMass'   : 4000.,
                 'PRResVtxChiDOF' : 9.,
                 'PRBMinM'        : 5000.,
                 'PRBMaxM'        : 5600.,
                 'PRPhiWindow'    : 25.,
                 'PRBVtxChi2DOF'  : 9.,
		 'PRIPCHI2' : 25}


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles, FilterDesktop#, OfflineVertexFitter
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

        _trkFilter = FilterDesktop(Code = "(TRCHI2DOF < 4) & (MIPCHI2DV(PRIMARY) > 16)")
        
	self.TrackListhh = Selection( 'TrackList' + self.name,
                                    Algorithm = _trkFilter,
                                    RequiredSelections = [StdNoPIDsPions])

        self.DiTrackList = mkDiTrackList( name="DiTracksForCharmlessB" + self.name,
                                            trkList=self.TrackListhh,
                                            MinPTCut = config['PRResMinPT'],
                                            MinPCut = config['PRResMinP'],
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
                                         selection = self.B2CharmlessPRKKhh )

        self.registerLine(self.PRKKhhLine)

def mkDiTrackList( name,
                     trkList,
                     MinPTCut,
                     MinPCut,
                     MaxMassCut,
                     VtxChi2DOFCut ) :
    """
    Di-track selection
    """
    _diTrackPreVertexCuts = "(APT> %(MinPTCut)s *MeV) & (AP> %(MinPCut)s *GeV) & (AM< %(MaxMassCut)s *MeV)" % locals()
    _diTrackPostVertexCuts = "(MIPCHI2DV(PRIMARY) > 10)"
    
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

    _code = "(MIPCHI2DV(PRIMARY) > 10) & (ADMASS('phi(1020)')< %(PhiWindowMassCut)s *MeV) & (PT> %(MinPTCut)s *MeV) & (P> %(MinPCut)s *GeV)" % locals()
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
    _B2PRPostVertexCuts = "(MIPCHI2DV(PRIMARY) < %(BIPchi2Cut)s) & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s )" % locals()

    _combineB2PR = CombineParticles( DecayDescriptor="B0 -> phi(1020) rho(770)0",
                                      MotherCut = _B2PRPostVertexCuts,
                                      CombinationCut = _B2PRPreVertexCuts )


    return Selection(name,
                     Algorithm = _combineB2PR,
                     RequiredSelections = [ diTrkList, diTrkListKK ])
