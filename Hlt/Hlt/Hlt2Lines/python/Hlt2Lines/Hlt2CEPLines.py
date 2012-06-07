###
#
# Low multiplicity hadronic lines for CEP
#
###

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, mm

class Hlt2CEPLinesConf(HltLinesConfigurableUser) :

    __slots__ = { 'Prescale' : { 'Hlt2LowMultD0'        : 1.0
                                 , 'Hlt2LowMultDp'      : 1.0
                                 , 'Hlt2LowMultChiC'    : 1.0
                                 , 'Hlt2LowMultD0Inc'   : 1.0
                                 , 'Hlt2LowMultDpInc'   : 1.0
                                 , 'Hlt2LowMultChiCInc' : 1.0
                                 }
                  , 'Postscale' : { 'Hlt2LowMultD0'        : 1.0
                                    , 'Hlt2LowMultDp'      : 1.0
                                    , 'Hlt2LowMultChiC'    : 1.0
                                    , 'Hlt2LowMultD0Inc'   : 1.0
                                    , 'Hlt2LowMultDpInc'   : 1.0
                                    , 'Hlt2LowMultChiCInc' : 1.0
                                    }
                  # D0 -> KPi
                  , 'D2KPi_H_PTmin'       : 100.0 * MeV
                  , 'D2KPi_H_Pmin'        : 1000.0 * MeV
                  , 'D2KPi_H_TrkChi2max'  : 3.0
                  , 'D2KPi_K_PIDKmin'     : 0.0
                  , 'D2KPi_APTmin'        : 0.0 * MeV
                  , 'D2KPi_ADAMASSmax'    : 80.0 * MeV
                  , 'D2KPi_ADOCAmax'      : 0.5 * mm
                  , 'D2KPi_APmin'         : 1500.0 * MeV
                  , 'D2KPi_VtxChi2DoFmax' : 15.0
                  # D+- -> KPiPi
                  , 'D2KPiPi_H_PTmin'       : 100.0 * MeV
                  , 'D2KPiPi_H_Pmin'        : 1000.0 * MeV
                  , 'D2KPiPi_H_TrkChi2max'  : 3.0
                  , 'D2KPiPi_K_PIDKmin'     : 0.0
                  , 'D2KPiPi_APTmin'        : 0.0 * MeV
                  , 'D2KPiPi_ADAMASSmax'    : 80.0 * MeV
                  , 'D2KPiPi_ADOCAmax'      : 0.7 * mm
                  , 'D2KPiPi_APmin'         : 1500.0 * MeV
                  , 'D2KPiPi_VtxChi2DoFmax' : 15.0
                  # ChiC -> 4H
                  , 'ChiC2HHHH_H_PTmin'       : 100.0 * MeV
                  , 'ChiC2HHHH_H_Pmin'        : 1000.0 * MeV
                  , 'ChiC2HHHH_H_TrkChi2max'  : 3.0
                  , 'ChiC2HHHH_K_PIDKmin'     : 0.0
                  , 'ChiC2HHHH_APTmin'        : 0.0 * MeV
                  , 'ChiC2HHHH_AMmin'         : 2500.0 * MeV
                  , 'ChiC2HHHH_AMmax'         : 4500.0 * MeV
                  , 'ChiC2HHHH_ADOCAmax'      : 1.0 * mm
                  , 'ChiC2HHHH_APmin'         : 1500.0 * MeV
                  , 'ChiC2HHHH_VtxChi2DoFmax' : 15.0
                  # ANNSvc IDs
                  , 'HltANNSvcID' : { "D0"        : 50401
                                      , "Dp"      : 50402
                                      , "ChiC"    : 50403
                                      , "D0Inc"   : 50404
                                      , "DpInc"   : 50405
                                      , "ChiCInc" : 50406
                                      }
                  } 

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions, BiKalmanFittedRichKaons
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        from HltLine.HltDecodeRaw import DecodeL0CALO
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, FilterDesktop
        from Configurables import LoKi__VoidFilter as VoidFilter

        Kaons = BiKalmanFittedRichKaons
        Pions = BiKalmanFittedPions

        #
        #=== D0 -> KPi ===#
        #

        D2KPi_K_cut      = "(PT > %(D2KPi_H_PTmin)s) & (P > %(D2KPi_H_Pmin)s) & (TRCHI2DOF < %(D2KPi_H_TrkChi2max)s) & " \
                           "(PIDK > %(D2KPi_K_PIDKmin)s) & (ISLONG) " % self.getProps()
        D2KPi_Pi_cut     = "(PT > %(D2KPi_H_PTmin)s) & (P > %(D2KPi_H_Pmin)s) & (TRCHI2DOF < %(D2KPi_H_TrkChi2max)s) & (ISLONG)" % self.getProps()
        D2KPi_Daug_cuts  = { "K+"  : D2KPi_K_cut,
                             "K-"  : D2KPi_K_cut,
                             "pi+" : D2KPi_Pi_cut,
                             "pi-" : D2KPi_Pi_cut }
        D2KPi_Comb_cut   = "(APT > %(D2KPi_APTmin)s) & (ADAMASS('D0') < %(D2KPi_ADAMASSmax)s) & " \
                           "(ADOCA(1, 2) < %(D2KPi_ADOCAmax)s) & (AP > %(D2KPi_APmin)s)" % self.getProps()
        D2KPi_Mother_cut = "(VFASPF(VCHI2PDOF) < %(D2KPi_VtxChi2DoFmax)s)" % self.getProps()
        
        CombineD0 = Hlt2Member(CombineParticles
                               , "Combine"
                               , DecayDescriptors = [ "D0 -> K+ pi-",
                                                      "D0 -> K- pi+" ]
                               , DaughtersCuts = D2KPi_Daug_cuts
                               , CombinationCut = D2KPi_Comb_cut
                               , MotherCut = D2KPi_Mother_cut
                               , Inputs = [ Kaons, Pions ]
                               )

        D0 = bindMembers ("D0", [ Kaons ] + [ Pions ] + [ CombineD0 ])
        
        CombineD0Inc = Hlt2Member(CombineParticles
                               , "Combine"
                               , DecayDescriptors = [ "D0 -> K+ pi+",
                                                      "D0 -> K+ pi-",
                                                      "D0 -> K- pi+",
                                                      "D0 -> K- pi-" ]
                               , DaughtersCuts = D2KPi_Daug_cuts
                               , CombinationCut = D2KPi_Comb_cut
                               , MotherCut = D2KPi_Mother_cut
                               , Inputs = [ Kaons, Pions ]
                               )
        
        D0Inc = bindMembers ("D0Inc", [ Kaons ] + [ Pions ] + [ CombineD0Inc ])

        #
        #=== D+ -> KPiPi ===#
        #

        D2KPiPi_K_cut      = "(PT > %(D2KPiPi_H_PTmin)s) & (P > %(D2KPiPi_H_Pmin)s) & (TRCHI2DOF < %(D2KPiPi_H_TrkChi2max)s) & " \
                             "(PIDK > %(D2KPiPi_K_PIDKmin)s) & (ISLONG)" % self.getProps()
        D2KPiPi_Pi_cut     = "(PT > %(D2KPiPi_H_PTmin)s) & (P > %(D2KPiPi_H_Pmin)s) & (TRCHI2DOF < %(D2KPiPi_H_TrkChi2max)s) & (ISLONG)" % self.getProps()
        D2KPiPi_Daug_cuts  = { "K+"  : D2KPiPi_K_cut,
                               "K-"  : D2KPiPi_K_cut,
                               "pi+" : D2KPiPi_Pi_cut,
                               "pi-" : D2KPiPi_Pi_cut }
        D2KPiPi_Comb_cut   = "(APT > %(D2KPiPi_APTmin)s) & (ADAMASS('D+') < %(D2KPiPi_ADAMASSmax)s) & " \
                             "(ADOCAMAX('') < %(D2KPiPi_ADOCAmax)s) & (AP > %(D2KPiPi_APmin)s)" % self.getProps()
        D2KPiPi_Mother_cut = "(VFASPF(VCHI2PDOF) < %(D2KPiPi_VtxChi2DoFmax)s)" % self.getProps()
        
        CombineDp = Hlt2Member(CombineParticles
                               , "Combine"
                               , DecayDescriptors = [ "D+ -> K+ pi+ pi-",
                                                      "D+ -> K- pi+ pi+",
                                                      "D+ -> K- pi+ pi-",
                                                      "D+ -> K+ pi- pi-" ]
                               , DaughtersCuts = D2KPiPi_Daug_cuts
                               , CombinationCut = D2KPiPi_Comb_cut
                               , MotherCut = D2KPiPi_Mother_cut
                               , Inputs = [ Kaons, Pions ]
                               )
        
        Dp = bindMembers ("Dp", [ Kaons ] + [ Pions ] + [ CombineDp ])
        
        CombineDpInc = Hlt2Member(CombineParticles
                                  , "Combine"
                                  , DecayDescriptors = [ "D+ -> K+ pi+ pi+",
                                                         "D+ -> K+ pi+ pi-",
                                                         "D+ -> K+ pi- pi-",
                                                         "D+ -> K- pi+ pi+",
                                                         "D+ -> K- pi+ pi-",
                                                         "D+ -> K- pi- pi-" ]
                                  , DaughtersCuts = D2KPiPi_Daug_cuts
                                  , CombinationCut = D2KPiPi_Comb_cut
                                  , MotherCut = D2KPiPi_Mother_cut
                                  , Inputs = [ Kaons, Pions ]
                                  )
        
        DpInc = bindMembers ("DpInc", [ Kaons ] + [ Pions ] + [ CombineDpInc ])

        #
        #=== 'chi_c' -> KKpipi ===#
        #

        ChiC2HHHH_K_cut      = "(PT > %(ChiC2HHHH_H_PTmin)s) & (P > %(ChiC2HHHH_H_Pmin)s) & (TRCHI2DOF < %(ChiC2HHHH_H_TrkChi2max)s) & " \
                               "(PIDK > %(ChiC2HHHH_K_PIDKmin)s) & (ISLONG)" % self.getProps()
        ChiC2HHHH_Pi_cut     = "(PT > %(ChiC2HHHH_H_PTmin)s) & (P > %(ChiC2HHHH_H_Pmin)s) & (TRCHI2DOF < %(ChiC2HHHH_H_TrkChi2max)s) & (ISLONG)" % self.getProps()
        ChiC2HHHH_Daug_cuts  = { "K+"  : ChiC2HHHH_K_cut,
                                 "K-"  : ChiC2HHHH_K_cut,
                                 "pi+" : ChiC2HHHH_Pi_cut,
                                 "pi-" : ChiC2HHHH_Pi_cut }
        ChiC2HHHH_Comb_cut   = "(APT > %(ChiC2HHHH_APTmin)s) & (AM > %(ChiC2HHHH_AMmin)s) & (AM < %(ChiC2HHHH_AMmax)s) & " \
                               "(ADOCAMAX('') < %(ChiC2HHHH_ADOCAmax)s) & (AP > %(ChiC2HHHH_APmin)s)" % self.getProps()
        ChiC2HHHH_Mother_cut = "(VFASPF(VCHI2PDOF) < %(ChiC2HHHH_VtxChi2DoFmax)s)" % self.getProps()
        
        CombineChiC = Hlt2Member(CombineParticles
                                 , "Combine"
                                 , DecayDescriptors = [ "chi_c1(1P) -> K+ K+ pi- pi-",
                                                        "chi_c1(1P) -> K+ K- pi+ pi-",
                                                        "chi_c1(1P) -> K- K- pi+ pi+",
                                                        "chi_c1(1P) -> K+ K+ K- K-",
                                                        "chi_c1(1P) -> pi+ pi+ pi- pi-" ]
                                 , DaughtersCuts = ChiC2HHHH_Daug_cuts
                                 , CombinationCut = ChiC2HHHH_Comb_cut
                                 , MotherCut = ChiC2HHHH_Mother_cut
                                 , Inputs = [ Kaons, Pions ]
                                 )

        ChiC = bindMembers("ChiC", [ Kaons ] + [ Pions ] + [ CombineChiC ])

        CombineChiCInc = Hlt2Member(CombineParticles
                                    , "Combine"
                                    , DecayDescriptors = [ "chi_c1(1P) -> K+ K+ pi+ pi+",
                                                           "chi_c1(1P) -> K+ K+ pi+ pi-",
                                                           "chi_c1(1P) -> K+ K+ pi- pi-",
                                                           "chi_c1(1P) -> K+ K- pi+ pi+",
                                                           "chi_c1(1P) -> K+ K- pi+ pi-",
                                                           "chi_c1(1P) -> K+ K- pi- pi-",
                                                           "chi_c1(1P) -> K- K- pi+ pi+",
                                                           "chi_c1(1P) -> K- K- pi+ pi-",
                                                           "chi_c1(1P) -> K- K- pi- pi-",
                                                           "chi_c1(1P) -> K+ K+ K+ K+",
                                                           "chi_c1(1P) -> K+ K+ K+ K-",
                                                           "chi_c1(1P) -> K+ K+ K- K-",
                                                           "chi_c1(1P) -> K+ K- K- K-",
                                                           "chi_c1(1P) -> K- K- K- K-",
                                                           "chi_c1(1P) -> pi+ pi+ pi+ pi+",
                                                           "chi_c1(1P) -> pi+ pi+ pi+ pi-",
                                                           "chi_c1(1P) -> pi+ pi+ pi- pi-",
                                                           "chi_c1(1P) -> pi+ pi- pi- pi-",
                                                           "chi_c1(1P) -> pi- pi- pi- pi-" ]
                                    , DaughtersCuts = ChiC2HHHH_Daug_cuts
                                    , CombinationCut = ChiC2HHHH_Comb_cut
                                    , MotherCut = ChiC2HHHH_Mother_cut
                                    , Inputs = [ Kaons, Pions ]
                                    )
        
        ChiCInc = bindMembers("ChiCInc", [ Kaons ] + [ Pions ] + [ CombineChiCInc ])

        #
        #=== Filtering algorithms ===#
        #
        
        velotracks = Hlt2BiKalmanFittedForwardTracking().hlt2VeloTracking()
        
        FilterNumVeloTracks = VoidFilter('Hlt2LowMultHadronFilterNumVeloTracks', Code = "CONTAINS('" + velotracks.outputSelection() + "') < 12")
        
        FilterNumVeloTracksChiC = VoidFilter('Hlt2LowMultHadronFilterNumVeloTracksChiC', Code = "CONTAINS('" + velotracks.outputSelection() + "') < 8")
        
        FilterNumBackTracks = VoidFilter('Hlt2LowMultHadronFilterNumBackTracks', Code = "TrNUM('%s', TrBACKWARD) < 1" % velotracks.outputSelection())

        #
        #=== Set up lines ===#
        #

        HLTreq = "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')"

        L0req  = "L0_CHANNEL('DiHadron,lowMult')"
        
        line = Hlt2Line( 'LowMultD0'
                         , prescale = self.prescale
                         , HLT = HLTreq
                         , L0DU = L0req
                         , algos = [ FilterNumVeloTracks, FilterNumBackTracks, Kaons, Pions, D0 ]
                         , postscale = self.postscale
                         )

        line = Hlt2Line( 'LowMultDp'
                         , prescale = self.prescale
                         , HLT = HLTreq    
                         , L0DU = L0req
                         , algos = [ FilterNumVeloTracks, FilterNumBackTracks, Kaons, Pions, Dp ]
                         , postscale = self.postscale
                         )

        line = Hlt2Line( 'LowMultChiC'
                         , prescale = self.prescale
                         , HLT = HLTreq    
                         , L0DU = L0req
                         , algos = [ FilterNumVeloTracksChiC, FilterNumBackTracks, Kaons, Pions, ChiC ]
                         , postscale = self.postscale
                         )

        line = Hlt2Line( 'LowMultD0Inc'
                         , prescale = self.prescale
                         , HLT = HLTreq
                         , L0DU = L0req
                         , algos = [ FilterNumVeloTracks, FilterNumBackTracks, Kaons, Pions, D0Inc ]
                         , postscale = self.postscale
                         )

        line = Hlt2Line( 'LowMultDpInc'
                         , prescale = self.prescale
                         , HLT = HLTreq    
                         , L0DU = L0req
                         , algos = [ FilterNumVeloTracks, FilterNumBackTracks, Kaons, Pions, DpInc ]
                         , postscale = self.postscale
                         )

        line = Hlt2Line( 'LowMultChiCInc'
                         , prescale = self.prescale
                         , HLT = HLTreq    
                         , L0DU = L0req
                         , algos = [ FilterNumVeloTracksChiC, FilterNumBackTracks, Kaons, Pions, ChiCInc ]
                         , postscale = self.postscale
                         )

        #
        #=== Register with ANNSvc ===#
        #
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultD0Decision"      : self.getProp('HltANNSvcID')['D0'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultDpDecision"      : self.getProp('HltANNSvcID')['Dp'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultChiCDecision"    : self.getProp('HltANNSvcID')['ChiC'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultD0IncDecision"   : self.getProp('HltANNSvcID')['D0Inc'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultDpIncDecision"   : self.getProp('HltANNSvcID')['DpInc'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultChiCIncDecision" : self.getProp('HltANNSvcID')['ChiCInc'] } )
