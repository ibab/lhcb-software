'''
B->charmless quasi 2-body selection
'''

__author__ = ['Fred Blanc']
__date__ = '15/03/2011'
__version__ = '$Revision: 2.1 $'

__all__ = ( 'B2Quasi2BodyConf',
            'makeDiTrackList',
            'makeB2Q2B4pi',
            'makeB2Q2B3pi' )

config_params = {'Q2BPrescale'     : 1.,
                 'Q2BResMinPT'     : 1000.,
                 'Q2BResMinP'      : 10.,
                 'Q2BResMaxMass'   : 1000.,
                 'Q2BResVtxChiDOF' : 9.,
                 'Q2BBMinM'        : 2500.,
                 'Q2BBMaxM'        : 5700.,
                 'Q2BBMinCorrM4pi' : 4000.,
                 'Q2BBMinCorrM3pi' : 4600.,
                 'Q2BBMaxCorrM'    : 7000.,
                 'Q2BBVtxChi2DOF'  : 9.,
                 'Q2BBMinTrkPT'    : 1000.}


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions

name = "B2Quasi2Body"

class B2Quasi2BodyConf(LineBuilder) :
    """
    Builder for B2Quasi2Body
    """

    __configuration_keys__ = ( 'Q2BPrescale',
                               'Q2BResMinPT',
                               'Q2BResMinP',
                               'Q2BResMaxMass',
                               'Q2BResVtxChiDOF',
                               'Q2BBMinM',
                               'Q2BBMaxM',
                               'Q2BBMinCorrM4pi',
                               'Q2BBMinCorrM3pi',
                               'Q2BBMaxCorrM',
                               'Q2BBVtxChi2DOF',
                               'Q2BBMinTrkPT')

    def __init__(self, name, config) :
	self.name = name
        LineBuilder.__init__(self, name, config)

        _trkFilter = FilterDesktop(Code = "(TRCHI2DOF < 4) & (MIPCHI2DV(PRIMARY) > 25) & (PT > 250*MeV)")
        self.TrackList = Selection( 'TrackList' + self.name,
                                    Algorithm = _trkFilter,
                                    RequiredSelections = [StdNoPIDsPions])

        _trkFilter_HiP = FilterDesktop(Code = "(PT > 2500*MeV) & (P > 10*GeV) & (MIPCHI2DV(PRIMARY) > 36)")
        self.TrackList_HiP = Selection( 'TrackList_HiP' + self.name,
                                         Algorithm = _trkFilter_HiP,
                                         RequiredSelections = [self.TrackList])

        self.DiTrackList = makeDiTrackList( name="DiTracksForCharmlessB" + self.name,
                                            trkList=self.TrackList,
                                            MinPTCut = config['Q2BResMinPT'],
                                            MinPCut = config['Q2BResMinP'],
                                            MaxMassCut = config['Q2BResMaxMass'],
                                            VtxChi2DOFCut = config['Q2BResVtxChiDOF'] )

        B2Q2B4piName = self.name + "4piSelection"
        self.B2CharmlessQ2B4pi = makeB2Q2B4pi( B2Q2B4piName,
                                               diTrkList=self.DiTrackList,
                                               MinMassCut = config['Q2BBMinM'],
                                               MaxMassCut = config['Q2BBMaxM'],
                                               MinCorrMCut = config['Q2BBMinCorrM4pi'],
                                               MaxCorrMCut = config['Q2BBMaxCorrM'],
                                               VtxChi2DOFCut = config['Q2BBVtxChi2DOF'],
                                               MinTrkPTCut = config['Q2BBMinTrkPT'] )

        self.Q2B4piLine = StrippingLine( B2Q2B4piName+"Line",
                                         prescale = config['Q2BPrescale'],
#                                         HLT = "(HLT_PASS_RE('Hlt1Track.*Decision'))",
                                         selection = self.B2CharmlessQ2B4pi )

        B2Q2B3piName = self.name + "3piSelection"
        self.B2CharmlessQ2B3pi = makeB2Q2B3pi( B2Q2B3piName,
                                               trkList = self.TrackList_HiP,
                                               diTrkList=self.DiTrackList,
                                               MinMassCut = config['Q2BBMinM'],
                                               MaxMassCut = config['Q2BBMaxM'],
                                               MinCorrMCut = config['Q2BBMinCorrM3pi'],
                                               MaxCorrMCut = config['Q2BBMaxCorrM'],
                                               VtxChi2DOFCut = config['Q2BBVtxChi2DOF'],
                                               MinTrkPTCut = config['Q2BBMinTrkPT'] )

        self.Q2B3piLine = StrippingLine( B2Q2B3piName+"Line",
                                         prescale = config['Q2BPrescale'],
#                                         HLT = "(HLT_PASS_RE('Hlt1Track.*Decision'))",
                                         selection = self.B2CharmlessQ2B3pi )

        self.registerLine(self.Q2B4piLine)
        self.registerLine(self.Q2B3piLine)

def makeDiTrackList( name,
                     trkList,
                     MinPTCut,
                     MinPCut,
                     MaxMassCut,
                     VtxChi2DOFCut ) :
    """
    Di-track selection
    """
    _diTrackPreVertexCuts = "(APT> %(MinPTCut)s *MeV) & (AP> %(MinPCut)s *GeV) & (AM< %(MaxMassCut)s *MeV)" %locals()
    _diTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s)" %locals()

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
                  MinCorrMCut,
                  MaxCorrMCut,
                  VtxChi2DOFCut,
                  MinTrkPTCut ) :
    """
    Charmless Q2B to 4pi selection with missing mass
    """

    _B2Q2B4piPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()

    _B2Q2B4piPostVertexCuts = "in_range( %(MinCorrMCut)s ,BPVCORRM, %(MaxCorrMCut)s )" %locals()
    _B2Q2B4piPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s )" %locals()
    _B2Q2B4piPostVertexCuts += " & (MAXTREE('pi+'==ABSID, PT) > %(MinTrkPTCut)s *MeV)" %locals()

    _combineB2Q2B4pi = CombineParticles( DecayDescriptor="B0 -> rho(770)0 rho(770)0",
                                      MotherCut = _B2Q2B4piPostVertexCuts,
                                      CombinationCut = _B2Q2B4piPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2Q2B4pi,
                     RequiredSelections = [ diTrkList ] )


def makeB2Q2B3pi( name,
                  trkList,
                  diTrkList,
                  MinMassCut,
                  MaxMassCut,
                  MinCorrMCut,
                  MaxCorrMCut,
                  VtxChi2DOFCut,
                  MinTrkPTCut ) :
    """
    Charmless Q2B to 3pi selection with missing mass
    """

    _B2Q2B3piPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2Q2B3piPostVertexCuts = "in_range( %(MinCorrMCut)s ,BPVCORRM, %(MaxCorrMCut)s )" %locals()
    _B2Q2B3piPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()
    _B2Q2B3piPostVertexCuts += " & CHILDCUT((MAXTREE('pi+'==ABSID, PT) > %(MinTrkPTCut)s *MeV) , 1) " % locals()

    _combineB2Q2B3pi = CombineParticles( DecayDescriptor="B+ -> rho(770)0 pi+",
                                         MotherCut = _B2Q2B3piPostVertexCuts,
                                         CombinationCut = _B2Q2B3piPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2Q2B3pi,
                     RequiredSelections = [ diTrkList, trkList ] )
