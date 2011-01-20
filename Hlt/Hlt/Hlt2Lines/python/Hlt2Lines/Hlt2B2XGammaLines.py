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
                   ,'KstMassWinT'         : 100      # MeV 80
                   ,'KstMassWinSB'        : 200      # MeV 120
                   ,'BsMassWin'           : 1000     # MeV
                   ,'B0MassWin'           : 1000     # MeV
                   ,'BMassWinSB'          : 2000     # MeV
                   ,'BsDirAngle'          : 0.063  #0.998    # Dimensionless
                   ,'B0DirAngle'          : 0.045  #0.999    # Dimensionless
                   ,'BDirAngleMoni'       : 0.140 #0.99     # Dimentionless
                   ,'BsPVIPchi2'          : 25       # Dimensionless
                   ,'B0PVIPchi2'          : 25       # Dimensionless
                   ,'photonPT'            : 2600     # MeV
                   ,'PhiVCHI2'            : 25       # dimensionless
                   ,'KstVCHI2'            : 16       # dimensionless
                   ,'TrChi2'              : 10.       # dimensionless
                   ,'HLT1FILTER'         : "HLT_PASS_RE('Hlt1.*Photon.*Decision')"
                   
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
        from Hlt2SharedParticles.GoodParticles import GoodKaons, GoodPions
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotons
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions
        from Hlt2SharedParticles.Phi import Phi2KK
        from Hlt2SharedParticles.Kstar import  Kstar2KPi
      
        hltfilter = self.getProp("HLT1FILTER")
        if hltfilter == "" : hltfilter = None
        
        ############################################################################
        #    Make Phi and Kstar candidates
        ############################################################################

        Hlt2Phi4PhiGamma = Hlt2Member( CombineParticles
                                       , "CombinePhi"
                                       , DecayDescriptors =[ "phi(1020) -> K+ K-" ]
                                       , DaughtersCuts = { "K+" : "(MIPCHI2DV(PRIMARY)>%(TrIPchi2Phi)s)" % self.getProps() }
                                       , CombinationCut =  "(ADAMASS('phi(1020)')<%(PhiMassWinL)s*MeV)" % self.getProps()
                                       , MotherCut = "(VFASPF(VCHI2) < %(PhiVCHI2)s) & (ADMASS('phi(1020)')<%(PhiMassWinT)s*MeV)" % self.getProps()
                                       #, ParticleCombiners = {'' : 'TrgVertexFitter'}
                                       , InputLocations  = [ BiKalmanFittedKaons ]
                                       )
        
        Phi4PhiGammaFilter = Hlt2Member( FilterDesktop
                                         ,"FilterPhi2KK"
                                         ,Code = " (MINTREE ( Q != 0 , MIPCHI2DV(PRIMARY) ) > %(TrIPchi2Phi)s ) "\
                                               " & (INTREE ( ( Q != 0 ) & ( TRCHI2DOF < %(TrChi2)s ) ) ) "\
                                               " & (INTREE ( (ABSID=='phi(1020)') & (ADMASS('phi(1020)')< %(PhiMassWinT)s) ) ) "\
                                               " & (INTREE ( (ABSID=='phi(1020)') & (VFASPF(VCHI2)< %(PhiVCHI2)s))) " % self.getProps()
                                         , InputLocations  = [ Phi2KK ]
                                         )
        
        
        Hlt2Kst4KstGamma = Hlt2Member( CombineParticles
                                       , "CombineKstar"
                                       , DecayDescriptors =["[K*(892)0 -> K+ pi-]cc"] 
                                       , DaughtersCuts  = { "K+"  : "(MIPCHI2DV(PRIMARY)>%(TrIPchi2Kst)s)" % self.getProps(),
                                                            "pi-" : "(MIPCHI2DV(PRIMARY)>%(TrIPchi2Kst)s)" % self.getProps()}
                                       , CombinationCut =  "(ADAMASS('K*(892)0')<%(KstMassWinL)s*MeV)" % self.getProps()
                                       , MotherCut      =  "(VFASPF(VCHI2) < %(KstVCHI2)s) & (ADMASS('K*(892)0')< %(KstMassWinT)s*MeV)" % self.getProps()
                                       #, ParticleCombiners = {'' : 'TrgVertexFitter'}
                                       , InputLocations  = [ BiKalmanFittedKaons, BiKalmanFittedPions ]
                                       )

        Kst4KstGammaFilter = Hlt2Member( FilterDesktop
                                         ,"FilterKstar2KPi"
                                         ,Code = " (MINTREE ( Q != 0 , MIPCHI2DV(PRIMARY) ) > %(TrIPchi2Kst)s ) "\
                                               " & (INTREE ( ( Q != 0 ) & ( TRCHI2DOF < %(TrChi2)s) ) )" \
                                               " & (INTREE ( (ABSID=='K*(892)0') & (ADMASS('K*(892)0')< %(KstMassWinT)s )) ) " \
                                               " & (INTREE ( (ABSID=='K*(892)0') & (VFASPF(VCHI2)< %(KstVCHI2)s)))" % self.getProps()
                                         , InputLocations  = [ Kstar2KPi] 
                                         )
        
        ############################################################################
        #   Bs -> Phi Gamma
        ############################################################################
        
        Hlt2BstoPhiGamma = Hlt2Member(CombineParticles
                                      , "CombineBs"
                                      , DecayDescriptors = ["B_s0 -> gamma phi(1020)"]
                                      , DaughtersCuts = { "gamma"    : "(PT>%(photonPT)s*MeV)" % self.getProps()}
                                      #"phi(1020)": "(ADMASS('phi(1020)')< %(PhiMassWinT)s*MeV)" % self.getProps()}
                                      , CombinationCut =  "(ADAMASS('B_s0')<%(BsMassWin)s*MeV)" % self.getProps()
                                      , MotherCut = "((acos(BPVDIRA) < %(BsDirAngle)s) & (BPVIPCHI2()< %(BsPVIPchi2)s))" % self.getProps() # TODO: take cos of left most expression...
                                      #, ParticleCombiners = {'' : 'TrgVertexFitter'}
                                      , InputLocations  = [BiKalmanFittedPhotons,  Phi4PhiGammaFilter] #  Hlt2Phi4PhiGamma] #  
                                      )
        
        
        
        ############################################################################
        #   B0 -> Kstar Gamma
        ############################################################################
        
        Hlt2BdtoKstGamma = Hlt2Member(CombineParticles
                                      , "CombineB0"
                                      , DecayDescriptors = ["[B0 -> K*(892)0  gamma]cc"]
                                      , DaughtersCuts = { "gamma"    : "(PT>%(photonPT)s*MeV)"%self.getProps()}
                                      #"K*(892)0" : "(ADMASS('K*(892)0')<%(KstMassWinT)s*MeV)"%self.getProps() }
                                      , CombinationCut =  "(ADAMASS('B0')<%(B0MassWin)s*MeV)" % self.getProps()
                                      , MotherCut = "( (acos(BPVDIRA) < %(B0DirAngle)s) & (BPVIPCHI2()<%(B0PVIPchi2)s))"%self.getProps() # TODO: take cos of left most expression
                                      #, ParticleCombiners = {'' : 'TrgVertexFitter'}
                                      , InputLocations  = [ BiKalmanFittedPhotons, Kst4KstGammaFilter ] #  Hlt2Kst4KstGamma]# 
                                     )
        
        
        ############################################################################
        #    Bs to Phi Gamma - Hlt2Line
        ############################################################################

        line = Hlt2Line('Bs2PhiGamma'
                        , prescale = self.prescale
                        , HLT = hltfilter
                        , algos = [  PV3D(),
                                     Phi2KK,
                                     Phi4PhiGammaFilter, 
                                     BiKalmanFittedPhotons,
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
                   , algos = [  PV3D(), BiKalmanFittedKaons, Hlt2Phi4PhiGamma, BiKalmanFittedPhotons, Hlt2BstoPhiGamma]
                   #, CombinePhi = {"InputLocations" : [BiKalmanFittedKaons] }
                   , CombineBs = {"InputLocations" : [BiKalmanFittedPhotons, Hlt2Phi4PhiGamma]}
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2PhiGammaNoCutsKDecision" :  self.getProp('HltANNSvcID')['Bs2PhiGammaNoCutsK'] } )
        
        #   Bs mass sideband
        line.clone('Bs2PhiGammaWideBMass'
                   , prescale = self.prescale
                   , algos = [ PV3D(), Phi2KK, Phi4PhiGammaFilter, BiKalmanFittedPhotons, Hlt2BstoPhiGamma] 
                   , CombineBs = {"CombinationCut" : "(ADAMASS('B_s0')<%(BMassWinSB)s*MeV)"%self.getProps()  }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2PhiGammaWideBMassDecision" : self.getProp('HltANNSvcID')['Bs2PhiGammaWideBMass'] } )

        #  Bs dira monitoring
        line.clone('Bs2PhiGammaLooseDira'
                   , prescale = self.prescale
                   , algos = [  PV3D(), Phi2KK, Phi4PhiGammaFilter, BiKalmanFittedPhotons, Hlt2BstoPhiGamma]
                   , CombineBs = {"MotherCut" : "( (acos(BPVDIRA) < %(BDirAngleMoni)s) & (BPVIPCHI2()<%(BsPVIPchi2)s))" % self.getProps()   } # TODO: take cos of left most expression
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2PhiGammaLooseDiraDecision" : self.getProp('HltANNSvcID')['Bs2PhiGammaLooseDira'] } )
        
        ############################################################################
        #    B0 to Kstar Gamma - Hlt2Line
        ############################################################################
        
        line = Hlt2Line('Bd2KstGamma'
                        , prescale = self.prescale
                        , HLT = hltfilter
                        , algos = [  PV3D(),
                                     Kstar2KPi,
                                     Kst4KstGammaFilter, 
                                     BiKalmanFittedPhotons,
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
                   , algos = [ PV3D(), BiKalmanFittedKaons, BiKalmanFittedPions, Hlt2Kst4KstGamma, BiKalmanFittedPhotons, Hlt2BdtoKstGamma]
                  # , CombineKstar = {"InputLocations" : [BiKalmanFittedKaons, BiKalmanFittedPions] }
                   , CombineB0 = {"InputLocations" : [BiKalmanFittedPhotons,  Hlt2Kst4KstGamma]}
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2KstGammaNoCutsKPiDecision" : self.getProp('HltANNSvcID')['Bd2KstGammaNoCutsKPi'] } )
        #  Kst mass sideband
        line.clone('Bd2KstGammaWideKMass'
                   , prescale = self.prescale
                   , algos = [ PV3D(), Kstar2KPi, Kst4KstGammaFilter,  BiKalmanFittedPhotons,  Hlt2BdtoKstGamma] 
                   , FilterKstar2KPi = {"Code" : " (MINTREE ( Q != 0 , MIPCHI2DV(PRIMARY) ) > %(TrIPchi2Kst)s) " \
                                               " & (INTREE ( (ABSID=='K*(892)0') & (ADMASS('K*(892)0')< %(KstMassWinL)s) ) ) "\
                                               " & (INTREE ( (ABSID=='K*(892)0') & (VFASPF(VCHI2PDOF)< %(KstVCHI2)s))) " % self.getProps() } # TODO: coalesce the last two INTREE
                   #, CombineKstar = {"MotherCut" : "(VFASPF(VCHI2) < %(KstVCHI2)s) & (ADMASS('K*(892)0')<%(KstMassWinL)s*MeV)"% self.getProps()}
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2KstGammaWideKMassDecision" :  self.getProp('HltANNSvcID')['Bd2KstGammaWideKMass'] } )
        # B0 mass sideband
        line.clone('Bd2KstGammaWideBMass'
                   , prescale = self.prescale
                   , algos = [ PV3D(), Kstar2KPi, Kst4KstGammaFilter, BiKalmanFittedPhotons, Hlt2BdtoKstGamma]
                   , CombineB0 = { "CombinationCut" :  "(ADAMASS('B0')<%(BMassWinSB)s*MeV)" %self.getProps() }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2KstGammaWideBMassDecision" :  self.getProp('HltANNSvcID')['Bd2KstGammaWideBMass'] } )
        # B0 dira monitoring
        line.clone('Bd2KstGammaLooseDira'
                   , prescale = self.prescale
                   , algos = [ PV3D(), Kstar2KPi, Kst4KstGammaFilter, BiKalmanFittedPhotons, Hlt2BdtoKstGamma]
                   , CombineB0 = { "MotherCut" :  "( (acos(BPVDIRA) < %(BDirAngleMoni)s) & (BPVIPCHI2()<%(B0PVIPchi2)s))" %self.getProps() } # TODO: take cos of left most expression
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2KstGammaLooseDiraDecision" :  self.getProp('HltANNSvcID')['Bd2KstGammaLooseDira'] } )
