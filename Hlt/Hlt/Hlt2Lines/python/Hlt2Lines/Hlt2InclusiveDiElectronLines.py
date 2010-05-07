## @file
#
#  Hlt2 dielectron selections
#
#  @author D.R.Ward
#  @date 2010-04-20
#

##
from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2InclusiveDiElectronLinesConf(HltLinesConfigurableUser) :
    '''
          Put only the most discriminating variables as slots:
           - prescale factors, mass windows and dielectron pt
           - electron pt _not_ as slot as this strongly modifies the angular acceptances
    '''
    
    
    __slots__ = {  'Prescale'                  : {   'HltZee'                :  1.0
                                                    ,'Hlt2DYee1'             :  1.0
                                                    ,'Hlt2DYee2'             :  1.0
                                                    ,'Hlt2DYee3'             :  1.0
                                                    ,'HltZeeUnfitted'        :  1.0
                                                    ,'Hlt2DYee1eUnfitted'    :  1.0
                                                    ,'Hlt2DYeeeUnfitted2'    :  1.0
                                                    ,'Hlt2DYee3eUnfitted'    :  1.0
                                                    # Unbiased Di-Electron
                                                    ,'Hlt2UnbiasedDiElectron'         :  1.0
                                                    ,'Hlt2UnbiasedDiElectronLowMass'  :  1.0 
                                                    ,'Hlt2UnbiasedJpsi2ee'            :  1.0
                                                    ,'Hlt2UnbiasedPsi2ee'             :  1.0
                                                    ,'Hlt2UnbiasedB2ee'               :  1.0
                                                    # Unbiased and Track Fitted (TF)
                                                    ,'Hlt2UnbiasedTFDiElectron'         :  1.0
                                                    ,'Hlt2UnbiasedTFDiElectronLowMass'  :  1.0 
                                                    ,'Hlt2UnbiasedTFJpsi2ee'            :  1.0
                                                    ,'Hlt2UnbiasedTFPsi2ee'             :  1.0
                                                    ,'Hlt2UnbiasedTFB2ee'               :  1.0
                                                     }

                   ,'DiElectronVertexChi2'    :   20
                   ,'DiElectronIP'            :  0.1      # mm 
                   ,'DiElectronTkChi2'        :   20
                   ,'DiElectronDphiMin'       :    0      # radian
                   ,'ZeeMinMass'              :40000      # MeV
                   ,'ZeePt'                   :10000      # MeV
                   ,'DYPt'                    :  500      # MeV
                   ,'DY1MinPt'                : 2000      # MeV
                   ,'DY1MinMass'              : 5000      # MeV
                   ,'DY2MinPt'                : 4000      # MeV
                   ,'DY2MinMass'              :10000      # MeV
                   ,'DY3MinPt'                : 6000      # MeV
                   ,'DY3MinMass'              :15000      # MeV
                   
                   #-----------------
                   # Track un-fitted 
                   #-----------------
                   # Di-Electron 
                   ,'UnbiasedDiElectronMinMass'    : 2700      # MeV
                   ,'UnbiasedDiElectronVtxCHI2'    :   25
                   ,'UnbiasedDiElectronPT'         : 1000      # MeV
                   ,'UnbiasedDiElectronElecPT'     :  500      # MeV

                   # Di-Electron Low mass
                   ,'UnbiasedDiElectronLowMassMinMass'   :  500      # MeV
                   ,'UnbiasedDiElectronLowMassVtxCHI2'   :   25
                   ,'UnbiasedDiElectronLowMassPT'        : -999.     # MeV
                   ,'UnbiasedDiElectronLowMassElecPT'    :  500      # MeV

                   # Jpsi -> ee 
                   ,'UnbiasedJpsi2eeMinMass'     : 2700      # MeV
                   ,'UnbiasedJpsi2eeMaxMass'     : 3200      # MeV
                   ,'UnbiasedJpsi2eeVtxCHI2'     :   25
                   ,'UnbiasedJpsi2eePT'          : 1000      # MeV
                   ,'UnbiasedJpsi2eeElecPT'      :  500      # MeV
                   
                   # Psi(2S) -> ee
                   ,'UnbiasedPsi2eeMinMass'    : 3300      # MeV
                   ,'UnbiasedPsi2eeMaxMass'    : 3800      # MeV
                   ,'UnbiasedPsi2eeVtxCHI2'    :   25
                   ,'UnbiasedPsi2eePT'         : 1000      # MeV
                   ,'UnbiasedPsi2eeElecPT'     :  500      # MeV

                   # B, upsilon -> ee
                   ,'UnbiasedB2eeMinMass'      : 5200      # MeV  
                   ,'UnbiasedB2eeVtxCHI2'      :   25
                   ,'UnbiasedB2eePT'           : 1000      # MeV
                   ,'UnbiasedB2eeElecPT'       : 1500      # MeV

                   #-----------------
                   # Track fitted 
                   #-----------------
                   # Di-Electron 
                   ,'UnbiasedTFDiElectronMinMass'    : 2700      # MeV
                   ,'UnbiasedTFDiElectronVtxCHI2'    :   25
                   ,'UnbiasedTFDiElectronPT'         : 1000      # MeV
                   ,'UnbiasedTFDiElectronElecPT'     :  500      # MeV

                   # Di-Electron Low mass
                   ,'UnbiasedTFDiElectronLowMassMinMass'   :  500      # MeV
                   ,'UnbiasedTFDiElectronLowMassVtxCHI2'   :   25
                   ,'UnbiasedTFDiElectronLowMassPT'        : -999.     # MeV
                   ,'UnbiasedTFDiElectronLowMassElecPT'    :  500      # MeV

                   # Jpsi -> ee 
                   ,'UnbiasedTFJpsi2eeMinMass'     : 2700      # MeV
                   ,'UnbiasedTFJpsi2eeMaxMass'     : 3200      # MeV
                   ,'UnbiasedTFJpsi2eeVtxCHI2'     :   25
                   ,'UnbiasedTFJpsi2eePT'          : 1000      # MeV
                   ,'UnbiasedTFJpsi2eeElecPT'      :  500      # MeV
                   
                   # Psi(2S) -> ee
                   ,'UnbiasedTFPsi2eeMinMass'    : 3300      # MeV
                   ,'UnbiasedTFPsi2eeMaxMass'    : 3800      # MeV
                   ,'UnbiasedTFPsi2eeVtxCHI2'    :   25
                   ,'UnbiasedTFPsi2eePT'         : 1000      # MeV
                   ,'UnbiasedTFPsi2eeElecPT'     :  500      # MeV

                   # B, upsilon -> ee
                   ,'UnbiasedTFB2eeMinMass'      : 5200      # MeV  
                   ,'UnbiasedTFB2eeVtxCHI2'      :   25
                   ,'UnbiasedTFB2eePT'           : 1000      # MeV
                   ,'UnbiasedTFB2eeElecPT'       : 1500      # MeV                   
                   
                   ,'HltANNSvcID'  : {
                                      'UnbiasedDiElectron'         :  51200
                                     ,'UnbiasedDiElectronLowMass'  :  51210
                                     ,'UnbiasedJpsi2ee'            :  51201
                                     ,'UnbiasedPsi2ee'             :  51202
                                     ,'UnbiasedB2ee'               :  51203
                                     # Track fitted 
                                     ,'UnbiasedTFDiElectron'         :  51250
                                     ,'UnbiasedTFDiElectronLowMass'  :  51260
                                     ,'UnbiasedTFJpsi2ee'            :  51251
                                     ,'UnbiasedTFPsi2ee'             :  51252
                                     ,'UnbiasedTFB2ee'               :  51253                                      
                                     }

                   }
    

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop, CombineParticles        
        from Hlt2SharedParticles.BasicParticles import Electrons
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedElectrons
        
        #some string definitions... 

        
#        TKQuality =  " & (MIPDV(PRIMARY)<"+str(self.getProp('DiElectronIP'))+"*mm) & (TRCHI2DOF<"+str(self.getProp('DiElectronTkChi2'))+")"
        TKQuality =  " & (TRCHI2DOF<"+str(self.getProp('DiElectronTkChi2'))+")"                                
                            

        #--------------------------------------------
        '''
        DY->ee lowest mass range
        '''

        combineDY1 = Hlt2Member( CombineParticles # type
                                 , "CombineDY1" # name 
                                 , DecayDescriptor = "Z0 -> e+ e-"
                                 , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DY1MinPt'))+"*MeV)" + TKQuality ,
                                                     "e-" : "(PT>"+str(self.getProp('DY1MinPt'))+"*MeV)" + TKQuality }
                                 , MotherCut = "(VFASPF(VCHI2/VDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                                 , CombinationCut = "(AM>"+str(self.getProp('DY1MinMass'))+"*MeV)"\
                                                 " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                                 , InputLocations  = [ BiKalmanFittedElectrons ]
                                 )
       
        line1 = Hlt2Line('DYee1'
                        , prescale = self.prescale 
                        , algos = [ BiKalmanFittedElectrons, combineDY1 ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYee1Decision":   50291 } )

        #--------------------------------------------
        '''
        DY->ee middle mass range
        '''

        combineDY2 = Hlt2Member( CombineParticles # type
                                 , "CombineDY2" # name 
                                 , DecayDescriptor = "Z0 -> e+ e-"
                                 , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DY2MinPt'))+"*MeV)" + TKQuality ,
                                                     "e-" : "(PT>"+str(self.getProp('DY2MinPt'))+"*MeV)" + TKQuality }
                                 , MotherCut = "(VFASPF(VCHI2/VDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                                 , CombinationCut = "(AM>"+str(self.getProp('DY2MinMass'))+"*MeV)"\
                                                 " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                                 , InputLocations  = [ BiKalmanFittedElectrons ]
                                 )
       
        line2 = Hlt2Line('DYee2'
                        , prescale = self.prescale 
                        , algos = [ BiKalmanFittedElectrons, combineDY2 ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYee2Decision":   50292 } )

        #--------------------------------------------
        '''
        DY->ee highest mass range
        '''

        combineDY3 = Hlt2Member( CombineParticles # type
                                 , "CombineDY3" # name 
                                 , DecayDescriptor = "Z0 -> e+ e-"
                                 , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DY3MinPt'))+"*MeV)" + TKQuality ,
                                                     "e-" : "(PT>"+str(self.getProp('DY3MinPt'))+"*MeV)" + TKQuality }
                                 , MotherCut = "(VFASPF(VCHI2/VDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                                 , CombinationCut = "(AM>"+str(self.getProp('DY3MinMass'))+"*MeV)"\
                                                 " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                                 , InputLocations  = [ BiKalmanFittedElectrons ]
                                 )
       
        line3 = Hlt2Line('DYee3'
                        , prescale = self.prescale 
                        , algos = [ BiKalmanFittedElectrons, combineDY3 ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYee3Decision":   50293 } )
        #--------------------------------------------
        '''
        Zee
        '''

        combineZ = Hlt2Member( CombineParticles # type
                               , "CombineZ" # name 
                               , DecayDescriptor = "Z0 -> e+ e-"
                               , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('ZeePt'))+"*MeV)" + TKQuality ,
                                                   "e-" : "(PT>"+str(self.getProp('ZeePt'))+"*MeV)" + TKQuality }
                               , MotherCut = "(VFASPF(VCHI2/VDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                               , CombinationCut = "(AM>"+str(self.getProp('ZeeMinMass'))+"*MeV) & "\
                                                  "(abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                               , InputLocations  = [ BiKalmanFittedElectrons ]
                               )
       
        line4 = Hlt2Line('Zee'
                        , prescale = self.prescale 
                        , algos = [ BiKalmanFittedElectrons, combineZ ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2ZeeDecision":   50295 } )

        ####################################################################
        #  Versions using unfitted tracks
        ####################################################################


        #--------------------------------------------
        '''
        DY->ee lowest mass range
        '''

        combineDY1U = Hlt2Member( CombineParticles # type
                                 , "CombineDY1U" # name 
                                 , DecayDescriptor = "Z0 -> e+ e-"
                                 , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DY1MinPt'))+"*MeV)" + TKQuality ,
                                                     "e-" : "(PT>"+str(self.getProp('DY1MinPt'))+"*MeV)" + TKQuality }
                                 , MotherCut = "(VFASPF(VCHI2/VDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                                 , CombinationCut = "(AM>"+str(self.getProp('DY1MinMass'))+"*MeV)"\
                                                 " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                                 , InputLocations  = [ Electrons ]
                                 )
       
        line1U = Hlt2Line('DYee1Unfitted'
                        , prescale = self.prescale 
                        , algos = [ Electrons, combineDY1U ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYee1UnfittedDecision":   50296 } )

        #--------------------------------------------
        '''
        DY->ee middle mass range
        '''

        combineDY2U = Hlt2Member( CombineParticles # type
                                 , "CombineDY2U" # name 
                                 , DecayDescriptor = "Z0 -> e+ e-"
                                 , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DY2MinPt'))+"*MeV)" + TKQuality ,
                                                     "e-" : "(PT>"+str(self.getProp('DY2MinPt'))+"*MeV)" + TKQuality }
                                 , MotherCut = "(VFASPF(VCHI2/VDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                                 , CombinationCut = "(AM>"+str(self.getProp('DY2MinMass'))+"*MeV)"\
                                                 " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                                 , InputLocations  = [ Electrons ]
                                 )
       
        line2U = Hlt2Line('DYee2Unfitted'
                        , prescale = self.prescale 
                        , algos = [ Electrons, combineDY2U ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYee2UnfittedDecision":   50297 } )

        #--------------------------------------------
        '''
        DY->ee highest mass range
        '''

        combineDY3U = Hlt2Member( CombineParticles # type
                                 , "CombineDY3U" # name 
                                 , DecayDescriptor = "Z0 -> e+ e-"
                                 , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DY3MinPt'))+"*MeV)" + TKQuality ,
                                                     "e-" : "(PT>"+str(self.getProp('DY3MinPt'))+"*MeV)" + TKQuality }
                                 , MotherCut = "(VFASPF(VCHI2/VDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                                 , CombinationCut = "(AM>"+str(self.getProp('DY3MinMass'))+"*MeV)"\
                                                 " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                                 , InputLocations  = [ Electrons ]
                                 )
       
        line3U = Hlt2Line('DYee3Unfitted'
                        , prescale = self.prescale 
                        , algos = [ Electrons, combineDY3U ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYee3UnfittedDecision":   50298 } )
        #--------------------------------------------
        '''
        Zee
        '''

        combineZU = Hlt2Member( CombineParticles # type
                               , "CombineZU" # name 
                               , DecayDescriptor = "Z0 -> e+ e-"
                               , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('ZeePt'))+"*MeV)" + TKQuality ,
                                                   "e-" : "(PT>"+str(self.getProp('ZeePt'))+"*MeV)" + TKQuality }
                               , MotherCut = "(VFASPF(VCHI2/VDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                               , CombinationCut = "(AM>"+str(self.getProp('ZeeMinMass'))+"*MeV) & "\
                                                  "(abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                               , InputLocations  = [ Electrons ]
                               )
       
        line4U = Hlt2Line('ZeeUnfitted'
                        , prescale = self.prescale 
                        , algos = [ Electrons, combineZU ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2ZeeUnfittedDecision":   50299 } )


        #####################################################################################
        # 
        # 1. 'Hlt2UnbiasedDiElectron',          Mass sidebands from 2.7 GeV, pre-scaled
        # 2. 'Hlt2UnbiasedDiElectronLowMass'    Mass sidebands from 500 GeV, pre-scaled 
        # 3. 'Hlt2UnbiasedJpsi2ee'              Jpsi -> ee
        # 4. 'Hlt2UnbiasedPsi2ee'               Psi(2S) -> ee
        # 5. 'Hlt2UnbiasedB2ee'                 For all with Mass > 5.2 GeV
        #
        #####################################################################################

        
        #--------------------------------------------
        # Filter DiElectron
        #--------------------------------------------
        from Hlt2SharedParticles.DiElectron import DiElectron
        
        FilterDiElectron = Hlt2Member( FilterDesktop # type
                                       , "FilterDiElectron" 
                                       , Code ="(MINTREE('e+'==ABSID,PT) > %(UnbiasedDiElectronElecPT)s *MeV)"\
                                       " & (MM > %(UnbiasedDiElectronMinMass)s *MeV)"\
                                       " & (VFASPF(VCHI2/VDOF) < %(UnbiasedDiElectronVtxCHI2)s)"\
                                       " & (PT > %(UnbiasedDiElectronPT)s *MeV)" %self.getProps() 
                                       , InputLocations = [ DiElectron ]
                                       )


        #--------------------------------------------
        # Unbiased DiElectron, prescaled
        #--------------------------------------------
        UnbiasedDiElectronLine = Hlt2Line("UnbiasedDiElectron"
                                          , prescale = self.prescale
                                          , algos = [ DiElectron, FilterDiElectron ]
                                          , postscale = self.postscale
                                          )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedDiElectronDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedDiElectron'] } ) 

        
        #--------------------------------------------
        # Unbiased DiElectron, low mass prescaled
        #--------------------------------------------
        UnbiasedDiElectronLine.clone("UnbiasedDiElectronLowMass"
                                     , prescale = self.prescale
                                     , algos = [ DiElectron, FilterDiElectron ]
                                     , FilterDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedDiElectronLowMassElecPT)s *MeV)"\
                                      " & (MM > %(UnbiasedDiElectronLowMassMinMass)s *MeV)"\
                                      " & (VFASPF(VCHI2/VDOF) < %(UnbiasedDiElectronLowMassVtxCHI2)s)"\
                                      " & (PT > %(UnbiasedDiElectronLowMassPT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedDiElectronLowMassDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedDiElectronLowMass'] } ) 


        #--------------------------------------------
        # Jpsi -> e+ e-
        #--------------------------------------------
        UnbiasedDiElectronLine.clone("UnbiasedJpsi2ee"
                                     , prescale = self.prescale
                                     , algos = [ DiElectron, FilterDiElectron ]
                                     , FilterDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedJpsi2eePT)s *MeV)"\
                                      " & (MM > %(UnbiasedJpsi2eeMinMass)s *MeV)"\
                                      " & (MM < %(UnbiasedJpsi2eeMaxMass)s *MeV)"\
                                      " & (VFASPF(VCHI2/VDOF) < %(UnbiasedJpsi2eeVtxCHI2)s)"\
                                      " & (PT > %(UnbiasedJpsi2eePT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedJpsi2eeDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedJpsi2ee'] } ) 


        #--------------------------------------------
        # Psi(2S) -> e+ e-
        #--------------------------------------------
        UnbiasedDiElectronLine.clone("UnbiasedPsi2ee"
                                     , prescale = self.prescale
                                     , algos = [ DiElectron, FilterDiElectron ]
                                     , FilterDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedPsi2eePT)s *MeV)"\
                                      " & (MM > %(UnbiasedPsi2eeMinMass)s *MeV)"\
                                      " & (MM < %(UnbiasedPsi2eeMaxMass)s *MeV)"\
                                      " & (VFASPF(VCHI2/VDOF) < %(UnbiasedPsi2eeVtxCHI2)s)"\
                                      " & (PT > %(UnbiasedPsi2eePT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedPsi2eeDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedPsi2ee'] } ) 


        #--------------------------------------------
        # B, Upsilon -> e+ e-
        #--------------------------------------------
        UnbiasedDiElectronLine.clone("UnbiasedB2ee"
                                     , prescale = self.prescale
                                     , algos = [ DiElectron, FilterDiElectron ]
                                     , FilterDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedB2eePT)s *MeV)"\
                                      " & (MM > %(UnbiasedB2eeMinMass)s *MeV)"\
                                      " & (VFASPF(VCHI2/VDOF) < %(UnbiasedB2eeVtxCHI2)s)"\
                                      " & (PT > %(UnbiasedB2eePT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedB2eeDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedB2ee'] } )



        #--------------------------------------------
        # Filter TFDiElectron
        #--------------------------------------------
        from Hlt2SharedParticles.TrackFittedDiElectron import TrackFittedDiElectron
        
        FilterTFDiElectron = Hlt2Member( FilterDesktop # type
                                       , "FilterTFDiElectron" 
                                       , Code ="(MINTREE('e+'==ABSID,PT) > %(UnbiasedTFDiElectronElecPT)s *MeV)"\
                                       " & (MM > %(UnbiasedTFDiElectronMinMass)s *MeV)"\
                                       " & (VFASPF(VCHI2/VDOF) < %(UnbiasedTFDiElectronVtxCHI2)s)"\
                                       " & (PT > %(UnbiasedTFDiElectronPT)s *MeV)" %self.getProps() 
                                       , InputLocations = [ TrackFittedDiElectron ]
                                       )


        #--------------------------------------------
        # UnbiasedTF DiElectron, prescaled
        #--------------------------------------------
        UnbiasedTFDiElectronLine = Hlt2Line("UnbiasedTFDiElectron"
                                          , prescale = self.prescale
                                          , algos = [ TrackFittedDiElectron, FilterTFDiElectron ]
                                          , postscale = self.postscale
                                          )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedTFDiElectronDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedTFDiElectron'] } ) 

        
        #--------------------------------------------
        # UnbiasedTF DiElectron, low mass prescaled
        #--------------------------------------------
        UnbiasedTFDiElectronLine.clone("UnbiasedTFDiElectronLowMass"
                                     , prescale = self.prescale
                                     , algos = [ TrackFittedDiElectron, FilterTFDiElectron ]
                                     , FilterTFDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedTFDiElectronLowMassElecPT)s *MeV)"\
                                      " & (MM > %(UnbiasedTFDiElectronLowMassMinMass)s *MeV)"\
                                      " & (VFASPF(VCHI2/VDOF) < %(UnbiasedTFDiElectronLowMassVtxCHI2)s)"\
                                      " & (PT > %(UnbiasedTFDiElectronLowMassPT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedTFDiElectronLowMassDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedTFDiElectronLowMass'] } ) 


        #--------------------------------------------
        # Jpsi -> e+ e-
        #--------------------------------------------
        UnbiasedTFDiElectronLine.clone("UnbiasedTFJpsi2ee"
                                     , prescale = self.prescale
                                     , algos = [ TrackFittedDiElectron, FilterTFDiElectron ]
                                     , FilterTFDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedTFJpsi2eePT)s *MeV)"\
                                      " & (MM > %(UnbiasedTFJpsi2eeMinMass)s *MeV)"\
                                      " & (MM < %(UnbiasedTFJpsi2eeMaxMass)s *MeV)"\
                                      " & (VFASPF(VCHI2/VDOF) < %(UnbiasedTFJpsi2eeVtxCHI2)s)"\
                                      " & (PT > %(UnbiasedTFJpsi2eePT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedTFJpsi2eeDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedTFJpsi2ee'] } ) 


        #--------------------------------------------
        # Psi(2S) -> e+ e-
        #--------------------------------------------
        UnbiasedTFDiElectronLine.clone("UnbiasedTFPsi2ee"
                                     , prescale = self.prescale
                                     , algos = [ TrackFittedDiElectron, FilterTFDiElectron ]
                                     , FilterTFDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedTFPsi2eePT)s *MeV)"\
                                      " & (MM > %(UnbiasedTFPsi2eeMinMass)s *MeV)"\
                                      " & (MM < %(UnbiasedTFPsi2eeMaxMass)s *MeV)"\
                                      " & (VFASPF(VCHI2/VDOF) < %(UnbiasedTFPsi2eeVtxCHI2)s)"\
                                      " & (PT > %(UnbiasedTFPsi2eePT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedTFPsi2eeDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedTFPsi2ee'] } ) 


        #--------------------------------------------
        # B, Upsilon -> e+ e-
        #--------------------------------------------
        UnbiasedTFDiElectronLine.clone("UnbiasedTFB2ee"
                                     , prescale = self.prescale
                                     , algos = [ TrackFittedDiElectron, FilterTFDiElectron ]
                                     , FilterTFDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedTFB2eePT)s *MeV)"\
                                      " & (MM > %(UnbiasedTFB2eeMinMass)s *MeV)"\
                                      " & (VFASPF(VCHI2/VDOF) < %(UnbiasedTFB2eeVtxCHI2)s)"\
                                      " & (PT > %(UnbiasedTFB2eePT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedTFB2eeDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedTFB2ee'] } )
