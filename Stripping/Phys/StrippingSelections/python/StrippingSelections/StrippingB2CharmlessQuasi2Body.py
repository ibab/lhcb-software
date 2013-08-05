'''
B->charmless quasi 2-body selection
'''

__author__ = ['Fred Blanc', 'Albert Puig']
__date__ = '05/08/2013'
__version__ = '$Revision: 2.7 $'

__all__ = ( 'B2Quasi2BodyConf',
            'makeDiTrackList',
            'makeB2Q2B4pi',
            'makeB2Q2B3pi' )

config_params = {'Q2BPrescale'     : 1.,
                 'Q2BTrkGhostProb' : 0.5,
                 'Q2BTrkMinIPChi2' : 16.,
                 'Q2BTrkMinHiPT'   : 1000.,
                 'Q2BResMinPT'     : 600.,
                 'Q2BResMinHiPT'   : 1000.,
                 'Q2BResMaxMass'   : 1100.,
                 'Q2BResVtxChi2DOF': 9.,
                 'Q2BBMinPT'       : 2500.,
                 'Q2BBMinM4pi'     : 3500.,
                 'Q2BBMinM3pi'     : 4200.,
                 'Q2BBMaxM'        : 6700.,
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
                               'Q2BTrkGhostProb',
                               'Q2BTrkMinIPChi2',
                               'Q2BTrkMinHiPT',
                               'Q2BResMinPT',
                               'Q2BResMinHiPT',
                               'Q2BResMaxMass',
                               'Q2BResVtxChi2DOF',
                               'Q2BBMinPT',
                               'Q2BBMinM4pi',
                               'Q2BBMinM3pi',
                               'Q2BBMaxM',
                               'Q2BBMaxCorrM',
                               'Q2BBVtxChi2DOF')

    __confdict__={}

    def __init__(self, name, config) :
	self.name = name
        self.__confdict__ = config_params
        LineBuilder.__init__(self, name, config)

        _trkFilter = FilterDesktop(Code = "(MIPCHI2DV(PRIMARY) > %(Q2BTrkMinIPChi2)s) & (TRGHOSTPROB < %(Q2BTrkGhostProb)s)" % self.__confdict__ )

        self.TrackList = Selection( 'TrackList' + self.name,
                                    Algorithm = _trkFilter,
                                    RequiredSelections = [StdNoPIDsPions])

        _trkFilter_HiP = FilterDesktop(Code = "(PT > %(Q2BTrkMinHiPT)s)" % self.__confdict__ )
        self.TrackList_HiP = Selection( 'TrackList_HiP' + self.name,
                                         Algorithm = _trkFilter_HiP,
                                         RequiredSelections = [self.TrackList])

        self.DiTrackList = makeDiTrackList( name="DiTracksForCharmlessB" + self.name,
                                            trkList=self.TrackList,
                                            MinPTCut = config['Q2BResMinPT'],
                                            MaxMassCut = config['Q2BResMaxMass'],
                                            VtxChi2DOFCut = config['Q2BResVtxChi2DOF'] )

        _diTrackFilter_HiPt = FilterDesktop(Code = "(PT > %(Q2BResMinHiPT)s)" % self.__confdict__ )
        self.DiTrackList_HiPt = Selection( 'DiTracksHiPtForCharmlessB' + self.name,
                                           Algorithm = _diTrackFilter_HiPt,
                                           RequiredSelections = [self.DiTrackList])

        B2Q2B4piName = self.name + "4piSelection"
        self.B2CharmlessQ2B4pi = makeB2Q2B4pi( B2Q2B4piName,
                                               diTrkList=self.DiTrackList,
                                               MinPTCut = config['Q2BBMinPT'],
                                               MinMassCut = config['Q2BBMinM4pi'],
                                               MaxMassCut = config['Q2BBMaxM'],
                                               MaxCorrMCut = config['Q2BBMaxCorrM'],
                                               VtxChi2DOFCut = config['Q2BBVtxChi2DOF'] )

        self.Q2B4piLine = StrippingLine( B2Q2B4piName+"Line",
                                         prescale = config['Q2BPrescale'],
                                         selection = tisTosSelection(self.B2CharmlessQ2B4pi) )

        B2Q2B3piName = self.name + "3piSelection"
        self.B2CharmlessQ2B3pi = makeB2Q2B3pi( B2Q2B3piName,
                                               trkList = self.TrackList_HiP,
                                               diTrkList=self.DiTrackList_HiPt,
                                               MinPTCut = config['Q2BBMinPT'],
                                               MinMassCut = config['Q2BBMinM3pi'],
                                               MaxMassCut = config['Q2BBMaxM'],
                                               MaxCorrMCut = config['Q2BBMaxCorrM'],
                                               VtxChi2DOFCut = config['Q2BBVtxChi2DOF'] )

        self.Q2B3piLine = StrippingLine( B2Q2B3piName+"Line",
                                         prescale = config['Q2BPrescale'],
                                         selection = tisTosSelection(self.B2CharmlessQ2B3pi) )

        self.registerLine(self.Q2B4piLine)
        self.registerLine(self.Q2B3piLine)

def makeDiTrackList( name,
                     trkList,
                     MinPTCut,
                     MaxMassCut,
                     VtxChi2DOFCut ) :
    """
    Di-track selection
    """
    _diTrackPreVertexCuts = "(APT> %(MinPTCut)s) & (AM< %(MaxMassCut)s)" %locals()
    _diTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s)" %locals()

    _combineDiTrack = CombineParticles( DecayDescriptor="rho(770)0 -> pi+ pi-",
                                        CombinationCut = _diTrackPreVertexCuts,
                                        MotherCut = _diTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineDiTrack,
                     RequiredSelections = [ trkList ] )

def makeB2Q2B4pi( name,
                  diTrkList,
                  MinPTCut,
                  MinMassCut,
                  MaxMassCut,
                  MaxCorrMCut,
                  VtxChi2DOFCut ) :
    """
    Charmless Q2B to 4pi selection with missing mass
    """

    _B2Q2B4piPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2Q2B4piPreVertexCuts += " & ( APT > %(MinPTCut)s )" %locals()

    _B2Q2B4piPostVertexCuts = "( BPVCORRM < %(MaxCorrMCut)s )" %locals()
    _B2Q2B4piPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s )" %locals()

    _combineB2Q2B4pi = CombineParticles( DecayDescriptor="B0 -> rho(770)0 rho(770)0",
                                        MotherCut = _B2Q2B4piPostVertexCuts,
                                        CombinationCut = _B2Q2B4piPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2Q2B4pi,
                     RequiredSelections = [ diTrkList ] )


def makeB2Q2B3pi( name,
                  trkList,
                  diTrkList,
                  MinPTCut,
                  MinMassCut,
                  MaxMassCut,
                  MaxCorrMCut,
                  VtxChi2DOFCut ) :
    """
    Charmless Q2B to 3pi selection with missing mass
    """

    _B2Q2B3piPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2Q2B3piPreVertexCuts += " & ( APT > %(MinPTCut)s )" %locals()

    _B2Q2B3piPostVertexCuts = "( BPVCORRM < %(MaxCorrMCut)s )" %locals()
    _B2Q2B3piPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()

    _combineB2Q2B3pi = CombineParticles( DecayDescriptor="[B+ -> rho(770)0 pi+]cc",
                                         MotherCut = _B2Q2B3piPostVertexCuts,
                                         CombinationCut = _B2Q2B3piPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2Q2B3pi,
                     RequiredSelections = [ diTrkList, trkList ] )


def makeTISTOSFilter(name):
    specs = {'Hlt1Track.*Decision%TOS':0}
    from Configurables import TisTosParticleTagger
    tisTosFilter = TisTosParticleTagger(name+'TISTOSFilter')
    tisTosFilter.TisTosSpecs = specs
    tisTosFilter.ProjectTracksToCalo = False
    tisTosFilter.CaloClustForCharged = False
    tisTosFilter.CaloClustForNeutral = False
    tisTosFilter.TOSFrac = {4:0.0, 5:0.0}
    return tisTosFilter

def tisTosSelection(sel):
    '''Filters Selection sel to be TOS OR TIS.'''
    tisTosFilter = makeTISTOSFilter(sel.name())
    return Selection(sel.name()+'TISTOS', Algorithm=tisTosFilter,
                     RequiredSelections=[sel])
