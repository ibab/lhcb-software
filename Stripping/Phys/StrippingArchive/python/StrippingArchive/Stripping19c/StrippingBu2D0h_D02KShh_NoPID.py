

__author__ = 'Susan Haines'
__date__ = '22/07/2011'
__version__ = '$Revision: 1.6 $'

'''
Bu->D0(KShh)h stripping selection using LoKi::Hybrid and python
configurables for new MC09 offline selection, using StdNoPIDs particles.  Only PID cut on Ks DD daughter pions.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


confdict_Bu2D0h_D02KShh_NoPID_offlinesel={#almost offline sel cuts 8/2/2011 (D daughter pi/K IPchi2s looser, Ks DD daughter pi PID cut looser)
                        'PrescaleLL'    : 1.0 
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
                       ,  'PrescaleDD'    : 1.0 
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

class StrippingBu2D0h_D02KShh_NoPIDConf(LineBuilder):
    '''
    Configuration object for a B->D0h, D0->KShh lines, KS->LL and DD pions, where h is K or Pi.
    
    Some cuts are hardcoded, e.g. mass windows.

    PID cuts only applied to downstream Ks daughter pions, all input Pi and K are StdNoPIDs particles.
    
    The Lines object holds the configured line.
    '''

    Line_LL_Bu2D0h_D02KShh=None
    Line_DD_Bu2D0h_D02KShh=None 

   # LLPi_FilterCut = ""
   # KSLL_CombCut = ""
   # KSLL_MotherCut = ""
    D_LL_DaughterCut = ""
    D_LL_CombCut = ""
    D_LL_MotherCut = ""  
    B_LL_BachCut = ""
    B_LL_CombCut = ""
    B_LL_MotherCut = ""
    
   # DDPi_FilterCut = ""
   # KSDD_CombCut = ""
   # KSDD_MotherCut = ""
    D_DD_DaughterCut = ""
    D_DD_CombCut = ""
    D_DD_MotherCut = ""
    B_DD_BachCut = ""
    B_DD_CombCut = ""
    B_DD_MotherCut = ""

  #  FilteredPions_KSLL_noPID=None
    SelKSLL_Bu2D0h_noPID=None
    mergedPiKforD_LL=None
    MergedD0_KShh_noPID_LL=None
    Bu2D0h_LLSel_noPID=None

  #  FilteredPions_KSDD_noPID=None
    SelKSDD_Bu2D0h_noPID=None
    mergedPiKforD_DD=None
    MergedD0_KShh_noPID_DD=None
    Bu2D0h_DDSel_noPID=None

    B2Dh_D2KShh_StrippingNumTracksGECLL=None
    B2Dh_D2KShh_StrippingNumTracksGECDD=None

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
     #   ,  'KSFlightCHI2_LL'    
     #   ,  'KSVertexCHI2_LL'    
     #   ,  'KSdaughterPiIPCHI2_LL'
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
     #   ,  'KSFlightCHI2_DD'    
     #   ,  'KSVertexCHI2_DD'    
     #   ,  'KSdaughterPiIPCHI2_DD' 
        )

    def __init__(self, name, config):
        '''The constructor of the configuration class.
        Requires a configuration dictionary, config, which must provide all the settings for cuts which are not hard coded'''

        LineBuilder.__init__(self, name, config)

        self._myname = name
            
        #define cuts
        #LL selection cuts
       # self.LLPi_FilterCut = "(P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<5.) & (BPVIPCHI2() > %(KSdaughterPiIPCHI2_LL)s)"%config
       # self.KSLL_CombCut = "(ADAMASS('KS0') < 25.*MeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<1.5)"
       # self.KSLL_MotherCut = "(ADMASS('KS0') < 20.*MeV) & (VFASPF(VCHI2/VDOF) < %(KSVertexCHI2_LL)s) & (BPVVDCHI2 > %(KSFlightCHI2_LL)s)"%config
        self.D_LL_DaughterCut = "(P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<5.) & (BPVIPCHI2() > %(DdaughterIPCHI2_LL)s) "%config
        self.D_LL_CombCut = "(ADAMASS('D0') < 55.*MeV) & (ADOCACUT(1.8, 'LoKi::TrgDistanceCalculator'))"#55
        self.D_LL_MotherCut = "(ADMASS('D0') < 50.*MeV) & (VFASPF(VCHI2/VDOF) < %(DVertexCHI2_LL)s)"%config#50
        self.B_LL_BachCut = "(PT > %(BachPt_LL)s *GeV ) & (P > 2.*GeV) & (P < 100.*GeV) &  (TRCHI2DOF<5.) & (BPVIPCHI2() > %(BachIPCHI2_LL)s )"%config
        self.B_LL_CombCut = "(ADAMASS('B+') < 505.*MeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<0.5)"
        self.B_LL_MotherCut = "(ADMASS('B+') < 500.*MeV)  & (VFASPF(VCHI2/VDOF)<%(BVertexCHI2_LL)s) & (BPVIPCHI2() < %(BIPCHI2_LL)s) & (BPVVDCHI2 > %(BFlightCHI2_LL)s) & (BPVDIRA > %(BDIRA_LL)s)"%config


        #DD selection cuts
       # self.DDPi_FilterCut = "(ISDOWN) & (P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<10.) & (BPVIPCHI2() > %(KSdaughterPiIPCHI2_DD)s)  & ((-PIDK) > 0.)"%config
       # self.KSDD_CombCut = "(ADAMASS('KS0') < 50.*MeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<22.)"
       # self.KSDD_MotherCut = "(ADMASS('KS0') < 28.*MeV) & (VFASPF(VCHI2/VDOF) < %(KSVertexCHI2_DD)s) & (BPVVDCHI2 > %(KSFlightCHI2_DD)s)" %config
        self.D_DD_DaughterCut = " (P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<5.) & (BPVIPCHI2() > %(DdaughterIPCHI2_DD)s)"%config
        self.D_DD_CombCut = "(ADAMASS('D0') < 65.*MeV) & (ADOCACUT(9.2, 'LoKi::TrgDistanceCalculator'))"#65
        self.D_DD_MotherCut = "(ADMASS('D0') < 60.*MeV) & (VFASPF(VCHI2/VDOF) < %(DVertexCHI2_DD)s)"%config#60
        self.B_DD_BachCut = "(PT > %(BachPt_DD)s *GeV) & (P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<5.) & (BPVIPCHI2() > %(BachIPCHI2_DD)s)"%config 
        self.B_DD_CombCut = "(ADAMASS('B+') < 505.*MeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<0.5)"
        self.B_DD_MotherCut = "(ADMASS('B+') < 500.*MeV)  & (VFASPF(VCHI2/VDOF) < %(BVertexCHI2_DD)s) & (BPVIPCHI2() < %(BIPCHI2_DD)s) & (BPVVDCHI2 > %(BFlightCHI2_DD)s)& (BPVDIRA > %(BDIRA_DD)s)"%config

        #Make the selections
      #  self.LLPionFilterForBu2D0h_D02KShhAlg()
      #  self.KSLLForBu2D0h_D02KShhAlg()
        self.DForBu2D0h_D02KShh_KSLLAlg()
        self.Bu2D0h_KShh_KSLLAlg()
        
      #  self.DDPionFilterForBu2D0h_D02KShhAlg()   
      #  self.KSDDForBu2D0h_D02KShhAlg()
        self.DForBu2D0h_D02KShh_KSDDAlg()
        self.Bu2D0h_KShh_KSDDAlg()

##     '''
##     Define the GEC on number of long tracks, needed in order to control
##     the time for the combinatorics (D in particular)
##     '''
        B2Dh_D2KShh_StrippingNumTracksGECLL = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < 250)",
                                               'Preambulo' : ["from LoKiTracks.decorators import *"]}
        B2Dh_D2KShh_StrippingNumTracksGECDD = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < 250)",
                                               'Preambulo' : ["from LoKiTracks.decorators import *"]}
        
##        B2Dh_D2KShh_StrippingNumTracksGECLL = {'Code' : "RECSUMMARY ( LHCb.RecSummary.nLongTracks , -1 ) < 250"}
##        B2Dh_D2KShh_StrippingNumTracksGECDD = {'Code' : "RECSUMMARY ( LHCb.RecSummary.nLongTracks , -1 ) < 250"}

        #Stripping lines
        self.Line_LL_Bu2D0h_D02KShh = StrippingLine(self._myname+'Bu2D0h_D02KShh_KSLL'
                                     , prescale = config['PrescaleLL']
                                     , postscale = config['PostscaleLL']
                                     , FILTER = B2Dh_D2KShh_StrippingNumTracksGECLL   
                                     , selection = self.Bu2D0h_LLSel_noPID       
                                     )

        self.Line_DD_Bu2D0h_D02KShh = StrippingLine(self._myname+'Bu2D0h_D02KShh_KSDD'
                                     , prescale = config['PrescaleDD']
                                     , postscale = config['PostscaleDD']
                                     , FILTER = B2Dh_D2KShh_StrippingNumTracksGECDD
                                     , selection = self.Bu2D0h_DDSel_noPID
                                     )
        
        self.registerLine(self.Line_LL_Bu2D0h_D02KShh)
        self.registerLine(self.Line_DD_Bu2D0h_D02KShh)

    def printCuts(self):
        '''Print the compiled cut values'''
     #   print 'KS daughter pi cuts, LL ', self.LLPi_FilterCut
     #   print 'KS combination cuts, LL ', self.KSLL_CombCut
     #   print 'KS mother cuts, LL ', self.KSLL_MotherCut
        print 'D daughter pi/K cuts, LL ', self.D_LL_DaughterCut
        print 'D combination cuts, LL ', self.D_LL_CombCut
        print 'D mother cuts, LL ', self.D_LL_MotherCut
        print 'Bachelor K/pi cuts, LL ', self.B_LL_BachCut
        print 'B combination cuts, LL ', self.B_LL_CombCut
        print 'B mother cuts, LL ', self.B_LL_MotherCut
     #   print 'KS daughter pi cuts, DD ', self.DDPi_FilterCut
     #   print 'KS combination cuts, DD ', self.KSDD_CombCut
     #   print 'KS mother cuts, DD ', self.KSDD_MotherCut
        print 'D daughter pi/K cuts, DD ', self.D_DD_DaughterCut
        print 'D combination cuts, DD ', self.D_DD_CombCut
        print 'D mother cuts, DD ', self.D_DD_MotherCut
        print 'Bachelor K/pi cuts, DD ', self.B_DD_BachCut
        print 'B combination cuts, DD ', self.B_DD_CombCut
        print 'B mother cuts, DD ', self.B_DD_MotherCut

    ###############################################
    # Create selections for StrippingLines        #
    ############################################### 

        ###########################
        # KS daughters, long long # 
        ###########################
#   def LLPionFilterForBu2D0h_D02KShhAlg(self):
#       '''
#       Pion filter for input to LL KS - uses long pions
#       '''
#       import GaudiKernel.SystemOfUnits as Units
#       from PhysSelPython.Wrappers import Selection
#	
#	import StandardParticles
#	if hasattr(StandardParticles, "StdAllNoPIDsPions"):
#		from StandardParticles import StdAllNoPIDsPions as MyStdPions_KS2PiPi_LL_noPID
#	else:
#  		from StandardParticles import StdNoPIDsPions as MyStdPions_KS2PiPi_LL_noPID
#        
#	#from StandardParticles import StdAllNoPIDsPions as MyStdPions_KS2PiPi_LL_noPID
#
#        LLPionFilterForBu2D0h_D02KShh = FilterDesktop(Code = self.LLPi_FilterCut)
#        
#        SelLLPionFilterForBu2D0h_D02KShh = Selection(self._myname+"SelLLPionFilterForBu2D0h_D02KShh", Algorithm=LLPionFilterForBu2D0h_D02KShh,
#                                                       RequiredSelections = [MyStdPions_KS2PiPi_LL_noPID]
#                                                       )
#        
#        self.FilteredPions_KSLL_noPID=SelLLPionFilterForBu2D0h_D02KShh
        
    
        #######################################
        # KS daughters, downstream downstream #
        #######################################
#    def DDPionFilterForBu2D0h_D02KShhAlg(self):
#        '''
#        Pion filter for input to DD KS - uses no PID downstream pions
#        '''
#        import GaudiKernel.SystemOfUnits as Units
#        from PhysSelPython.Wrappers import Selection
#
#        from StandardParticles import StdNoPIDsDownPions as MyStdPions_KS2PiPi_DD_noPID
#
#        DDPionFilterForBu2D0h_D02KShh = FilterDesktop(Code= self.DDPi_FilterCut)
#       
#        SelDDPionFilterForBu2D0h_D02KShh = Selection(self._myname+"SelDDPionFilterForBu2D0h_D02KShh", Algorithm=DDPionFilterForBu2D0h_D02KShh,
#                                                       RequiredSelections = [MyStdPions_KS2PiPi_DD_noPID]
#                                                       )
#        
#        self.FilteredPions_KSDD_noPID=SelDDPionFilterForBu2D0h_D02KShh
        
        ###########################
        # KS, long long daughters #
        ###########################
#    def KSLLForBu2D0h_D02KShhAlg(self):
#        '''
#        KS LL selection
#        '''
#        import GaudiKernel.SystemOfUnits as Units
#        from PhysSelPython.Wrappers import Selection
#
#        KSLLForBu2D0h_D02KShh = CombineParticles(DecayDescriptor = "KS0 -> pi+ pi-",
#                                                 DaughtersCuts = {"pi+": "ALL"},
#                                                 CombinationCut = self.KSLL_CombCut,
#                                                 MotherCut = self.KSLL_MotherCut)
#
#        SelKSLLForBu2D0h_D02KShh = Selection(self._myname+"SelKSLLForBu2D0h_D02KShh", Algorithm=KSLLForBu2D0h_D02KShh,
#                                               RequiredSelections = [self.FilteredPions_KSLL_noPID]
#                                               )
#        
#        self.SelKSLL_Bu2D0h_noPID=SelKSLLForBu2D0h_D02KShh
        
        #######################################
        # KS, downstream downstream daughters #
        #######################################
#    def KSDDForBu2D0h_D02KShhAlg(self):
#        '''
#        KS DD selection
#        '''
#        import GaudiKernel.SystemOfUnits as Units
#        from PhysSelPython.Wrappers import Selection
#
#        KSDDForBu2D0h_D02KShh = CombineParticles(DecayDescriptor = "KS0 -> pi+ pi-" ,
#                                                 DaughtersCuts = {"pi+" : "ALL"},
#                                                 CombinationCut = self.KSDD_CombCut,
#                                                 MotherCut = self.KSDD_MotherCut)
#        
#        SelKSDDForBu2D0h_D02KShh = Selection(self._myname+"SelKSDDForBu2D0h_D02KShh", Algorithm=KSDDForBu2D0h_D02KShh,
#                                               RequiredSelections = [self.FilteredPions_KSDD_noPID]
#                                               )
#        
#        self.SelKSDD_Bu2D0h_noPID=SelKSDDForBu2D0h_D02KShh
        
        ###################################
        # D0->KShh, with KS LL daughter #
        ###################################
    def DForBu2D0h_D02KShh_KSLLAlg(self):
        '''
        D0->KS Pi Pi, D0->KS K Pi, D0->KS K K, KS LL selections 
        '''
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import Selection

	import StandardParticles
	if hasattr(StandardParticles, "StdAllNoPIDsPions"):
		from StandardParticles import StdAllNoPIDsPions as MyStdLoosePions_D2KShh_LL
	else:
       		from StandardParticles import StdNoPIDsPions as MyStdLoosePions_D2KShh_LL

	if hasattr(StandardParticles, "StdAllNoPIDsKaons"):
		from StandardParticles import StdAllNoPIDsKaons as MyStdLooseKaons_D2KShh_LL
	else:
       		from StandardParticles import StdNoPIDsKaons as MyStdLooseKaons_D2KShh_LL


	SelKSLL_Bu2D0h_noPID = DataOnDemand( Location = "Phys/StdVeryLooseKsLL/Particles" )


        DForBu2D0h_D02KShh_KSLL = CombineParticles(DecayDescriptors = ["[D0 -> KS0 pi+ pi-]cc", "[D0 -> KS0 K+ pi-]cc","[D0 -> KS0 K+ K-]cc"],
                                                   DaughtersCuts  = {"pi+" : self.D_LL_DaughterCut
                                                  ,"K+": self.D_LL_DaughterCut
                                                  ,"KS0" : "ALL"
                                                  },
                                                   CombinationCut =  self.D_LL_CombCut,
                                                   MotherCut      =  self.D_LL_MotherCut)
  
        SelDForBu2D0h_D02KShh_KSLL = Selection(self._myname+"SelDForBu2D0h_D02KShh_KSLL", Algorithm=DForBu2D0h_D02KShh_KSLL,
                                                 RequiredSelections = [SelKSLL_Bu2D0h_noPID, MyStdLoosePions_D2KShh_LL, MyStdLooseKaons_D2KShh_LL ])
        
        self.MergedD0_KShh_noPID_LL=SelDForBu2D0h_D02KShh_KSLL

        ###################################
        # D0->KShh, with KS DD daughter #
        ###################################
    def DForBu2D0h_D02KShh_KSDDAlg(self):
        '''
        D0->KS Pi Pi, D0->KS K Pi, D0->KS K K, KS DD selections
        '''
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import Selection
        
	import StandardParticles
	if hasattr(StandardParticles, "StdAllNoPIDsPions"):
		from StandardParticles import StdAllNoPIDsPions as MyStdLoosePions_D2KShh_DD
	else:
       		from StandardParticles import StdNoPIDsPions as MyStdLoosePions_D2KShh_DD

	if hasattr(StandardParticles, "StdAllNoPIDsKaons"):
		from StandardParticles import StdAllNoPIDsKaons as MyStdLooseKaons_D2KShh_DD
	else:
       		from StandardParticles import StdNoPIDsKaons as MyStdLooseKaons_D2KShh_DD


 	SelKSDD_Bu2D0h_noPID = DataOnDemand( Location = "Phys/StdVeryLooseKsDD/Particles" )


        DForBu2D0h_D02KShh_KSDD = CombineParticles(DecayDescriptors = ["[D0 -> KS0 pi+ pi-]cc", "[D0 -> KS0 K+ pi-]cc","[D0 -> KS0 K+ K-]cc"],
                                                   DaughtersCuts  = {"pi+" : self.D_DD_DaughterCut
                                                   , "K+": self.D_DD_DaughterCut
                                                   ,"KS0" : "ALL"
                                                   },
                                                   CombinationCut =  self.D_DD_CombCut,
                                                   MotherCut      =  self.D_DD_MotherCut)
        
        SelDForBu2D0h_D02KShh_KSDD = Selection(self._myname+"SelDForBu2D0h_D02KShh_KSDD", Algorithm=DForBu2D0h_D02KShh_KSDD,
                                                 RequiredSelections = [SelKSDD_Bu2D0h_noPID, MyStdLoosePions_D2KShh_DD, MyStdLooseKaons_D2KShh_DD])
                
        self.MergedD0_KShh_noPID_DD=SelDForBu2D0h_D02KShh_KSDD


        ################
        # B, LL KS     #
        ################
    def Bu2D0h_KShh_KSLLAlg(self):
        '''
        B->D0K,pi D0->KS Pi Pi, KS LL selection
        '''
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import Selection

        from StandardParticles import StdNoPIDsKaons as MyStdNoPIDKaons_BDh_LL
        from StandardParticles import StdNoPIDsPions as MyStdNoPIDPions_BDh_LL
        
        Bu2D0h_KSLL = CombineParticles(DecayDescriptors = ["[B+ -> D~0 K+]cc", "[B+ -> D~0 pi+]cc"],
                                       DaughtersCuts = { "D0" : "ALL",
                                      "K+" : self.B_LL_BachCut,
                                      "pi+" : self.B_LL_BachCut
                                                         },
                                       CombinationCut =  self.B_LL_CombCut,
                                       MotherCut = self.B_LL_MotherCut )

        SelBu2D0h_KSLL = Selection(self._myname+"SelBu2D0h_KSLL", Algorithm=Bu2D0h_KSLL,
                                    RequiredSelections = [self.MergedD0_KShh_noPID_LL, MyStdNoPIDKaons_BDh_LL, MyStdNoPIDPions_BDh_LL])
        
        self.Bu2D0h_LLSel_noPID=SelBu2D0h_KSLL



        ################
        # B, DD KS     #
        ################
    def Bu2D0h_KShh_KSDDAlg(self):
        '''
        B->D0K,pi D0->KS Pi Pi, KS DD selection
        '''
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import Selection

        from StandardParticles import StdNoPIDsKaons as MyStdNoPIDKaons_BDh_DD
        from StandardParticles import StdNoPIDsPions as MyStdNoPIDPions_BDh_DD

        Bu2D0h_KSDD = CombineParticles(DecayDescriptors = ["[B+ -> D~0 K+]cc", "[B+ -> D~0 pi+]cc"],
                                       DaughtersCuts = { "D0" : "ALL",
                                      "K+" : self.B_DD_BachCut,
                                      "pi+" : self.B_DD_BachCut
                                      },
                                       CombinationCut = self.B_DD_CombCut,
                                       MotherCut = self.B_DD_MotherCut)

        SelBu2D0h_KSDD = Selection(self._myname+"SelBu2D0h_KSDD", Algorithm=Bu2D0h_KSDD,
                                    RequiredSelections = [self.MergedD0_KShh_noPID_DD, MyStdNoPIDKaons_BDh_DD, MyStdNoPIDPions_BDh_DD])
        
        self.Bu2D0h_DDSel_noPID=SelBu2D0h_KSDD
