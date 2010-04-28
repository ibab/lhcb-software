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


