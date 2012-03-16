###
#
#  Hlt2 Charmonium -> Phi Phi
#  @ author Serezha.Barsuk@cern.ch
#  @ author Jibo.He@cern.ch
#  @ date 2012-03-01
#
###

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2InclusiveDiPhiLinesConf(HltLinesConfigurableUser):
    """
    
    """
    
    __slots__ = { 'Prescale'           : {    'Hlt2DiPhi'                   : 1.  
                                              }
                  
                  ,'Postscale'         : {    'Hlt2DiPhi'                   : 1.  
                                              }
                  
                  , 'SpdMult'          :   600.                     # w/o dedicated Hlt1 line                 
                  # Track Fitted
                  , 'KaonPT'           :   500.   # MeV
                  , 'KaonTrkChi2'      :     4.
                  , 'KaonPIDK'         :     0. 
                  , 'PhiComMassW'      :    30.   # MeV, bfore Vtx fit
                  , 'PhiComPT'         :   400.   # MeV
                  , 'PhiMassW'         :    20.   # MeV
                  , 'PhiPT'            :   500.   # MeV
                  , 'PhiVtxCHI2'       :     9.
                  , 'CombLowerMass'    :  2750.   # MeV, before Vtx fit
                  , 'CombUpperMass'    :  4850.   # MeV, before Vtx fit
                  , 'CombPT'           :   900.   # MeV, before Vtx fit
                  , 'LowerMass'        :  2800.   # MeV, after Vtx fit
                  , 'UpperMass'        :  4800.   # MeV, after Vtx fit
                  , 'CCbarPT'          :  1000.   # MeV, after Vtx fit 
                  , 'VtxCHI2'          :     9.   
                  
                  , 'HltANNSvcID'      : {  'DiPhi'                   : 52000
                                            }
                  }
    
    def __apply_configuration__(self):
        self.__makeHlt2DiPhiLines() 

        
    def __makeHlt2DiPhiLines(self):        
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop, CombineParticles, TisTosParticleTagger
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiPhiDecision" : self.getProp('HltANNSvcID')['DiPhi'] } )

            
        #------------------------------------
        # Phi without PID
        #------------------------------------
        KaonCut = "(PT> %(KaonPT)s *MeV) & (TRCHI2DOF < %(KaonTrkChi2)s)" % self.getProps()
        PhiCombCut = "(ADAMASS('phi(1020)')<%(PhiComMassW)s*MeV) & (APT> %(PhiComPT)s *MeV)" % self.getProps()
        PhiMomCut = "(VFASPF(VCHI2PDOF)< %(PhiVtxCHI2)s)" \
                    " & (ADMASS('phi(1020)')<%(PhiMassW)s*MeV)" \
                    " & (PT> %(PhiPT)s *MeV)" % self.getProps()
        
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons        
        
        PhiCombine = Hlt2Member( CombineParticles
                                 , "PhiCombine"
                                 , DecayDescriptor = "phi(1020) -> K+ K-"
                                 , DaughtersCuts = { "K+" : KaonCut }
                                 , CombinationCut = PhiCombCut
                                 , MotherCut = PhiMomCut
                                 , Inputs = [ BiKalmanFittedKaons ]
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
        
        RichPhiCombine = Hlt2Member( CombineParticles
                                     , "RichPhiCombine"
                                     , DecayDescriptor = "phi(1020) -> K+ K-"
                                     , DaughtersCuts = { "K+" : KaonCut+" & "+RichKaonPID }
                                     , CombinationCut = PhiCombCut
                                     , MotherCut = PhiMomCut
                                     , Inputs = [ BiKalmanFittedRichKaons ]
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
        line = Hlt2Line('DiPhi'
                        , prescale = self.prescale
                        , L0DU = "(L0_DATA('Spd(Mult)') < %(SpdMult)s )" % self.getProps()
                        , algos = [ BiKalmanFittedKaons
                                    , PhiCombine
                                    , Combine
                                    , BiKalmanFittedRichKaons
                                    , RichPhiCombine
                                    , RichCombine
                                    ]
                        , postscale = self.postscale
                        )
        
