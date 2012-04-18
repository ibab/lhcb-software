# HLT2 D->PhiGamma and D->Kstar Gamma? selections
#
# Fatima Soomro( Fatima.Soomro@cern.ch)
# 

__author__ = ['Fatima Soomro']
__date__ = '15/04/2012'
__version__ = '$Revision: 1.0$'

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2D2XGammaLinesConf(HltLinesConfigurableUser) :
    '''
    Selection for D -> Phi(KK) Gamma [heavily borrowed from Hlt2B2XGamma lines]
    
    @author Fatima Soomro
    @date 15-04-2012
    '''

    __slots__ = { # Common cuts    
                    'TrChi2'       : 5.       # dimensionless
                   ,'TrPT'         : 300.      # MeV
                   ,'photonPT'     : 2000.     # MeV
                    # D2PhiGamma    
                   ,'TrIPchi2Phi'  : 9.       # Dimensionless
                   ,'PhiVCHI2'     : 25.       # dimensionless
                   ,'PhiMassWinL'  : 105.       # MeV
                   ,'PhiMassWinT'  : 100.       # MeV
                   ,'DPVIPchi2'   : 20.      # Dimensionless
                   ,'DMassWin'    : 100.     # MeV
                   ,'DDirAngle'   : 0.063  #0.998    # Dimensionless
                   ,'D_PT'         : 500     # MeV
                  # Photons
                   ,'UseL0Photons' : False
                  # HLT filter
                   ,'HLT1FILTER'   : "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!Velo)(?!BeamGas)(?!Incident).*Decision')" 
                   ,'L0FILTER'     : ""#"|".join( [ "L0_CHANNEL('%s')" % channel for channel in ['Photon','Electron'] ] ) 
                  # Pre- and postscale
                   ,'Prescale'     : { 'Hlt2D2PhiGamma$'   : 1.0
                                      ,'Hlt2D2PhiGamma.+' : 0.1       # prescale by a factor of 10
                                       }
                                   
                   ,'Postscale'    : {'Hlt2D2PhiGamma$'   : 1.0
                                      ,'Hlt2D2PhiGamma.+' : 1.0
                                      }
                  # IDs            
                   ,'HltANNSvcID'  : {'D2PhiGamma'            : 50590
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
        Hlt2Phi4D2PhiGamma = Hlt2Member( CombineParticles
                                       , "CombinePhi"
                                       , DecayDescriptors = [ "phi(1020) -> K+ K-" ]
                                       , DaughtersCuts = { "K+": "(TRCHI2DOF<%(TrChi2)s) & (MIPCHI2DV(PRIMARY)>%(TrIPchi2Phi)s) & (PT>%(TrPT)s*MeV)" % self.getProps(),
                                                           "K-": "(TRCHI2DOF<%(TrChi2)s) & (MIPCHI2DV(PRIMARY)>%(TrIPchi2Phi)s) & (PT>%(TrPT)s*MeV)" % self.getProps()  }
                                       , CombinationCut = "(ADAMASS('phi(1020)')<%(PhiMassWinL)s*MeV)" % self.getProps()
                                       , MotherCut = "(VFASPF(VCHI2)<%(PhiVCHI2)s) & (ADMASS('phi(1020)')<%(PhiMassWinT)s*MeV)" % self.getProps()
                                       , Inputs = [ BiKalmanFittedKaons ]
                                       )
        
##         Hlt2Kst4KstGamma = Hlt2Member( CombineParticles
##                                        , "CombineKstar"
##                                        , DecayDescriptors =["[K*(892)0 -> K+ pi-]cc"] 
##                                        , DaughtersCuts  = { "K+" : "(TRCHI2DOF<%(TrChi2)s) & (MIPCHI2DV(PRIMARY)>%(TrIPchi2Kst)s) & (PT>%(TrPT)s*MeV)" % self.getProps(),
##                                                             "pi-": "(TRCHI2DOF<%(TrChi2)s) & (MIPCHI2DV(PRIMARY)>%(TrIPchi2Kst)s) & (PT>%(TrPT)s*MeV)" % self.getProps() }
##                                        , CombinationCut =  "(ADAMASS('K*(892)0')<%(KstMassWinL)s*MeV)" % self.getProps()
##                                        , MotherCut      =  "(VFASPF(VCHI2)<%(KstVCHI2)s) & (ADMASS('K*(892)0')<%(KstMassWinT)s*MeV)" % self.getProps()
##                                        , Inputs  = [ BiKalmanFittedKaons, BiKalmanFittedPions ]
##                                        )

        ############################################################################
        #   D -> Phi Gamma
        ############################################################################
        Hlt2DtoPhiGamma = Hlt2Member(CombineParticles
                                      , "CombineD"
                                      , DecayDescriptors = ["D0 -> phi(1020) gamma"]
                                      , DaughtersCuts = { "gamma": "(PT>%(photonPT)s*MeV)" % self.getProps() }
                                      , CombinationCut = "(ADAMASS('D0')<1.1*%(DMassWin)s*MeV)" % self.getProps()
                                      , MotherCut = "((BPVDIRA > cos(%(DDirAngle)s)) & (BPVIPCHI2()< %(DPVIPchi2)s) & (ADMASS('D0')<%(DMassWin)s*MeV) & (PT > %(D_PT)s*MeV))" % self.getProps()
                                      , Preambulo = ["from math import cos"]
                                      , Inputs  = [ Photons,  Hlt2Phi4D2PhiGamma ] #  
                                      )
        
        ############################################################################
        #    D to Phi Gamma - Hlt2Line
        ############################################################################
        line = Hlt2Line('D2PhiGamma'
                        , prescale = self.prescale
                        , HLT = hltfilter
                        , L0DU = l0filter
                        , algos = [ PV3D(), BiKalmanFittedKaons, Hlt2Phi4D2PhiGamma ] + photonAlgos + [ Hlt2DtoPhiGamma ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2D2PhiGammaDecision" : self.getProp('HltANNSvcID')['D2PhiGamma']} )

        
 
 
