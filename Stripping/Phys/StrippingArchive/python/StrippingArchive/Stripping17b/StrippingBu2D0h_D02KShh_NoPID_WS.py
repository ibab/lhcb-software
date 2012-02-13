

__author__ = 'Susan Haines'
__date__ = '22/07/2011'
__version__ = '$Revision: 1.6 $'

'''
Bu->D0(KShh)h stripping selection using LoKi::Hybrid and python
configurables for new MC09 offline selection, using StdNoPIDs particles.  Only PID cut on Ks DD daughter pions.
Wrong sign D daughter combinations, prescaled.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


confdict_Bu2D0h_D02KShh_NoPID_offlinesel_WS={#almost offline sel cuts 8/2/2011 (D daughter pi/K IPchi2s looser)
                        'PrescaleLL'    : 0.15
                        ,  'PostscaleLL'   : 1.0
                        #B cuts, LL seln
                       ,  'BFlightCHI2_LL'        : 169. 
                       ,  'BDIRA_LL'           : 0.99999     
                       ,  'BIPCHI2_LL'         : 9.         
                       ,  'BVertexCHI2_LL'     : 6.25
                       #Bachelor K cuts, LL seln
                       ,  'BachIPCHI2_LL'     : 16.
                       ,  'BachPt_LL'         : 1.0  
                       #D0 and D0 daughter pi cuts, LL seln
                       ,  'DVertexCHI2_LL'     : 4.     
                       ,  'DdaughterIPCHI2_LL' : 16.
                       #KS and KS daughter pi cuts, LL seln
                      # ,  'KSFlightCHI2_LL'    : 4.
                      # ,  'KSVertexCHI2_LL'    : 16.
                      # ,  'KSdaughterPiIPCHI2_LL' : 9.
                       ,  'PrescaleDD'    : 0.15 
                       ,  'PostscaleDD'   : 1.0
                       #B cuts, DD seln
                       ,  'BFlightCHI2_DD'     : 100.
                       ,  'BDIRA_DD'           : 0.9999    
                       ,  'BIPCHI2_DD'         : 16.         
                       ,  'BVertexCHI2_DD'     : 9.
                       #Bachelor K cuts, DD seln
                       ,  'BachIPCHI2_DD'     : 6.25
                       ,  'BachPt_DD'         : 0.5 
                       #D0 and D0 daughter pi cuts, DD seln
                       ,  'DVertexCHI2_DD'     : 16.        
                       ,  'DdaughterIPCHI2_DD' : 9.
                        #KS and KS daughter pi cuts, DD seln
                      # ,  'KSFlightCHI2_DD'    : 4.
                      # ,  'KSVertexCHI2_DD'    : 16.
                      # ,  'KSdaughterPiIPCHI2_DD' : 6.25
                       
 }

class StrippingBu2D0h_D02KShh_NoPID_WSConf(LineBuilder):
    '''
    Configuration object for a B->D0h, D0->KShh lines, KS->LL and DD pions, where h is K or Pi.
    
    Some cuts are hardcoded, e.g. mass windows.

    PID cuts only applied to downstream Ks daughter pions, all input Pi and K are StdNoPIDs particles.
    
    The Lines object holds the configured line.
    '''

    Line_LL_Bu2D0h_D02KShh_WS=None
    Line_DD_Bu2D0h_D02KShh_WS=None 

  #  LLPi_FilterCut_WS = ""
  #  KSLL_CombCut_WS = ""
  #  KSLL_MotherCut_WS = ""
    D_LL_DaughterCut_WS = ""
    D_LL_CombCut_WS = ""
    D_LL_MotherCut_WS = ""  
    B_LL_BachCut_WS = ""
    B_LL_CombCut_WS = ""
    B_LL_MotherCut_WS = ""
    
  #  DDPi_FilterCut_WS = ""
  #  KSDD_CombCut_WS = ""
  #  KSDD_MotherCut_WS = ""
    D_DD_DaughterCut_WS = ""
    D_DD_CombCut_WS = ""
    D_DD_MotherCut_WS = ""
    B_DD_BachCut_WS = ""
    B_DD_CombCut_WS = ""
    B_DD_MotherCut_WS = ""

   # FilteredPions_KSLL_noPID_WS=None
    SelKSLL_Bu2D0h_noPID_WS=None
    mergedPiKforD_LL_WS=None
    MergedD0_KShh_noPID_LL_WS=None
    Bu2D0h_LLSel_noPID_WS=None

   # FilteredPions_KSDD_noPID_WS=None
    SelKSDD_Bu2D0h_noPID_WS=None
    mergedPiKforD_DD_WS=None
    MergedD0_KShh_noPID_DD_WS=None
    Bu2D0h_DDSel_noPID_WS=None

    B2Dh_D2KShh_StrippingNumTracksGECLL_WS=None
    B2Dh_D2KShh_StrippingNumTracksGECDD_WS=None

    __configuration_keys__ = ( 
          'PrescaleLL'
        ,  'PostscaleLL'
        #B cuts, LL seln
        ,  'BFlightCHI2_LL'         
        ,  'BDIRA_LL'               
        ,  'BIPCHI2_LL'                
        ,  'BVertexCHI2_LL'
        #Bachelor K cuts, LL seln
        ,  'BachIPCHI2_LL'    
        ,  'BachPt_LL'
        #D0 and D0 daughter pi cuts, LL seln
        ,  'DVertexCHI2_LL'       
        ,  'DdaughterIPCHI2_LL'
        #KS and KS daughter pi cuts, LL seln  
      #  ,  'KSFlightCHI2_LL'    
      #  ,  'KSVertexCHI2_LL'    
      #  ,  'KSdaughterPiIPCHI2_LL'
        ,  'PrescaleDD'
        ,  'PostscaleDD'
        #B cuts, DD seln
        ,  'BFlightCHI2_DD'     
        ,  'BDIRA_DD'               
        ,  'BIPCHI2_DD'                  
        ,  'BVertexCHI2_DD'
        #Bachelor K cuts, DD seln  
        ,  'BachIPCHI2_DD'    
        ,  'BachPt_DD'
        #D0 and D0 daughter pi cuts, DD seln
        ,  'DVertexCHI2_DD'           
        ,  'DdaughterIPCHI2_DD'
        #KS and KS daughter pi cuts, DD seln
       # ,  'KSFlightCHI2_DD'    
       # ,  'KSVertexCHI2_DD'    
       # ,  'KSdaughterPiIPCHI2_DD' 
        )

    def __init__(self, name, config):
        '''The constructor of the configuration class.
        Requires a configuration dictionary, config, which must provide all the settings for cuts which are not hard coded'''

        LineBuilder.__init__(self, name, config)

        self._myname = name
            
        #define cuts
        #LL selection cuts
      #  self.LLPi_FilterCut_WS = "(P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<5.) & (BPVIPCHI2() > %(KSdaughterPiIPCHI2_LL)s)"%config
      #  self.KSLL_CombCut_WS = "(ADAMASS('KS0') < 25.*MeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<1.5)"
      #  self.KSLL_MotherCut_WS = "(ADMASS('KS0') < 20.*MeV) & (VFASPF(VCHI2/VDOF) < %(KSVertexCHI2_LL)s) & (BPVVDCHI2 > %(KSFlightCHI2_LL)s)"%config
        self.D_LL_DaughterCut_WS = "(P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<5.) & (BPVIPCHI2() > %(DdaughterIPCHI2_LL)s) "%config
        self.D_LL_CombCut_WS = "(ADAMASS('D0') < 55.*MeV) & (ADOCACUT(1.8, 'LoKi::TrgDistanceCalculator'))"#55
        self.D_LL_MotherCut_WS = "(ADMASS('D0') < 50.*MeV) & (VFASPF(VCHI2/VDOF) < %(DVertexCHI2_LL)s)"%config#50
        self.B_LL_BachCut_WS = "(PT > %(BachPt_LL)s *GeV ) & (P > 2.*GeV) & (P < 100.*GeV) &  (TRCHI2DOF<5.) & (BPVIPCHI2() > %(BachIPCHI2_LL)s )"%config
        self.B_LL_CombCut_WS = "(ADAMASS('B+') < 505.*MeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<0.5)"
        self.B_LL_MotherCut_WS = "(ADMASS('B+') < 500.*MeV)  & (VFASPF(VCHI2/VDOF)<%(BVertexCHI2_LL)s) & (BPVIPCHI2() < %(BIPCHI2_LL)s) & (BPVVDCHI2 > %(BFlightCHI2_LL)s) & (BPVDIRA > %(BDIRA_LL)s)"%config


        #DD selection cuts
      #  self.DDPi_FilterCut_WS = "(ISDOWN) & (P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<10.) & (BPVIPCHI2() > %(KSdaughterPiIPCHI2_DD)s)  & ((-PIDK) > 0.)"%config
      #  self.KSDD_CombCut_WS = "(ADAMASS('KS0') < 50.*MeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<22.)"
      #  self.KSDD_MotherCut_WS = "(ADMASS('KS0') < 28.*MeV) & (VFASPF(VCHI2/VDOF) < %(KSVertexCHI2_DD)s) & (BPVVDCHI2 > %(KSFlightCHI2_DD)s)" %config
        self.D_DD_DaughterCut_WS = " (P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<5.) & (BPVIPCHI2() > %(DdaughterIPCHI2_DD)s)"%config
        self.D_DD_CombCut_WS = "(ADAMASS('D0') < 65.*MeV) & (ADOCACUT(9.2, 'LoKi::TrgDistanceCalculator'))"#65
        self.D_DD_MotherCut_WS = "(ADMASS('D0') < 60.*MeV) & (VFASPF(VCHI2/VDOF) < %(DVertexCHI2_DD)s)"%config#60
        self.B_DD_BachCut_WS = "(PT > %(BachPt_DD)s *GeV) & (P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<5.) & (BPVIPCHI2() > %(BachIPCHI2_DD)s)"%config 
        self.B_DD_CombCut_WS = "(ADAMASS('B+') < 505.*MeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<0.5)"
        self.B_DD_MotherCut_WS = "(ADMASS('B+') < 500.*MeV)  & (VFASPF(VCHI2/VDOF) < %(BVertexCHI2_DD)s) & (BPVIPCHI2() < %(BIPCHI2_DD)s) & (BPVVDCHI2 > %(BFlightCHI2_DD)s)& (BPVDIRA > %(BDIRA_DD)s)"%config

        #Make the selections
      #  self.LLPionFilterForBu2D0h_D02KShhAlg_WS()
      #  self.KSLLForBu2D0h_D02KShhAlg_WS()
        self.DForBu2D0h_D02KShh_KSLLAlg_WS()
        self.Bu2D0h_KShh_KSLLAlg_WS()
        
      #  self.DDPionFilterForBu2D0h_D02KShhAlg_WS()   
      #  self.KSDDForBu2D0h_D02KShhAlg_WS()
        self.DForBu2D0h_D02KShh_KSDDAlg_WS()
        self.Bu2D0h_KShh_KSDDAlg_WS()

##     '''
##     Define the GEC on number of long tracks, needed in order to control
##     the time for the combinatorics (D in particular)
##     '''
       
        B2Dh_D2KShh_StrippingNumTracksGECLL_WS = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < 250)",
                                                  'Preambulo' : ["from LoKiTracks.decorators import *"]}
        B2Dh_D2KShh_StrippingNumTracksGECDD_WS = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < 250)",
                                                  'Preambulo' : ["from LoKiTracks.decorators import *"]}

##        B2Dh_D2KShh_StrippingNumTracksGECLL_WS = {'Code' : "RECSUMMARY ( LHCb.RecSummary.nLongTracks , -1 ) < 250"}
##        B2Dh_D2KShh_StrippingNumTracksGECDD_WS = {'Code' : "RECSUMMARY ( LHCb.RecSummary.nLongTracks , -1 ) < 250"}

        #Stripping lines
        self.Line_LL_Bu2D0h_D02KShh_WS = StrippingLine(self._myname+'Bu2D0h_D02KShh_KSLL_WS'
                                     , prescale = config['PrescaleLL']
                                     , postscale = config['PostscaleLL']
                                     , FILTER = B2Dh_D2KShh_StrippingNumTracksGECLL_WS   
                                     , selection = self.Bu2D0h_LLSel_noPID_WS       
                                     )

        self.Line_DD_Bu2D0h_D02KShh_WS = StrippingLine(self._myname+'Bu2D0h_D02KShh_KSDD_WS'
                                     , prescale = config['PrescaleDD']
                                     , postscale = config['PostscaleDD']
                                     , FILTER = B2Dh_D2KShh_StrippingNumTracksGECDD_WS
                                     , selection = self.Bu2D0h_DDSel_noPID_WS
                                     )
        
        self.registerLine(self.Line_LL_Bu2D0h_D02KShh_WS)
        self.registerLine(self.Line_DD_Bu2D0h_D02KShh_WS)

    def printCuts(self):
        '''Print the compiled cut values'''
     #   print 'KS daughter pi cuts, LL ', self.LLPi_FilterCut_WS
     #   print 'KS combination cuts, LL ', self.KSLL_CombCut_WS
     #   print 'KS mother cuts, LL ', self.KSLL_MotherCut_WS
        print 'D daughter pi/K cuts, LL ', self.D_LL_DaughterCut_WS
        print 'D combination cuts, LL ', self.D_LL_CombCut_WS
        print 'D mother cuts, LL ', self.D_LL_MotherCut_WS
        print 'Bachelor K/pi cuts, LL ', self.B_LL_BachCut_WS
        print 'B combination cuts, LL ', self.B_LL_CombCut_WS
        print 'B mother cuts, LL ', self.B_LL_MotherCut_WS
     #   print 'KS daughter pi cuts, DD ', self.DDPi_FilterCut_WS
     #   print 'KS combination cuts, DD ', self.KSDD_CombCut_WS
     #   print 'KS mother cuts, DD ', self.KSDD_MotherCut_WS
        print 'D daughter pi/K cuts, DD ', self.D_DD_DaughterCut_WS
        print 'D combination cuts, DD ', self.D_DD_CombCut_WS
        print 'D mother cuts, DD ', self.D_DD_MotherCut_WS
        print 'Bachelor K/pi cuts, DD ', self.B_DD_BachCut_WS
        print 'B combination cuts, DD ', self.B_DD_CombCut_WS
        print 'B mother cuts, DD ', self.B_DD_MotherCut_WS

    ###############################################
    # Create selections for StrippingLines        #
    ############################################### 

        ###########################
        # KS daughters, long long # 
        ###########################
#    def LLPionFilterForBu2D0h_D02KShhAlg_WS(self):
#        '''
#        Pion filter for input to LL KS - uses long pions
#        '''
#        import GaudiKernel.SystemOfUnits as Units
#        from PhysSelPython.Wrappers import Selection
#
#	import StandardParticles
#	if hasattr(StandardParticles, "StdAllNoPIDsPions"):
#		from StandardParticles import StdAllNoPIDsPions as MyStdPions_KS2PiPi_LL_noPID_WS
#	else:
#      		from StandardParticles import StdNoPIDsPions as MyStdPions_KS2PiPi_LL_noPID_WS 
#
#        #from StandardParticles import StdAllNoPIDsPions as MyStdPions_KS2PiPi_LL_noPID_WS
#
#        LLPionFilterForBu2D0h_D02KShh_WS = FilterDesktop(Code = self.LLPi_FilterCut_WS)
#        
#        SelLLPionFilterForBu2D0h_D02KShh_WS = Selection(self._myname+"SelLLPionFilterForBu2D0h_D02KShh_WS", Algorithm=LLPionFilterForBu2D0h_D02KShh_WS,
#                                                       RequiredSelections = [MyStdPions_KS2PiPi_LL_noPID_WS]
#                                                       )
#       
#        self.FilteredPions_KSLL_noPID_WS=SelLLPionFilterForBu2D0h_D02KShh_WS
        
    
        #######################################
        # KS daughters, downstream downstream #
        #######################################
#    def DDPionFilterForBu2D0h_D02KShhAlg_WS(self):
#        '''
#        Pion filter for input to DD KS - uses no PID downstream pions
#        '''
#        import GaudiKernel.SystemOfUnits as Units
#        from PhysSelPython.Wrappers import Selection
#
#        from StandardParticles import StdNoPIDsDownPions as MyStdPions_KS2PiPi_DD_noPID_WS
#
#        DDPionFilterForBu2D0h_D02KShh_WS = FilterDesktop(Code = self.DDPi_FilterCut_WS)
#       
#        SelDDPionFilterForBu2D0h_D02KShh_WS = Selection(self._myname+"SelDDPionFilterForBu2D0h_D02KShh_WS", Algorithm=DDPionFilterForBu2D0h_D02KShh_WS,
#                                                       RequiredSelections = [MyStdPions_KS2PiPi_DD_noPID_WS]
#                                                       )
#        
#        self.FilteredPions_KSDD_noPID_WS=SelDDPionFilterForBu2D0h_D02KShh_WS
        
        ###########################
        # KS, long long daughters #
        ###########################
#    def KSLLForBu2D0h_D02KShhAlg_WS(self):
#        '''
#        KS LL selection
#        '''
#        import GaudiKernel.SystemOfUnits as Units
#        from PhysSelPython.Wrappers import Selection
#
#        KSLLForBu2D0h_D02KShh_WS = CombineParticles(DecayDescriptor = "KS0 -> pi+ pi-",
#                                                    DaughtersCuts = {"pi+": "ALL"},
#                                                    CombinationCut = self.KSLL_CombCut_WS,
#                                                    MotherCut = self.KSLL_MotherCut_WS)
#
#        SelKSLLForBu2D0h_D02KShh_WS = Selection(self._myname+"SelKSLLForBu2D0h_D02KShh_WS", Algorithm=KSLLForBu2D0h_D02KShh_WS,
#                                               RequiredSelections = [self.FilteredPions_KSLL_noPID_WS]
#                                               )
#        
#        self.SelKSLL_Bu2D0h_noPID_WS=SelKSLLForBu2D0h_D02KShh_WS
        
        #######################################
        # KS, downstream downstream daughters #
        #######################################
#    def KSDDForBu2D0h_D02KShhAlg_WS(self):
#        '''
#        KS DD selection
#        '''
#        import GaudiKernel.SystemOfUnits as Units
#        from PhysSelPython.Wrappers import Selection
#
#        KSDDForBu2D0h_D02KShh_WS = CombineParticles(DecayDescriptor = "KS0 -> pi+ pi-", 
#                                                    DaughtersCuts = {"pi+" : "ALL"},
#                                                    CombinationCut = self.KSDD_CombCut_WS,
#                                                    MotherCut = self.KSDD_MotherCut_WS)
#        
#        SelKSDDForBu2D0h_D02KShh_WS = Selection(self._myname+"SelKSDDForBu2D0h_D02KShh_WS", Algorithm=KSDDForBu2D0h_D02KShh_WS,
#                                               RequiredSelections = [self.FilteredPions_KSDD_noPID_WS]
#                                               )
#        
#        self.SelKSDD_Bu2D0h_noPID_WS=SelKSDDForBu2D0h_D02KShh_WS
        
        ###################################
        # D0->KShh, with KS LL daughter #
        ###################################
    def DForBu2D0h_D02KShh_KSLLAlg_WS(self):
        '''
        D0->KS Pi Pi, D0->KS K Pi, D0->KS K K, KS LL selections 
        '''
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import Selection
	import StandardParticles
	if hasattr(StandardParticles, "StdAllNoPIDsPions"):
		from StandardParticles import StdAllNoPIDsPions as MyStdLoosePions_D2KShh_LL_WS
	else:
      		from StandardParticles import StdNoPIDsPions as MyStdLoosePions_D2KShh_LL_WS

	if hasattr(StandardParticles, "StdAllNoPIDsKaons"):
		from StandardParticles import StdAllNoPIDsKaons as MyStdLooseKaons_D2KShh_LL_WS
	else:
      		from StandardParticles import StdNoPIDsKaons as MyStdLooseKaons_D2KShh_LL_WS


        #from StandardParticles import StdAllNoPIDsPions as MyStdLoosePions_D2KShh_LL_WS
        #from StandardParticles import StdAllNoPIDsKaons as MyStdLooseKaons_D2KShh_LL_WS

	SelKSLL_Bu2D0h_noPID_WS = DataOnDemand( Location = "Phys/StdVeryLooseKsLL/Particles" )

        DForBu2D0h_D02KShh_KSLL_WS = CombineParticles(DecayDescriptors = ["D0 -> KS0 pi+ pi+","D0 -> KS0 pi- pi-", "D0 -> KS0 K+ pi+","D0 -> KS0 K- pi-","D0 -> KS0 K+ K+", "D0 -> KS0 K- K-"],
                                                      DaughtersCuts  = {"pi+" : self.D_LL_DaughterCut_WS
                                                                        ,"K+": self.D_LL_DaughterCut_WS
                                                                        ,"KS0" : "ALL"
                                                                        },
                                                      CombinationCut =  self.D_LL_CombCut_WS,
                                                      MotherCut      =  self.D_LL_MotherCut_WS)
  
        SelDForBu2D0h_D02KShh_KSLL_WS = Selection(self._myname+"SelDForBu2D0h_D02KShh_KSLL_WS", Algorithm=DForBu2D0h_D02KShh_KSLL_WS,
                                                 RequiredSelections = [SelKSLL_Bu2D0h_noPID_WS, MyStdLoosePions_D2KShh_LL_WS, MyStdLooseKaons_D2KShh_LL_WS ])
        
        self.MergedD0_KShh_noPID_LL_WS=SelDForBu2D0h_D02KShh_KSLL_WS

        ###################################
        # D0->KShh, with KS DD daughter #
        ###################################
    def DForBu2D0h_D02KShh_KSDDAlg_WS(self):
        '''
        D0->KS Pi Pi, D0->KS K Pi, D0->KS K K, KS DD selections
        '''
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import Selection 

	import StandardParticles
	if hasattr(StandardParticles, "StdAllNoPIDsPions"):
		from StandardParticles import StdAllNoPIDsPions as MyStdLoosePions_D2KShh_DD_WS
	else:
       		from StandardParticles import StdNoPIDsPions as MyStdLoosePions_D2KShh_DD_WS

	if hasattr(StandardParticles, "StdAllNoPIDsKaons"):
		from StandardParticles import StdAllNoPIDsKaons as MyStdLooseKaons_D2KShh_DD_WS
	else:
       		from StandardParticles import StdNoPIDsKaons as MyStdLooseKaons_D2KShh_DD_WS


        #from StandardParticles import StdAllNoPIDsPions as MyStdLoosePions_D2KShh_DD_WS
        #from StandardParticles import StdAllNoPIDsKaons as MyStdLooseKaons_D2KShh_DD_WS

	SelKSDD_Bu2D0h_noPID_WS = DataOnDemand( Location = "Phys/StdVeryLooseKsDD/Particles" )


        DForBu2D0h_D02KShh_KSDD_WS = CombineParticles(DecayDescriptors = ["D0 -> KS0 pi+ pi+","D0 -> KS0 pi- pi-", "D0 -> KS0 K+ pi+","D0 -> KS0 K- pi-","D0 -> KS0 K+ K+", "D0 -> KS0 K- K-"],
                                                      DaughtersCuts  = {"pi+" : self.D_DD_DaughterCut_WS
                                                                        , "K+": self.D_DD_DaughterCut_WS
                                                                        ,"KS0" : "ALL"
                                                                        },
                                                      CombinationCut =  self.D_DD_CombCut_WS,
                                                      MotherCut      =  self.D_DD_MotherCut_WS)
        
        SelDForBu2D0h_D02KShh_KSDD_WS = Selection(self._myname+"SelDForBu2D0h_D02KShh_KSDD_WS", Algorithm=DForBu2D0h_D02KShh_KSDD_WS,
                                                 RequiredSelections = [SelKSDD_Bu2D0h_noPID_WS, MyStdLoosePions_D2KShh_DD_WS, MyStdLooseKaons_D2KShh_DD_WS])
                
        self.MergedD0_KShh_noPID_DD_WS=SelDForBu2D0h_D02KShh_KSDD_WS


        ################
        # B, LL KS     #
        ################
    def Bu2D0h_KShh_KSLLAlg_WS(self):
        '''
        B->D0K,pi D0->KS Pi Pi, KS LL selection
        '''
       
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import Selection

        from StandardParticles import StdNoPIDsKaons as MyStdNoPIDKaons_BDh_LL_WS
        from StandardParticles import StdNoPIDsPions as MyStdNoPIDPions_BDh_LL_WS
        
        Bu2D0h_KSLL_WS = CombineParticles(DecayDescriptors = ["B+ -> D0 K+", "B- -> D0 K-", "B+ -> D0 pi+", "B- -> D0 pi-"],
                                          DaughtersCuts = { "D0" : "ALL",
                                                            "K+" : self.B_LL_BachCut_WS,
                                                            "pi+" : self.B_LL_BachCut_WS
                                                            },
                                          CombinationCut =  self.B_LL_CombCut_WS,
                                          MotherCut = self.B_LL_MotherCut_WS)
        
        SelBu2D0h_KSLL_WS = Selection(self._myname+"SelBu2D0h_KSLL_WS", Algorithm=Bu2D0h_KSLL_WS,
                                    RequiredSelections = [self.MergedD0_KShh_noPID_LL_WS, MyStdNoPIDKaons_BDh_LL_WS, MyStdNoPIDPions_BDh_LL_WS])
        
        self.Bu2D0h_LLSel_noPID_WS=SelBu2D0h_KSLL_WS



        ################
        # B, DD KS     #
        ################
    def Bu2D0h_KShh_KSDDAlg_WS(self):
        '''
        B->D0K,pi D0->KS Pi Pi, KS DD selection
        '''
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import Selection

        from StandardParticles import StdNoPIDsKaons as MyStdNoPIDKaons_BDh_DD_WS
        from StandardParticles import StdNoPIDsPions as MyStdNoPIDPions_BDh_DD_WS

        Bu2D0h_KSDD_WS = CombineParticles(DecayDescriptors = ["B+ -> D0 K+", "B- -> D0 K-", "B+ -> D0 pi+", "B- -> D0 pi-"],
                                          DaughtersCuts = { "D0" : "ALL",
                                                            "K+" : self.B_DD_BachCut_WS,
                                                            "pi+" : self.B_DD_BachCut_WS
                                                            },
                                          CombinationCut = self.B_DD_CombCut_WS,
                                          MotherCut = self.B_DD_MotherCut_WS)

        SelBu2D0h_KSDD_WS = Selection(self._myname+"SelBu2D0h_KSDD_WS", Algorithm=Bu2D0h_KSDD_WS,
                                    RequiredSelections = [self.MergedD0_KShh_noPID_DD_WS, MyStdNoPIDKaons_BDh_DD_WS, MyStdNoPIDPions_BDh_DD_WS])
        
        self.Bu2D0h_DDSel_noPID_WS=SelBu2D0h_KSDD_WS
