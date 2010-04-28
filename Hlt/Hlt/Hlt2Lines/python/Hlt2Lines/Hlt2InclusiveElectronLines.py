# Hlt2 Inclusive single Electron and Electron+track selections 
#
#  @author D.R.Ward
#  @date 2010-04-20
#

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2InclusiveElectronLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = { 'Prescale'             : { 'Hlt2SingleHighPTElectron'    : 1.0
                                            ,'Hlt2DYeh1'                   : 1.0
                                            ,'Hlt2DYeh2'                   : 1.0
                                            ,'Hlt2SingleHighPTUnfittedElectron'    : 1.0
                                            ,'Hlt2DYUnfittedeh1'           : 1.0
                                            ,'Hlt2DYUnfittedeh2'           : 1.0
                                             }
                                             
                 
                  ,'SingleElectronHighPt'    : 20000      # MeV
                  ,'SingleElectronIP'        :   0.1      # mm
                  ,'SingleElectronTkChi2'    :    20     
                  
                  ,'DYeh1ePt'                 : 4000      # MeV
                  ,'DYeh1trPt'                : 4000      # MeV                  
                  ,'DYeh1MinMass'             :10000      # MeV                  
                  ,'DYeh2ePt'                 : 8000      # MeV
                  ,'DYeh2trPt'                : 8000      # MeV                  
                  ,'DYeh2MinMass'             :20000      # MeV                  
                  ,'DYehDphiMin'             :     0      # radian                 
                  ,'DYehVertexChi2'          :    20      #                   
                  }
    
    
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from HltLine.HltLine import bindMembers
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, PhysDesktop
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.BasicParticles import Electrons, NoCutsPions
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedElectrons

        #some string definitions... 


#        TKQuality =  " & (MIPDV(PRIMARY)<"+str(self.getProp('SingleElectronIP'))+"*mm) & (TRCHI2DOF<"+str(self.getProp('SingleElectronTkChi2'))+")"                                
        TKQuality =  " & (TRCHI2DOF<"+str(self.getProp('SingleElectronTkChi2'))+")"                                

        ############################################################################
        #    Selection for a large PT single electron:  
        ############################################################################
        
        Hlt2SelSingleHighPTElectron = Hlt2Member(   FilterDesktop
                                                    , "Filter"
                                                    , Code = " (PT>"+str(self.getProp('SingleElectronHighPt'))+"*MeV) " + TKQuality
                                                    , InputLocations  = [BiKalmanFittedElectrons]
                                                    )
        line = Hlt2Line( 'SingleHighPTElectron'
                         , prescale = self.prescale 
                         , algos = [ BiKalmanFittedElectrons, Hlt2SelSingleHighPTElectron]
                         , postscale = self.postscale
                         )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleHighPTElectronDecision" : 50460 } )
        
        
        ############################################################################
        #    Selection for inclusive decays with high mass Drell-Yan ->e+track
        ############################################################################

        combine_DY1 = Hlt2Member( CombineParticles
                                  , "Combine"
                                  , DecayDescriptors = ["[Z0 -> e+ pi-]cc"] 
                                  , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DYeh1ePt'))+"*MeV)" + TKQuality ,     
                                                      "pi-" : "(PT>"+str(self.getProp('DYeh1trPt'))+"*MeV)" + TKQuality }
                                  , CombinationCut = "(AM >"+str(self.getProp('DYeh1MinMass'))+"*MeV)"\
                                  " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DYehDphiMin'))+")"
                                  , MotherCut = "(VFASPF(VCHI2/VDOF)<"+str(self.getProp('DYehVertexChi2'))+")"
                                  , InputLocations  = [ BiKalmanFittedElectrons , NoCutsPions ]
                                  )
        line1 = Hlt2Line( 'DYeh1'
                         , prescale = self.prescale 
                         , algos = [BiKalmanFittedElectrons, NoCutsPions, combine_DY1]
                         , postscale = self.postscale
                         )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYeh1Decision" : 50461 } )

        combine_DY2 = Hlt2Member( CombineParticles
                                  , "Combine"
                                  , DecayDescriptors = ["[Z0 -> e+ pi-]cc"] 
                                  , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DYeh2ePt'))+"*MeV)" + TKQuality ,     
                                                      "pi-" : "(PT>"+str(self.getProp('DYeh2trPt'))+"*MeV)" + TKQuality }
                                  , CombinationCut = "(AM>"+str(self.getProp('DYeh2MinMass'))+"*MeV)"\
                                  " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DYehDphiMin'))+")"
                                  , MotherCut = "(VFASPF(VCHI2/VDOF)<"+str(self.getProp('DYehVertexChi2'))+")"
                                  , InputLocations  = [ BiKalmanFittedElectrons , NoCutsPions ]
                                  )
        line2 = Hlt2Line( 'DYeh2'
                         , prescale = self.prescale 
                         , algos = [BiKalmanFittedElectrons, NoCutsPions, combine_DY2]
                         , postscale = self.postscale
                         )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYeh2Decision" : 50462 } )

 
        ############################################################################
        #    Now similar lines for unfitted electron tracks
        ############################################################################
        
        ############################################################################
        #    Selection for a large PT single electron:  
        ############################################################################
        
        Hlt2SelSingleHighPTElectronU = Hlt2Member(   FilterDesktop
                                                    , "Filter"
                                                    , Code = " (PT>"+str(self.getProp('SingleElectronHighPt'))+"*MeV) " + TKQuality
                                                    , InputLocations  = [Electrons]
                                                    )
        lineU = Hlt2Line( 'SingleHighPTElectronUnfitted'
                         , prescale = self.prescale 
                         , algos = [ Electrons, Hlt2SelSingleHighPTElectronU]
                         , postscale = self.postscale
                         )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleHighPTElectronUnfittedDecision" : 50450 } )
        
        
        ############################################################################
        #    Selections for inclusive decays with high mass Drell-Yan ->e+track
        ############################################################################

        combine_DY1U = Hlt2Member( CombineParticles
                                  , "Combine"
                                  , DecayDescriptors = ["[Z0 -> e+ pi-]cc"] 
                                  , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DYeh1ePt'))+"*MeV)" + TKQuality ,     
                                                      "pi-" : "(PT>"+str(self.getProp('DYeh1trPt'))+"*MeV)" + TKQuality }
                                  , CombinationCut = "(AM >"+str(self.getProp('DYeh1MinMass'))+"*MeV)"\
                                  " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DYehDphiMin'))+")"
                                  , MotherCut = "(VFASPF(VCHI2/VDOF)<"+str(self.getProp('DYehVertexChi2'))+")"
                                  , InputLocations  = [ Electrons , NoCutsPions ]
                                  )
        line1U = Hlt2Line( 'DYeh1Unfitted'
                         , prescale = self.prescale 
                         , algos = [Electrons, NoCutsPions, combine_DY1U]
                         , postscale = self.postscale
                         )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYeh1UnfittedDecision" : 50451 } )

        combine_DY2U = Hlt2Member( CombineParticles
                                  , "Combine"
                                  , DecayDescriptors = ["[Z0 -> e+ pi-]cc"] 
                                  , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DYeh2ePt'))+"*MeV)" + TKQuality ,     
                                                      "pi-" : "(PT>"+str(self.getProp('DYeh2trPt'))+"*MeV)" + TKQuality }
                                  , CombinationCut = "(AM>"+str(self.getProp('DYeh2MinMass'))+"*MeV)"\
                                  " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DYehDphiMin'))+")"
                                  , MotherCut = "(VFASPF(VCHI2/VDOF)<"+str(self.getProp('DYehVertexChi2'))+")"
                                  , InputLocations  = [ Electrons , NoCutsPions ]
                                  )
        line2U = Hlt2Line( 'DYeh2Unfitted'
                         , prescale = self.prescale 
                         , algos = [Electrons, NoCutsPions, combine_DY2U]
                         , postscale = self.postscale
                         )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYeh2UnfittedDecision" : 50452 } )

 


