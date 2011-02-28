'''
B->charmless quasi 2-body selection
'''

__author__ = ['Fred Blanc']
__date__ = '25/02/2011'
__version__ = '$Revision: 2.0 $'

__all__ = ( 'B2Quasi2Body4piConf',
            'makeDiTrackList',
            'makeB2Q2B4pi' )

config_params = {'Q2BPrescale'     : 1.,
                 'Q2BResMinPT'     : 1500.,
                 'Q2BResMinP'      : 10.,
                 'Q2BResMaxMass'   : 900.,
                 'Q2BResVtxChiDOF' : 4.,
                 'Q2BBMinM'        : 2800.,
                 'Q2BBMaxM'        : 5700.,
                 'Q2BBVtxChi2DOF'  : 4.,
                 'Q2BBMinTrkPT'    : 1500.,
                 'Q2BBPVVDZ'       : 2. }


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions

name = "B2Quasi2Body4pi"

class B2Quasi2Body4piConf(LineBuilder) :
    """
    Builder for B2Quasi2Body4pi
    """

    __configuration_keys__ = ( 'Q2BPrescale',
                               'Q2BResMinPT',
                               'Q2BResMinP',
                               'Q2BResMaxMass',
                               'Q2BResVtxChiDOF',
                               'Q2BBMinM',
                               'Q2BBMaxM',
                               'Q2BBVtxChi2DOF',
                               'Q2BBMinTrkPT',
                               'Q2BBPVVDZ' )

    def __init__(self, name, config) :
	self.name = name
        LineBuilder.__init__(self, name, config)

        _trkFilter = FilterDesktop(Code = "(TRCHI2DOF < 4) & (MIPCHI2DV(PRIMARY) > 25) & (PT > 250*MeV)")
        self.TrackList = Selection( 'TrackList' + self.name,
                                    Algorithm = _trkFilter,
                                    RequiredSelections = [StdNoPIDsPions])

        self.DiTrackList = makeDiTrackList( name="DiTracksForCharmlessB" + self.name,
                                            trkList=self.TrackList,
                                            MinPTCut = config['Q2BResMinPT'],
                                            MinPCut = config['Q2BResMinP'],
                                            MaxMassCut = config['Q2BResMaxMass'],
                                            VtxChi2DOFCut = config['Q2BResVtxChiDOF'] )

        B2Q2BName = self.name + "Selection"
        self.B2CharmlessQ2B4pi = makeB2Q2B4pi( B2Q2BName,
                                               diTrkList=self.DiTrackList,
                                               MinMassCut = config['Q2BBMinM'],
                                               MaxMassCut = config['Q2BBMaxM'],
                                               VtxChi2DOFCut = config['Q2BBVtxChi2DOF'],
                                               MinTrkPTCut = config['Q2BBMinTrkPT'],
                                               MinPVVDZCut = config['Q2BBPVVDZ'] )

        self.Q2B4piLine = StrippingLine( B2Q2BName+"Line",
                                         prescale = config['Q2BPrescale'],
                                         selection = self.B2CharmlessQ2B4pi )

        self.registerLine(self.Q2B4piLine)

def makeDiTrackList( name,
                     trkList,
                     MinPTCut,
                     MinPCut,
                     MaxMassCut,
                     VtxChi2DOFCut ) :
    """
    Di-track selection
    """
    _diTrackPreVertexCuts = "(APT> %(MinPTCut)s *MeV) & (AP> %(MinPCut)s *GeV) & (AM< %(MaxMassCut)s *MeV)" % locals()
    _diTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s)" % locals()
#    _diTrackPreVertexCuts = "(APT>1500*MeV) & (AP>10*GeV) & (AM<900*MeV)"
#    _diTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < 4)"

    _combineDiTrack = CombineParticles( DecayDescriptor="rho(770)0 -> pi+ pi-",
                                        CombinationCut = _diTrackPreVertexCuts,
                                        MotherCut = _diTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineDiTrack,
                     RequiredSelections = [ trkList ] )

def makeB2Q2B4pi( name,
                  diTrkList,
                  MinMassCut,
                  MaxMassCut,
                  VtxChi2DOFCut,
                  MinTrkPTCut,
                  MinPVVDZCut ) :
    """
    Charmless Q2B selection
    """

    _B2Q2BPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" % locals()
    _B2Q2BPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) & (MAXTREE('pi+'==ABSID, PT) > %(MinTrkPTCut)s *MeV) & (BPVVDZ > %(MinPVVDZCut)s *mm)" % locals()
#    _B2Q2BPreVertexCuts = "in_range(2800,AM,5700)"
#    _B2Q2BPostVertexCuts = "(VFASPF(VCHI2/VDOF) < 4) & (MAXTREE('pi+'==ABSID, PT) > 1500*MeV) & (BPVVDZ > 2*mm)"

    _combineB2Q2B = CombineParticles( DecayDescriptor="B0 -> rho(770)0 rho(770)0",
                                      MotherCut = _B2Q2BPostVertexCuts,
                                      CombinationCut = _B2Q2BPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2Q2B,
                     RequiredSelections = [ diTrkList ] )
