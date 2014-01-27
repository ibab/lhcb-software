'''
B-> X Gamma selections
'''

__author__ = ['Cedric Potterat','Zhirui Xu','Pablo Ruiz Valls']
__date__ = '17/1/2014'
__version__ = '$Revision: 3.0 $'

__all__ = ( 'B2Quasi2BodyConf',
            'makeDiTrackList',
            'makeDiTrackLambda',
            'makeDiTrackList_wKS0',
            'makeTriTrackList',
#            'makeTriTrackList_alt',
            'makeTriTrackList_wKS0',
            'makeTriTrackList_wpi0M',
            'makeTriTrackList_wpi0R',
            'makeFourTrackList',
            'makeFourTrackList_wKS0',
#            'makeFourTrackListPions',
            'makeFourTrackList_wpi0M',
            'makeFourTrackList_wpi0R',
            'makeB2B2XG4piGamma',
            'makeB2B2XG3piGamma',
            'makeB2B2XG3piGammaCNV',
            'makeB2B2XG2pipi0Gamma',
            'makeB2B2XG2piKsGamma',
            'makeB2B2XGpiKsGamma',
            'makeB2B2XG2piGamma'
            'makeB2B2XG2piGammaCNV'
            'makeB2B2XG3pipi0Gamma',
            'makeB2B2XG3piKsGamma',
            'makeB2B2XGLambdapiGamma'
            'makeB2B2XGLambda2piGamma'
            'makeB2B2XGLambda3piGamma')

config_params = {'B2XG2piPrescale'        : 1.,
                 'B2XG2piCNVPrescale'     : 1.,
                 'B2XGpiKsPrescale'       : 1.,

                 'B2XG3piPrescale'        : 1.,
                 'B2XG3piCNVPrescale'     : 1.,
                 'B2XG2pipi0MPrescale'    : 1.,
                 'B2XG2pipi0RPrescale'    : 1.,
                 'B2XG2piKsPrescale'      : 1.,

                 'B2XG4piPrescale'        : 1.,
                 'B2XG3piKsPrescale'      : 1.,
                 'B2XG3pipi0MPrescale'    : 1.,
                 'B2XG3pipi0RPrescale'    : 1.,
                 'B2XG2pi2KsPrescale'     : 1.,

                 'B2XGLambdapiPrescale'   : 1.,
                 'B2XGLambda2piPrescale'  : 1.,
                 'B2XGLambda3piPrescale'  : 1.,
                 'B2XGLbLambdaPrescale'   : 1., # UNUSED

                 # TRACK CUTS
                 'B2XGTrkChi2DOF': 3., # was nothing
                 'B2XGTrkGhostProb' : 0.4, # was 0.5
                 'B2XGTrkMinIPChi2' : 16., # was 5
                 'B2XGTrkMinPT'     : 300., # was 100
                 'B2XGTrkMinP'      : 1000, # P was 2000, old stripping has 1 GeV...

                 # PHOTON CUTS
                 'B2XGGammaPTMin'       : 2000.,
                 'B2XGGammaCL'          : 0.25,
                 'B2XGGammaCNVPTMin'       : 1000.,

                 # TRACK COMBINATION CUTS
                 'B2XGResMinPT'     : 0.,
                 'B2XGResMinMass'   : 0., #was 520
                 'B2XGResMaxMass'   : 7900., # was 4940 (3500+4*(mK-mpi). Now is maxMLambda+(mp-mpi)+(mK-mpi) to allow for Kp decays is StdLooseNoPIDPions also covers proton tracks
                 'B2XGResVtxChi2DOF': 10., # was 25
                 'B2XGResSumPtMin'  : 1500.,
                 'B2XGResDocaMax'   : 100.5, # NOT USED
                 'B2XGResBPVVDCHI2Min'  : -1., # FD avoid if possible. was 64
                 'B2XGResIPCHI2Min' : 0.0,

                 # B HADRON CUTS
                 'B2XGBMinPT'       : 0., #250., # was 300
                 'B2XGBMinM2pi'     : 3280., # was 3280
                 'B2XGBMinM3pi'     : 2900., # was 2900
                 'B2XGBMinM4pi'     : 2560., # was 2560
                 'B2XGBMinMLambda'  : 2560., # was 2900
                 'B2XGBMaxM'        : 9000., # was 8540
                 'B2XGBSumPtMin' : 5000,  #SUMPT
                 'B2XGBMinBPVDIRA'  : -1.1,  #BPVDIRA avoid if possible
                 'B2XGBMaxCorrM'    : 73000., # NOT USED
                 'B2XGBVtxChi2DOF'  : 9.,
                 'B2XGBVtxMaxIPChi2': 9.} # was 25


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsKaons, StdAllNoPIDsPions, StdLooseAllPhotons, StdLooseMergedPi0, StdLooseResolvedPi0, StdAllLooseGammaDD,StdAllLooseGammaLL, StdAllNoPIDsProtons

name = "Beauty2XGamma"

def topoInputsCuts(): # Don't need IP chi2 cut b/c is in 1st filter
    return "(TRCHI2DOF<3) & (PT > 500*MeV) & (P > 5000*MeV)"

def topoKSInputsCuts(): # Don't need IP chi2 cut b/c is in 1st filter
    return "(PT > 500*MeV) & (P > 5000*MeV)"# & (BPVVDCHI2 > 1000)"

def hasTopoChild():
    return "AHASCHILD((ISBASIC & "+topoInputsCuts()+")|((ABSID=='KS0') & "\
           +topoKSInputsCuts() +"))"

def highTopoInputsCuts():
    return "((P>5000*MeV) & (PT>1000*MeV) & (TRCHI2DOF<2.5) & (MIPCHI2DV(PRIMARY)>16) & (MIPDV(PRIMARY)>0.1*mm))"

def hasHighTopoChild():
    return "INTREE(ISBASIC & "+highTopoInputsCuts()+")"

# def has2TopoChild():
#     return "(NINTREE((ISBASIC & "+topoInputsCuts()+")|((ABSID=='KS0') & "\
#            +topoKSInputsCuts() +"))>1)"



class Beauty2XGammaConf(LineBuilder) :
    """
    Builder for Beauty2XGamma
    """

    __configuration_keys__ = ( 'B2XG2piPrescale',
                               'B2XG2piCNVPrescale',
                               'B2XGpiKsPrescale',
                               'B2XG3piPrescale',
                               'B2XG3piCNVPrescale',
                               'B2XG2pipi0MPrescale',
                               'B2XG2pipi0RPrescale',
                               'B2XG2piKsPrescale',
                               'B2XG4piPrescale',
                               'B2XG3piKsPrescale',
                               'B2XG3pipi0MPrescale',
                               'B2XG3pipi0RPrescale',
                               'B2XG2pi2KsPrescale',
                               'B2XGLambdapiPrescale',
                               'B2XGLambda2piPrescale',
                               'B2XGLambda3piPrescale',
                               'B2XGLbLambdaPrescale',
                               'B2XGTrkChi2DOF',
                               'B2XGTrkGhostProb',
                               'B2XGTrkMinIPChi2',
                               'B2XGTrkMinPT',
                               'B2XGTrkMinP',
                               'B2XGGammaPTMin',
                               'B2XGGammaCL',
                               'B2XGGammaCNVPTMin',
                               'B2XGResMinPT',
                               'B2XGResMinMass',
                               'B2XGResMaxMass',
                               'B2XGResVtxChi2DOF',
                               'B2XGResSumPtMin',
                               'B2XGResDocaMax',
                               'B2XGResBPVVDCHI2Min',
                               'B2XGResIPCHI2Min',
                               'B2XGBMinPT',
                               'B2XGBMinM2pi',
                               'B2XGBMinM3pi',
                               'B2XGBMinM4pi',
                               'B2XGBMinMLambda',
                               'B2XGBMaxM',
                               'B2XGBSumPtMin',
                               'B2XGBMinBPVDIRA',
                               'B2XGBMaxCorrM',
                               'B2XGBVtxChi2DOF',
                               'B2XGBVtxMaxIPChi2')

    __confdict__={}

    def __init__(self, name, config) :
        self.name = name
        self.__confdict__ = config_params
        LineBuilder.__init__(self, name, config)

        # Generic track (pi) = h
        _trkFilter = FilterDesktop(Code = "(TRCHI2DOF < %(B2XGTrkChi2DOF)s)& (MIPCHI2DV(PRIMARY) > %(B2XGTrkMinIPChi2)s) & (TRGHOSTPROB < %(B2XGTrkGhostProb)s) & (PT > %(B2XGTrkMinPT)s) & (P > %(B2XGTrkMinP)s)" % self.__confdict__ )
        self.TrackList = Selection( 'TrackList' + self.name,
                                    Algorithm = _trkFilter,
                                    RequiredSelections = [StdAllNoPIDsPions])

        # Neutral Pions, both merged and resolved
        _Pi0Sel = FilterDesktop(Code = "(P > 4000*MeV) & (PT > 1200*MeV)" % self.__confdict__ )
        self.MergedPi0 = Selection( 'MergedPi0Sel' + self.name,
                                    Algorithm = _Pi0Sel,
                                    RequiredSelections = [StdLooseMergedPi0])
        self.ResolvedPi0 = Selection( 'ResolvedPi0Sel' + self.name,
                                      Algorithm = _Pi0Sel,
                                      RequiredSelections = [StdLooseResolvedPi0])
        # Ks0s
        mergedKshorts = MergedSelection('MergedKshorts',RequiredSelections = [DataOnDemand("Phys/StdLooseKsDD/Particles"), DataOnDemand("Phys/StdLooseKsLL/Particles")])
        _KS0Sel = FilterDesktop(Code = "(PT > 1000) & (MM > 480*MeV) & (MM < 515*MeV) & (VFASPF(VCHI2/VDOF)<9)" % self.__confdict__ )
        self.Kshort = Selection( 'KS0Sel' + self.name,
                                 Algorithm = _KS0Sel,
                                 RequiredSelections = [mergedKshorts])

        # Lambdas
        mergedLambdas = MergedSelection('MergedLambdas',RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles"),DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles")])
        _LambdaSel = FilterDesktop(Code = "(PT > %(B2XGResMinPT)s)  & in_range( %(B2XGResMinMass)s ,ADMASS('Lambda0'), %(B2XGResMaxMass)s )" % self.__confdict__ )

        self.Lambda = Selection( 'LambdaSel' + self.name,
                                 Algorithm = _LambdaSel,
                                 RequiredSelections = [mergedLambdas])
        # hh
        self.DiTrackList = makeDiTrackList( name="DiTracksForRadiativeB" + self.name,
                                            trkList=self.TrackList,
                                            MinPTCut = config['B2XGResMinPT'],
                                            MinSumPTCut = config['B2XGResSumPtMin'],
                                            MaxMassCut = config['B2XGResMaxMass'],
                                            MinMassCut = config['B2XGResMinMass'],
                                            IPChi2MinCut = config['B2XGResIPCHI2Min'],
                                            MaxDocaCut = config['B2XGResDocaMax'],
                                            VtxChi2DOFCut = config['B2XGResVtxChi2DOF'] )


        # h + Ks0
        self.DiTrackList_wKS0 = makeDiTrackList_wKS0( name="Track_wKs0ForRadiativeB" + self.name,
                                                      trkList=self.TrackList,
                                                      ksList=self.Kshort,
                                                      MinPTCut = config['B2XGResMinPT'],
                                                      MinSumPTCut = config['B2XGResSumPtMin'],
                                                      MaxMassCut = config['B2XGResMaxMass'],
                                                      MinMassCut = config['B2XGResMinMass'],
                                                      IPChi2MinCut = config['B2XGResIPCHI2Min'],
                                                      MaxDocaCut = config['B2XGResDocaMax'],
                                                      VtxChi2DOFCut = config['B2XGResVtxChi2DOF'] )

        # hhh built as h + h + h
        self.TriTrackList = makeTriTrackList( name="TriTracksForRadiativeB" + self.name,
                                              trkList=self.TrackList,
                                              #                                              ditrkList=self.DiTrackList,
                                              MinPTCut = config['B2XGResMinPT'],
                                              MinSumPTCut = config['B2XGResSumPtMin'],
                                              MaxMassCut = config['B2XGResMaxMass'],
                                              MinMassCut = config['B2XGResMinMass'],
                                              VtxChi2DOFCut = config['B2XGResVtxChi2DOF'],
                                              BPVVDCHI2MinCut = config['B2XGResBPVVDCHI2Min'],
                                              IPChi2MinCut = config['B2XGResIPCHI2Min'],
                                              MaxDocaCut = config['B2XGResDocaMax']
                                              )

#         # hhh built as hh + h
#         self.TriTrackList_alt = makeTriTrackList_alt( name="Alt_TriTracksForRadiativeB" + self.name,
#                                                       trkList=self.TrackList,
#                                                       ditrkList=self.DiTrackList,
#                                                       MinPTCut = config['B2XGResMinPT'],
#                                                       MinSumPTCut = config['B2XGResSumPtMin'],
#                                                       MaxMassCut = config['B2XGResMaxMass'],
#                                                       MinMassCut = config['B2XGResMinMass'],
#                                                       VtxChi2DOFCut = config['B2XGResVtxChi2DOF'],
#                                                       BPVVDCHI2MinCut = config['B2XGResBPVVDCHI2Min'],
#                                                       IPChi2MinCut = config['B2XGResIPCHI2Min'],
#                                                       MaxDocaCut = config['B2XGResDocaMax']
#                                                       )


        # hh + Ks0 [built as hh + Ks0]
        self.TriTrackList_wKS0 = makeTriTrackList_wKS0( name="DiTracks_wKS0ForRadiativeB" + self.name,
                                                        ksList=self.Kshort,
                                                        ditrkList=self.DiTrackList,
                                                        MinPTCut = config['B2XGResMinPT'],
                                                        MinSumPTCut = config['B2XGResSumPtMin'],
                                                        MaxMassCut = config['B2XGResMaxMass'],
                                                        MinMassCut = config['B2XGResMinMass'],
                                                        VtxChi2DOFCut = config['B2XGResVtxChi2DOF'],
                                                        BPVVDCHI2MinCut = config['B2XGResBPVVDCHI2Min'],
                                                        IPChi2MinCut = config['B2XGResIPCHI2Min'],
                                                        MaxDocaCut = config['B2XGResDocaMax']
                                                        )

        # hh + merged pi0
        self.TriTrackList_wpi0M = makeTriTrackList_wpi0M( name="DiTracks_wpi0_merged_ForRadiativeB" + self.name,
                                                          pi0List=self.MergedPi0,
                                                          ditrkList=self.DiTrackList,
                                                          MinPTCut = config['B2XGResMinPT'],
                                                          MinSumPTCut = config['B2XGResSumPtMin'],
                                                          MaxMassCut = config['B2XGResMaxMass'],
                                                          MinMassCut = config['B2XGResMinMass'],
                                                          VtxChi2DOFCut = config['B2XGResVtxChi2DOF'],
                                                          BPVVDCHI2MinCut = config['B2XGResBPVVDCHI2Min'],
                                                          IPChi2MinCut = config['B2XGResIPCHI2Min'],
                                                          MaxDocaCut = config['B2XGResDocaMax']
                                                          )

        # hh + resolved pi0
        self.TriTrackList_wpi0R = makeTriTrackList_wpi0R( name="DiTracks_wpi0_resolved_ForRadiativeB" + self.name,
                                                          pi0List=self.ResolvedPi0,
                                                          ditrkList=self.DiTrackList,
                                                          MinPTCut = config['B2XGResMinPT'],
                                                          MinSumPTCut = config['B2XGResSumPtMin'],
                                                          MaxMassCut = config['B2XGResMaxMass'],
                                                          MinMassCut = config['B2XGResMinMass'],
                                                          VtxChi2DOFCut = config['B2XGResVtxChi2DOF'],
                                                          BPVVDCHI2MinCut = config['B2XGResBPVVDCHI2Min'],
                                                          IPChi2MinCut = config['B2XGResIPCHI2Min'],
                                                          MaxDocaCut = config['B2XGResDocaMax']
                                                          )

        # hhhh (built as h+h+h+h)
        self.FourTrackList = makeFourTrackList( name="FourTracksForRadiativeB" + self.name,
                                                trkList=self.TrackList,
                                                MinPTCut = config['B2XGResMinPT'],
                                                MinSumPTCut = config['B2XGResSumPtMin'],
                                                MaxMassCut = config['B2XGResMaxMass'],
                                                MinMassCut = config['B2XGResMinMass'],
                                                VtxChi2DOFCut = config['B2XGResVtxChi2DOF'],
                                                BPVVDCHI2MinCut = config['B2XGResBPVVDCHI2Min'],
                                                IPChi2MinCut = config['B2XGResIPCHI2Min'],
                                                MaxDocaCut = config['B2XGResDocaMax']
                                                )

#         # hhhh (built as hh+hh)
#         self.FourTrackListPions = makeFourTrackListPions( name="FourTracksPionsForRadiativeB" + self.name,
#                                                           diTrkList = self.DiTrackList,
#                                                           MinPTCut = config['B2XGResMinPT'],
#                                                           MinSumPTCut = config['B2XGResSumPtMin'],
#                                                           MaxMassCut = config['B2XGResMaxMass'],
#                                                           MinMassCut = config['B2XGResMinMass'],
#                                                           VtxChi2DOFCut = config['B2XGResVtxChi2DOF'],
#                                                           BPVVDCHI2MinCut = config['B2XGResBPVVDCHI2Min'],
#                                                           IPChi2MinCut = config['B2XGResIPCHI2Min'],
#                                                           MaxDocaCut = config['B2XGResDocaMax']
#                                                           )

        # hhh + Ks0 [built as hhh + Ks0]
        self.FourTrackList_wKS0 = makeFourTrackList_wKS0( name="TriTracks_wKS0ForRadiativeB" + self.name,
                                                          ksList=self.Kshort,
                                                          tritrkList=self.TriTrackList,
                                                          MinPTCut = config['B2XGResMinPT'],
                                                          MinSumPTCut = config['B2XGResSumPtMin'],
                                                          MaxMassCut = config['B2XGResMaxMass'],
                                                          MinMassCut = config['B2XGResMinMass'],
                                                          VtxChi2DOFCut = config['B2XGResVtxChi2DOF'],
                                                          BPVVDCHI2MinCut = config['B2XGResBPVVDCHI2Min'],
                                                          IPChi2MinCut = config['B2XGResIPCHI2Min'],
                                                          MaxDocaCut = config['B2XGResDocaMax']
                                                          )

        # hhKs0Ks0 (built as hKs0+hKs0)
        self.FourTrackList_w2KS0 = makeFourTrackList_w2KS0( name="DiTracks_w2KS0ForRadiativeB" + self.name,
                                                            diTrkList = self.DiTrackList_wKS0,
                                                            MinPTCut = config['B2XGResMinPT'],
                                                            MinSumPTCut = config['B2XGResSumPtMin'],
                                                            MaxMassCut = config['B2XGResMaxMass'],
                                                            MinMassCut = config['B2XGResMinMass'],
                                                            VtxChi2DOFCut = config['B2XGResVtxChi2DOF'],
                                                            BPVVDCHI2MinCut = config['B2XGResBPVVDCHI2Min'],
                                                            IPChi2MinCut = config['B2XGResIPCHI2Min'],
                                                            MaxDocaCut = config['B2XGResDocaMax']
                                                            )


        # hhh + merged pi0
        self.FourTrackList_wpi0M = makeFourTrackList_wpi0M( name="TriTracks_wpi0_merged_ForRadiativeB" + self.name,
                                                            pi0List=self.MergedPi0,
                                                            tritrkList=self.TriTrackList,
                                                            MinPTCut = config['B2XGResMinPT'],
                                                            MinSumPTCut = config['B2XGResSumPtMin'],
                                                            MaxMassCut = config['B2XGResMaxMass'],
                                                            MinMassCut = config['B2XGResMinMass'],
                                                            VtxChi2DOFCut = config['B2XGResVtxChi2DOF'],
                                                            BPVVDCHI2MinCut = config['B2XGResBPVVDCHI2Min'],
                                                            IPChi2MinCut = config['B2XGResIPCHI2Min'],
                                                            MaxDocaCut = config['B2XGResDocaMax']
                                                            )

        # hhh + resolved pi0
        self.FourTrackList_wpi0R = makeFourTrackList_wpi0R( name="TriTracks_wpi0_resolved_ForRadiativeB" + self.name,
                                                            pi0List=self.ResolvedPi0,
                                                            tritrkList=self.TriTrackList,
                                                            MinPTCut = config['B2XGResMinPT'],
                                                            MinSumPTCut = config['B2XGResSumPtMin'],
                                                            MaxMassCut = config['B2XGResMaxMass'],
                                                            MinMassCut = config['B2XGResMinMass'],
                                                            VtxChi2DOFCut = config['B2XGResVtxChi2DOF'],
                                                            BPVVDCHI2MinCut = config['B2XGResBPVVDCHI2Min'],
                                                            IPChi2MinCut = config['B2XGResIPCHI2Min'],
                                                            MaxDocaCut = config['B2XGResDocaMax']
                                                            )

        #####################################################################################################################################
        ################################# STRIPPING LINES BUILT BELOW #######################################################################
        #####################################################################################################################################

        # Photons to add to the track combinations defined previously for the whole decay
        _PhotonSel = FilterDesktop(Code = "(PT > %(B2XGGammaPTMin)s) & (CL > %(B2XGGammaCL)s)" % self.__confdict__ )
        self.Photon = Selection( 'PhotonSel' + self.name,
                                 Algorithm = _PhotonSel,
                                 RequiredSelections = [StdLooseAllPhotons])

        # Converted Photons, for the hh[h]+g(-> e+e-) lines
        mergedConvPhotons = MergedSelection('MergedConvertedPhotons', RequiredSelections = [StdAllLooseGammaDD,StdAllLooseGammaLL])
        _ConvPhotonSel = FilterDesktop(Code = "(MM < 100*MeV) & (VFASPF(VCHI2/VDOF)<9) & (PT >  %(B2XGGammaCNVPTMin)s)" % self.__confdict__ )
        self.ConvPhoton = Selection( 'ConvPhoton' + self.name,
                                     Algorithm = _ConvPhotonSel,
                                     RequiredSelections = [mergedConvPhotons])

        B2XG2piName = self.name + "2pi_"
        self.RadiativeB2XG2pi = makeB2B2XG2piGamma( B2XG2piName,
                                                    diTrkList=self.DiTrackList,
                                                    photons=self.Photon,
                                                    MinPTCut = config['B2XGBMinPT'],
                                                    MinMassCut = config['B2XGBMinM2pi'],
                                                    MaxMassCut = config['B2XGBMaxM'],
                                                    MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                    VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                    MinSumPtCut = config['B2XGBSumPtMin'],
                                                    MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                    VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XG2piLine = StrippingLine( B2XG2piName+"Line",
                                          prescale = config['B2XG2piPrescale'],
                                          selection = tisTosSelection(self.RadiativeB2XG2pi),
                                          EnableFlavourTagging = True,
                                          ExtraInfoTools = [{'Type' : 'VertexIsolation'}])

        B2XG2piCNVName = self.name + "2pi_wCNV_"
        self.RadiativeB2XG2piCNV = makeB2B2XG2piGammaCNV( B2XG2piCNVName,
                                                          diTrkList=self.DiTrackList,
                                                          CNVphotons=self.ConvPhoton,
                                                          MinPTCut = config['B2XGBMinPT'],
                                                          MinMassCut = config['B2XGBMinM2pi'],
                                                          MaxMassCut = config['B2XGBMaxM'],
                                                          MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                          VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                          MinSumPtCut = config['B2XGBSumPtMin'],
                                                          MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                          VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XG2piCNVLine = StrippingLine( B2XG2piCNVName+"Line",
                                             prescale = config['B2XG2piCNVPrescale'],
                                             selection = tisTosSelection(self.RadiativeB2XG2piCNV),
                                             EnableFlavourTagging = True,
                                             ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )

        B2XGpiKsName = self.name + "pi_Ks0_"
        self.RadiativeB2XGpiKs = makeB2B2XGpiKsGamma( B2XGpiKsName,
                                                      diTrkList=self.DiTrackList_wKS0,
                                                      photons=self.Photon,
                                                      MinPTCut = config['B2XGBMinPT'],
                                                      MinMassCut = config['B2XGBMinM2pi'],
                                                      MaxMassCut = config['B2XGBMaxM'],
                                                      MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                      VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                      MinSumPtCut = config['B2XGBSumPtMin'],
                                                      MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                      VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XGpiKsLine = StrippingLine( B2XGpiKsName+"Line",
                                           prescale = config['B2XGpiKsPrescale'],
                                           selection = tisTosSelection(self.RadiativeB2XGpiKs),
                                           EnableFlavourTagging = True,
                                           ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )

        B2XG3piName = self.name + "3pi_"
        self.RadiativeB2XG3pi = makeB2B2XG3piGamma ( B2XG3piName,
                                                     triTrkList=self.TriTrackList,
                                                     photons=self.Photon,
                                                     MinPTCut = config['B2XGBMinPT'],
                                                     MinMassCut = config['B2XGBMinM3pi'],
                                                     MaxMassCut = config['B2XGBMaxM'],
                                                     MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                     VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                     MinSumPtCut = config['B2XGBSumPtMin'],
                                                     MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                     VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XG3piLine = StrippingLine( B2XG3piName+"Line",
                                          prescale = config['B2XG3piPrescale'],
                                          selection = tisTosSelection(self.RadiativeB2XG3pi),
                                          EnableFlavourTagging = True,
                                          ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )

#         B2XG3pi_altName = self.name + "3pi_alt_"
#         self.RadiativeB2XG3pi_alt = makeB2B2XG3piGamma ( B2XG3pi_altName,
#                                                          triTrkList=self.TriTrackList_alt,
#                                                          photons=self.Photon,
#                                                          MinPTCut = config['B2XGBMinPT'],
#                                                          MinMassCut = config['B2XGBMinM3pi'],
#                                                          MaxMassCut = config['B2XGBMaxM'],
#                                                          MaxCorrMCut = config['B2XGBMaxCorrM'],
#                                                          VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
#                                                          MinSumPtCut = config['B2XGBSumPtMin'],
#                                                          MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
#                                                          VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

#         self.B2XG3pi_altLine = StrippingLine( B2XG3pi_altName+"Line",
#                                               prescale = config['B2XG3piPrescale'],
#                                               selection = tisTosSelection(self.RadiativeB2XG3pi_alt),
#                                               EnableFlavourTagging = True,
#                                               ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )


        B2XG3piCNVName = self.name + "3pi_wCNV_"
        self.RadiativeB2XG3piCNV = makeB2B2XG3piGamma ( B2XG3piCNVName,
                                                        triTrkList=self.TriTrackList,
                                                        photons=self.ConvPhoton,
                                                        MinPTCut = config['B2XGBMinPT'],
                                                        MinMassCut = config['B2XGBMinM3pi'],
                                                        MaxMassCut = config['B2XGBMaxM'],
                                                        MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                        VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                        MinSumPtCut = config['B2XGBSumPtMin'],
                                                        MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                        VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XG3piCNVLine = StrippingLine( B2XG3piCNVName+"Line",
                                             prescale = config['B2XG3piCNVPrescale'],
                                             selection = tisTosSelection(self.RadiativeB2XG3piCNV),
                                             EnableFlavourTagging = True,
                                             ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )

        B2XG2piKsName = self.name + "2pi_Ks0_"
        self.RadiativeB2XG2piKs = makeB2B2XG2piKsGamma ( B2XG2piKsName,
                                                         triTrkList=self.TriTrackList_wKS0,
                                                         photons=self.Photon,
                                                         MinPTCut = config['B2XGBMinPT'],
                                                         MinMassCut = config['B2XGBMinM3pi'],
                                                         MaxMassCut = config['B2XGBMaxM'],
                                                         MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                         VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                         MinSumPtCut = config['B2XGBSumPtMin'],
                                                         MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                         VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XG2piKsLine = StrippingLine( B2XG2piKsName+"Line",
                                            prescale = config['B2XG2piKsPrescale'],
                                            selection = tisTosSelection(self.RadiativeB2XG2piKs),
                                            EnableFlavourTagging = True,
                                            ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )

        B2XG2pipi0MName = self.name + "2pi_pi0M_"
        self.RadiativeB2XG2pipi0M = makeB2B2XG2pipi0Gamma ( B2XG2pipi0MName,
                                                            triTrkList=self.TriTrackList_wpi0M,
                                                            photons=self.Photon,
                                                            MinPTCut = config['B2XGBMinPT'],
                                                            MinMassCut = config['B2XGBMinM3pi'],
                                                            MaxMassCut = config['B2XGBMaxM'],
                                                            MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                            VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                            MinSumPtCut = config['B2XGBSumPtMin'],
                                                            MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                            VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XG2pipi0MLine = StrippingLine( B2XG2pipi0MName+"Line",
                                              prescale = config['B2XG2pipi0MPrescale'],
                                              selection = tisTosSelection(self.RadiativeB2XG2pipi0M),
                                              EnableFlavourTagging = True,
                                              ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )


        B2XG2pipi0RName = self.name + "2pi_pi0R_"
        self.RadiativeB2XG2pipi0R = makeB2B2XG2pipi0Gamma ( B2XG2pipi0RName,
                                                            triTrkList=self.TriTrackList_wpi0R,
                                                            photons=self.Photon,
                                                            MinPTCut = config['B2XGBMinPT'],
                                                            MinMassCut = config['B2XGBMinM3pi'],
                                                            MaxMassCut = config['B2XGBMaxM'],
                                                            MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                            VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                            MinSumPtCut = config['B2XGBSumPtMin'],
                                                            MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                            VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XG2pipi0RLine = StrippingLine( B2XG2pipi0RName+"Line",
                                              prescale = config['B2XG2pipi0RPrescale'],
                                              selection = tisTosSelection(self.RadiativeB2XG2pipi0R),
                                              ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )#,
        #                                              EnableFlavourTagging = True )

        B2XG4piName = self.name + "4pi_"
        self.RadiativeB2XG4pi = makeB2B2XG4piGamma ( B2XG4piName,
                                                     fourTrkList=self.FourTrackList,
                                                     photons=self.Photon,
                                                     MinPTCut = config['B2XGBMinPT'],
                                                     MinMassCut = config['B2XGBMinM4pi'],
                                                     MaxMassCut = config['B2XGBMaxM'],
                                                     MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                     VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                     MinSumPtCut = config['B2XGBSumPtMin'],
                                                     MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                     VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XG4piLine = StrippingLine( B2XG4piName+"Line",
                                          prescale = config['B2XG4piPrescale'],
                                          selection = tisTosSelection(self.RadiativeB2XG4pi),
                                          EnableFlavourTagging = True,
                                          ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )

#         B2XG4pi_2rhoName = self.name + "4pi_2rho_"
#         self.RadiativeB2XG4pi_2rho = makeB2B2XG4piGamma ( B2XG4pi_2rhoName,
# #                                                               fourTrkListPions=self.FourTrackListPions,
#                                                           fourTrkList=self.FourTrackListPions,
#                                                           photons=self.Photon,
#                                                           MinPTCut = config['B2XGBMinPT'],
#                                                           MinMassCut = config['B2XGBMinM4pi'],
#                                                           MaxMassCut = config['B2XGBMaxM'],
#                                                           MaxCorrMCut = config['B2XGBMaxCorrM'],
#                                                           VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
#                                                           MinSumPtCut = config['B2XGBSumPtMin'],
#                                                           MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
#                                                           VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

#         self.B2XG4pi_2rhoLine = StrippingLine( B2XG4pi_2rhoName+"Line",
#                                                prescale = config['B2XG4piPrescale'],
#                                                selection = tisTosSelection(self.RadiativeB2XG4pi_2rho),
#                                                EnableFlavourTagging = True,
#                                                ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )

        B2XG3piKsName = self.name + "3pi_Ks0_"
        self.RadiativeB2XG3piKs = makeB2B2XG3piKsGamma ( B2XG3piKsName,
                                                         fourTrkList=self.FourTrackList_wKS0,
                                                         photons=self.Photon,
                                                         MinPTCut = config['B2XGBMinPT'],
                                                         MinMassCut = config['B2XGBMinM3pi'],
                                                         MaxMassCut = config['B2XGBMaxM'],
                                                         MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                         VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                         MinSumPtCut = config['B2XGBSumPtMin'],
                                                         MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                         VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XG3piKsLine = StrippingLine( B2XG3piKsName+"Line",
                                            prescale = config['B2XG3piKsPrescale'],
                                            selection = tisTosSelection(self.RadiativeB2XG3piKs),
                                            EnableFlavourTagging = True,
                                            ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )

        B2XG2pi2KsName = self.name + "2pi_2Ks_"
        self.RadiativeB2XG2pi2Ks = makeB2B2XG4piGamma ( B2XG2pi2KsName,
                                                        fourTrkList=self.FourTrackList_w2KS0,
                                                        photons=self.Photon,
                                                        MinPTCut = config['B2XGBMinPT'],
                                                        MinMassCut = config['B2XGBMinM4pi'],
                                                        MaxMassCut = config['B2XGBMaxM'],
                                                        MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                        VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                        MinSumPtCut = config['B2XGBSumPtMin'],
                                                        MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                        VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])


        self.B2XG2pi2KsLine = StrippingLine( B2XG2pi2KsName+"Line",
                                             prescale = config['B2XG2pi2KsPrescale'],
                                             selection = tisTosSelection(self.RadiativeB2XG2pi2Ks),
                                             EnableFlavourTagging = True,
                                             ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )

        B2XG3pipi0MName = self.name + "3pi_pi0M_"
        self.RadiativeB2XG3pipi0M = makeB2B2XG3pipi0Gamma ( B2XG3pipi0MName,
                                                            fourTrkList=self.FourTrackList_wpi0M,
                                                            photons=self.Photon,
                                                            MinPTCut = config['B2XGBMinPT'],
                                                            MinMassCut = config['B2XGBMinM3pi'],
                                                            MaxMassCut = config['B2XGBMaxM'],
                                                            MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                            VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                            MinSumPtCut = config['B2XGBSumPtMin'],
                                                            MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                            VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XG3pipi0MLine = StrippingLine( B2XG3pipi0MName+"Line",
                                              prescale = config['B2XG3pipi0MPrescale'],
                                              selection = tisTosSelection(self.RadiativeB2XG3pipi0M),
                                              EnableFlavourTagging = True,
                                              ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )

        B2XG3pipi0RName = self.name + "3pi_pi0R_"
        self.RadiativeB2XG3pipi0R = makeB2B2XG3pipi0Gamma ( B2XG3pipi0RName,
                                                            fourTrkList=self.FourTrackList_wpi0R,
                                                            photons=self.Photon,
                                                            MinPTCut = config['B2XGBMinPT'],
                                                            MinMassCut = config['B2XGBMinM3pi'],
                                                            MaxMassCut = config['B2XGBMaxM'],
                                                            MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                            VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                            MinSumPtCut = config['B2XGBSumPtMin'],
                                                            MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                            VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XG3pipi0RLine = StrippingLine( B2XG3pipi0RName+"Line",
                                              prescale = config['B2XG3pipi0RPrescale'],
                                              selection = tisTosSelection(self.RadiativeB2XG3pipi0R),
                                              ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )#,
        #                                              EnableFlavourTagging = True )

        B2XGLambdapiName = self.name + "pi_Lambda_"
        self.RadiativeB2XGLambdapi = makeB2B2XGLambdapiGamma ( B2XGLambdapiName,
                                                               trkList=self.TrackList,
                                                               lambdas=self.Lambda,
                                                               photons=self.Photon,
                                                               MinPTCut = config['B2XGBMinPT'],
                                                               MinMassCut = config['B2XGBMinMLambda'],
                                                               MaxMassCut = config['B2XGBMaxM'],
                                                               MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                               VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                               MinSumPtCut = config['B2XGBSumPtMin'],
                                                               MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                               VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XGLambdapiLine = StrippingLine( B2XGLambdapiName+"Line",
                                               prescale = config['B2XGLambdapiPrescale'],
                                               selection = tisTosSelection(self.RadiativeB2XGLambdapi),
                                               EnableFlavourTagging = True,
                                               ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )

        B2XGLambda2piName = self.name + "2pi_Lambda_"
        self.RadiativeB2XGLambda2pi = makeB2B2XGLambda2piGamma ( B2XGLambda2piName,
                                                                 diTrkList=self.DiTrackList,
                                                                 lambdas=self.Lambda,
                                                                 photons=self.Photon,
                                                                 MinPTCut = config['B2XGBMinPT'],
                                                                 MinMassCut = config['B2XGBMinMLambda'],
                                                                 MaxMassCut = config['B2XGBMaxM'],
                                                                 MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                                 VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                                 MinSumPtCut = config['B2XGBSumPtMin'],
                                                                 MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                                 VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XGLambda2piLine = StrippingLine( B2XGLambda2piName+"Line",
                                                prescale = config['B2XGLambda2piPrescale'],
                                                selection = tisTosSelection(self.RadiativeB2XGLambda2pi) ,
                                                EnableFlavourTagging = True,
                                                ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )

        B2XGLambda3piName = self.name + "3pi_Lambda_"
        self.RadiativeB2XGLambda3pi = makeB2B2XGLambda3piGamma ( B2XGLambda3piName,
                                                                 triTrkList=self.TriTrackList,
                                                                 lambdas=self.Lambda,
                                                                 photons=self.Photon,
                                                                 MinPTCut = config['B2XGBMinPT'],
                                                                 MinMassCut = config['B2XGBMinMLambda'],
                                                                 MaxMassCut = config['B2XGBMaxM'],
                                                                 MaxCorrMCut = config['B2XGBMaxCorrM'],
                                                                 VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
                                                                 MinSumPtCut = config['B2XGBSumPtMin'],
                                                                 MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
                                                                 VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

        self.B2XGLambda3piLine = StrippingLine( B2XGLambda3piName+"Line",
                                                prescale = config['B2XGLambda3piPrescale'],
                                                selection = tisTosSelection(self.RadiativeB2XGLambda3pi),
                                                EnableFlavourTagging = True,
                                                ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )

#         B2XGLbLambdaName = self.name + "LbLambda_"
#         self.RadiativeB2XGLbLambda = makeB2B2XGLbLambdaGamma ( B2XGLbLambdaName,
#                                                                lambdas=self.Lambda,
#                                                                photons=self.Photon,
#                                                                MinPTCut = config['B2XGBMinPT'],
#                                                                MinMassCut = config['B2XGBMinMLambda'],
#                                                                MaxMassCut = config['B2XGBMaxM'],
#                                                                MaxCorrMCut = config['B2XGBMaxCorrM'],
#                                                                VtxChi2DOFCut = config['B2XGBVtxChi2DOF'],
#                                                                MinSumPtCut = config['B2XGBSumPtMin'],
#                                                                MinBPVDIRACut = config['B2XGBMinBPVDIRA'],
#                                                                VtxMaxIPChi2Cut = config['B2XGBVtxMaxIPChi2'])

#         self.B2XGLbLambdaLine = StrippingLine( B2XGLbLambdaName+"Line",
#                                                prescale = config['B2XGLbLambdaPrescale'],
#                                                selection = tisTosSelection(self.RadiativeB2XGLbLambda),
#                                                EnableFlavourTagging = True,
#                                                ExtraInfoTools = [{'Type' : 'VertexIsolation'}] )


        self.registerLine(self.B2XG2piLine)
        self.registerLine(self.B2XG2piCNVLine)
        self.registerLine(self.B2XGpiKsLine)

        self.registerLine(self.B2XG3piLine)
#        self.registerLine(self.B2XG3pi_altLine)
        self.registerLine(self.B2XG3piCNVLine)
        self.registerLine(self.B2XG2pipi0RLine)
        self.registerLine(self.B2XG2pipi0MLine)
        self.registerLine(self.B2XG2piKsLine)

        self.registerLine(self.B2XG4piLine)
#        self.registerLine(self.B2XG4pi_2rhoLine)
        self.registerLine(self.B2XG3piKsLine)
        self.registerLine(self.B2XG3pipi0RLine)
        self.registerLine(self.B2XG3pipi0MLine)
        self.registerLine(self.B2XG2pi2KsLine)

        self.registerLine(self.B2XGLambdapiLine)
        self.registerLine(self.B2XGLambda2piLine)
        self.registerLine(self.B2XGLambda3piLine)

def makeDiTrackList( name,
                     trkList,
                     MinPTCut,
                     MinSumPTCut,
                     MaxMassCut,
                     MinMassCut,
                     IPChi2MinCut,
                     MaxDocaCut,
                     VtxChi2DOFCut ) :
    """
    Di-track selection
    """
    _diTrackPreVertexCuts = "(ASUM(SUMTREE(PT,ISBASIC,0.0))> %(MinSumPTCut)s)"%locals()
    _diTrackPreVertexCuts += " & in_range( %(MinMassCut)s , AM ,%(MaxMassCut)s)" %locals()

    _diTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s)" %locals()

    _combineDiTrack = CombineParticles( DecayDescriptor="rho(770)0 -> pi+ pi-",
                                        CombinationCut = _diTrackPreVertexCuts,
                                        MotherCut = _diTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineDiTrack,
                     RequiredSelections = [ trkList ] )

def makeDiTrackList_wKS0( name,
                          trkList,
                          ksList,
                          MinPTCut,
                          MinSumPTCut,
                          MaxMassCut,
                          MinMassCut,
                          IPChi2MinCut,
                          MaxDocaCut,
                          VtxChi2DOFCut ) :
    """
    Di-track selection
    """
    _diTrackPreVertexCuts = "(ASUM(SUMTREE(PT,(ISBASIC | (ID=='gamma') | (ID=='KS0')),0.0))> %(MinSumPTCut)s)"%locals()
    _diTrackPreVertexCuts += " & in_range( %(MinMassCut)s , AM ,%(MaxMassCut)s)" %locals()

    _diTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s)" %locals()
    _diTrackPostVertexCuts += " & (MIPCHI2DV(PRIMARY) > %(IPChi2MinCut)s)" %locals()

    _combineDiTrack = CombineParticles( DecayDescriptor="[rho(770)+ -> pi+ KS0]cc",
                                        CombinationCut = _diTrackPreVertexCuts,
                                        MotherCut = _diTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineDiTrack,
                     RequiredSelections = [ trkList , ksList ] )

def makeTriTrackList( name,
                      trkList,
                      MinPTCut,
                      MinSumPTCut,
                      MaxMassCut,
                      MinMassCut,
                      VtxChi2DOFCut,
                      BPVVDCHI2MinCut,
                      IPChi2MinCut,
                      MaxDocaCut) :
    """
    Tri-track selection
    """
    _TriTrackPreVertexCuts = hasTopoChild()
    _TriTrackPreVertexCuts += " & (ASUM(SUMTREE(PT,(ISBASIC | (ID=='gamma')),0.0))> %(MinSumPTCut)s)"%locals()
    _TriTrackPreVertexCuts += " & in_range( %(MinMassCut)s , AM ,%(MaxMassCut)s)" %locals()

    _TriTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s) & (PT > %(MinPTCut)s) & (BPVVDCHI2  > %(BPVVDCHI2MinCut)s) & (MIPCHI2DV(PRIMARY) > %(IPChi2MinCut)s)" %locals()

    _combineTriTrack = CombineParticles( DecayDescriptor="[K_1(1270)+ -> pi+ pi- pi+]cc",
                                         CombinationCut = _TriTrackPreVertexCuts,
                                         MotherCut = _TriTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineTriTrack,
                     RequiredSelections = [ trkList ] )

def makeTriTrackList_alt( name,
                          trkList,
                          ditrkList,
                          MinPTCut,
                          MinSumPTCut,
                          MaxMassCut,
                          MinMassCut,
                          VtxChi2DOFCut,
                          BPVVDCHI2MinCut,
                          IPChi2MinCut,
                          MaxDocaCut) :
    """
    Tri-track selection
    """
    _TriTrackPreVertexCuts = " (ASUM(SUMTREE(PT,(ISBASIC | (ID=='rho(770)0') | (ID=='gamma')),0.0))> %(MinSumPTCut)s)"%locals()
    _TriTrackPreVertexCuts += " & in_range( %(MinMassCut)s , AM ,%(MaxMassCut)s)" %locals()

    _TriTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s) & (PT > %(MinPTCut)s) & (BPVVDCHI2  > %(BPVVDCHI2MinCut)s) & (MIPCHI2DV(PRIMARY) > %(IPChi2MinCut)s)" %locals()

    _combineTriTrack = CombineParticles( DecayDescriptor="[K_1(1270)+ -> rho(770)0 pi+]cc",
                                         CombinationCut = _TriTrackPreVertexCuts,
                                         MotherCut = _TriTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineTriTrack,
                     RequiredSelections = [ trkList,ditrkList ] )

def makeTriTrackList_wKS0( name,
                           ksList,
                           ditrkList,
                           MinPTCut,
                           MinSumPTCut,
                           MaxMassCut,
                           MinMassCut,
                           VtxChi2DOFCut,
                           BPVVDCHI2MinCut,
                           IPChi2MinCut,
                           MaxDocaCut) :

    """
    Tri-track selection
    """
    _TriTrackPreVertexCuts = "(ASUM(SUMTREE(PT,(ISBASIC | (ID =='rho(770)0') | (ID=='gamma') | (ID=='KS0')),0.0))> %(MinSumPTCut)s)"%locals()
    _TriTrackPreVertexCuts += " & in_range( %(MinMassCut)s , AM ,%(MaxMassCut)s)" %locals()

    _TriTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s) & (PT > %(MinPTCut)s) & (BPVVDCHI2  > %(BPVVDCHI2MinCut)s) & (MIPCHI2DV(PRIMARY) > %(IPChi2MinCut)s)" %locals()

    _combineTriTrack = CombineParticles( DecayDescriptor="K*_2(1430)0 -> rho(770)0 KS0",
                                         CombinationCut = _TriTrackPreVertexCuts,
                                         MotherCut = _TriTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineTriTrack,
                     RequiredSelections = [ ksList,ditrkList ] )

def makeTriTrackList_wpi0M( name,
                            pi0List,
                            ditrkList,
                            MinPTCut,
                            MinSumPTCut,
                            MaxMassCut,
                            MinMassCut,
                            VtxChi2DOFCut,
                            BPVVDCHI2MinCut,
                            IPChi2MinCut,
                            MaxDocaCut) :
    """
    Tri-track selection
    """
    _TriTrackPreVertexCuts = hasTopoChild()
    _TriTrackPreVertexCuts = "(ASUM(SUMTREE(PT,(ISBASIC | (ID=='rho(770)0') | (ID=='gamma') | (ID=='pi0')),0.0))> %(MinSumPTCut)s)"%locals()
    _TriTrackPreVertexCuts += " & in_range( %(MinMassCut)s , AM ,%(MaxMassCut)s)" %locals()

    _TriTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s) & (PT > %(MinPTCut)s) & (BPVVDCHI2  > %(BPVVDCHI2MinCut)s) & (MIPCHI2DV(PRIMARY) > %(IPChi2MinCut)s)" %locals()

    _combineTriTrack = CombineParticles( DecayDescriptor="eta -> rho(770)0 pi0",
                                         CombinationCut = _TriTrackPreVertexCuts,
                                         MotherCut = _TriTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineTriTrack,
                     RequiredSelections = [ pi0List,ditrkList ] )

def makeTriTrackList_wpi0R( name,
                            pi0List,
                            ditrkList,
                            MinPTCut,
                            MinSumPTCut,
                            MaxMassCut,
                            MinMassCut,
                            VtxChi2DOFCut,
                            BPVVDCHI2MinCut,
                            IPChi2MinCut,
                            MaxDocaCut) :
    """
    Tri-track selection
    """
#    _TriTrackPreVertexCuts = hasTopoChild()
    _TriTrackPreVertexCuts = "(ASUM(SUMTREE(PT,(ISBASIC | (ID=='rho(770)0') | (ID=='gamma') | (ID=='pi0')),0.0))> %(MinSumPTCut)s)"%locals()
    _TriTrackPreVertexCuts += " & in_range( %(MinMassCut)s , AM ,%(MaxMassCut)s)" %locals()

    _TriTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s) & (PT > %(MinPTCut)s) & (BPVVDCHI2  > %(BPVVDCHI2MinCut)s) & (MIPCHI2DV(PRIMARY) > %(IPChi2MinCut)s)" %locals()

    _combineTriTrack = CombineParticles( DecayDescriptor="eta -> rho(770)0 pi0",
                                         CombinationCut = _TriTrackPreVertexCuts,
                                         MotherCut = _TriTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineTriTrack,
                     RequiredSelections = [ pi0List,ditrkList ] )

def makeFourTrackList( name,
                       trkList,
                       MinPTCut,
                       MinSumPTCut,
                       MaxMassCut,
                       MinMassCut,
                       VtxChi2DOFCut,
                       BPVVDCHI2MinCut,
                       IPChi2MinCut,
                       MaxDocaCut) :
    """
    Four-track selection
    """
    _FourTrackPreVertexCuts = hasTopoChild()
    _FourTrackPreVertexCuts += "& (ASUM(SUMTREE(PT,ISBASIC,0.0))> %(MinSumPTCut)s)"%locals()
    _FourTrackPreVertexCuts += " & in_range( %(MinMassCut)s , AM ,%(MaxMassCut)s)" %locals()

    _FourTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s) & (PT > %(MinPTCut)s) & (BPVVDCHI2  > %(BPVVDCHI2MinCut)s) & (MIPCHI2DV(PRIMARY) > %(IPChi2MinCut)s)" %locals()

    _combineFourTrack = CombineParticles( DecayDescriptor="f_2(1270) -> pi+ pi+ pi- pi-",
                                          CombinationCut = _FourTrackPreVertexCuts,
                                          MotherCut = _FourTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineFourTrack,
                     RequiredSelections = [ trkList  ] )

def makeFourTrackListPions( name,
                            diTrkList,
                            MinPTCut,
                            MinSumPTCut,
                            MaxMassCut,
                            MinMassCut,
                            VtxChi2DOFCut,
                            BPVVDCHI2MinCut,
                            IPChi2MinCut,
                            MaxDocaCut) :
    """
    Four-track selection
    """
    _FourTrackPreVertexCuts = "(ASUM(SUMTREE(PT,(ID=='rho(770)0'),0.0))> %(MinSumPTCut)s)"%locals()
    _FourTrackPreVertexCuts += " & in_range( %(MinMassCut)s , AM ,%(MaxMassCut)s)" %locals()

    _FourTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s) & (PT > %(MinPTCut)s) & (BPVVDCHI2  > %(BPVVDCHI2MinCut)s) & (MIPCHI2DV(PRIMARY) > %(IPChi2MinCut)s)" %locals()

    _combineFourTracks = CombineParticles( DecayDescriptor="f_2(1270) -> rho(770)0 rho(770)0",
                                           CombinationCut = _FourTrackPreVertexCuts,
                                           MotherCut = _FourTrackPostVertexCuts )
    return Selection(name,
                     Algorithm = _combineFourTracks,
                     RequiredSelections = [diTrkList])

def makeFourTrackList_w2KS0( name,
                             diTrkList,
                             MinPTCut,
                             MinSumPTCut,
                             MaxMassCut,
                             MinMassCut,
                             VtxChi2DOFCut,
                             BPVVDCHI2MinCut,
                             IPChi2MinCut,
                             MaxDocaCut) :
    """
    Four-track selection
    """
    _FourTrackPreVertexCuts = "(ASUM(SUMTREE(PT,((ID=='rho(770)+') | (ID=='rho(770)-')),0.0))> %(MinSumPTCut)s)"%locals()
    _FourTrackPreVertexCuts += " & in_range( %(MinMassCut)s , AM ,%(MaxMassCut)s)" %locals()

    _FourTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s) & (PT > %(MinPTCut)s) & (BPVVDCHI2  > %(BPVVDCHI2MinCut)s) & (MIPCHI2DV(PRIMARY) > %(IPChi2MinCut)s)" %locals()

    _combineFourTracks = CombineParticles( DecayDescriptor="f_2(1270) -> rho(770)+ rho(770)-",
                                           CombinationCut = _FourTrackPreVertexCuts,
                                           MotherCut = _FourTrackPostVertexCuts )
    return Selection(name,
                     Algorithm = _combineFourTracks,
                     RequiredSelections = [diTrkList])


def makeFourTrackList_wKS0( name,
                           ksList,
                           tritrkList,
                           MinPTCut,
                           MinSumPTCut,
                           MaxMassCut,
                           MinMassCut,
                           VtxChi2DOFCut,
                           BPVVDCHI2MinCut,
                           IPChi2MinCut,
                           MaxDocaCut) :
    """
    Tri-track selection
    """
    _TriTrackPreVertexCuts = "(ASUM(SUMTREE(PT,(ISBASIC | (ID =='K_1(1270)+') | (ID=='gamma') | (ID=='KS0')),0.0))> %(MinSumPTCut)s)"%locals()
    _TriTrackPreVertexCuts += " & in_range( %(MinMassCut)s , AM ,%(MaxMassCut)s)" %locals()

    _TriTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s) & (PT > %(MinPTCut)s) & (BPVVDCHI2  > %(BPVVDCHI2MinCut)s) & (MIPCHI2DV(PRIMARY) > %(IPChi2MinCut)s)" %locals()

    _combineTriTrack = CombineParticles( DecayDescriptor="[K_2(1770)+ -> K_1(1270)+ KS0]cc",
                                         CombinationCut = _TriTrackPreVertexCuts,
                                         MotherCut = _TriTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineTriTrack,
                     RequiredSelections = [ ksList,tritrkList ] )


def makeFourTrackList_wpi0M( name,
                             pi0List,
                             tritrkList,
                             MinPTCut,
                             MinSumPTCut,
                             MaxMassCut,
                             MinMassCut,
                             VtxChi2DOFCut,
                             BPVVDCHI2MinCut,
                             IPChi2MinCut,
                             MaxDocaCut) :
    """
    Four-track selection
    """
#    _FourTrackPreVertexCuts = hasTopoChild()
    _FourTrackPreVertexCuts = "(ASUM(SUMTREE(PT,(ISBASIC | (ID=='K_1(1270)+') | (ID=='gamma') | (ID=='pi0')),0.0))> %(MinSumPTCut)s)"%locals()
    _FourTrackPreVertexCuts += " & in_range( %(MinMassCut)s , AM ,%(MaxMassCut)s)" %locals()

    _FourTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s) & (PT > %(MinPTCut)s) & (BPVVDCHI2  > %(BPVVDCHI2MinCut)s) & (MIPCHI2DV(PRIMARY) > %(IPChi2MinCut)s)" %locals()

    _combineFourTrack = CombineParticles( DecayDescriptor="[K_2(1770)+ -> K_1(1270)+ pi0]cc",
                                          CombinationCut = _FourTrackPreVertexCuts,
                                          MotherCut = _FourTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineFourTrack,
                     RequiredSelections = [ pi0List,tritrkList ] )

def makeFourTrackList_wpi0R( name,
                             pi0List,
                             tritrkList,
                             MinPTCut,
                             MinSumPTCut,
                             MaxMassCut,
                             MinMassCut,
                             VtxChi2DOFCut,
                             BPVVDCHI2MinCut,
                             IPChi2MinCut,
                             MaxDocaCut) :
    """
    Four-track selection
    """
    _FourTrackPreVertexCuts = "(ASUM(SUMTREE(PT,(ISBASIC | (ID=='K_1(1270)+') | (ID=='gamma') | (ID=='pi0')),0.0))> %(MinSumPTCut)s)"%locals()
    _FourTrackPreVertexCuts += " & in_range( %(MinMassCut)s , AM ,%(MaxMassCut)s)" %locals()

    _FourTrackPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s) & (PT > %(MinPTCut)s) & (BPVVDCHI2  > %(BPVVDCHI2MinCut)s) & (MIPCHI2DV(PRIMARY) > %(IPChi2MinCut)s)" %locals()

    _combineFourTrack = CombineParticles( DecayDescriptor="[K_2(1770)+ -> K_1(1270)+ pi0]cc",
                                          CombinationCut = _FourTrackPreVertexCuts,
                                          MotherCut = _FourTrackPostVertexCuts )

    return Selection(name,
                     Algorithm = _combineFourTrack,
                     RequiredSelections = [ pi0List,tritrkList ] )

##################################################################################################################################################################################################
##################################################################### Build B Decays Below #######################################################################################################
##################################################################################################################################################################################################

def makeB2B2XG2piGamma( name,
                        diTrkList,
                        photons,
                        MinPTCut,
                        MinMassCut,
                        MaxMassCut,
                        MaxCorrMCut,
                        VtxChi2DOFCut,
                        MinSumPtCut,
                        MinBPVDIRACut,
                        VtxMaxIPChi2Cut) :
    """
    Charmless B2XG to 2pi selection with missing mass
    """
    _B2B2XG2piPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2B2XG2piPreVertexCuts += " & (ASUM(SUMTREE(PT,(ISBASIC | (ID=='rho(770)0') | (ID=='gamma')),0.0)) > %(MinSumPtCut)s )"%locals()


    _B2B2XG2piPostVertexCuts = hasHighTopoChild()
    _B2B2XG2piPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()
    _B2B2XG2piPostVertexCuts += " & (BPVIPCHI2() < %(VtxMaxIPChi2Cut)s )" %locals()
    _B2B2XG2piPostVertexCuts += " & (BPVDIRA > %(MinBPVDIRACut)s)"%locals()

    _combineB2B2XG2pi = CombineParticles( DecayDescriptor="B0 -> rho(770)0 gamma",
                                          MotherCut = _B2B2XG2piPostVertexCuts,
                                          CombinationCut = _B2B2XG2piPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2B2XG2pi,
                     RequiredSelections = [ diTrkList,photons ] )


def makeB2B2XG2piGammaCNV( name,
                           diTrkList,
                           CNVphotons,
                           MinPTCut,
                           MinMassCut,
                           MaxMassCut,
                           MaxCorrMCut,
                           VtxChi2DOFCut,
                           MinSumPtCut,
                           MinBPVDIRACut,
                           VtxMaxIPChi2Cut) :
    """
    Charmless B2XG to 2pi and cobnverted photon selection with missing mass
    """
    _B2B2XG2piPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2B2XG2piPreVertexCuts += "& (ASUM(SUMTREE(PT,(ISBASIC | (ID=='rho(770)+') | (ID=='gamma')),0.0)) > %(MinSumPtCut)s )"%locals()


    _B2B2XG2piPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()
    _B2B2XG2piPostVertexCuts += " & (BPVIPCHI2() < %(VtxMaxIPChi2Cut)s )" %locals()
    _B2B2XG2piPostVertexCuts += " & (BPVDIRA > %(MinBPVDIRACut)s)"%locals()

    _combineB2B2XG2pi = CombineParticles( DecayDescriptor="B0 -> rho(770)0 gamma",
                                          MotherCut = _B2B2XG2piPostVertexCuts,
                                          CombinationCut = _B2B2XG2piPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2B2XG2pi,
                     RequiredSelections = [ diTrkList,CNVphotons ] )


def makeB2B2XGpiKsGamma( name,
                         diTrkList,
                         photons,
                         MinPTCut,
                         MinMassCut,
                         MaxMassCut,
                         MaxCorrMCut,
                         VtxChi2DOFCut,
                         MinSumPtCut,
                         MinBPVDIRACut,
                         VtxMaxIPChi2Cut) :
    """
    Charmless B2XG to pi+Ks0 selection with missing mass
    """

    _B2B2XG2piPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2B2XG2piPreVertexCuts += " & (ASUM(SUMTREE(PT,(ISBASIC | (ID=='rho(770)+') | (ID=='gamma') | (ID == 'KS0')),0.0)) > %(MinSumPtCut)s )"%locals()

    _B2B2XG2piPostVertexCuts = hasHighTopoChild()
    _B2B2XG2piPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()
    _B2B2XG2piPostVertexCuts += " & (BPVIPCHI2() < %(VtxMaxIPChi2Cut)s )" %locals()
    _B2B2XG2piPostVertexCuts += " & (BPVDIRA > %(MinBPVDIRACut)s)"%locals()

    _combineB2B2XG2pi = CombineParticles( DecayDescriptor="[B+ -> rho(770)+ gamma]cc",
                                          MotherCut = _B2B2XG2piPostVertexCuts,
                                          CombinationCut = _B2B2XG2piPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2B2XG2pi,
                     RequiredSelections = [ diTrkList,photons ] )


def makeB2B2XG3piGamma( name,
                        triTrkList,
                        photons,
                        MinPTCut,
                        MinMassCut,
                        MaxMassCut,
                        MaxCorrMCut,
                        VtxChi2DOFCut,
                        MinSumPtCut,
                        MinBPVDIRACut,
                        VtxMaxIPChi2Cut) :
    """
    Charmless B2XG to 3pi selection with missing mass
    """

    _B2B2XG3piPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2B2XG3piPreVertexCuts += " & (ASUM(SUMTREE(PT,(ISBASIC | (ID=='K_1(1270)+') | (ID=='gamma')),0.0)) > %(MinSumPtCut)s )"%locals()

    _B2B2XG3piPostVertexCuts = "(VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()
    _B2B2XG3piPostVertexCuts += " & (BPVIPCHI2() < %(VtxMaxIPChi2Cut)s )" %locals()
    _B2B2XG3piPostVertexCuts += " & (BPVDIRA > %(MinBPVDIRACut)s)"%locals()

    _combineB2B2XG3pi = CombineParticles( DecayDescriptor="[B+ -> K_1(1270)+ gamma]cc",
                                          MotherCut = _B2B2XG3piPostVertexCuts,
                                          CombinationCut = _B2B2XG3piPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2B2XG3pi,
                     RequiredSelections = [ triTrkList, photons ] )


def makeB2B2XG2piKsGamma( name,
                          triTrkList,
                          photons,
                          MinPTCut,
                          MinMassCut,
                          MaxMassCut,
                          MaxCorrMCut,
                          VtxChi2DOFCut,
                          MinSumPtCut,
                          MinBPVDIRACut,
                          VtxMaxIPChi2Cut) :
    """
    Charmless B2XG to 2pi+Ks0 selection with missing mass
    """

    _B2B2XG3piPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2B2XG3piPreVertexCuts += " & (ASUM(SUMTREE(PT,(ISBASIC | (ID=='K*_2(1430)0') | (ID=='gamma') | (ID == 'KS0')),0.0)) > %(MinSumPtCut)s )"%locals()

    _B2B2XG3piPostVertexCuts = hasHighTopoChild()
    _B2B2XG3piPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()
    _B2B2XG3piPostVertexCuts += " & (BPVIPCHI2() < %(VtxMaxIPChi2Cut)s )" %locals()
    _B2B2XG3piPostVertexCuts += " & (BPVDIRA > %(MinBPVDIRACut)s)"%locals()

    _combineB2B2XG3pi = CombineParticles( DecayDescriptor="B0 -> K*_2(1430)0 gamma",
                                          MotherCut = _B2B2XG3piPostVertexCuts,
                                          CombinationCut = _B2B2XG3piPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2B2XG3pi,
                     RequiredSelections = [ triTrkList, photons ] )

def makeB2B2XG2pipi0Gamma( name,
                            triTrkList,
                            photons,
                            MinPTCut,
                            MinMassCut,
                            MaxMassCut,
                            MaxCorrMCut,
                            VtxChi2DOFCut,
                            MinSumPtCut,
                            MinBPVDIRACut,
                            VtxMaxIPChi2Cut) :
    """
    Charmless B2XG to 2pi+ pi0 selection with missing mass
    """

    _B2B2XG3piPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2B2XG3piPreVertexCuts += " & (ASUM(SUMTREE(PT,(ISBASIC | (ID=='eta')| (ID=='gamma') | (ID == 'pi0')),0.0)) > %(MinSumPtCut)s )"%locals()

    _B2B2XG3piPostVertexCuts = hasHighTopoChild()
    _B2B2XG3piPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()
    _B2B2XG3piPostVertexCuts += " & (BPVIPCHI2() < %(VtxMaxIPChi2Cut)s )" %locals()
    _B2B2XG3piPostVertexCuts += " & (BPVDIRA > %(MinBPVDIRACut)s)"%locals()

    _combineB2B2XG3pi = CombineParticles( DecayDescriptor="B0 -> eta gamma",
                                          MotherCut = _B2B2XG3piPostVertexCuts,
                                          CombinationCut = _B2B2XG3piPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2B2XG3pi,
                     RequiredSelections = [ triTrkList, photons ] )

def makeB2B2XG4piGamma( name,
                        fourTrkList,
                        photons,
                        MinPTCut,
                        MinMassCut,
                        MaxMassCut,
                        MaxCorrMCut,
                        VtxChi2DOFCut,
                        MinSumPtCut,
                        MinBPVDIRACut,
                        VtxMaxIPChi2Cut) :
    """
    Charmless B2XG to 4pi (a group of three tracks with an additional one) selection with missing mass
    """

    _B2B2XG4piPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2B2XG4piPreVertexCuts += " & (ASUM(SUMTREE(PT,(ISBASIC| (ID=='f_2(1270)') | (ID=='gamma')),0.0)) > %(MinSumPtCut)s )"%locals()

    _B2B2XG4piPostVertexCuts = hasHighTopoChild()
    _B2B2XG4piPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()
    _B2B2XG4piPostVertexCuts += " & (BPVIPCHI2() < %(VtxMaxIPChi2Cut)s )" %locals()
    _B2B2XG4piPostVertexCuts += " & (BPVDIRA > %(MinBPVDIRACut)s)"%locals()

    _combineB2B2XG4pi = CombineParticles( DecayDescriptor="B0 -> f_2(1270) gamma",
                                          MotherCut = _B2B2XG4piPostVertexCuts,
                                          CombinationCut = _B2B2XG4piPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2B2XG4pi,
                     RequiredSelections = [ fourTrkList, photons ] )

def makeB2B2XG3piKsGamma( name,
                          fourTrkList,
                          photons,
                          MinPTCut,
                          MinMassCut,
                          MaxMassCut,
                          MaxCorrMCut,
                          VtxChi2DOFCut,
                          MinSumPtCut,
                          MinBPVDIRACut,
                          VtxMaxIPChi2Cut) :
    """
    Charmless B2XG to 3pi+Ks0 selection with missing mass
    """

    _B2B2XG3piPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2B2XG3piPreVertexCuts += " & (ASUM(SUMTREE(PT,(ISBASIC | (ID=='K_2(1770)+') | (ID=='gamma') | (ID == 'KS0')),0.0)) > %(MinSumPtCut)s )"%locals()

    _B2B2XG3piPostVertexCuts = hasHighTopoChild()
    _B2B2XG3piPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()
    _B2B2XG3piPostVertexCuts += " & (BPVIPCHI2() < %(VtxMaxIPChi2Cut)s )" %locals()
    _B2B2XG3piPostVertexCuts += " & (BPVDIRA > %(MinBPVDIRACut)s)"%locals()

    _combineB2B2XG3pi = CombineParticles( DecayDescriptor="[B+ -> K_2(1770)+ gamma]cc",
                                          MotherCut = _B2B2XG3piPostVertexCuts,
                                          CombinationCut = _B2B2XG3piPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2B2XG3pi,
                     RequiredSelections = [ fourTrkList, photons ] )

def makeB2B2XG3pipi0Gamma( name,
                            fourTrkList,
                            photons,
                            MinPTCut,
                            MinMassCut,
                            MaxMassCut,
                            MaxCorrMCut,
                            VtxChi2DOFCut,
                            MinSumPtCut,
                            MinBPVDIRACut,
                            VtxMaxIPChi2Cut) :
    """
    Charmless B2XG to 3pi+merged pi0 selection with missing mass
    """

    _B2B2XG3piPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2B2XG3piPreVertexCuts += " & (ASUM(SUMTREE(PT,(ISBASIC | (ID=='K_2(1770)+')| (ID=='gamma') | (ID == 'pi0')),0.0)) > %(MinSumPtCut)s )"%locals()

    _B2B2XG3piPostVertexCuts = hasHighTopoChild()
    _B2B2XG3piPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()
    _B2B2XG3piPostVertexCuts += " & (BPVIPCHI2() < %(VtxMaxIPChi2Cut)s )" %locals()
    _B2B2XG3piPostVertexCuts += " & (BPVDIRA > %(MinBPVDIRACut)s)"%locals()

    _combineB2B2XG3pi = CombineParticles( DecayDescriptor="[B+ -> K_2(1770)+ gamma]cc",
                                          MotherCut = _B2B2XG3piPostVertexCuts,
                                          CombinationCut = _B2B2XG3piPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2B2XG3pi,
                     RequiredSelections = [ fourTrkList, photons ] )

def makeB2B2XGLambdapiGamma( name,
                             trkList,
                             lambdas,
                             photons,
                             MinPTCut,
                             MinMassCut,
                             MaxMassCut,
                             MaxCorrMCut,
                             VtxChi2DOFCut,
                             MinSumPtCut,
                             MinBPVDIRACut,
                             VtxMaxIPChi2Cut) :
    """
    Charmless B to pi Lambda gamma selection with missing mass
    """

    _B2B2XGLambdaPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2B2XGLambdaPreVertexCuts += " & (ASUM(SUMTREE(PT,(ISBASIC | (ID=='Lambda0')| (ID=='gamma')),0.0)) > %(MinSumPtCut)s )"%locals()

    _B2B2XGLambdaPostVertexCuts = hasHighTopoChild()
    _B2B2XGLambdaPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()
    _B2B2XGLambdaPostVertexCuts += " & (BPVIPCHI2() < %(VtxMaxIPChi2Cut)s )" %locals()
    _B2B2XGLambdaPostVertexCuts += " & (BPVDIRA > %(MinBPVDIRACut)s)"%locals()

    _combineB2B2XGLambdapi = CombineParticles( DecayDescriptor="[B+ -> Lambda0 gamma pi+]cc",
                                               MotherCut = _B2B2XGLambdaPostVertexCuts,
                                               CombinationCut = _B2B2XGLambdaPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2B2XGLambdapi,
                     RequiredSelections = [ lambdas, photons, trkList ] )


def makeB2B2XGLambda2piGamma( name,
                              diTrkList,
                              lambdas,
                              photons,
                              MinPTCut,
                              MinMassCut,
                              MaxMassCut,
                              MaxCorrMCut,
                              VtxChi2DOFCut,
                              MinSumPtCut,
                              MinBPVDIRACut,
                              VtxMaxIPChi2Cut) :
    """
    Charmless B to 2pi Lambda gamma selection with missing mass
    """

    _B2B2XGLambdaPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2B2XGLambdaPreVertexCuts += " & (ASUM(SUMTREE(PT,(ISBASIC | (ID=='Lambda0')| (ID=='rho(770)0')| (ID=='gamma')),0.0)) > %(MinSumPtCut)s )"%locals()

    _B2B2XGLambdaPostVertexCuts = hasHighTopoChild()
    _B2B2XGLambdaPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()
    _B2B2XGLambdaPostVertexCuts += " & (BPVIPCHI2() < %(VtxMaxIPChi2Cut)s )" %locals()
    _B2B2XGLambdaPostVertexCuts += " & (BPVDIRA > %(MinBPVDIRACut)s)"%locals()

    _combineB2B2XGLambda = CombineParticles( DecayDescriptor="B0 -> Lambda0 gamma rho(770)0",
                                             MotherCut = _B2B2XGLambdaPostVertexCuts,
                                             CombinationCut = _B2B2XGLambdaPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2B2XGLambda,
                     RequiredSelections = [ lambdas, photons, diTrkList ] )

def makeB2B2XGLambda3piGamma( name,
                              triTrkList,
                              lambdas,
                              photons,
                              MinPTCut,
                              MinMassCut,
                              MaxMassCut,
                              MaxCorrMCut,
                              VtxChi2DOFCut,
                              MinSumPtCut,
                              MinBPVDIRACut,
                              VtxMaxIPChi2Cut) :
    """
    Charmless B to 3pi Lambda gamma selection with missing mass
    """

    _B2B2XGLambdaPreVertexCuts = "in_range( %(MinMassCut)s ,AM, %(MaxMassCut)s )" %locals()
    _B2B2XGLambdaPreVertexCuts += " & (ASUM(SUMTREE(PT,(ISBASIC | (ID=='Lambda0')| (ID=='K_1(1270)+') | (ID=='gamma')),0.0)) > %(MinSumPtCut)s )"%locals()

    _B2B2XGLambdaPostVertexCuts = hasHighTopoChild()
    _B2B2XGLambdaPostVertexCuts += " & (VFASPF(VCHI2/VDOF) < %(VtxChi2DOFCut)s ) " %locals()
    _B2B2XGLambdaPostVertexCuts += " & (BPVIPCHI2() < %(VtxMaxIPChi2Cut)s )" %locals()
    _B2B2XGLambdaPostVertexCuts += " & (BPVDIRA > %(MinBPVDIRACut)s)"%locals()

    _combineB2B2XGLambda = CombineParticles( DecayDescriptor="[B+ -> Lambda0 gamma K_1(1270)+]cc",
                                             MotherCut = _B2B2XGLambdaPostVertexCuts,
                                             CombinationCut = _B2B2XGLambdaPreVertexCuts )

    return Selection(name,
                     Algorithm = _combineB2B2XGLambda,
                     RequiredSelections = [ lambdas, photons, triTrkList ] )

# def makeB2B2XGLbLambdaGamma( name,
#                              lambdas,
#                              photons,
#                              MinPTCut,
#                              MinMassCut,
#                              MaxMassCut,
#                              MaxCorrMCut,
#                              VtxChi2DOFCut,
#                              MinSumPtCut,
#                              MinBPVDIRACut,
#                              VtxMaxIPChi2Cut) :
#     """
#     Charmless Lambda_b0 to Lambda gamma selection with missing mass
#     """

#     _B2Lb2XGLambdaPreVertexCuts = "(ASUM(SUMTREE(PT,(ISBASIC | (ID=='Lambda0') | (ID=='gamma')),0.0)) > %(MinSumPtCut)s )"%locals()

#     _B2Lb2XGLambdaPostVertexCuts = "in_range( 0 ,M, 10000*MeV )"

#     _combineB2Lb2XGLambda = CombineParticles( DecayDescriptor="[Lambda_b0 -> Lambda0 gamma]cc",
#                                               MotherCut = _B2Lb2XGLambdaPostVertexCuts,
#                                               CombinationCut = _B2Lb2XGLambdaPreVertexCuts )

#     return Selection(name,
#                      Algorithm = _combineB2Lb2XGLambda,
#                      RequiredSelections = [ lambdas, photons ] )

def makeTISTOSFilter(name):
    specs = {'Hlt2Topo(2|3|4)Body.*Decision%TOS':0,
             'Hlt2Topo(2|3|4)Body.*Decision%TIS':0,
             'Hlt2IncPhi.*Decision%TOS':0,
             'Hlt2IncPhi.*Decision%TIS':0,
             'Hlt2RadiativeTopo.*Decision%TOS':0, ## Cut based raditive topological
             'Hlt2RadiativeTopo.*Decision%TIS':0, ## Cut based raditive topological
             'Hlt2TopoRad.*Decision%TOS':0, ## BBDT based radiative topological
             'Hlt2TopoRad.*Decision%TIS':0, ## BBDT based radiative topological
             'Hlt2Bs2PhiGamma.*Decision%TOS':0,
             'Hlt2Bs2PhiGamma.*Decision%TIS':0,
             'Hlt2Bd2KstGamma.*Decision%TOS':0,
             'Hlt2Bd2KstGamma.*Decision%TIS':0}
    from Configurables import TisTosParticleTagger
    tisTosFilter = TisTosParticleTagger(name+'TISTOSFilter')
    tisTosFilter.TisTosSpecs = specs
    #    tisTosFilter.ProjectTracksToCalo = False
    #    tisTosFilter.CaloClustForCharged = False
    #    tisTosFilter.CaloClustForNeutral = False
    #    tisTosFilter.TOSFrac = {4:0.0, 5:0.0}
    return tisTosFilter

def tisTosSelection(sel):
    '''Filters Selection sel to be TOS OR TIS.'''
    tisTosFilter = makeTISTOSFilter(sel.name())
#    tisTosFilter = FilterDesktop(Code = "ALL")
    return Selection(sel.name()+'TISTOS', Algorithm=tisTosFilter, RequiredSelections=[sel])




