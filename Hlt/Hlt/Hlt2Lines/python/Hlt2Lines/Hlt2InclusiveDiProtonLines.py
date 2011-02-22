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

                  , 'SpdMult'          :   600
                  , 'ProtonPT'         :  1800.   # MeV
                  , 'CombUpperMass'    :  4000.   # MeV, before Vtx fit
                  , 'CombLowerMass'    :  2800.   # MeV, before Vtx fit
                  , 'UpperMass'        :  3950.   # MeV, after Vtx fit
                  , 'LowerMass'        :  2850.   # MeV, after Vtx fit
                  , 'VtxCHI2'          :    16.   # dimensionless
                  # Track Fitted
                  , 'TFProtonPT'         :  1900.   # MeV
                  , 'TFProtonTrkChi2'    :     4.   
                  , 'TFCombUpperMass'    :  3950.   # MeV, before Vtx fit
                  , 'TFCombLowerMass'    :  2850.   # MeV, before Vtx fit
                  , 'TFUpperMass'        :  3900.   # MeV, after Vtx fit
                  , 'TFLowerMass'        :  2900.   # MeV, after Vtx fit
                  , 'TFVtxCHI2'          :     9.   # dimensionless
                  # Track Fitted & RichPID
                  , 'TFRichProtonPIDppi'   :  10.   # CombDLL(p-pi)
                  , 'TFRichProtonPIDpK'    :   5.   # CombDLL(p-K)
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
        CombCut = "(in_range( %(CombLowerMass)s *MeV, AM, %(CombUpperMass)s *MeV))" % self.getProps()
        MomCut = "(VFASPF(VCHI2PDOF)< %(VtxCHI2)s) & (in_range(%(LowerMass)s *MeV, MM, %(UpperMass)s *MeV))" % self.getProps()

        from Hlt2SharedParticles.BasicParticles import NoCutsProtons
        Combine = Hlt2Member( CombineParticles
                              , "Combine"
                              , DecayDescriptor = "J/psi(1S) -> p+ p~-"
                              , DaughtersCuts = { "p+" : ProtonCut }
                              , CombinationCut = CombCut
                              , MotherCut = MomCut
                              , Inputs = [ NoCutsProtons ]
                              , InputPrimaryVertices = "None"
                              , UseP2PVRelations = False
                              )

        #------------------------------------
        # Track Fitted
        #------------------------------------
        TFProtonCut = "(PT> %(TFProtonPT)s *MeV) & (TRCHI2DOF < %(TFProtonTrkChi2)s)" % self.getProps()
        TFCombCut = "(in_range( %(TFCombLowerMass)s *MeV, AM, %(TFCombUpperMass)s *MeV))" % self.getProps()
        TFMomCut = "(VFASPF(VCHI2PDOF)< %(TFVtxCHI2)s) & (in_range( %(TFLowerMass)s *MeV, MM, %(TFUpperMass)s *MeV))" % self.getProps()
        
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedProtons
        TFCombine = Hlt2Member( CombineParticles
                                , "TFCombine"
                                , DecayDescriptor = "J/psi(1S) -> p+ p~-"
                                , DaughtersCuts = { "p+" : TFProtonCut }
                                , CombinationCut = TFCombCut
                                , MotherCut = TFMomCut
                                , Inputs = [ BiKalmanFittedProtons ]
                                , InputPrimaryVertices = "None"
                                , UseP2PVRelations = False
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
                                    , Inputs = [ BiKalmanFittedRichProtons ]
                                    , InputPrimaryVertices = "None"
                                    , UseP2PVRelations = False
                                    )

        #------------------------------------
        # Inclusive DiProton complete
        #------------------------------------
        line = Hlt2Line('IncDiProton'
                        , prescale = self.prescale
                        , L0DU = "(L0_DATA('Spd(Mult)') < %(SpdMult)s )" % self.getProps()   
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
                        , L0DU = "(L0_DATA('Spd(Mult)') < %(SpdMult)s )" % self.getProps()   
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
                        , L0DU = "(L0_DATA('Spd(Mult)') < %(SpdMult)s )" % self.getProps()   
                        , algos = [ NoCutsProtons
                                    , Combine
                                    , BiKalmanFittedProtons
                                    , TFCombine
                                    ]
                        , postscale = self.postscale
                        )       
