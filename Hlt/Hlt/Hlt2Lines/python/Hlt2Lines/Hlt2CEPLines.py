###
#
# Low multiplicity hadronic lines for CEP
#
###

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, mm

class Hlt2CEPLinesConf(HltLinesConfigurableUser) :

    __slots__ = { 'Prescale' : { 'Hlt2LowMultD2KPi'         : 1.0
                                 , 'Hlt2LowMultD2KPiPi'     : 1.0
                                 , 'Hlt2LowMultD2K3Pi'      : 1.0
                                 , 'Hlt2LowMultChiC2HH'     : 1.0
                                 , 'Hlt2LowMultChiC2HHHH'   : 1.0
                                 , 'Hlt2LowMultD2KPiWS'     : 0.1
                                 , 'Hlt2LowMultD2KPiPiWS'   : 0.1
                                 , 'Hlt2LowMultD2K3PiWS'    : 0.1
                                 , 'Hlt2LowMultChiC2HHWS'   : 0.1
                                 , 'Hlt2LowMultChiC2HHHHWS' : 0.1
                                 }
                  , 'Postscale' : { 'Hlt2LowMultD2KPi'         : 1.0
                                    , 'Hlt2LowMultD2KPiPi'     : 1.0
                                    , 'Hlt2LowMultD2K3Pi'      : 1.0
                                    , 'Hlt2LowMultChiC2HH'     : 1.0
                                    , 'Hlt2LowMultChiC2HHHH'   : 1.0
                                    , 'Hlt2LowMultD2KPiWS'     : 1.0
                                    , 'Hlt2LowMultD2KPiPiWS'   : 1.0
                                    , 'Hlt2LowMultD2K3PiWS'    : 1.0
                                    , 'Hlt2LowMultChiC2HHWS'   : 1.0
                                    , 'Hlt2LowMultChiC2HHHHWS' : 1.0
                                    }
                  # Final-state particles
                  , 'H_PTmin'       : 100.0 * MeV
                  , 'H_Pmin'        : 5000.0 * MeV
                  , 'H_TrkChi2max'  : 3.0
                  , 'K_PIDKmin'     : 0.0
                  # D0 -> KPi
                  , 'D2KPi_APTmin'        : 0.0 * MeV
                  , 'D2KPi_ADAMASSmax'    : 80.0 * MeV
                  , 'D2KPi_ADOCAmax'      : 0.5 * mm
                  , 'D2KPi_APmin'         : 5000.0 * MeV
                  , 'D2KPi_SSumPTmin'     : 200.0 * MeV
                  , 'D2KPi_VtxChi2DoFmax' : 15.0
                  # D+- -> KPiPi
                  , 'D2KPiPi_APTmin'        : 0.0 * MeV
                  , 'D2KPiPi_ADAMASSmax'    : 80.0 * MeV
                  , 'D2KPiPi_ADOCAmax'      : 0.5 * mm
                  , 'D2KPiPi_APmin'         : 5000.0 * MeV
                  , 'D2KPiPi_SSumPTmin'     : 300.0 * MeV
                  , 'D2KPiPi_VtxChi2DoFmax' : 15.0
                  # D0 -> K3Pi
                  , 'D2K3Pi_APTmin'        : 0.0 * MeV
                  , 'D2K3Pi_ADAMASSmax'    : 80.0 * MeV
                  , 'D2K3Pi_ADOCAmax'      : 0.7 * mm
                  , 'D2K3Pi_APmin'         : 5000.0 * MeV
                  , 'D2K3Pi_SSumPTmin'     : 400.0 * MeV
                  , 'D2K3Pi_VtxChi2DoFmax' : 15.0
                  # ChiC -> HH
                  , 'ChiC2HH_APTmin'        : 0.0 * MeV
                  , 'ChiC2HH_APTmax'        : 5000.0 * MeV
                  , 'ChiC2HH_ADAMASSmax'    : 120.0 * MeV
                  , 'ChiC2HH_ADOCAmax'      : 0.5 * mm
                  , 'ChiC2HH_APmin'         : 5000.0 * MeV
                  , 'ChiC2HH_SSumPTmin'     : 200.0 * MeV
                  , 'ChiC2HH_VtxChi2DoFmax' : 15.0
                  # ChiC -> HHHH
                  , 'ChiC2HHHH_APTmin'        : 0.0 * MeV
                  , 'ChiC2HHHH_APTmax'        : 5000.0 * MeV
                  , 'ChiC2HHHH_AMmin'         : 2850.0 * MeV
                  , 'ChiC2HHHH_AMmax'         : 4500.0 * MeV
                  , 'ChiC2HHHH_ADOCAmax'      : 0.7 * mm
                  , 'ChiC2HHHH_APmin'         : 5000.0 * MeV
                  , 'ChiC2HHHH_SSumPTmin'     : 400.0 * MeV
                  , 'ChiC2HHHH_VtxChi2DoFmax' : 15.0
                  # ANNSvc IDs
                  , 'HltANNSvcID' : { "D2KPi"         : 50401
                                      , "D2KPiPi"     : 50402
                                      , "D2K3Pi"      : 50403
                                      , "ChiC2HH"     : 50404
                                      , "ChiC2HHHH"   : 50405
                                      , "D2KPiWS"     : 50406
                                      , "D2KPiPiWS"   : 50407
                                      , "D2K3PiWS"    : 50408
                                      , "ChiC2HHWS"   : 50409
                                      , "ChiC2HHHHWS" : 50410
                                      }
                  } 

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions, BiKalmanFittedRichKaons
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedRichLowPTKaons 
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        from HltLine.HltDecodeRaw import DecodeL0CALO
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, FilterDesktop
        from Configurables import LoKi__VoidFilter as VoidFilter

        Kaons = BiKalmanFittedRichLowPTKaons
        Pions = BiKalmanFittedPions

        #
        #=== Final-state particles ===#
        #

        K_cut  = "(PT > %(H_PTmin)s) & (P > %(H_Pmin)s) & (TRCHI2DOF < %(H_TrkChi2max)s) & (PIDK > %(K_PIDKmin)s)" % self.getProps()
        Pi_cut = "(PT > %(H_PTmin)s) & (P > %(H_Pmin)s) & (TRCHI2DOF < %(H_TrkChi2max)s)" % self.getProps()
        Daug_cuts = {"K+" : K_cut, "K-" : K_cut, "pi+" : Pi_cut, "pi-" : Pi_cut}
        
        #
        #=== D0 -> KPi ===#
        #

        D2KPi_Comb_cut   = "(APT > %(D2KPi_APTmin)s) & (ADAMASS('D0') < %(D2KPi_ADAMASSmax)s) & (ADOCAMAX('LoKi::DistanceCalculator') < %(D2KPi_ADOCAmax)s) & " \
                           "(AP > %(D2KPi_APmin)s) & (ACHILD(PT, 1) + ACHILD(PT, 2) > %(D2KPi_SSumPTmin)s)" % self.getProps()
        D2KPi_Mother_cut = "(VFASPF(VCHI2PDOF) < %(D2KPi_VtxChi2DoFmax)s)" % self.getProps()
        
        CombineD2KPi = Hlt2Member(CombineParticles
                                  , "Combine"
                                  , DecayDescriptors = [ "[D0 -> K- pi+]cc" ]
                                  , DaughtersCuts = Daug_cuts
                                  , CombinationCut = D2KPi_Comb_cut
                                  , MotherCut = D2KPi_Mother_cut
                                  , Inputs = [ Kaons, Pions ]
                                  )
        
        D2KPi = bindMembers ("D2KPi", [ Kaons ] + [ Pions ] + [ CombineD2KPi ])
        
        CombineD2KPiWS = Hlt2Member(CombineParticles
                                    , "Combine"
                                    , DecayDescriptors = [ "[D0 -> K+ pi+]cc" ]
                                    , DaughtersCuts = Daug_cuts
                                    , CombinationCut = D2KPi_Comb_cut
                                    , MotherCut = D2KPi_Mother_cut
                                    , Inputs = [ Kaons, Pions ]
                                    )
        
        D2KPiWS = bindMembers ("D2KPiWS", [ Kaons ] + [ Pions ] + [ CombineD2KPiWS ])

        #
        #=== D+ -> KPiPi ===#
        #

        D2KPiPi_Comb_cut   = "(APT > %(D2KPiPi_APTmin)s) & (ADAMASS('D+') < %(D2KPiPi_ADAMASSmax)s) & (ADOCAMAX('LoKi::DistanceCalculator') < %(D2KPiPi_ADOCAmax)s) & " \
                             "(AP > %(D2KPiPi_APmin)s) & (ACHILD(PT, 1) + ACHILD(PT, 2) + ACHILD(PT, 3) > %(D2KPiPi_SSumPTmin)s)" % self.getProps()
        D2KPiPi_Mother_cut = "(VFASPF(VCHI2PDOF) < %(D2KPiPi_VtxChi2DoFmax)s)" % self.getProps()
        
        CombineD2KPiPi = Hlt2Member(CombineParticles
                                    , "Combine"
                                    , DecayDescriptors = [ "[D+ -> K+ pi+ pi-]cc",
                                                           "[D+ -> K- pi+ pi+]cc" ]
                                    , DaughtersCuts = Daug_cuts
                                    , CombinationCut = D2KPiPi_Comb_cut
                                    , MotherCut = D2KPiPi_Mother_cut
                                    , Inputs = [ Kaons, Pions ]
                                    )
        
        D2KPiPi = bindMembers ("D2KPiPi", [ Kaons ] + [ Pions ] + [ CombineD2KPiPi ])
        
        CombineD2KPiPiWS = Hlt2Member(CombineParticles
                                      , "Combine"
                                      , DecayDescriptors = [ "[D+ -> K+ pi+ pi+]cc" ]
                                      , DaughtersCuts = Daug_cuts
                                      , CombinationCut = D2KPiPi_Comb_cut
                                      , MotherCut = D2KPiPi_Mother_cut
                                      , Inputs = [ Kaons, Pions ]
                                      )
        
        D2KPiPiWS = bindMembers ("D2KPiPiWS", [ Kaons ] + [ Pions ] + [ CombineD2KPiPiWS ])

        #
        #=== D0 -> K3Pi ===#
        #

        D2K3Pi_Comb_cut   = "(APT > %(D2K3Pi_APTmin)s) & (ADAMASS('D0') < %(D2K3Pi_ADAMASSmax)s) & (ADOCAMAX('LoKi::DistanceCalculator') < %(D2K3Pi_ADOCAmax)s) & " \
                            "(AP > %(D2K3Pi_APmin)s) & (ACHILD(PT, 1) + ACHILD(PT, 2) + ACHILD(PT, 3) + ACHILD(PT, 4) > %(D2K3Pi_SSumPTmin)s)" % self.getProps()
        D2K3Pi_Mother_cut = "(VFASPF(VCHI2PDOF) < %(D2K3Pi_VtxChi2DoFmax)s)" % self.getProps()
        
        CombineD2K3Pi = Hlt2Member(CombineParticles
                                   , "Combine"
                                   , DecayDescriptors = [ "[D0 -> K- pi+ pi- pi+]cc" ]
                                   , DaughtersCuts = Daug_cuts
                                   , CombinationCut = D2K3Pi_Comb_cut
                                   , MotherCut = D2K3Pi_Mother_cut
                                   , Inputs = [ Kaons, Pions ]
                                   )
        
        D2K3Pi = bindMembers ("D2K3Pi", [ Kaons ] + [ Pions ] + [ CombineD2K3Pi ])
        
        CombineD2K3PiWS = Hlt2Member(CombineParticles
                                     , "Combine"
                                     , DecayDescriptors = [ "[D0 -> K+ pi+ pi+ pi+]cc",
                                                            "[D0 -> K+ pi+ pi+ pi-]cc",
                                                            "[D0 -> K+ pi- pi- pi-]cc" ]
                                     , DaughtersCuts = Daug_cuts
                                     , CombinationCut = D2K3Pi_Comb_cut
                                     , MotherCut = D2K3Pi_Mother_cut
                                     , Inputs = [ Kaons, Pions ]
                                     )
        
        D2K3PiWS = bindMembers ("D2K3PiWS", [ Kaons ] + [ Pions ] + [ CombineD2K3PiWS ])

        #
        #=== 'chi_c' -> KK, pipi ===#
        #

        ChiC2HH_Comb_cut   = "(APT > %(ChiC2HH_APTmin)s) & (APT < %(ChiC2HH_APTmax)s) & (ADAMASS('chi_c1(1P)') < %(ChiC2HH_ADAMASSmax)s) & " \
                             "(ADOCAMAX('LoKi::DistanceCalculator') < %(ChiC2HH_ADOCAmax)s) & " \
                             "(AP > %(ChiC2HH_APmin)s) & (ACHILD(PT, 1) + ACHILD(PT, 2) > %(ChiC2HH_SSumPTmin)s)" % self.getProps()
        ChiC2HH_Mother_cut = "(VFASPF(VCHI2PDOF) < %(ChiC2HH_VtxChi2DoFmax)s)" % self.getProps()
        
        CombineChiC2HH = Hlt2Member(CombineParticles
                                    , "Combine"
                                    , DecayDescriptors = [ "chi_c1(1P) -> K+ K-",
                                                           "chi_c1(1P) -> pi+ pi-" ]
                                    , DaughtersCuts = Daug_cuts
                                    , CombinationCut = ChiC2HH_Comb_cut
                                    , MotherCut = ChiC2HH_Mother_cut
                                    , Inputs = [ Kaons, Pions ]
                                    )
        
        ChiC2HH = bindMembers("ChiC2HH", [ Kaons ] + [ Pions ] + [ CombineChiC2HH ])

        CombineChiC2HHWS = Hlt2Member(CombineParticles
                                      , "Combine"
                                      , DecayDescriptors = [ "[chi_c1(1P) -> K+ K+]cc",
                                                             "[chi_c1(1P) -> pi+ pi+]cc" ]
                                      , DaughtersCuts = Daug_cuts
                                      , CombinationCut = ChiC2HH_Comb_cut
                                      , MotherCut = ChiC2HH_Mother_cut
                                      , Inputs = [ Kaons, Pions ]
                                      )
        
        ChiC2HHWS = bindMembers("ChiC2HHWS", [ Kaons ] + [ Pions ] + [ CombineChiC2HHWS ])
        
        #
        #=== 'chi_c' -> KKpipi ===#
        #

        ChiC2HHHH_Comb_cut   = "(APT > %(ChiC2HHHH_APTmin)s) & (APT < %(ChiC2HHHH_APTmax)s) & (AM > %(ChiC2HHHH_AMmin)s) & (AM < %(ChiC2HHHH_AMmax)s) & " \
                               "(ADOCAMAX('LoKi::DistanceCalculator') < %(ChiC2HHHH_ADOCAmax)s) & " \
                               "(AP > %(ChiC2HHHH_APmin)s) & (ACHILD(PT, 1) + ACHILD(PT, 2) + ACHILD(PT, 3) + ACHILD(PT, 4) > %(ChiC2HHHH_SSumPTmin)s)" % self.getProps()
        ChiC2HHHH_Mother_cut = "(VFASPF(VCHI2PDOF) < %(ChiC2HHHH_VtxChi2DoFmax)s)" % self.getProps()
        
        CombineChiC2HHHH = Hlt2Member(CombineParticles
                                      , "Combine"
                                      , DecayDescriptors = [ "[chi_c1(1P) -> K+ K+ pi- pi-]cc",
                                                             "chi_c1(1P) -> K+ K- pi+ pi-",
                                                             "chi_c1(1P) -> K+ K+ K- K-",
                                                             "chi_c1(1P) -> pi+ pi+ pi- pi-" ]
                                      , DaughtersCuts = Daug_cuts
                                      , CombinationCut = ChiC2HHHH_Comb_cut
                                      , MotherCut = ChiC2HHHH_Mother_cut
                                      , Inputs = [ Kaons, Pions ]
                                      )

        ChiC2HHHH = bindMembers("ChiC2HHHH", [ Kaons ] + [ Pions ] + [ CombineChiC2HHHH ])

        CombineChiC2HHHHWS = Hlt2Member(CombineParticles
                                        , "Combine"
                                        , DecayDescriptors = [ "[chi_c1(1P) -> K+ K+ pi+ pi+]cc",
                                                               "[chi_c1(1P) -> K+ K+ pi+ pi-]cc",
                                                               "[chi_c1(1P) -> K+ K- pi+ pi+]cc",
                                                               "[chi_c1(1P) -> K+ K+ K+ K+]cc",
                                                               "[chi_c1(1P) -> K+ K+ K+ K-]cc",
                                                               "[chi_c1(1P) -> pi+ pi+ pi+ pi+]cc",
                                                               "[chi_c1(1P) -> pi+ pi+ pi+ pi-]cc" ]
                                        , DaughtersCuts = Daug_cuts
                                        , CombinationCut = ChiC2HHHH_Comb_cut
                                        , MotherCut = ChiC2HHHH_Mother_cut
                                        , Inputs = [ Kaons, Pions ]
                                        )
        
        ChiC2HHHHWS = bindMembers("ChiC2HHHHWS", [ Kaons ] + [ Pions ] + [ CombineChiC2HHHHWS ])

        #
        #=== Filtering algorithms ===#
        #
        
        velotracks = Hlt2BiKalmanFittedForwardTracking().hlt2VeloTracking()
        VT = velotracks.outputSelection()
        
        FilterNumVeloTracks          = VoidFilter('Hlt2LowMultHadronFilterNumVeloTracks',          Code = "CONTAINS('%s') < 12" % VT)
        FilterNumVeloTracksChiC2HH   = VoidFilter('Hlt2LowMultHadronFilterNumVeloTracksChiC2HH',   Code = "CONTAINS('%s') < 6"  % VT)
        FilterNumVeloTracksChiC2HHHH = VoidFilter('Hlt2LowMultHadronFilterNumVeloTracksChiC2HHHH', Code = "CONTAINS('%s') < 8"  % VT)
        FilterNumBackTracks          = VoidFilter('Hlt2LowMultHadronFilterNumBackTracks',          Code = "TrNUM('%s', TrBACKWARD) < 1" % VT)

        #
        #=== Set up lines ===#
        #

        HLTreq = "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')"
        L0req  = "L0_CHANNEL('DiHadron,lowMult')"
        
        line = Hlt2Line( 'LowMultD2KPi'
                         , prescale = self.prescale
                         , HLT = HLTreq
                         , L0DU = L0req
                         , algos = [ velotracks, FilterNumVeloTracks, FilterNumBackTracks, Kaons, Pions, D2KPi ]
                         , postscale = self.postscale
                         )

        line = Hlt2Line( 'LowMultD2KPiPi'
                         , prescale = self.prescale
                         , HLT = HLTreq    
                         , L0DU = L0req
                         , algos = [ velotracks, FilterNumVeloTracks, FilterNumBackTracks, Kaons, Pions, D2KPiPi ]
                         , postscale = self.postscale
                         )

        line = Hlt2Line( 'LowMultD2K3Pi'
                         , prescale = self.prescale
                         , HLT = HLTreq
                         , L0DU = L0req
                         , algos = [ velotracks, FilterNumVeloTracks, FilterNumBackTracks, Kaons, Pions, D2K3Pi ]
                         , postscale = self.postscale
                         )

        line = Hlt2Line( 'LowMultChiC2HH'
                         , prescale = self.prescale
                         , HLT = HLTreq    
                         , L0DU = L0req
                         , algos = [ velotracks, FilterNumVeloTracksChiC2HH, FilterNumBackTracks, Kaons, Pions, ChiC2HH ]
                         , postscale = self.postscale
                         )

        line = Hlt2Line( 'LowMultChiC2HHHH'
                         , prescale = self.prescale
                         , HLT = HLTreq    
                         , L0DU = L0req
                         , algos = [ velotracks, FilterNumVeloTracksChiC2HHHH, FilterNumBackTracks, Kaons, Pions, ChiC2HHHH ]
                         , postscale = self.postscale
                         )

        line = Hlt2Line( 'LowMultD2KPiWS'
                         , prescale = self.prescale
                         , HLT = HLTreq
                         , L0DU = L0req
                         , algos = [ velotracks, FilterNumVeloTracks, FilterNumBackTracks, Kaons, Pions, D2KPiWS ]
                         , postscale = self.postscale
                         )

        line = Hlt2Line( 'LowMultD2KPiPiWS'
                         , prescale = self.prescale
                         , HLT = HLTreq    
                         , L0DU = L0req
                         , algos = [ velotracks, FilterNumVeloTracks, FilterNumBackTracks, Kaons, Pions, D2KPiPiWS ]
                         , postscale = self.postscale
                         )

        line = Hlt2Line( 'LowMultD2K3PiWS'
                         , prescale = self.prescale
                         , HLT = HLTreq
                         , L0DU = L0req
                         , algos = [ velotracks, FilterNumVeloTracks, FilterNumBackTracks, Kaons, Pions, D2K3PiWS ]
                         , postscale = self.postscale
                         )

        line = Hlt2Line( 'LowMultChiC2HHWS'
                         , prescale = self.prescale
                         , HLT = HLTreq    
                         , L0DU = L0req
                         , algos = [ velotracks, FilterNumVeloTracksChiC2HH, FilterNumBackTracks, Kaons, Pions, ChiC2HHWS ]
                         , postscale = self.postscale
                         )

        line = Hlt2Line( 'LowMultChiC2HHHHWS'
                         , prescale = self.prescale
                         , HLT = HLTreq    
                         , L0DU = L0req
                         , algos = [ velotracks, FilterNumVeloTracksChiC2HHHH, FilterNumBackTracks, Kaons, Pions, ChiC2HHHHWS ]
                         , postscale = self.postscale
                         )

        #
        #=== Register with ANNSvc ===#
        #
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultD2KPiDecision"       : self.getProp('HltANNSvcID')['D2KPi'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultD2KPiPiDecision"     : self.getProp('HltANNSvcID')['D2KPiPi'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultD2K3PiDecision"      : self.getProp('HltANNSvcID')['D2K3Pi'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultChiC2HHDecision"     : self.getProp('HltANNSvcID')['ChiC2HH'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultChiC2HHHHDecision"   : self.getProp('HltANNSvcID')['ChiC2HHHH'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultD2KPiWSDecision"     : self.getProp('HltANNSvcID')['D2KPiWS'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultD2KPiPiWSDecision"   : self.getProp('HltANNSvcID')['D2KPiPiWS'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultD2K3PiWSDecision"    : self.getProp('HltANNSvcID')['D2K3PiWS'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultChiC2HHWSDecision"   : self.getProp('HltANNSvcID')['ChiC2HHWS'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultChiC2HHHHWSDecision" : self.getProp('HltANNSvcID')['ChiC2HHHHWS'] } )
