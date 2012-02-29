###
#
#  Hlt2 Charmonium -> Phi Phi
#  @ author Serezha.Barsuk@cern.ch
#  @ author Jibo.He@cern.ch
#  @ date 2010-03-19
#
###

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2InclusiveDiPhiLinesConf(HltLinesConfigurableUser):
    """
    
    """
    
    __slots__ = { 'Prescale'           : {    'Hlt2DiPhiTis'                   : 1.  
                                              }
                  
                  ,'Postscale'         : {    'Hlt2DiPhiTis'                   : 1.  
                                              }
                  
                  , 'TIS_SpdMult'      :   600.                     # w/o dedicated Hlt1 line                 
                  , 'DiPhi_Hlt1TIS'    :  "Hlt1.*Decision%TIS"      # w/o dedicated Hlt1 line, only on Hlt1 TIS
                  # Track Fitte
                  , 'KaonPT'           :   500.   # MeV
                  , 'KaonTrkChi2'      :     4.
                  , 'KaonPIDK'         :     5. 
                  , 'PhiComMassW'      :    30.   # MeV, bfore Vtx fit
                  , 'PhiComPT'         :   500.   # MeV
                  , 'PhiMassW'         :    20.   # MeV
                  , 'PhiPT'            :   500.   # MeV
                  , 'PhiVtxCHI2'       :     9.
                  , 'CombLowerMass'    :  2750.   # MeV, before Vtx fit
                  , 'CombUpperMass'    :  4100.   # MeV, before Vtx fit
                  , 'CombPT'           :     0.   # MeV, before Vtx fit
                  , 'LowerMass'        :  2800.   # MeV, after Vtx fit
                  , 'UpperMass'        :  4000.   # MeV, after Vtx fit
                  , 'CCbarPT'          :  2000.   # MeV, after Vtx fit 
                  , 'VtxCHI2'          :     9.   
                  
                  , 'HltANNSvcID'      : {  'DiPhiTis'                   : 52000
                                            }
                  }
    
    def __apply_configuration__(self):
        self.__makeHlt2DiPhiReqHlt1TISLines() 

        
    def __makeHlt2DiPhiReqHlt1TISLines(self):        
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop, CombineParticles, TisTosParticleTagger
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiPhiTisDecision" : self.getProp('HltANNSvcID')['DiPhiTis'] } )

            
        #------------------------------------
        # Phi without PID
        #------------------------------------
        KaonCut = "(PT> %(KaonPT)s *MeV) & (TRCHI2DOF < %(KaonTrkChi2)s)" % self.getProps()
        PhiCombCut = "(ADAMASS('phi(1020)')<%(PhiComMassW)s*MeV) & (APT> %(PhiComPT)s *MeV)" % self.getProps()
        PhiMomCut = "(VFASPF(VCHI2PDOF)< %(PhiVtxCHI2)s)" \
                    " & (ADMASS('phi(1020)')<%(PhiMassW)s*MeV)" \
                    " & (PT> %(PhiPT)s *MeV)" % self.getProps()
        
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons        
        
        # Fitler Kaons before requring Hlt1 TIS
        FilterKaonsForDiPhi = Hlt2Member( FilterDesktop
                                          , 'FilterKaonsForDiPhi'
                                          , Inputs  = [ BiKalmanFittedKaons ]
                                          , Code    = KaonCut )
        
        KaonsForDiPhi = bindMembers( "KaonsForDiPhi", [ BiKalmanFittedKaons,
                                                        FilterKaonsForDiPhi
                                                        ] )
        
        
        PhiCombine = Hlt2Member( CombineParticles
                                 , "PhiCombine"
                                 , DecayDescriptor = "phi(1020) -> K+ K-"
                                 , DaughtersCuts = { "K+" : KaonCut }
                                 , CombinationCut = PhiCombCut
                                 , MotherCut = PhiMomCut
                                 #, Inputs = [ Hlt1TISKaonsForDiPhi ]
                                 , Inputs = [ KaonsForDiPhi ]
                                 , InputPrimaryVertices = "None"
                                 , UseP2PVRelations = False
                                 )

  
        #------------------------------------
        # Charmonium to DiPhi
        #------------------------------------      
        PhiCut = "(PT> %(PhiPT)s *MeV)" % self.getProps()
        CombCut = "(in_range( %(CombLowerMass)s *MeV, AM, %(CombUpperMass)s *MeV)) & (APT> %(CombPT)s *MeV)" % self.getProps()
        MomCut = "(VFASPF(VCHI2PDOF)< %(VtxCHI2)s)" \
                 " & (in_range( %(LowerMass)s *MeV, MM, %(UpperMass)s *MeV))" \
                 " & (PT> %(CCbarPT)s *MeV)" % self.getProps()
        
        Combine = Hlt2Member( CombineParticles
                              , "Combine"
                              , DecayDescriptor = "J/psi(1S) -> phi(1020) phi(1020)"
                              , DaughtersCuts = { "phi(1020)" : PhiCut }
                              , CombinationCut = CombCut
                              , MotherCut = MomCut
                              , Inputs = [ PhiCombine ]
                              , InputPrimaryVertices = "None"
                              , UseP2PVRelations = False
                              )
        
        #------------------------------------
        # Track Fitted & RichPID
        #------------------------------------
        RichKaonPID = "((PIDK-PIDpi) > %(KaonPIDK)s)" % self.getProps()   
        
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedRichKaons
        
        # Fitler RichKaons before requring Hlt1 TIS
        FilterRichKaonsForDiPhi = Hlt2Member( FilterDesktop
                                              , 'FilterRichKaonsForDiPhi'
                                              , Inputs  = [ BiKalmanFittedRichKaons ]
                                              , Code    = KaonCut+" & "+RichKaonPID  )
        
        RichKaonsForDiPhi = bindMembers( "RichKaonsForDiPhi", [ BiKalmanFittedRichKaons,
                                                                FilterRichKaonsForDiPhi
                                                                ] )

        """
        TaggerHlt1TISRichKaonsForDiPhi = Hlt2Member( TisTosParticleTagger
                                                     , 'TaggerHlt1TISRichKaonsForDiPhi'
                                                     , ProjectTracksToCalo = False
                                                     , CaloClustForCharged = False
                                                     , CaloClustForNeutral = False
                                                     , TOSFrac = {4:0.0,5:0.0 }
                                                     , TisTosSpecs = { self.getProp("DiPhi_Hlt1TIS") : 0 }
                                                     , Inputs = [ RichKaonsForDiPhi ]
                                                     )
        
        Hlt1TISRichKaonsForDiPhi = bindMembers("Hlt1TISRichKaonsForDiPhi", [ RichKaonsForDiPhi,
                                                                             TaggerHlt1TISRichKaonsForDiPhi ])
        """
                                                     
        RichPhiCombine = Hlt2Member( CombineParticles
                                     , "RichPhiCombine"
                                     , DecayDescriptor = "phi(1020) -> K+ K-"
                                     , DaughtersCuts = { "K+" : KaonCut+" & "+RichKaonPID }
                                     , CombinationCut = PhiCombCut
                                     , MotherCut = PhiMomCut
                                     , Inputs = [ RichKaonsForDiPhi ]
                                     , InputPrimaryVertices = "None"
                                     , UseP2PVRelations = False
                                     )

        RichCombine = Hlt2Member( CombineParticles
                                  , "RichCombine"
                                  , DecayDescriptor = "J/psi(1S) -> phi(1020) phi(1020)"
                                  , DaughtersCuts = { "phi(1020)" : PhiCut }
                                  , CombinationCut = CombCut
                                  , MotherCut = MomCut
                                  , Inputs = [ RichPhiCombine ]
                                  , InputPrimaryVertices = "None"
                                  , UseP2PVRelations = False
                                  )
        

        #------------------------------------
        # Inclusive DiPhi complete
        #------------------------------------
        line = Hlt2Line('DiPhiTis'
                        , prescale = self.prescale
                        , L0DU = "(L0_DATA('Spd(Mult)') < %(TIS_SpdMult)s )" % self.getProps()
                        , algos = [ #Hlt1TISKaonsForDiPhi
                                    KaonsForDiPhi
                                    , PhiCombine
                                    , Combine
                                    #, Hlt1TISRichKaonsForDiPhi
                                    , RichKaonsForDiPhi 
                                    , RichPhiCombine
                                    , RichCombine
                                    ]
                        , postscale = self.postscale
                        )
        
