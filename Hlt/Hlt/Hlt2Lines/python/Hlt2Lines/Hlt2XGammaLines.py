# Hlt2 Bs->PhiGamma and B->Kstar Gamma selections 06/23/09
#
# Fatima Soomro( Fatima.Soomro@cern.ch)
#
# 


from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from HltLine.HltLine import Hlt2Line
from HltLine.HltLine import Hlt2Member


class Hlt2XGammaLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = {  'TrIPchi2Phi'          : 20       # Dimensionless
                   ,'TrIPchi2Kst'         : 20       # Dimensionless
                   ,'PhiMassWinL'         : 40       # MeV
                   ,'PhiMassWinT'         : 30       # MeV
                   ,'KstMassWinL'         : 150      # MeV
                   ,'KstMassWinT'         : 120      # MeV
                   ,'BsMassWin'           : 1000     # MeV
                   ,'B0MassWin'           : 1000     # MeV
                   ,'BsDirAngle'          : 0.998    # Dimensionless
                   ,'B0DirAngle'          : 0.999    # Dimensionless
                   ,'BsPVIPchi2'          : 25       # Dimensionless
                   ,'B0PVIPchi2'          : 25       # Dimensionless
                   ,'photonPT'            : 2600     # MeV
                   ,'PhiVCHI2'            : 25       # dimensionless
                   ,'KstVCHI2'            : 16       # dimensionless
                   
                   , 'IncludeLines' :['Hlt2KstGamma',
                                      'Hlt2PhiGamma'
                                      ]
                   }
    
    
    def __apply_configuration__(self) :
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, PhysDesktop
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.GoodParticles import GoodKaons, GoodPions
        from Hlt2SharedParticles.BasicParticles import Photons

      
      
        ############################################################################
        #    Make Phi and Kstar candidates
        ############################################################################

        Hlt2Phi4PhiGamma = Hlt2Member( CombineParticles
                                       , "CombinePhi"
                                       , DecayDescriptors =[ "phi(1020) -> K+ K-" ] #decayDesc
                                       , DaughtersCuts = { "K+" : "(MIPCHI2DV(PRIMARY)>"+str(self.getProp('TrIPchi2Phi'))+")" }
                                       , CombinationCut =  "(ADAMASS('phi(1020)')<"+str(self.getProp('PhiMassWinL'))+"*MeV)"
                                       , MotherCut = "( VFASPF(VCHI2) < "+str(self.getProp('PhiVCHI2'))+")"
                                       , InputLocations  = [ GoodKaons ]
                                       )
        
        
        Hlt2Kst4KstGamma = Hlt2Member( CombineParticles
                                       , "CombineKstar"
                                       , DecayDescriptors =["[K*(892)0 -> K+ pi-]cc"] 
                                       , DaughtersCuts = { "K+"  : "(MIPCHI2DV(PRIMARY)>"+str(self.getProp('TrIPchi2Kst'))+")",
                                                           "pi-" : "(MIPCHI2DV(PRIMARY)>"+str(self.getProp('TrIPchi2Kst'))+")"}
                                       , CombinationCut =  "(ADAMASS('K*(892)0')<"+str(self.getProp('KstMassWinL'))+"*MeV)"
                                       , MotherCut = "( VFASPF(VCHI2) < "+str(self.getProp('KstVCHI2'))+")"
                                       , InputLocations  = [ GoodKaons, GoodPions ]
                                       )

     

        ############################################################################
        #   Bs -> Phi Gamma
        ############################################################################
        
        Hlt2BstoPhiGamma = Hlt2Member(CombineParticles
                                      , "CombineBs"
                                      , DecayDescriptors = ["[B_s0 -> gamma phi(1020)]cc"]
                                      , DaughtersCuts = { "gamma"    : "(PT>"+str(self.getProp('photonPT'))+"*MeV)",
                                                          "phi(1020)": "(ADMASS('phi(1020)')<"+str(self.getProp('PhiMassWinT'))+"*MeV)"}
                                      , CombinationCut =  "(ADAMASS('B_s0')<"+str(self.getProp('BsMassWin'))+"*MeV)"
                                      , MotherCut = "( (BPVDIRA > " + str(self.getProp('BsDirAngle'))+") & (BPVIPCHI2()<" + str(self.getProp('BsPVIPchi2'))+"))"
                                      , InputLocations  = [ Hlt2Phi4PhiGamma, Photons ]
                                      )
        
        
        
        ############################################################################
        #   B0 -> Kstar Gamma
        ############################################################################
        
        Hlt2BtoKstGamma = Hlt2Member(CombineParticles
                                     , "CombineBs"
                                     , DecayDescriptors = ["[B0 -> K*(892)0  gamma]cc"]
                                     , DaughtersCuts = { "gamma"    : "(PT>"+str(self.getProp('photonPT'))+"*MeV)",
                                                         "K*(892)0" : "(ADMASS('K*(892)0')<"+str(self.getProp('KstMassWinT'))+"*MeV) " }
                                     , CombinationCut =  "(ADAMASS('B0')<"+str(self.getProp('B0MassWin'))+"*MeV)"
                                     , MotherCut = "( (BPVDIRA > " + str(self.getProp('B0DirAngle'))+") & (BPVIPCHI2()<" + str(self.getProp('B0PVIPchi2'))+"))"
                                     , InputLocations  = [ Hlt2Kst4KstGamma, Photons ]
                                     )
        
        
        ############################################################################
        #    Bs to Phi Gamma - Hlt2Line
        ############################################################################

        line = Hlt2Line('PhiGamma'
                        , prescale = self.prescale
                        , algos = [ GoodKaons, 
                                    Hlt2Phi4PhiGamma, 
                                    Photons,
                                    Hlt2BstoPhiGamma]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2PhiGammaDecision" : 50500} )

        
        ############################################################################
        #    B0 to Kstar Gamma - Hlt2Line
        ############################################################################

        line = Hlt2Line('KstGamma'
                        , prescale = self.prescale
                        , algos = [ GoodKaons,
                                    GoodPions,
                                    Hlt2Kst4KstGamma, 
                                    Photons,
                                    Hlt2BtoKstGamma]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2KstGammaDecision" : 50510} )
