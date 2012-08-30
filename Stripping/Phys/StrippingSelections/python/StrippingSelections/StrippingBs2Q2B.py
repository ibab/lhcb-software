'''
Bs->quasi 2-body selection
Note: This script borrows heavily from that constructed by
Fred Blanc in the B2CharmlessQuasi2Body selection

Track PT cuts removed as these are in the standard 
particles now anyway. Also corrected mass cuts
removed.
'''

__author__ = ['Sean Benson']
__date__ = '30/08/2012'
__version__ = '2.2'

__all__ = ( 'Bs2Q2Body4piConf',
            'mkDiTrackList',
            'mkBs2Q2B4pi' )

config_params = {'Q2BPrescale'     : 1.,
                 'Q2BResMinPT'     : 900.,
                 'Q2BResMinP'      : 1.,
                 'Q2BResMinMass'   : 0.,
                 'Q2BResMaxMass'   : 1100.,
                 'Q2BResVtxChiDOF' : 9.,
                 'Q2BBMinM'        : 4500.,
                 'Q2BBMaxM'        : 5700.,
                 'Q2BBVtxChi2DOF'  : 9.,
                 'Q2BIPCHI2' : 15}


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles#, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions

name = "Bs2Q2Body4pi"

class Bs2Q2Body4piConf(LineBuilder) :
    """
    Builder for Bs2Q2Body4pi
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

        _trkFilter = FilterDesktop(Code = "(PT>500.*MeV) & (TRCHI2DOF < 4) & (MIPCHI2DV(PRIMARY) > 16)")
        self.TrackList = Selection( 'TrackList' + self.name,
                                    Algorithm = _trkFilter,
                                    RequiredSelections = [StdNoPIDsPions])

        self.DiTrackList = mkDiTrackList( name="DiTracksForCharmlessB" + self.name,
                                            trkList=self.TrackList,
                                            MinPTCut = config['Q2BResMinPT'],
                                            MinPCut = config['Q2BResMinP'],
                                            MinMassCut = config['Q2BResMinMass'],
                                            MaxMassCut = config['Q2BResMaxMass'],
                                            VtxChi2DOFCut = config['Q2BResVtxChiDOF'] )

        Bs2Q2BName = self.name
        self.B2CharmlessQ2B4pi = mkBs2Q2B4pi( Bs2Q2BName,
                                               diTrkList=self.DiTrackList,
                                               MinMassCut = config['Q2BBMinM'],
                                               MaxMassCut = config['Q2BBMaxM'],
                                               VtxChi2DOFCut = config['Q2BBVtxChi2DOF'],
                                              BIPchi2Cut = config['Q2BIPCHI2'])

        self.Q2B4piLine = StrippingLine( Bs2Q2BName+"Line",
                                         prescale = config['Q2BPrescale'],
                                         selection = self.B2CharmlessQ2B4pi )

        self.registerLine(self.Q2B4piLine)

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
    
    _combineDiTrack = CombineParticles( DecayDescriptor="rho(770)0 -> pi+ pi-",
                                        CombinationCut = _diTrackPreVertexCuts,
                                        MotherCut = _diTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineDiTrack,
                     RequiredSelections = [ trkList ] )

def mkBs2Q2B4pi( name,
                  diTrkList,
                  MinMassCut,
                  MaxMassCut,
                  VtxChi2DOFCut,
                 BIPchi2Cut) :
    """
    Charmless Q2B selection
    """
    _B2Q2BPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" % locals()
    _B2Q2BPostVertexCuts = "(BPVDIRA > 0.99) & (MIPCHI2DV(PRIMARY) < %(BIPchi2Cut)s) &  (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s )" % locals()

    _combineB2Q2B = CombineParticles( DecayDescriptor="B0 -> rho(770)0 rho(770)0",
                                      MotherCut = _B2Q2BPostVertexCuts,
                                      CombinationCut = _B2Q2BPreVertexCuts )


    return Selection(name,
                     Algorithm = _combineB2Q2B,
                     RequiredSelections = [ diTrkList ] )
