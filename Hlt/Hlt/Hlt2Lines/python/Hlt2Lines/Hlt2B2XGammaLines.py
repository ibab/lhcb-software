# Hlt2 Bs->PhiGamma and B->Kstar Gamma selections 06/23/09
#
# Fatima Soomro( Fatima.Soomro@cern.ch)
#
# 


from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2B2XGammaLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = {  'TrIPchi2Phi'          : 10       # Dimensionless
                   ,'TrIPchi2Kst'         : 10       # Dimensionless
                   ,'PhiMassWinL'         : 40       # MeV
                   ,'PhiMassWinT'         : 20       # MeV
                   ,'KstMassWinL'         : 150      # MeV
                   ,'KstMassWinT'         : 80       # MeV
                   ,'KstMassWinSB'        : 120      # MeV
                   ,'BsMassWin'           : 1000     # MeV
                   ,'B0MassWin'           : 1000     # MeV
                   ,'BMassWinSB'          : 2000     # MeV
                   ,'BsDirAngle'          : 0.998    # Dimensionless
                   ,'B0DirAngle'          : 0.999    # Dimensionless
                   ,'BDirAngleMoni'       : 0.99     # Dimentionless
                   ,'BsPVIPchi2'          : 25       # Dimensionless
                   ,'B0PVIPchi2'          : 25       # Dimensionless
                   ,'photonPT'            : 2600     # MeV
                   ,'PhiVCHI2'            : 25       # dimensionless
                   ,'KstVCHI2'            : 16       # dimensionless
                   ,'TrChi2'              : 10.       # dimensionless
                   ,'Prescale'           : {'Hlt2Bs2PhiGamma$'          : 1.0
                                            ,'Hlt2Bs2PhiGamma.+'        : 0.1       # prescale by a factor of 10
                                            ,'Hlt2Bd2KstGamma$'         : 1.0
                                            ,'Hlt2Bd2KstGamma.+'        : 0.05      # prescale by a factor of 20
                                            }
                   
                   ,'Postscale'          : {'Hlt2Bs2PhiGamma$'          : 1.0
                                            ,'Hlt2Bs2PhiGamma.+': 1.0
                                            ,'Hlt2Bd2KstGamma$'         : 1.0
                                            ,'Hlt2Bd2KstGamma.+': 1.0
                                            }
                   ,'HltANNSvcID'        : {'Bs2PhiGamma'            : 50500
                                            ,'Bs2PhiGammaNoCutsK'    : 50501
                                            ,'Bs2PhiGammaWideBMass'  : 50502
                                            ,'Bs2PhiGammaLooseDira'  : 50503
                                            ,'Bd2KstGamma'           : 50510
                                            ,'Bd2KstGammaNoCutsKPi'  : 50511
                                            ,'Bd2KstGammaWideKMass'  : 50512
                                            ,'Bd2KstGammaWideBMass'  : 50513
                                            ,'Bd2KstGammaLooseDira'  : 50514
                                            }     

                   }
    
    
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, PhysDesktop
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.BasicParticles import Photons
        from Hlt2SharedParticles.BasicParticles import NoCutsKaons, NoCutsPions
        from Hlt2SharedParticles.TrackFittedBasicParticles import  BiKalmanFittedKaons, BiKalmanFittedPions
      
      
        ############################################################################
        #    Make Phi and Kstar candidates
        ############################################################################

        Hlt2Phi4PhiGamma = Hlt2Member( CombineParticles
                                       , "CombinePhi"
                                       , DecayDescriptors =[ "phi(1020) -> K+ K-" ] #decayDesc
                                       , DaughtersCuts = { "K+" : "(TRCHI2DOF <"+str(self.getProp('TrChi2'))+")&(MIPCHI2DV(PRIMARY)>"+str(self.getProp('TrIPchi2Phi'))+")" }
                                       , CombinationCut =  "(ADAMASS('phi(1020)')<"+str(self.getProp('PhiMassWinL'))+"*MeV)"
                                       , MotherCut = "( VFASPF(VCHI2) < "+str(self.getProp('PhiVCHI2'))+")"
                                       , ParticleCombiners = {'' : 'TrgVertexFitter'}
                                        , InputLocations  = [ BiKalmanFittedKaons ]
                                       )
        
        
        Hlt2Kst4KstGamma = Hlt2Member( CombineParticles
                                       , "CombineKstar"
                                       , DecayDescriptors =["[K*(892)0 -> K+ pi-]cc"] 
                                       , DaughtersCuts = { "K+"  : "(TRCHI2DOF <"+str(self.getProp('TrChi2'))+")&(MIPCHI2DV(PRIMARY)>"+str(self.getProp('TrIPchi2Kst'))+")",
                                                           "pi-" : "(TRCHI2DOF <"+str(self.getProp('TrChi2'))+")&(MIPCHI2DV(PRIMARY)>"+str(self.getProp('TrIPchi2Kst'))+")"}
                                       , CombinationCut =  "(ADAMASS('K*(892)0')<"+str(self.getProp('KstMassWinL'))+"*MeV)"
                                       , MotherCut = "( VFASPF(VCHI2) < "+str(self.getProp('KstVCHI2'))+")"
                                       , ParticleCombiners = {'' : 'TrgVertexFitter'} 
                                       , InputLocations  = [ BiKalmanFittedKaons, BiKalmanFittedPions ]
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
                                      , ParticleCombiners = {'' : 'TrgVertexFitter'}
                                      , InputLocations  = [ Hlt2Phi4PhiGamma, Photons ]
                                      )
        
        
        
        ############################################################################
        #   B0 -> Kstar Gamma
        ############################################################################
        
        Hlt2BdtoKstGamma = Hlt2Member(CombineParticles
                                     , "CombineB0"
                                     , DecayDescriptors = ["[B0 -> K*(892)0  gamma]cc"]
                                     , DaughtersCuts = { "gamma"    : "(PT>"+str(self.getProp('photonPT'))+"*MeV)",
                                                         "K*(892)0" : "(ADMASS('K*(892)0')<"+str(self.getProp('KstMassWinT'))+"*MeV) " }
                                     , CombinationCut =  "(ADAMASS('B0')<"+str(self.getProp('B0MassWin'))+"*MeV)"
                                     , MotherCut = "( (BPVDIRA > " + str(self.getProp('B0DirAngle'))+") & (BPVIPCHI2()<" + str(self.getProp('B0PVIPchi2'))+"))"
                                     , ParticleCombiners = {'' : 'TrgVertexFitter'}
                                     , InputLocations  = [ Hlt2Kst4KstGamma, Photons ]
                                     )
        
        
        ############################################################################
        #    Bs to Phi Gamma - Hlt2Line
        ############################################################################

        line = Hlt2Line('Bs2PhiGamma'
                        , prescale = self.prescale
                        , algos = [ BiKalmanFittedKaons, 
                                    PV3D(),
                                    Hlt2Phi4PhiGamma, 
                                    Photons,
                                    Hlt2BstoPhiGamma]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2PhiGammaDecision" : self.getProp('HltANNSvcID')['Bs2PhiGamma']} )

        
        ############################################################################
        #    Bs to Phi Gamma - Monitoring Lines
        ############################################################################
        #   use noCuts particles
        line.clone('Bs2PhiGammaNoCutsK'
                   , prescale = self.prescale
                   , algos = [ NoCutsKaons, PV3D(), Hlt2Phi4PhiGamma, Photons, Hlt2BstoPhiGamma]
                   , CombinePhi = {"InputLocations" : [NoCutsKaons] }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2PhiGammaNoCutsKDecision" :  self.getProp('HltANNSvcID')['Bs2PhiGammaNoCutsK'] } )
        
        #   Bs mass sideband
        line.clone('Bs2PhiGammaWideBMass'
                   , prescale = self.prescale
                   , algos = [BiKalmanFittedKaons, PV3D(), Hlt2Phi4PhiGamma, Photons, Hlt2BstoPhiGamma]
                   , CombineBs = {"CombinationCut" :  "(ADAMASS('B_s0')<"+str(self.getProp('BMassWinSB'))+"*MeV)"  }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2PhiGammaWideBMassDecision" : self.getProp('HltANNSvcID')['Bs2PhiGammaWideBMass'] } )

        #  Bs dira monitoring
        line.clone('Bs2PhiGammaLooseDira'
                   , prescale = self.prescale
                   , algos = [ BiKalmanFittedKaons, PV3D(), Hlt2Phi4PhiGamma, Photons, Hlt2BstoPhiGamma]
                   , CombineBs = {"MotherCut" : "( (BPVDIRA > " + str(self.getProp('BDirAngleMoni'))+") & (BPVIPCHI2()<" + str(self.getProp('BsPVIPchi2'))+"))"   }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2PhiGammaLooseDiraDecision" : self.getProp('HltANNSvcID')['Bs2PhiGammaLooseDira'] } )
        
        ############################################################################
        #    B0 to Kstar Gamma - Hlt2Line
        ############################################################################
        
        line = Hlt2Line('Bd2KstGamma'
                        , prescale = self.prescale
                        , algos = [ BiKalmanFittedKaons,
                                    BiKalmanFittedPions,
                                    PV3D(),
                                    Hlt2Kst4KstGamma, 
                                    Photons,
                                    Hlt2BdtoKstGamma]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2KstGammaDecision" :self.getProp('HltANNSvcID')['Bd2KstGamma'] } )
        
        ############################################################################
        #    B0 to Kstar Gamma - Monitoring Lines
        ############################################################################
             
        # use  noCuts particles
        line.clone('Bd2KstGammaNoCutsKPi'
                   , prescale = self.prescale
                   , algos = [ NoCutsKaons, NoCutsPions, PV3D(), Hlt2Kst4KstGamma, Photons, Hlt2BdtoKstGamma]
                   , CombineKstar = {"InputLocations" : [NoCutsKaons, NoCutsPions] }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2KstGammaNoCutsKPiDecision" : self.getProp('HltANNSvcID')['Bd2KstGammaNoCutsKPi'] } )
        #  Kst mass sideband
        line.clone('Bd2KstGammaWideKMass'
                   , prescale = self.prescale
                   , algos = [ BiKalmanFittedKaons, BiKalmanFittedPions, PV3D(), Hlt2Kst4KstGamma, Photons, Hlt2BdtoKstGamma]
                   , CombineB0   = { "DaughtersCuts" :{ "gamma"    : "(PT>"+str(self.getProp('photonPT'))+"*MeV)",
                                                        "K*(892)0" : "(ADMASS('K*(892)0')<"+str(self.getProp('KstMassWinSB'))+"*MeV) " } }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2KstGammaWideKMassDecision" :  self.getProp('HltANNSvcID')['Bd2KstGammaWideKMass'] } )
        # B0 mass sideband
        line.clone('Bd2KstGammaWideBMass'
                   , prescale = self.prescale
                   , algos = [BiKalmanFittedKaons, BiKalmanFittedPions, PV3D(), Hlt2Kst4KstGamma, Photons, Hlt2BdtoKstGamma]
                   , CombineB0 = { "CombinationCut" :  "(ADAMASS('B0')<"+str(self.getProp('BMassWinSB'))+"*MeV)" }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2KstGammaWideBMassDecision" :  self.getProp('HltANNSvcID')['Bd2KstGammaWideBMass'] } )
        # B0 dira monitoring
        line.clone('Bd2KstGammaLooseDira'
                   , prescale = self.prescale
                   , algos = [BiKalmanFittedKaons, BiKalmanFittedPions, PV3D(), Hlt2Kst4KstGamma, Photons, Hlt2BdtoKstGamma]
                   , CombineB0 = { "MotherCut" :  "( (BPVDIRA > " + str(self.getProp('BDirAngleMoni'))+") & (BPVIPCHI2()<" + str(self.getProp('B0PVIPchi2'))+"))" }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2KstGammaLooseDiraDecision" :  self.getProp('HltANNSvcID')['Bd2KstGammaLooseDira'] } )
