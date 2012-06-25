"""
Stripping line for J/psi -> mu mu candidates
where only one muon is identified.
For measuring muon detection asymmetries for the semileptonic
asymmetry analyses.
"""
__author__ = ['Mika Vesterinen']
__date__ = '25/06/2012'
__version__ = '$Revision: 0.0 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseMuons, StdLooseKaons, StdLooseProtons, StdNoPIDsPions, StdNoPIDsMuons, StdLooseMergedPi0,StdLooseResolvedPi0,StdLooseElectrons,StdNoPIDsKaons
from Configurables import TisTosParticleTagger

__all__ = ('JPsiForSLAllLinesConf',
           'confdict')

confdict = {
    "PrescaleSemiIncJpsi2mumu"    : 1.0
    ,"MuonPT" : 1200.
    ,"MuonP" : 6000.
    ,"MuonChi2" : 3.
    ,"MuonIPChi2" : 4. 
    ,"TrackPT" : 500.
    ,"TrackP" : 3000.
    ,"TrackChi2" : 3.
    ,"TrackIPChi2" : 5.
    ,"PsiMasswinPreFit" : 150
    ,"PsiDocaChi2Max" : 10
    ,"PsiVChi2NdofMax" : 10
    ,"PsiMasswin" : 140
    ,"PsiPT" : 500
    ,"PsiFDChi2Min" : 100
    ,"PsiDIRAMin" : 0.99
    ,"BCombMassMin" : 3400
    ,"BCombMassMax" : 6000
    ,"BDocaChi2Max" : 10.
    ,"BVChi2NdofMax" : 3.
    ,"BPTMin" : 800.
    ,"BFDChi2Min" : 200.
    ,"BDIRAMin" : 0.9995
    ,"nLongTrackMax" : 250
    }

class JPsiForSLAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "PrescaleSemiIncJpsi2mumu"
        ,"MuonPT"
        ,"MuonP"
        ,"MuonChi2"
        ,"MuonIPChi2"
        ,"TrackPT"
        ,"TrackP"
        ,"TrackChi2"
        ,"TrackIPChi2"
        ,"PsiMasswinPreFit"
        ,"PsiDocaChi2Max"
        ,"PsiVChi2NdofMax"
        ,"PsiMasswin"
        ,"PsiPT"
        ,"PsiFDChi2Min"
        ,"PsiDIRAMin"
        ,"BCombMassMin"
        ,"BCombMassMax"
        ,"BDocaChi2Max"
        ,"BVChi2NdofMax"
        ,"BPTMin"
        ,"BFDChi2Min"
        ,"BDIRAMin"
        ,"nLongTrackMax"
        )
    
    __confdict__={}
        
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.__confdict__=config

        #### muon selection
        Muon_cuts = "(PT > %(MuonPT)s) & (P> %(MuonP)s) & (TRCHI2DOF < %(MuonChi2)s) & (MIPCHI2DV(PRIMARY)> %(MuonIPChi2)s)" % config
        self.muons = Selection( "Muons_for_" + name,
                                Algorithm = FilterDesktop(Code = Muon_cuts),
                                RequiredSelections = [StdNoPIDsMuons])

        #### additional track selection
        Track_cuts = "(PT > %(TrackPT)s) & (P> %(TrackP)s) & (TRCHI2DOF < %(TrackChi2)s) & (MIPCHI2DV(PRIMARY)> %(TrackIPChi2)s)" % config
        self.tracks = Selection( "Tracks_for_" + name,
                                 Algorithm = FilterDesktop(Code = Track_cuts),
                                 RequiredSelections = [StdNoPIDsKaons]
                                 )

        #### J/psi->mumu selection
        self.JPsi_to_mumu = CombineParticles(DecayDescriptor = "J/psi(1S) -> mu+ mu-",                                         
                                             CombinationCut = "(ADAMASS('J/psi(1S)')<%(PsiMasswinPreFit)s) & (ADOCACHI2CUT(%(PsiDocaChi2Max)s,''))" %config,
                                             MotherCut = "(INTREE( (ABSID=='mu+') & ISMUON ) )"\
                                             "& (VFASPF(VCHI2/VDOF)< %(PsiVChi2NdofMax)s)"\
                                             "& (ADMASS('J/psi(1S)')< %(PsiMasswin)s)"
                                             " & (SUMTREE( PT,  ISBASIC )>%(PsiPT)s)"\
                                             "& (BPVVDCHI2 > %(PsiFDChi2Min)s) & (BPVDIRA>%(PsiDIRAMin)s)" %config
                                             )
        self.Sel_JPsi_to_mumu = Selection ("JPsi_to_mumu_for_"+name,
                                           Algorithm = self.JPsi_to_mumu,
                                           RequiredSelections = [self.muons])
        
        
        #### B -> J/psi + >=1 track selection
        self.B_to_JPsi_track = CombineParticles(DecayDescriptor = "[B+ -> J/psi(1S) K+]cc",                                         
                                                CombinationCut = "(AM > %(BCombMassMin)s) & (AM < %(BCombMassMax)s) & (ADOCACHI2CUT(%(BDocaChi2Max)s,''))" %config,
                                                MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVChi2NdofMax)s)"\
                                                " & (SUMTREE( PT,  ISBASIC )>%(BPTMin)s) & (BPVVDCHI2 > %(BFDChi2Min)s) & (BPVDIRA>%(BDIRAMin)s)" %config
                                                )
        
        self.Sel_B_to_JPsi_track = Selection( "B_to_JPsi_track_For" + name,
                                              Algorithm = self.B_to_JPsi_track,
                                              RequiredSelections = [self.Sel_JPsi_to_mumu,self.tracks]
                                              )

        #### GECs
        GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(nLongTrackMax)s )" %config,
                 "Preambulo": ["from LoKiTracks.decorators import *"]}
        
        #### register the line
        self.registerLine(StrippingLine("SemiIncJpsi2mumu" + name + "Line",
                                        prescale = config["PrescaleSemiIncJpsi2mumu"],
                                        FILTER=GECs,
                                        selection = self.Sel_B_to_JPsi_track))

