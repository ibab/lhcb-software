'''
Bs->Phi f0 selection
Note: This script borrows heavily from that constructed by
Fred Blanc in the B2CharmlessQuasi2Body selection

Track PT cuts removed as these are in the standard 
particles now anyway. Also corrected mass cuts
removed.
'''

__author__ = ['Haofei Luo']
__date__ = '03/06/2013'
__version__ = '2.3'

__all__ = ( 'Bs2Phif0Conf',
            'mkDiTrackList',
            'mkDiTrackList1',
            'mkBs2Phif0' )

config_params = {'Q2BPrescale'       : 1.,
                 'Q2BResMinPT'       : 900.,
                 'Q2BResMinP'        : 1.,
                 'Q2BResPhiMinMass'  : 969.445,
                 'Q2BResPhiMaxMass'  : 1069.445,
                 'Q2BResf0MinMass'   : 0.,
                 'Q2BResf0MaxMass'   : 1800.,
                 'Q2BResVtxChiDOF'   : 12.,
                 'Q2BBMinM'          : 4800.,
                 'Q2BBMaxM'          : 5750.,
                 'Q2BBVtxChi2DOF'    : 12.,
                 'Q2BIPCHI2' : 20}


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles#, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions
from StandardParticles import StdLooseKaons

name = "Bs2Phif0"

class Bs2Phif0Conf(LineBuilder) :
    """
    Builder for Bs2Phif0Line
    """

    __configuration_keys__ = ( 'Q2BPrescale',
                               'Q2BResMinPT',
                               'Q2BResMinP',
                               'Q2BResPhiMinMass',
                               'Q2BResPhiMaxMass',
                               'Q2BResf0MinMass',
                               'Q2BResf0MaxMass',
                               'Q2BResVtxChiDOF',
                               'Q2BBMinM',
                               'Q2BBMaxM',
                               'Q2BBVtxChi2DOF',
                               'Q2BIPCHI2')

    def __init__(self, name, config) :
	self.name = name
        LineBuilder.__init__(self, name, config)

        _trkFilter = FilterDesktop(Code = "(TRGHOSTPROB < 0.6) & (PT>250.*MeV) & (TRCHI2DOF < 4) & (MIPCHI2DV(PRIMARY) > 16)")
        self.TrackList = Selection( 'TrackList' + self.name,
                                    Algorithm = _trkFilter,
                                    RequiredSelections = [StdLooseKaons])

        self.TrackList1 = Selection( 'TrackList1' + self.name,
                                    Algorithm = _trkFilter,
                                    RequiredSelections = [StdNoPIDsPions])

        self.DiTrackList = mkDiTrackList( name="TracksForPhi" + self.name,
                                            trkList=self.TrackList,
                                            MinPTCut = config['Q2BResMinPT'],
                                            MinPCut = config['Q2BResMinP'],
                                            MinMassCut = config['Q2BResPhiMinMass'],
                                            MaxMassCut = config['Q2BResPhiMaxMass'],
                                            VtxChi2DOFCut = config['Q2BResVtxChiDOF'] )

        self.DiTrackList1 = mkDiTrackList1( name="TracksForf0" + self.name,
                                            trkList=self.TrackList1,
                                            MinPTCut = config['Q2BResMinPT'],
                                            MinPCut = config['Q2BResMinP'],
                                            MinMassCut = config['Q2BResf0MinMass'],
                                            MaxMassCut = config['Q2BResf0MaxMass'],
                                            VtxChi2DOFCut = config['Q2BResVtxChiDOF'] )

        Bs2Phif0Name = self.name
        self.B2CharmlessPhif0 = mkBs2Phif0( Bs2Phif0Name,
                                              diTrkList=self.DiTrackList,
                                              diTrkList1=self.DiTrackList1,
                                              MinMassCut = config['Q2BBMinM'],
                                              MaxMassCut = config['Q2BBMaxM'],
                                              VtxChi2DOFCut = config['Q2BBVtxChi2DOF'],
                                              BIPchi2Cut = config['Q2BIPCHI2'])

        self.Bs2Phif0Line = StrippingLine( name+"Line",
                                         prescale = config['Q2BPrescale'],
                                         selection = self.B2CharmlessPhif0)

        self.registerLine(self.Bs2Phif0Line)

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

def mkDiTrackList1( name,
                     trkList,
                     MinPTCut,
                     MinPCut,
                     MinMassCut,
                     MaxMassCut,
                     VtxChi2DOFCut ) :
    """
    Di-track selection
    """
    _diTrackPreVertexCuts1 = "(APT> %(MinPTCut)s *MeV) & (AP> %(MinPCut)s *GeV) & in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" % locals()
    _diTrackPostVertexCuts1 = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s)" % locals()

    _combineDiTrack1 = CombineParticles( DecayDescriptor="f_0(980) -> pi+ pi-",
                                        CombinationCut = _diTrackPreVertexCuts1,
                                        MotherCut = _diTrackPostVertexCuts1 )

    return Selection(name,
                     Algorithm = _combineDiTrack1,
                     RequiredSelections = [ trkList ] )


def mkBs2Phif0( name,
                  diTrkList,
                  diTrkList1,
                  MinMassCut,
                  MaxMassCut,
                  VtxChi2DOFCut,
                 BIPchi2Cut) :
    """
    Charmless Q2B selection
    """
    _B2Q2BPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" % locals()
    _B2Q2BPostVertexCuts = "(BPVDIRA > 0.99) & (MIPCHI2DV(PRIMARY) < %(BIPchi2Cut)s) &  (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s )" % locals()

    _combineB2Q2B = CombineParticles( DecayDescriptor="B_s0 -> phi(1020) f_0(980)",
                                      MotherCut = _B2Q2BPostVertexCuts,
                                      CombinationCut = _B2Q2BPreVertexCuts )


    return Selection(name,
                     Algorithm = _combineB2Q2B,
                     RequiredSelections = [ diTrkList,diTrkList1 ] )
