###
#
#  Hlt2 Charmonium -> p pbar
#  @ author Serezha.Barsuk@cern.ch
#  @ author Jibo.He@cern.ch
#  @ date 2010-03-19
#
###

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2InclusiveDiProtonLinesConf(HltLinesConfigurableUser):
    """

    """

    __slots__ = { 'Prescale'           : {  'Hlt2IncDiProton'             : 1.  
                                           ,'Hlt2IncDiProtonPresel'       : 0.001  # of 1000
                                           ,'Hlt2IncDiProtonTrackFitted'  : 0.005  # of 200  
                                           }
                  
                  ,'Postscale'         : {  'Hlt2IncDiProton'             : 1.
                                           ,'Hlt2IncDiProtonPresel'       : 1.
                                           ,'Hlt2IncDiProtonTrackFitted'  : 1.  
                                           }
                  
                  , 'ProtonPT'         :  1600.   # MeV
                  , 'CombUpperMass'    :  4000.   # MeV, before Vtx fit
                  , 'CombLowerMass'    :  2700.   # MeV, before Vtx fit
                  , 'UpperMass'        :  3950.   # MeV, after Vtx fit
                  , 'LowerMass'        :  2750.   # MeV, after Vtx fit
                  , 'VtxCHI2'          :    36.   # dimensionless
                  # Track Fitted
                  , 'TFProtonPT'         :  1800.   # MeV
                  , 'TFCombUpperMass'    :  3950.   # MeV, before Vtx fit
                  , 'TFCombLowerMass'    :  2750.   # MeV, before Vtx fit
                  , 'TFUpperMass'        :  3900.   # MeV, after Vtx fit
                  , 'TFLowerMass'        :  2800.   # MeV, after Vtx fit
                  , 'TFVtxCHI2'          :    25.   # dimensionless
                  # Track Fitted & RichPID
                  , 'TFRichProtonPIDppi'   :   2.   # CombDLL(p-pi)
                  , 'TFRichProtonPIDpK'    :   2.   # CombDLL(p-K)
                  , 'HltANNSvcID'      : { 'IncDiProton'             : 51000
                                           ,'IncDiProtonPresel'      : 51001
                                           ,'IncDiProtonTrackFitted' : 51002 
                                           }
                  }

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop, CombineParticles

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncDiProtonDecision" : self.getProp('HltANNSvcID')['IncDiProton'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncDiProtonPreselDecision" : self.getProp('HltANNSvcID')['IncDiProtonPresel'] } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncDiProtonTrackFittedDecision" : self.getProp('HltANNSvcID')['IncDiProtonTrackFitted'] } )

        #------------------------------------
        # Preselection 
        #------------------------------------
        ProtonCut = "(PT> %(ProtonPT)s *MeV)" % self.getProps()
        CombCut = "(%(CombLowerMass)s *MeV < AM) & (AM < %(CombUpperMass)s *MeV)" % self.getProps()
        MomCut = "(VFASPF(VCHI2/VDOF)< %(VtxCHI2)s) & (%(LowerMass)s *MeV < MM) & (MM < %(UpperMass)s *MeV)" % self.getProps()

        from Hlt2SharedParticles.BasicParticles import NoCutsProtons
        Combine = Hlt2Member( CombineParticles
                              , "Combine"
                              , DecayDescriptor = "J/psi(1S) -> p+ p~-"
                              , DaughtersCuts = { "p+" : ProtonCut }
                              , CombinationCut = CombCut
                              , MotherCut = MomCut
                              , InputLocations = [ NoCutsProtons ]
                              )

        #------------------------------------
        # Track Fitted
        #------------------------------------
        TFProtonCut = "(PT> %(TFProtonPT)s *MeV)" % self.getProps()
        TFCombCut = "(%(TFCombLowerMass)s *MeV < AM) & (AM < %(TFCombUpperMass)s *MeV)" % self.getProps()
        TFMomCut = "(VFASPF(VCHI2/VDOF)< %(TFVtxCHI2)s) & (%(TFLowerMass)s *MeV < MM) & (MM < %(TFUpperMass)s *MeV)" % self.getProps()
        
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedProtons
        TFCombine = Hlt2Member( CombineParticles
                                , "TFCombine"
                                , DecayDescriptor = "J/psi(1S) -> p+ p~-"
                                , DaughtersCuts = { "p+" : TFProtonCut }
                                , CombinationCut = TFCombCut
                                , MotherCut = TFMomCut
                                , InputLocations = [ BiKalmanFittedProtons ]
                                )

        #------------------------------------
        # Track Fitted & RichPID
        #------------------------------------
        TFRichProtonPID = "((PIDp-PIDpi) > %(TFRichProtonPIDppi)s) & ((PIDp-PIDK) > %(TFRichProtonPIDpK)s)" % self.getProps()   
        
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedRichProtons
        TFRichCombine = Hlt2Member( CombineParticles
                                    , "TFRichCombine"
                                    , DecayDescriptor = "J/psi(1S) -> p+ p~-"
                                    , DaughtersCuts = { "p+" : TFProtonCut+" & "+TFRichProtonPID }
                                    , CombinationCut = TFCombCut
                                    , MotherCut = TFMomCut
                                    , InputLocations = [ BiKalmanFittedProtons ]
                                )

        #------------------------------------
        # Inclusive DiProton complete
        #------------------------------------
        line = Hlt2Line('IncDiProton'
                        , prescale = self.prescale
                        , algos = [ NoCutsProtons
                                    , Combine
                                    , BiKalmanFittedProtons
                                    , TFCombine
                                    , BiKalmanFittedRichProtons
                                    , TFRichCombine
                                    ]
                        , postscale = self.postscale
                        )

        #------------------------------------
        # Inclusive DiProton presel
        #------------------------------------        
        line = Hlt2Line('IncDiProtonPresel'
                        , prescale = self.prescale
                        , algos = [ NoCutsProtons
                                    , Combine
                                    ]
                        , postscale = self.postscale
                        )

        #------------------------------------
        # Inclusive DiProton presel & TrackFitted
        #------------------------------------
        line = Hlt2Line('IncDiProtonTrackFitted'
                        , prescale = self.prescale
                        , algos = [ NoCutsProtons
                                    , Combine
                                    , BiKalmanFittedProtons
                                    , TFCombine
                                    ]
                        , postscale = self.postscale
                        )        
        
        
