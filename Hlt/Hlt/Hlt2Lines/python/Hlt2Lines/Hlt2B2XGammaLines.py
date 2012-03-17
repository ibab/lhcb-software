# HLT2 Bs->PhiGamma and B->Kstar Gamma selections
#
# Fatima Soomro( Fatima.Soomro@cern.ch)
# Albert Puig (Albert.Puig@cern.ch)
# 

__author__ = ['Fatima Soomro', 'Albert Puig']
__date__ = '28/02/2011'
__version__ = '$Revision: 1.8 $'

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2XGammaLinesConf(HltLinesConfigurableUser) :
    __slots__ = { # Common cuts    
                    'TrChi2'       : 5.       # dimensionless
                   ,'TrPT'         : 500      # MeV
                   ,'photonPT'     : 2600     # MeV
                   ,'BDirAngleMoni': 0.140 #0.99     # Dimentionless
                  # Bs2PhiGamma    
                   ,'TrIPchi2Phi'  : 20       # Dimensionless
                   ,'PhiVCHI2'     : 25       # dimensionless
                   ,'PhiMassWinL'  : 40       # MeV
                   ,'PhiMassWinT'  : 20       # MeV
                   ,'BsPVIPchi2'   : 12       # Dimensionless
                   ,'BsMassWin'    : 1000     # MeV
                   ,'BsDirAngle'   : 0.063  #0.998    # Dimensionless
                  # Bd2KstGamma    
                   ,'TrIPchi2Kst'  : 20       # Dimensionless
                   ,'KstVCHI2'     : 16       # dimensionless
                   ,'KstMassWinL'  : 125      # MeV
                   ,'KstMassWinT'  : 100      # MeV 80
                   ,'KstMassWinSB' : 200      # MeV 120
                   ,'B0PVIPchi2'   : 12       # Dimensionless
                   ,'B0MassWin'    : 1000     # MeV
                   ,'BMassWinSB'   : 2000     # MeV
                   ,'B0DirAngle'   : 0.045  #0.999    # Dimensionless
                   ,'B_PT'         : 2000     # MeV
                  # Photons
                   ,'UseL0Photons' : True
                  # HLT filter
                   ,'HLT1FILTER'   : "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!Velo)(?!BeamGas)(?!Incident).*Decision')" 
                   ,'L0FILTER'     : "|".join( [ "L0_CHANNEL('%s')" % channel for channel in ['Photon','Electron'] ] ) 
                  # Pre- and postscale
                   ,'Prescale'     : { 'Hlt2Bs2PhiGamma$'   : 1.0
                                      ,'Hlt2Bs2PhiGamma.+' : 0.1       # prescale by a factor of 10
                                      ,'Hlt2Bd2KstGamma$'  : 1.0
                                      ,'Hlt2Bd2KstGamma.+' : 0.05      # prescale by a factor of 20
                                      }
                                   
                   ,'Postscale'    : {'Hlt2Bs2PhiGamma$'   : 1.0
                                      ,'Hlt2Bs2PhiGamma.+' : 1.0
                                      ,'Hlt2Bd2KstGamma$'  : 1.0
                                      ,'Hlt2Bd2KstGamma.+' : 1.0
                                      }
                  # IDs            
                   ,'HltANNSvcID'  : {'Bs2PhiGamma'            : 50500
                                      ,'Bs2PhiGammaWideBMass'  : 50502
                                      ,'Bs2PhiGammaLooseDira'  : 50503
                                      ,'Bd2KstGamma'           : 50510
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
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        # from Hlt2SharedParticles.GoodParticles import GoodKaons, GoodPions
        if self.getProp('UseL0Photons'):
            from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotonsFromL0 as Photons
            from HltLine.HltDecodeRaw import DecodeL0CALO
            photonAlgos = [ DecodeL0CALO, Photons ]
        else:
            from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotons as Photons
            photonAlgos = [Photons]
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions
        # from Hlt2SharedParticles.Phi import Phi2KK
        # from Hlt2SharedParticles.Kstar import  Kstar2KPi
      
        hltfilter = self.getProp("HLT1FILTER")
        if hltfilter == "" : hltfilter = None
        
        l0filter = self.getProp("L0FILTER")
        if l0filter == "" : l0filter = None
        
        ############################################################################
        #    Make Phi and Kstar candidates
        ############################################################################
        Hlt2Phi4PhiGamma = Hlt2Member( CombineParticles
                                       , "CombinePhi"
                                       , DecayDescriptors = [ "phi(1020) -> K+ K-" ]
                                       , DaughtersCuts = { "K+": "(TRCHI2DOF<%(TrChi2)s) & (MIPCHI2DV(PRIMARY)>%(TrIPchi2Phi)s) & (PT>%(TrPT)s*MeV)" % self.getProps(),
                                                           "K-": "(TRCHI2DOF<%(TrChi2)s) & (MIPCHI2DV(PRIMARY)>%(TrIPchi2Phi)s) & (PT>%(TrPT)s*MeV)" % self.getProps()  }
                                       , CombinationCut = "(ADAMASS('phi(1020)')<%(PhiMassWinL)s*MeV)" % self.getProps()
                                       , MotherCut = "(VFASPF(VCHI2)<%(PhiVCHI2)s) & (ADMASS('phi(1020)')<%(PhiMassWinT)s*MeV)" % self.getProps()
                                       , Inputs = [ BiKalmanFittedKaons ]
                                       )
        
        Hlt2Kst4KstGamma = Hlt2Member( CombineParticles
                                       , "CombineKstar"
                                       , DecayDescriptors =["[K*(892)0 -> K+ pi-]cc"] 
                                       , DaughtersCuts  = { "K+" : "(TRCHI2DOF<%(TrChi2)s) & (MIPCHI2DV(PRIMARY)>%(TrIPchi2Kst)s) & (PT>%(TrPT)s*MeV)" % self.getProps(),
                                                            "pi-": "(TRCHI2DOF<%(TrChi2)s) & (MIPCHI2DV(PRIMARY)>%(TrIPchi2Kst)s) & (PT>%(TrPT)s*MeV)" % self.getProps() }
                                       , CombinationCut =  "(ADAMASS('K*(892)0')<%(KstMassWinL)s*MeV)" % self.getProps()
                                       , MotherCut      =  "(VFASPF(VCHI2)<%(KstVCHI2)s) & (ADMASS('K*(892)0')<%(KstMassWinT)s*MeV)" % self.getProps()
                                       , Inputs  = [ BiKalmanFittedKaons, BiKalmanFittedPions ]
                                       )

        ############################################################################
        #   Bs -> Phi Gamma
        ############################################################################
        Hlt2BstoPhiGamma = Hlt2Member(CombineParticles
                                      , "CombineBs"
                                      , DecayDescriptors = ["B_s0 -> gamma phi(1020)"]
                                      , DaughtersCuts = { "gamma": "(PT>%(photonPT)s*MeV)" % self.getProps() }
                                      , CombinationCut = "(ADAMASS('B_s0')<1.5*%(BsMassWin)s*MeV)" % self.getProps()
                                      , MotherCut = "((BPVDIRA > cos(%(BsDirAngle)s)) & (BPVIPCHI2()< %(BsPVIPchi2)s) & (ADMASS('B_s0')<%(BsMassWin)s*MeV) & (PT > %(B_PT)s*MeV))" % self.getProps()
                                      , Preambulo = ["from math import cos"]
                                      , Inputs  = [ Photons,  Hlt2Phi4PhiGamma ] #  
                                      )
        
        ############################################################################
        #   B0 -> Kstar Gamma
        ############################################################################
        Hlt2BdtoKstGamma = Hlt2Member(CombineParticles
                                      , "CombineB0"
                                      , DecayDescriptors = ["[B0 -> K*(892)0  gamma]cc"]
                                      , DaughtersCuts = { "gamma": "(PT>%(photonPT)s*MeV)" % self.getProps() }
                                      , CombinationCut =  "(ADAMASS('B0')<1.5*%(B0MassWin)s*MeV)" % self.getProps()
                                      , MotherCut = "((BPVDIRA > cos(%(B0DirAngle)s)) & (BPVIPCHI2()<%(B0PVIPchi2)s) & (ADMASS('B0')<%(B0MassWin)s*MeV) & (PT > %(B_PT)s*MeV))" % self.getProps()
                                      , Preambulo = ["from math import cos"]
                                      , Inputs  = [ Photons, Hlt2Kst4KstGamma ]# 
                                     )
        
        
        ############################################################################
        #    Bs to Phi Gamma - Hlt2Line
        ############################################################################
        line = Hlt2Line('Bs2PhiGamma'
                        , prescale = self.prescale
                        , HLT = hltfilter
                        , L0DU = l0filter
                        , algos = [ PV3D(), BiKalmanFittedKaons, Hlt2Phi4PhiGamma ] + photonAlgos + [ Hlt2BstoPhiGamma ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2PhiGammaDecision" : self.getProp('HltANNSvcID')['Bs2PhiGamma']} )

        
        ############################################################################
        #    Bs to Phi Gamma - Monitoring Lines
        ############################################################################
        #   use noCuts particles
        # line.clone('Bs2PhiGammaNoCutsK'
        #            , prescale = self.prescale
        #            , algos = [  PV3D(), BiKalmanFittedKaons, Hlt2Phi4PhiGamma, Photons, Hlt2BstoPhiGamma]
        #            #, CombinePhi = {"Inputs" : [BiKalmanFittedKaons] }
        #            , CombineBs = {"Inputs" : [Photons, Hlt2Phi4PhiGamma]}
        #            , postscale = self.postscale
        #            )
        # HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2PhiGammaNoCutsKDecision" :  self.getProp('HltANNSvcID')['Bs2PhiGammaNoCutsK'] } )
        
        #   Bs mass sideband
        line.clone('Bs2PhiGammaWideBMass'
                   , prescale = self.prescale
                   # , algos = [ PV3D(), BiKalmanFittedKaons, Hlt2Phi4PhiGamma, Photons, Hlt2BstoPhiGamma ]
                   , CombineBs = { "CombinationCut" : "(ADAMASS('B_s0')<1.5*%(BMassWinSB)s*MeV)" % self.getProps(),
                                   "MotherCut"      : "((BPVDIRA > cos(%(BsDirAngle)s)) & (BPVIPCHI2()< %(BsPVIPchi2)s) & (ADMASS('B_s0')<%(BMassWinSB)s*MeV) & (PT > %(B_PT)s*MeV))" % self.getProps()}
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2PhiGammaWideBMassDecision": self.getProp('HltANNSvcID')['Bs2PhiGammaWideBMass'] } )

        #  Bs dira monitoring
        line.clone('Bs2PhiGammaLooseDira'
                   , prescale = self.prescale
                   # , algos = [ PV3D(), BiKalmanFittedKaons, Hlt2Phi4PhiGamma, Photons, Hlt2BstoPhiGamma ]
                   , CombineBs = {"MotherCut": "( (BPVDIRA > cos(%(BDirAngleMoni)s)) & (BPVIPCHI2()<%(BsPVIPchi2)s) & (ADMASS('B_s0')<%(BsMassWin)s*MeV) & (PT > %(B_PT)s*MeV))" % self.getProps() }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2PhiGammaLooseDiraDecision" : self.getProp('HltANNSvcID')['Bs2PhiGammaLooseDira'] } )
        
        ############################################################################
        #    B0 to Kstar Gamma - Hlt2Line
        ############################################################################
        
        line = Hlt2Line('Bd2KstGamma'
                        , prescale = self.prescale
                        , HLT = hltfilter
                        , L0DU = l0filter
                        , algos = [ PV3D(), BiKalmanFittedKaons, BiKalmanFittedPions, Hlt2Kst4KstGamma ] + photonAlgos + [ Hlt2BdtoKstGamma]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2KstGammaDecision" :self.getProp('HltANNSvcID')['Bd2KstGamma'] } )
        
        ############################################################################
        #    B0 to Kstar Gamma - Monitoring Lines
        ############################################################################

        #  Kst mass sideband
        line.clone('Bd2KstGammaWideKMass'
                   , prescale = self.prescale
                   # , algos = [ PV3D(), BiKalmanFittedKaons, BiKalmanFittedPions, Hlt2Kst4KstGamma, Photons, Hlt2BdtoKstGamma ]
                   , CombineKstar = { 'CombinationCut' : "(ADAMASS('K*(892)0')<1.5*%(KstMassWinL)s*MeV)" % self.getProps(),
                                      'MotherCut'      : "(VFASPF(VCHI2)<%(KstVCHI2)s) & (ADMASS('K*(892)0')<%(KstMassWinL)s*MeV)" % self.getProps() }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2KstGammaWideKMassDecision" :  self.getProp('HltANNSvcID')['Bd2KstGammaWideKMass'] } )
        # B0 mass sideband
        line.clone('Bd2KstGammaWideBMass'
                   , prescale = self.prescale
                   # , algos = [ PV3D(), BiKalmanFittedKaons, BiKalmanFittedPions, Hlt2Kst4KstGamma, Photons, Hlt2BdtoKstGamma ]
                   , CombineB0 = { "CombinationCut": "(ADAMASS('B0')<1.5*%(BMassWinSB)s*MeV)" %self.getProps(),
                                   "MotherCut"     : "((BPVDIRA > cos(%(B0DirAngle)s)) & (BPVIPCHI2()<%(B0PVIPchi2)s) & (ADMASS('B0')<%(BMassWinSB)s*MeV) & (PT > %(B_PT)s*MeV))" % self.getProps()}
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2KstGammaWideBMassDecision" :  self.getProp('HltANNSvcID')['Bd2KstGammaWideBMass'] } )
        # B0 dira monitoring
        line.clone('Bd2KstGammaLooseDira'
                   , prescale = self.prescale
                   # , algos = [ PV3D(), BiKalmanFittedKaons, BiKalmanFittedPions, Hlt2Kst4KstGamma, Photons, Hlt2BdtoKstGamma ]
                   , CombineB0 = { "MotherCut": "( (BPVDIRA > cos(%(BDirAngleMoni)s)) & (BPVIPCHI2()<%(B0PVIPchi2)s) & (ADMASS('B0')<%(B0MassWin)s*MeV) & (PT > %(B_PT)s*MeV))" %self.getProps() }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2KstGammaLooseDiraDecision" :  self.getProp('HltANNSvcID')['Bd2KstGammaLooseDira'] } )

# EOF

