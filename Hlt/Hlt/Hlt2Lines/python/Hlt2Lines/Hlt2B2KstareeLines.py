###############
#
# @author M. Nicol, nicol@lal.in2p3.fr
# @author Jibo.He@cern.ch
# @date 20.04.2010
#
##
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2KstareeLinesConf(HltLinesConfigurableUser) :


    __slots__ = {
                'Prescale'   :  {  'Hlt2B2KstareeRobustAndFitted'                        : 1.
                                  ,'Hlt2B2KstareeRobustAndFittedSignal'                  : 1.
                                  ,'Hlt2B2KstareeRobustAndFittedLargeBMassWindow'        : 1.
                                  ,'Hlt2B2KstareeRobustAndFittedLargeBMassWindowSignal'  : 1.  
                                  ,'Hlt2B2KstareeFitted'                                 : 1.
                                  ,'Hlt2B2KstareeFittedSignal'                           : 1.
                                  ,'Hlt2B2KstareeFittedLargeBMassWindow'                 : 1.
                                  ,'Hlt2B2KstareeFittedLargeBMassWindowSignal'           : 1.                                     

                                  }
                
                ,'Postscale' :  {  'Hlt2B2KstareeRobustAndFitted'                        : 1.
                                  ,'Hlt2B2KstareeRobustAndFittedSignal'                  : 1.
                                  ,'Hlt2B2KstareeRobustAndFittedLargeBMassWindow'        : 1.
                                  ,'Hlt2B2KstareeRobustAndFittedLargeBMassWindowSignal'  : 1.  
                                  ,'Hlt2B2KstareeFitted'                                 : 1.
                                  ,'Hlt2B2KstareeFittedSignal'                           : 1.
                                  ,'Hlt2B2KstareeFittedLargeBMassWindow'                 : 1.
                                  ,'Hlt2B2KstareeFittedLargeBMassWindowSignal'           : 1.                                     

                                  }
                
                #----------------------------
                # L0 && Hlt1 requiremnets
                #----------------------------
                , 'L0Requirement'   :  "L0_CHANNEL('Electron')"
                , 'Hlt1Requirement' :  "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')"
        
                #----------------------------
                # cuts for track unfitted
                #----------------------------
                ,  'RobustElectronPT'               :  200.           # MeV
                ,  'RobustElectronPIDe'             :   -2.
                ,  'RobusteeVertexCHI2'             :   64.
 		,  'RobustPionPT'                   :  200.           # MeV
                ,  'RobustPionIPCHI2'               :    1.           # adimentional
                ,  'RobustKaonPT'                   :  250.           # MeV
		,  'RobustKaonIPCHI2'               :    1.           # adimentional
		,  'RobustKstarMassWindow'          :  200.           # MeV
		,  'RobustKstarVertexCHI2'          :   64.           # adimentional
		,  'RobustKstarIPCHI2'             :     1.           # adimentional
                #----------------------------
                # cuts for track fitted 
                #----------------------------
                ,  'ElectronPT'               :  300.           # MeV
                ,  'ElectronPIDe'             :   -1.
                ,  'ElectronIPCHI2'           :    1.           # adimentional
                ,  'ElectronTrackCHI2pNDOF'   :   25.           #
		,  'eeVertexCHI2'             :   25.           # adimentional
                ,  'eeMinMass'                : -999.           # MeV
                ,  'eeMaxMass'                : 6000.           # MeV
		,  'PionPT'                   :  300.           # MeV
                ,  'PionTrackCHI2pNDOF'       :   25.
                ,  'PionIPCHI2'               :    2.25         # adimentional
                ,  'KaonPT'                   :  300.           # MeV
                ,  'KaonTrackCHI2pNDOF'       :   25.
		,  'KaonIPCHI2'               :    2.25         # adimentional
		,  'KstarMassWindow'          :  150.           # MeV
		,  'KstarVertexCHI2'          :   25.           # adimentional
		,  'KstarIPCHI2'              :    1.           # adimentional			
		,  'BLargeMassWindow'         : 1000.           # MeV
		,  'BMassWindow'              :  400.           # MeV	
		,  'BVertexCHI2'              :   25.           # adimentional	
		,  'BIPCHI2'                  :  100.           # 	
		,  'BDIRA'                    :    0.99955      # adimentional
		,  'BIP'                      :    1.           # mm
                #----------------------------
                # cuts for signal while different
                #----------------------------
                ,  'Signal_eeMaxMass'         : 2000.           # MeV
                }

    


    def __apply_configuration__(self) :
        self.__makeHlt2B2KstareeRobustAndFittedLines()
        self.__makeHlt2B2KstareeFittedLines()
	
        
    def __makeHlt2B2KstareeRobustAndFittedLines(self):
    
        from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, FilterDesktop
        from Hlt2SharedParticles.BasicParticles import Electrons, NoCutsKaons, NoCutsPions      
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedElectrons, BiKalmanFittedKaons, BiKalmanFittedPions
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2KstareeRobustAndFittedDecision"       : 55320 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2KstareeRobustAndFittedSignalDecision" : 55321 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2KstareeRobustAndFittedLargeBMassWindowDecision"       : 55330 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2KstareeRobustAndFittedLargeBMassWindowSignalDecision" : 55331 } )


        """
        #------------------------
        # L0 & Hlt1 Requirements
        #------------------------ 
        """
        L0Req  = self.getProp("L0Requirement")
        Hlt1Req = self.getProp("Hlt1Requirement")
        
        if not L0Req:
            L0Req = None

        if not Hlt1Req:
            Hlt1Req= None
            
	
        ##########################
	
        #make the dielectron
        LowPtDiElectron=Hlt2Member( CombineParticles
                                    , "LowPtDiElectron"
                                    , DecayDescriptor = "J/psi(1S) -> e+ e-"
                                    , Inputs = [Electrons]
                                    , DaughtersCuts = {"e+" : "(PT> %(RobustElectronPT)s *MeV) & (PIDe>%(RobustElectronPIDe)s)" % self.getProps()}
                                    , MotherCut = "(VFASPF(VCHI2PDOF)< %(RobusteeVertexCHI2)s)" % self.getProps()
                                    , InputPrimaryVertices = "None"
                                    , UseP2PVRelations = False
                                    )	

        #Make fitted dielectron
        FittedDiElectron=Hlt2Member( CombineParticles
                                     , "FittedDiElectron"
                                     , DecayDescriptor = "J/psi(1S) -> e+ e-"
                                     , Inputs = [BiKalmanFittedElectrons]
                                     , DaughtersCuts = {"e+" : "(PT> %(ElectronPT)s *MeV) & (PIDe>%(ElectronPIDe)s) & (BPVIPCHI2() > %(ElectronIPCHI2)s) & (TRCHI2DOF < %(ElectronTrackCHI2pNDOF)s)" % self.getProps()}
                                     , MotherCut = "(VFASPF(VCHI2PDOF)< %(eeVertexCHI2)s) & (in_range( %(eeMinMass)s, MM, %(eeMaxMass)s))" % self.getProps()
                                     )
	
        #Make Kstar
        Kstar=Hlt2Member( CombineParticles
                          , "Kstar"
                          , DecayDescriptor = "[K*(892)0 -> K+ pi-]cc" 
                          , Inputs = [NoCutsKaons, NoCutsPions]
                          , DaughtersCuts = {"K+" : "(PT> %(RobustKaonPT)s *MeV) & (BPVIPCHI2()> %(RobustKaonIPCHI2)s )" % self.getProps()
                                            ,"pi-": "(PT> %(RobustPionPT)s *MeV) & (BPVIPCHI2()> %(RobustPionIPCHI2)s )" % self.getProps()}
                          , CombinationCut =  "(ADAMASS('K*(892)0')< %(RobustKstarMassWindow)s *MeV)" % self.getProps()
                          , MotherCut = "(VFASPF(VCHI2PDOF)< %(RobustKstarVertexCHI2)s) & (BPVIPCHI2()> %(RobustKstarIPCHI2)s )" % self.getProps()
                          )	
	
	
        #Make fitted Kstar
        FittedKstar=Hlt2Member( CombineParticles
                                , "FittedKstar"
                                , DecayDescriptor = "[K*(892)0 -> K+ pi-]cc" 
                                , Inputs = [BiKalmanFittedKaons, BiKalmanFittedPions]
                                , DaughtersCuts = {"K+" : "(PT> %(KaonPT)s *MeV) & (BPVIPCHI2()> %(KaonIPCHI2)s ) & (TRCHI2DOF < %(KaonTrackCHI2pNDOF)s)" % self.getProps()
                                                  ,"pi-": "(PT> %(PionPT)s *MeV) & (BPVIPCHI2()> %(PionIPCHI2)s ) & (TRCHI2DOF < %(PionTrackCHI2pNDOF)s)" % self.getProps()}
                                , CombinationCut =  "(ADAMASS('K*(892)0')< %(KstarMassWindow)s *MeV)" % self.getProps()
                                , MotherCut = "(VFASPF(VCHI2PDOF)< %(KstarVertexCHI2)s) & (BPVIPCHI2()> %(KstarIPCHI2)s )" % self.getProps()
                                )	
	
	
        #make the Bd
        CombineB=Hlt2Member( CombineParticles
                             , "CombineB"
                             , DecayDescriptor = "[B0 -> K*(892)0 J/psi(1S)]cc"
                             , CombinationCut = "(ADAMASS('B0')< %(BLargeMassWindow)s *MeV)" % self.getProps()
                             , MotherCut = "(VFASPF(VCHI2PDOF)< %(BVertexCHI2)s) & (BPVIPCHI2()< %(BIPCHI2)s ) & (BPVDIRA> %(BDIRA)s ) & (BPVIP()< %(BIP)s *mm)" % self.getProps()
                             , Inputs = [FittedDiElectron, FittedKstar]
                             )
        
        B2KstareeRobustAndFittedAll = bindMembers( "B2KstareeRobustAndFitted", [ PV3D('Hlt2'),
                                                                                 Electrons,
                                                                                 LowPtDiElectron,
                                                                                 BiKalmanFittedElectrons,
                                                                                 FittedDiElectron,
                                                                                 NoCutsPions,
                                                                                 NoCutsKaons,
                                                                                 Kstar,
                                                                                 BiKalmanFittedKaons,
                                                                                 BiKalmanFittedPions,
                                                                                 FittedKstar,
                                                                                 CombineB  ] )
	
        ##########################
        #define the line
        line=Hlt2Line('B2KstareeRobustAndFittedLargeBMassWindow'
                      , prescale = self.prescale
                      , L0DU = L0Req
                      , HLT  = Hlt1Req
                      , algos = [ PV3D('Hlt2'),
                                  Electrons,
                                  LowPtDiElectron,
                                  BiKalmanFittedElectrons,
                                  FittedDiElectron,
                                  NoCutsPions,
                                  NoCutsKaons,
                                  Kstar,
                                  BiKalmanFittedKaons,
                                  BiKalmanFittedPions,
                                  FittedKstar,
                                  CombineB ] 
                      , postscale = self.postscale
                      )

        #----------------------------
        # Filtering signal
        #----------------------------
        SignalEEMassCut = "(INTREE( (ID=='J/psi(1S)') & (MM<%(Signal_eeMaxMass)s*MeV)))" % self.getProps()
        
        FilterSignal  = Hlt2Member( FilterDesktop # type
                                    , "FilterSignal" 
                                    , Code =  SignalEEMassCut 
                                    , Inputs = [ B2KstareeRobustAndFittedAll ]
                                    )

        line.clone('B2KstareeRobustAndFittedLargeBMassWindowSignal'
                   , prescale = self.prescale
                   , L0DU = L0Req
                   , HLT  = Hlt1Req
                   , algos = [ B2KstareeRobustAndFittedAll, FilterSignal ]
                   , postscale = self.postscale )  

        
        #----------------------------
        # Filtering B mass window
        #----------------------------
        SmallBMassWindowCut = "(ADMASS('B0')<%(BMassWindow)s*MeV)" % self.getProps()
        
        FilterB = Hlt2Member( FilterDesktop
                              , "FilterB"
                              , Code = SmallBMassWindowCut
                              , Inputs = [ B2KstareeRobustAndFittedAll ]
                              )

        line.clone( 'B2KstareeRobustAndFitted'
                    , prescale = self.prescale
                    , L0DU = L0Req
                    , HLT  = Hlt1Req
                    , algos = [ B2KstareeRobustAndFittedAll, FilterB ]
                    , postscale = self.postscale )

            
        #----------------------------
        # Filtering signal & B mass window
        #----------------------------        
        FilterSignalAndB  = Hlt2Member( FilterDesktop # type
                                        , "FilterSignalAndB" 
                                        , Code =  SmallBMassWindowCut + ' & ' + SignalEEMassCut 
                                        , Inputs = [ B2KstareeRobustAndFittedAll ]
                                        )

        line.clone('B2KstareeRobustAndFittedSignal'
                   , prescale = self.prescale
                   , L0DU = L0Req
                   , HLT  = Hlt1Req
                   , algos = [ B2KstareeRobustAndFittedAll, FilterSignalAndB ]
                   , postscale = self.postscale )
        
	
        ##########################
		
        
    def __makeHlt2B2KstareeFittedLines(self):

        from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedElectrons, BiKalmanFittedKaons, BiKalmanFittedPions

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2KstareeFittedDecision"        : 55300 } )  
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2KstareeFittedSignalDecision"  : 55301 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2KstareeFittedLargeBMassWindowDecision"        : 55310 } )  
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2KstareeFittedLargeBMassWindowSignalDecision"  : 55311 } )

        """
        #------------------------
        # L0 & Hlt1 Requirements
        #------------------------ 
        """
        L0Req  = self.getProp("L0Requirement")
        Hlt1Req = self.getProp("Hlt1Requirement")
        
        if not L0Req:
            L0Req = None

        if not Hlt1Req:
            Hlt1Req= None
            
	
        ##########################

        #Make fitted dielectron
        FittedDiElectron=Hlt2Member( CombineParticles
                                   , "FittedDiElectron"
                                   , DecayDescriptor = "J/psi(1S) -> e+ e-"
                                   , Inputs = [BiKalmanFittedElectrons]
                                   , DaughtersCuts = {"e+" : "(PT> %(ElectronPT)s *MeV) & (PIDe>%(ElectronPIDe)s) & (BPVIPCHI2() > %(ElectronIPCHI2)s) & (TRCHI2DOF < %(ElectronTrackCHI2pNDOF)s)" % self.getProps()}
                                   , MotherCut = "(VFASPF(VCHI2PDOF)< %(eeVertexCHI2)s) & (in_range(%(eeMinMass)s, MM, %(eeMaxMass)s))" % self.getProps()
                                   )
        
        #Make fitted Kstar
        FittedKstar=Hlt2Member( CombineParticles
                              , "FittedKstar"
                              , DecayDescriptor = "[K*(892)0 -> K+ pi-]cc" 
                              , Inputs = [BiKalmanFittedKaons, BiKalmanFittedPions]
                              , DaughtersCuts = {"K+" : "(PT> %(KaonPT)s *MeV) & (BPVIPCHI2()> %(KaonIPCHI2)s ) & (TRCHI2DOF < %(KaonTrackCHI2pNDOF)s)" % self.getProps()
                                                ,"pi-": "(PT> %(PionPT)s *MeV) & (BPVIPCHI2()> %(PionIPCHI2)s ) & (TRCHI2DOF < %(PionTrackCHI2pNDOF)s)" % self.getProps()}
                              , CombinationCut =  "(ADAMASS('K*(892)0')< %(KstarMassWindow)s *MeV)" % self.getProps()
                              , MotherCut = "(VFASPF(VCHI2PDOF)< %(KstarVertexCHI2)s) &  (BPVIPCHI2()> %(KstarIPCHI2)s )" % self.getProps()
                              )	
	
		
        #make the Bd
        CombineB=Hlt2Member( CombineParticles
                           , "CombineB"
                           , DecayDescriptor = "[B0 -> K*(892)0 J/psi(1S)]cc"
                           , CombinationCut = "(ADAMASS('B0')< %(BLargeMassWindow)s *MeV)" % self.getProps()
                           , MotherCut = "(VFASPF(VCHI2PDOF)< %(BVertexCHI2)s) & (BPVIPCHI2()< %(BIPCHI2)s ) & (BPVDIRA> %(BDIRA)s ) & (BPVIP()< %(BIP)s *mm)" % self.getProps()
                           , Inputs = [FittedDiElectron, FittedKstar]
	                       )

        B2KstareeFittedAll = bindMembers('B2KstareeFitted', [ PV3D('Hlt2'),
                                                              BiKalmanFittedElectrons,
                                                              FittedDiElectron,
                                                              BiKalmanFittedKaons,
                                                              BiKalmanFittedPions,
                                                              FittedKstar,
                                                              CombineB ] )
                                         
	
        ##########################
        #define the line
        line=Hlt2Line('B2KstareeFittedLargeBMassWindow'
                     , L0DU = L0Req
                     , HLT  = Hlt1Req
                     , prescale = self.prescale
                     , algos = [ PV3D('Hlt2'),
                                 BiKalmanFittedElectrons,
                                 FittedDiElectron,
                                 BiKalmanFittedKaons,
                                 BiKalmanFittedPions,
                                 FittedKstar,
                                 CombineB ] 
                     , postscale = self.postscale
                     )
        

        #----------------------------
        # Filtering signal
        #----------------------------
        SignalEEMassCut = "(INTREE( (ID=='J/psi(1S)') & (MM<%(Signal_eeMaxMass)s*MeV)))" % self.getProps()
        
        FilterSignal  = Hlt2Member( FilterDesktop # type
                                  , "FilterSignal" 
                                  , Code = SignalEEMassCut 
                                  , Inputs = [ B2KstareeFittedAll ]
                                  )
        
        line.clone('B2KstareeFittedLargeBMassWindowSignal'
                  , prescale = self.prescale
                  , L0DU = L0Req
                  , HLT  = Hlt1Req
                  , algos = [ B2KstareeFittedAll, FilterSignal ]
                  , postscale = self.postscale )

        
        #----------------------------
        # Filtering B mass window
        #----------------------------
        SmallBMassWindowCut = "(ADMASS('B0')<%(BMassWindow)s*MeV)" % self.getProps()
        
        FilterB = Hlt2Member( FilterDesktop
                            , "FilterB"
                            , Code = SmallBMassWindowCut
                            , Inputs = [ B2KstareeFittedAll ]
                            )

        line.clone( 'B2KstareeFitted'
                  , prescale = self.prescale
                  , L0DU = L0Req
                  , HLT  = Hlt1Req 
                  , algos = [ B2KstareeFittedAll, FilterB ]
                  , postscale = self.postscale )

            
        #----------------------------
        # Filtering signal & B mass window
        #----------------------------        
        FilterSignalAndB  = Hlt2Member( FilterDesktop # type
                                      , "FilterSignalAndB" 
                                      , Code =  SmallBMassWindowCut + ' & ' + SignalEEMassCut 
                                      , Inputs = [ B2KstareeFittedAll ]
                                      )

        line.clone('B2KstareeFittedSignal'
                   , prescale = self.prescale
                   , L0DU = L0Req
                   , HLT  = Hlt1Req 
                   , algos = [ B2KstareeFittedAll, FilterSignalAndB ]
                   , postscale = self.postscale )
	
        ##########################  
