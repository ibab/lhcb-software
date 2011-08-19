'''
B->charmless quasi 2-body selection
'''

__author__ = ['Fred Blanc']
__date__ = '11/08/2011'
__version__ = '$Revision: 2.3 $'

__all__ = ( 'B2Quasi2BodyConf',
            'makeDiTrackList',
            'makeB2Q2B4pi1n',
            'makeB2Q2B4pi2n',
            'makeB2Q2B3pi' )

config_params = {'Q2BPrescale'     : 1.,
                 'Q2BResMinPT'     : 600.,
                 'Q2BResMinP'      : 10.,
                 'Q2BResMaxMass'   : 1000.,
                 'Q2BResVtxChiDOF' : 9.,
                 'Q2BBMinM4pi'     : 2500.,
                 'Q2BBMinM3pi'     : 4000.,
                 'Q2BBMaxM'        : 5700.,
                 'Q2BBMinCorrM4pi' : 4000.,
                 'Q2BBMinCorrM3pi' : 4600.,
                 'Q2BBMaxCorrM'    : 7000.,
                 'Q2BBVtxChi2DOF'  : 9.}


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
                               'Q2BBMinM4pi',
                               'Q2BBMinM3pi',
                               'Q2BBMaxM',
                               'Q2BBMinCorrM4pi',
                               'Q2BBMinCorrM3pi',
                               'Q2BBMaxCorrM',
                               'Q2BBVtxChi2DOF')

    def __init__(self, name, config) :
	self.name = name
        LineBuilder.__init__(self, name, config)

        _trkFilter = FilterDesktop(Code = "(TRCHI2DOF < 4) & (MIPCHI2DV(PRIMARY) > 25)")
        self.TrackList = Selection( 'TrackList' + self.name,
                                    Algorithm = _trkFilter,
                                    RequiredSelections = [StdNoPIDsPions])

        _trkFilter_HiP = FilterDesktop(Code = "(PT > 2500*MeV) & (P > 10*GeV)")
        self.TrackList_HiP = Selection( 'TrackList_HiP' + self.name,
                                         Algorithm = _trkFilter_HiP,
                                         RequiredSelections = [self.TrackList])

        self.DiTrackList = makeDiTrackList( name="DiTracksForCharmlessB" + self.name,
                                            trkList=self.TrackList,
                                            MinPTCut = config['Q2BResMinPT'],
                                            MinPCut = config['Q2BResMinP'],
                                            MaxMassCut = config['Q2BResMaxMass'],
                                            VtxChi2DOFCut = config['Q2BResVtxChiDOF'] )

        _diTrackFilter_HiPt = FilterDesktop(Code = "(PT > 1000*MeV)")
        self.DiTrackList_HiPt = Selection( 'DiTracksHiPtForCharmlessB' + self.name,
                                           Algorithm = _diTrackFilter_HiPt,
                                           RequiredSelections = [self.DiTrackList])

        B2Q2B4pi1nName = self.name + "4pi1nSelection"
        self.B2CharmlessQ2B4pi1n = makeB2Q2B4pi1n( B2Q2B4pi1nName,
                                                   diTrkList=self.DiTrackList_HiPt,
                                                   MinMassCut = config['Q2BBMinM4pi'],
                                                   MaxMassCut = config['Q2BBMaxM'],
                                                   MinCorrMCut = config['Q2BBMinCorrM4pi'],
                                                   MaxCorrMCut = config['Q2BBMaxCorrM'],
                                                   VtxChi2DOFCut = config['Q2BBVtxChi2DOF'] )

        self.Q2B4pi1nLine = StrippingLine( B2Q2B4pi1nName+"Line",
                                           prescale = config['Q2BPrescale'],
                                           selection = self.B2CharmlessQ2B4pi1n )

        B2Q2B4pi2nName = self.name + "4pi2nSelection"
        self.B2CharmlessQ2B4pi2n = makeB2Q2B4pi2n( B2Q2B4pi2nName,
                                                   diTrkList=self.DiTrackList,
                                                   MinMassCut = config['Q2BBMinM4pi'],
                                                   MaxMassCut = config['Q2BBMaxM'],
                                                   MinCorrMCut = config['Q2BBMinCorrM4pi'],
                                                   MaxCorrMCut = config['Q2BBMaxCorrM'],
                                                   VtxChi2DOFCut = config['Q2BBVtxChi2DOF'] )

        self.Q2B4pi2nLine = StrippingLine( B2Q2B4pi2nName+"Line",
                                           prescale = config['Q2BPrescale'],
                                           selection = self.B2CharmlessQ2B4pi2n )

        B2Q2B3piName = self.name + "3piSelection"
        self.B2CharmlessQ2B3pi = makeB2Q2B3pi( B2Q2B3piName,
                                               trkList = self.TrackList_HiP,
                                               diTrkList=self.DiTrackList_HiPt,
                                               MinMassCut = config['Q2BBMinM3pi'],
                                               MaxMassCut = config['Q2BBMaxM'],
                                               MinCorrMCut = config['Q2BBMinCorrM3pi'],
                                               MaxCorrMCut = config['Q2BBMaxCorrM'],
                                               VtxChi2DOFCut = config['Q2BBVtxChi2DOF'] )

        self.Q2B3piLine = StrippingLine( B2Q2B3piName+"Line",
                                         prescale = config['Q2BPrescale'],
                                         selection = self.B2CharmlessQ2B3pi )

        self.registerLine(self.Q2B4pi1nLine)
        self.registerLine(self.Q2B4pi2nLine)
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

def makeB2Q2B4pi1n( name,
                    diTrkList,
                    MinMassCut,
                    MaxMassCut,
                    MinCorrMCut,
                    MaxCorrMCut,
                    VtxChi2DOFCut ) :
    """
    Charmless Q2B to 4pi selection with missing mass from 1 (neutral) particle
    """

    _B2Q2B4pi1nPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2Q2B4pi1nPreVertexCuts += " & ( ACHILD(MM,1) + ACHILD(MM,2) < 1100. + 0.2*(AM-3500.) )" %locals()

    _B2Q2B4pi1nPostVertexCuts = "in_range( %(MinCorrMCut)s ,BPVCORRM, %(MaxCorrMCut)s )" %locals()
    _B2Q2B4pi1nPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s )" %locals()

    _combineB2Q2B4pi1n = CombineParticles( DecayDescriptor="B0 -> rho(770)0 rho(770)0",
                                        MotherCut = _B2Q2B4pi1nPostVertexCuts,
                                        CombinationCut = _B2Q2B4pi1nPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2Q2B4pi1n,
                     RequiredSelections = [ diTrkList ] )


def makeB2Q2B4pi2n( name,
                    diTrkList,
                    MinMassCut,
                    MaxMassCut,
                    MinCorrMCut,
                    MaxCorrMCut,
                    VtxChi2DOFCut ) :
    """
    Charmless Q2B to 4pi selection with missing mass from 2 (neutral) particles
    """

    _B2Q2B4pi2nPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2Q2B4pi2nPreVertexCuts += " & ( ACHILD(MM,1) + ACHILD(MM,2) < 1300. )" %locals()
    _B2Q2B4pi2nPreVertexCuts += " & ( ACHILD(MM,1) + ACHILD(MM,2) < 500.+1.6*(AM-2500.) )" %locals()
    _B2Q2B4pi2nPreVertexCuts += " & ( ACHILD(PT,1) + ACHILD(PT,2) < 3000.+AM )" %locals()

    _B2Q2B4pi2nPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s )" %locals()
    _B2Q2B4pi2nPostVertexCuts += " & ( BPVCORRM < 6000. )" %locals()
    _B2Q2B4pi2nPostVertexCuts += " & ( MINTREE('rho(770)0'==ABSID, MM) <450.*MeV)" %locals()

    _combineB2Q2B4pi2n = CombineParticles( DecayDescriptor="B0 -> rho(770)0 rho(770)0",
                                        MotherCut = _B2Q2B4pi2nPostVertexCuts,
                                        CombinationCut = _B2Q2B4pi2nPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2Q2B4pi2n,
                     RequiredSelections = [ diTrkList ] )


def makeB2Q2B3pi( name,
                  trkList,
                  diTrkList,
                  MinMassCut,
                  MaxMassCut,
                  MinCorrMCut,
                  MaxCorrMCut,
                  VtxChi2DOFCut ) :
    """
    Charmless Q2B to 3pi selection with missing mass
    """

    _B2Q2B3piPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()

    _B2Q2B3piPostVertexCuts = "in_range( %(MinCorrMCut)s ,BPVCORRM, %(MaxCorrMCut)s )" %locals()
    _B2Q2B3piPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()

    _combineB2Q2B3pi = CombineParticles( DecayDescriptor="B+ -> rho(770)0 pi+",
                                         MotherCut = _B2Q2B3piPostVertexCuts,
                                         CombinationCut = _B2Q2B3piPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2Q2B3pi,
                     RequiredSelections = [ diTrkList, trkList ] )
