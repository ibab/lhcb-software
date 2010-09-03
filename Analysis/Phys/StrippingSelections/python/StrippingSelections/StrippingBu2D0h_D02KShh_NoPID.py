

__author__ = 'Susan Haines'
__date__ = '03/10/2010'
__version__ = '$Revision: 1.3 $'

'''
Bu->D0(KShh)h stripping selection using LoKi::Hybrid and python
configurables for new MC09 offline selection, using StdNoPIDs particles.  Only PID cut on Ks DD daughter pions.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

## confdict={ #pre-17/08/2010
##     'DefaultCuts' :   {   'PrescaleLL'    : 1.0 
##                        ,  'PostscaleLL'   : 1.0
##                        #B cuts, LL seln
##                        ,  'BFlightCHI2_LL'        : 144.  
##                        ,  'BDIRA_LL'           : 0.99985     
##                        ,  'BIPCHI2_LL'         : 16.         
##                        ,  'BVertexCHI2_LL'     : 9.
##                        #Bachelor K cuts, LL seln
##                        ,  'BachIPCHI2_LL'     : 9.
##                        ,  'BachPt_LL'         : 1.0
##                        #D0 and D0 daughter pi cuts, LL seln
##                        ,  'DVertexCHI2_LL'     : 6.25     
##                        ,  'DdaughterIPCHI2_LL' : 9.
##                        #KS and KS daughter pi cuts, LL seln
##                        ,  'KSFlightCHI2_LL'    : 4.
##                        ,  'KSVertexCHI2_LL'    : 20.25
##                        ,  'KSdaughterPiIPCHI2_LL' : 4.
##                        ,  'PrescaleDD'    : 1.0 
##                        ,  'PostscaleDD'   : 1.0
##                        #B cuts, DD seln
##                        ,  'BFlightCHI2_DD'     : 81.
##                        ,  'BDIRA_DD'           : 0.9998    
##                        ,  'BIPCHI2_DD'         : 25.         
##                        ,  'BVertexCHI2_DD'     : 12.25
##                        #Bachelor K cuts, DD seln
##                        ,  'BachIPCHI2_DD'     : 4.
##                        ,  'BachPt_DD'         : 0.5  
##                        #D0 and D0 daughter pi cuts, DD seln
##                        ,  'DVertexCHI2_DD'     : 16.        
##                        ,  'DdaughterIPCHI2_DD' : 4.
##                         #KS and KS daughter pi cuts, DD seln
##                        ,  'KSFlightCHI2_DD'    : 4.
##                        ,  'KSVertexCHI2_DD'    : 20.25
##                        ,  'KSdaughterPiIPCHI2_DD' : 4.
##                        }
##  }

confdict={# new cuts 17/08/2010 onwards
     'DefaultCuts' :   {   'PrescaleLL'    : 1.0 
                        ,  'PostscaleLL'   : 1.0
                        #B cuts, LL seln
                       ,  'BFlightCHI2_LL'        : 156.25 
                       ,  'BDIRA_LL'           : 0.9999     
                       ,  'BIPCHI2_LL'         : 12.25         
                       ,  'BVertexCHI2_LL'     : 9.
                       #Bachelor K cuts, LL seln
                       ,  'BachIPCHI2_LL'     : 12.25
                       ,  'BachPt_LL'         : 1.0  
                       #D0 and D0 daughter pi cuts, LL seln
                       ,  'DVertexCHI2_LL'     : 6.25     
                       ,  'DdaughterIPCHI2_LL' : 9.
                       #KS and KS daughter pi cuts, LL seln
                       ,  'KSFlightCHI2_LL'    : 4.
                       ,  'KSVertexCHI2_LL'    : 18.
                       ,  'KSdaughterPiIPCHI2_LL' : 6.25
                       ,  'PrescaleDD'    : 1.0 
                       ,  'PostscaleDD'   : 1.0
                       #B cuts, DD seln
                       ,  'BFlightCHI2_DD'     : 90.25
                       ,  'BDIRA_DD'           : 0.99985    
                       ,  'BIPCHI2_DD'         : 18.         
                       ,  'BVertexCHI2_DD'     : 12.25
                       #Bachelor K cuts, DD seln
                       ,  'BachIPCHI2_DD'     : 4.
                       ,  'BachPt_DD'         : 0.5 
                       #D0 and D0 daughter pi cuts, DD seln
                       ,  'DVertexCHI2_DD'     : 16.        
                       ,  'DdaughterIPCHI2_DD' : 9.
                        #KS and KS daughter pi cuts, DD seln
                       ,  'KSFlightCHI2_DD'    : 4.
                       ,  'KSVertexCHI2_DD'    : 18.
                       ,  'KSdaughterPiIPCHI2_DD' : 4.
                       }
 }






class StrippingBu2D0h_D02KShh_NoPIDConf(object):
    '''
    Configuration object for a B->D0h, D0->KShh lines, KS->LL and DD pions, where h is K or Pi.
    
    Some cuts are hardcoded, e.g. mass windows.

    PID cuts only applied to downstream Ks daughter pions, all input Pi and K are StdNoPIDs particles.
    
    Use conf.printCuts to check the cuts in python.
    
    The Lines object holds the configured line.
    '''

    Line_LL_noPID=None
    Line_DD_noPID=None

    LLPi_FilterCut = ""
    KSLL_CombCut = ""
    KSLL_MotherCut = ""
    D_LL_DaughterCut = ""
    D_LL_CombCut = ""
    D_LL_MotherCut = ""  
    B_LL_BachCut = ""
    B_LL_CombCut = ""
    B_LL_MotherCut = ""
    
    DDPi_FilterCut = ""
    KSDD_CombCut = ""
    KSDD_MotherCut = ""
    D_DD_DaughterCut = ""
    D_DD_CombCut = ""
    D_DD_MotherCut = ""
    B_DD_BachCut = ""
    B_DD_CombCut = ""
    B_DD_MotherCut = ""
   
    FilteredPions_KSLL_noPID=None
    SelKSLL_Bu2D0h_noPID=None
    mergedPiKforD_LL=None
    MergedD0_KShh_noPID_LL=None
    Bu2D0h_LLSel_noPID=None
    
    FilteredPions_KSDD_noPID=None
    SelKSDD_Bu2D0h_noPID=None
    mergedPiKforD_DD=None
    MergedD0_KShh_noPID_DD=None
    Bu2D0h_DDSel_noPID=None

    __configuration_keys__ = [ 
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
        ,  'KSFlightCHI2_LL'    
        ,  'KSVertexCHI2_LL'    
        ,  'KSdaughterPiIPCHI2_LL'
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
        ,  'KSFlightCHI2_DD'    
        ,  'KSVertexCHI2_DD'    
        ,  'KSdaughterPiIPCHI2_DD' 
        ]

    def __init__(self, config = confdict['DefaultCuts']):
        '''The constructor of the configuration class.
        Requires a configuration dictionary, config, which must provide all the settings for cuts which are not hard coded'''
        
        from StrippingSelections.Utils import checkConfig
        
        checkConfig(StrippingBu2D0h_D02KShh_NoPIDConf.__configuration_keys__, config)

        #define cuts
        #LL selection cuts
        self.LLPi_FilterCut = "(P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<10.) & (BPVIPCHI2() > %(KSdaughterPiIPCHI2_LL)s)"%config
        self.KSLL_CombCut = "(ADAMASS('KS0') < 35.*MeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<1.5)"
        self.KSLL_MotherCut = "(ADMASS('KS0') < 30.*MeV) & (VFASPF(VCHI2/VDOF) < %(KSVertexCHI2_LL)s) & (BPVVDCHI2 > %(KSFlightCHI2_LL)s)"%config
        self.D_LL_DaughterCut = "(P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<10.) & (BPVIPCHI2() > %(DdaughterIPCHI2_LL)s) "%config
        self.D_LL_CombCut = "(ADAMASS('D0') < 55.*MeV) & (ADOCACUT(1.8, 'LoKi::TrgDistanceCalculator'))"
        self.D_LL_MotherCut = "(ADMASS('D0') < 50.*MeV) & (VFASPF(VCHI2/VDOF) < %(DVertexCHI2_LL)s)"%config
        self.B_LL_BachCut = "(PT > %(BachPt_LL)s *GeV ) & (P > 2.*GeV) & (P < 100.*GeV) &  (TRCHI2DOF<10.) & (BPVIPCHI2() > %(BachIPCHI2_LL)s )"%config
        self.B_LL_CombCut = "(ADAMASS('B+') < 505.*MeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<0.5)"
        self.B_LL_MotherCut = "(ADMASS('B+') < 500.*MeV)  & (VFASPF(VCHI2/VDOF)<%(BVertexCHI2_LL)s) & (BPVIPCHI2() < %(BIPCHI2_LL)s) & (BPVVDCHI2 > %(BFlightCHI2_LL)s) & (BPVDIRA > %(BDIRA_LL)s)"%config


        #DD selection cuts
        self.DDPi_FilterCut = "(ISDOWN) & (P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<10.) & (BPVIPCHI2() > %(KSdaughterPiIPCHI2_DD)s)  & ((-PIDK) > 0.)"%config
        self.KSDD_CombCut = "(ADAMASS('KS0') < 50.*MeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<22.)"
        self.KSDD_MotherCut = "(ADMASS('KS0') < 42.*MeV) & (VFASPF(VCHI2/VDOF) < %(KSVertexCHI2_DD)s) & (BPVVDCHI2 > %(KSFlightCHI2_DD)s)" %config
        self.D_DD_DaughterCut = " (P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<10.) & (BPVIPCHI2() > %(DdaughterIPCHI2_DD)s)"%config
        self.D_DD_CombCut = "(ADAMASS('D0') < 65.*MeV) & (ADOCACUT(9.2, 'LoKi::TrgDistanceCalculator'))"
        self.D_DD_MotherCut = "(ADMASS('D0') < 60.*MeV) & (VFASPF(VCHI2/VDOF) < %(DVertexCHI2_DD)s)"%config
        self.B_DD_BachCut = "(PT > %(BachPt_DD)s *GeV) & (P > 2.*GeV) & (P < 100.*GeV) & (TRCHI2DOF<10.) & (BPVIPCHI2() > %(BachIPCHI2_DD)s)"%config 
        self.B_DD_CombCut = "(ADAMASS('B+') < 505.*MeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<0.5)"
        self.B_DD_MotherCut = "(ADMASS('B+') < 500.*MeV)  & (VFASPF(VCHI2/VDOF) < %(BVertexCHI2_DD)s) & (BPVIPCHI2() < %(BIPCHI2_DD)s) & (BPVVDCHI2 > %(BFlightCHI2_DD)s)& (BPVDIRA > %(BDIRA_DD)s)"%config


        from StrippingConf.StrippingLine import StrippingLine
        from PhysSelPython.Wrappers import SelectionSequence

        #Make the selections
        self.GEC_NumTracks_LL()
        self.LLPionFilterForBu2D0h_D02KShhAlg()
        self.KSLLForBu2D0h_D02KShhAlg()
        self.DForBu2D0h_D02KShh_KSLLAlg()
        self.Bu2D0h_KShh_KSLLAlg()
        
        self.GEC_NumTracks_DD()
        self.DDPionFilterForBu2D0h_D02KShhAlg()   
        self.KSDDForBu2D0h_D02KShhAlg()
        self.DForBu2D0h_D02KShh_KSDDAlg()
        self.Bu2D0h_KShh_KSDDAlg()
    
        #Stripping lines
        LL_line=StrippingLine('Bu2D0h_D02KShh_KSLL'
                          , prescale = config['PrescaleLL']
                          , postscale = config['PostscaleLL']
                          , algos = [self.GEC_NumTracks_LL(), self.Bu2D0h_LLSel_noPID]
                         )
    
        DD_line=StrippingLine('Bu2D0h_D02KShh_KSDD'
                          , prescale = config['PrescaleDD']
                          , postscale = config['PostscaleDD']
                          , algos = [self.GEC_NumTracks_DD(), self.Bu2D0h_DDSel_noPID]
                         )


        self.Line_LL = LL_line
        self.Line_DD = DD_line


    def printCuts(self):
        '''Print the compiled cut values'''
        print 'KS daughter pi cuts, LL ', self.LLPi_FilterCut
        print 'KS combination cuts, LL ', self.KSLL_CombCut
        print 'KS mother cuts, LL ', self.KSLL_MotherCut
        print 'D daughter pi/K cuts, LL ', self.D_LL_DaughterCut
        print 'D combination cuts, LL ', self.D_LL_CombCut
        print 'D mother cuts, LL ', self.D_LL_MotherCut
        print 'Bachelor K/pi cuts, LL ', self.B_LL_BachCut
        print 'B combination cuts, LL ', self.B_LL_CombCut
        print 'B mother cuts, LL ', self.B_LL_MotherCut
        print 'KS daughter pi cuts, DD ', self.DDPi_FilterCut
        print 'KS combination cuts, DD ', self.KSDD_CombCut
        print 'KS mother cuts, DD ', self.KSDD_MotherCut
        print 'D daughter pi/K cuts, DD ', self.D_DD_DaughterCut
        print 'D combination cuts, DD ', self.D_DD_CombCut
        print 'D mother cuts, DD ', self.D_DD_MotherCut
        print 'Bachelor K/pi cuts, DD ', self.B_DD_BachCut
        print 'B combination cuts, DD ', self.B_DD_CombCut
        print 'B mother cuts, DD ', self.B_DD_MotherCut

    

    ###############################################
    # Create selections for StrippingLines        #
    ############################################### 

    def GEC_NumTracks_LL( self ):
        '''
        Define the GEC on number of tracks, needed in order to control
        the time for the combinatorics (D in particular)
        '''
        from Configurables import LoKi__VoidFilter as VoidFilter# Need the Void filter for the GEC
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        modules =  CoreFactory('CoreFactory').Modules
        for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)

        B2Dh_KSLL_StrippingNumTracksGEC = VoidFilter('B2Dh_KSLL_StrippingNumTracksGEC'
                                          , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )"
                                          )

        return B2Dh_KSLL_StrippingNumTracksGEC

    def GEC_NumTracks_DD( self ):
        '''
        Define the GEC on number of tracks, needed in order to control
        the time for the combinatorics (D in particular)
        '''
        from Configurables import LoKi__VoidFilter as VoidFilter# Need the Void filter for the GEC
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        modules =  CoreFactory('CoreFactory').Modules
        for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)

        B2Dh_KSDD_StrippingNumTracksGEC = VoidFilter('B2Dh_KSDD_StrippingNumTracksGEC'
                                          , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )"
                                          )
      
        return B2Dh_KSDD_StrippingNumTracksGEC


        ###########################
        # KS daughters, long long # 
        ###########################
    def LLPionFilterForBu2D0h_D02KShhAlg(self):
        '''
        Pion filter for input to LL KS - uses long pions
        '''
        from Configurables import FilterDesktop
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import Selection, DataOnDemand

        MyStdPions_KS2PiPi_LL_noPID = DataOnDemand(Location = 'Phys/StdNoPIDsPions')

        LLPionFilterForBu2D0h_D02KShh = FilterDesktop("LLPionFilterForBu2D0h_D02KShh")
        
        LLPionFilterForBu2D0h_D02KShh.Code = self.LLPi_FilterCut
        
        SelLLPionFilterForBu2D0h_D02KShh = Selection("SelLLPionFilterForBu2D0h_D02KShh", Algorithm=LLPionFilterForBu2D0h_D02KShh,
                                                       RequiredSelections = [MyStdPions_KS2PiPi_LL_noPID]
                                                       )
        
        self.FilteredPions_KSLL_noPID=SelLLPionFilterForBu2D0h_D02KShh
        
    
        #######################################
        # KS daughters, downstream downstream #
        #######################################
    def DDPionFilterForBu2D0h_D02KShhAlg(self):
        '''
        Pion filter for input to DD KS - uses no PID downstream pions
        '''
        from Configurables import FilterDesktop
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import Selection, DataOnDemand

        MyStdPions_KS2PiPi_DD_noPID = DataOnDemand(Location = 'Phys/StdNoPIDsDownPions')

        DDPionFilterForBu2D0h_D02KShh = FilterDesktop("DDPionFilterForBu2D0h_D02KShh")
        
        DDPionFilterForBu2D0h_D02KShh.Code = self.DDPi_FilterCut
       
        SelDDPionFilterForBu2D0h_D02KShh = Selection("SelDDPionFilterForBu2D0h_D02KShh", Algorithm=DDPionFilterForBu2D0h_D02KShh,
                                                       RequiredSelections = [MyStdPions_KS2PiPi_DD_noPID]
                                                       )
        
        self.FilteredPions_KSDD_noPID=SelDDPionFilterForBu2D0h_D02KShh
        
        ###########################
        # KS, long long daughters #
        ###########################
    def KSLLForBu2D0h_D02KShhAlg(self):
        '''
        KS LL selection
        '''
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import Selection

        KSLLForBu2D0h_D02KShh = CombineParticles("KSLLForBu2D0h_D02KShh")
        
        KSLLForBu2D0h_D02KShh.DecayDescriptor = "KS0 -> pi+ pi-"
        KSLLForBu2D0h_D02KShh.DaughtersCuts = {"pi+": "ALL"}
        KSLLForBu2D0h_D02KShh.CombinationCut = self.KSLL_CombCut
        KSLLForBu2D0h_D02KShh.MotherCut = self.KSLL_MotherCut

#        from Configurables import LoKi__VertexFitter
#        lvf = LoKi__VertexFitter()
#        KSLLForBu2D0h_D02KShh.addTool(lvf)
#        KSLLForBu2D0h_D02KShh.ParticleCombiners.update({'':lvf, 'Offline':lvf}) 

        SelKSLLForBu2D0h_D02KShh = Selection("SelKSLLForBu2D0h_D02KShh", Algorithm=KSLLForBu2D0h_D02KShh,
                                               RequiredSelections = [self.FilteredPions_KSLL_noPID]
                                               )
        
        self.SelKSLL_Bu2D0h_noPID=SelKSLLForBu2D0h_D02KShh
        
        #######################################
        # KS, downstream downstream daughters #
        #######################################
    def KSDDForBu2D0h_D02KShhAlg(self):
        '''
        KS DD selection
        '''
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import Selection

        KSDDForBu2D0h_D02KShh = CombineParticles("KSDDForBu2D0h_D02KShh")
  
        KSDDForBu2D0h_D02KShh.DecayDescriptor = "KS0 -> pi+ pi-" 
        KSDDForBu2D0h_D02KShh.DaughtersCuts = {"pi+" : "ALL"}
        KSDDForBu2D0h_D02KShh.CombinationCut = self.KSDD_CombCut
        KSDDForBu2D0h_D02KShh.MotherCut = self.KSDD_MotherCut

#        from Configurables import LoKi__VertexFitter
#        lvf = LoKi__VertexFitter()
#        KSDDForBu2D0h_D02KShh.addTool(lvf)
#        KSDDForBu2D0h_D02KShh.ParticleCombiners.update({'':lvf, 'Offline':lvf}) 
        
        SelKSDDForBu2D0h_D02KShh = Selection("SelKSDDForBu2D0h_D02KShh", Algorithm=KSDDForBu2D0h_D02KShh,
                                               RequiredSelections = [self.FilteredPions_KSDD_noPID]
                                               )
        
        self.SelKSDD_Bu2D0h_noPID=SelKSDDForBu2D0h_D02KShh
        
        ###################################
        # D0->KShh, with KS LL daughter #
        ###################################
    def DForBu2D0h_D02KShh_KSLLAlg(self):
        '''
        D0->KS Pi Pi, D0->KS K Pi, D0->KS K K, KS LL selections 
        '''
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import MergedSelection, Selection, DataOnDemand

        MyStdLoosePions_D2KShh_LL = DataOnDemand(Location = 'Phys/StdNoPIDsPions')
        MyStdLooseKaons_D2KShh_LL = DataOnDemand(Location = 'Phys/StdNoPIDsKaons')
        mergedD0_PionsandKaonsLL = MergedSelection('mergedD0_PionsandKaonsLL', RequiredSelections = [MyStdLoosePions_D2KShh_LL, MyStdLooseKaons_D2KShh_LL])
        self.mergedPiKforD_LL = mergedD0_PionsandKaonsLL
        
        DForBu2D0h_D02KShh_KSLL = CombineParticles("DForBu2D0h_D02KShh_KSLL")
        DForBu2D0h_D02KShh_KSLL.DecayDescriptors = ["[D0 -> KS0 pi+ pi-]cc", "[D0 -> KS0 K+ pi-]cc","[D0 -> KS0 K+ K-]cc"]
        DForBu2D0h_D02KShh_KSLL.DaughtersCuts  = {"pi+" : self.D_LL_DaughterCut
                                                  ,"K+": self.D_LL_DaughterCut
                                                  ,"KS0" : "ALL"
                                                  }
        DForBu2D0h_D02KShh_KSLL.CombinationCut =  self.D_LL_CombCut
        DForBu2D0h_D02KShh_KSLL.MotherCut      =  self.D_LL_MotherCut

#        from Configurables import LoKi__VertexFitter
#        lvf = LoKi__VertexFitter()
#        DForBu2D0h_D02KShh_KSLL.addTool(lvf)
#        DForBu2D0h_D02KShh_KSLL.ParticleCombiners.update({'':lvf, 'Offline':lvf}) 
  
        SelDForBu2D0h_D02KShh_KSLL = Selection("SelDForBu2D0h_D02KShh_KSLL", Algorithm=DForBu2D0h_D02KShh_KSLL,
                                                 RequiredSelections = [self.SelKSLL_Bu2D0h_noPID, MyStdLoosePions_D2KShh_LL, MyStdLooseKaons_D2KShh_LL ])
        
        self.MergedD0_KShh_noPID_LL=SelDForBu2D0h_D02KShh_KSLL

        ###################################
        # D0->KShh, with KS DD daughter #
        ###################################
    def DForBu2D0h_D02KShh_KSDDAlg(self):
        '''
        D0->KS Pi Pi, D0->KS K Pi, D0->KS K K, KS DD selections
        '''
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import MergedSelection, Selection, DataOnDemand

        MyStdLoosePions_D2KShh_DD = DataOnDemand(Location = 'Phys/StdNoPIDsPions')
        MyStdLooseKaons_D2KShh_DD = DataOnDemand(Location = 'Phys/StdNoPIDsKaons')

        DForBu2D0h_D02KShh_KSDD = CombineParticles("DForBu2D0h_D02KShh_KSDD")
        DForBu2D0h_D02KShh_KSDD.DecayDescriptors = ["[D0 -> KS0 pi+ pi-]cc", "[D0 -> KS0 K+ pi-]cc","[D0 -> KS0 K+ K-]cc"]
        DForBu2D0h_D02KShh_KSDD.DaughtersCuts  = {"pi+" : self.D_DD_DaughterCut
                                                   , "K+": self.D_DD_DaughterCut
                                                   ,"KS0" : "ALL"
                                                   }
        DForBu2D0h_D02KShh_KSDD.CombinationCut =  self.D_DD_CombCut
        DForBu2D0h_D02KShh_KSDD.MotherCut      =  self.D_DD_MotherCut

#        from Configurables import LoKi__VertexFitter
#        lvf = LoKi__VertexFitter()
#        DForBu2D0h_D02KShh_KSDD.addTool(lvf)
#        DForBu2D0h_D02KShh_KSDD.ParticleCombiners.update({'':lvf, 'Offline':lvf}) 
        
        SelDForBu2D0h_D02KShh_KSDD = Selection("SelDForBu2D0h_D02KShh_KSDD", Algorithm=DForBu2D0h_D02KShh_KSDD,
                                                 RequiredSelections = [self.SelKSDD_Bu2D0h_noPID, MyStdLoosePions_D2KShh_DD, MyStdLooseKaons_D2KShh_DD])
                
        self.MergedD0_KShh_noPID_DD=SelDForBu2D0h_D02KShh_KSDD


        ################
        # B, LL KS     #
        ################
    def Bu2D0h_KShh_KSLLAlg(self):
        '''
        B->D0K, D0->KS Pi Pi, KS LL selection
        '''
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import Selection, DataOnDemand

        MyStdNoPIDKaons_BDh_LL = DataOnDemand(Location = 'Phys/StdNoPIDsKaons')

        Bu2D0h_KSLL = CombineParticles("Bu2D0h_KSLL")
        Bu2D0h_KSLL.DecayDescriptor = "[B+ -> D~0 K+]cc"

        Bu2D0h_KSLL.DaughtersCuts = { "D0" : "ALL",
                                      "K+" : self.B_LL_BachCut
                                    }
        Bu2D0h_KSLL.CombinationCut =  self.B_LL_CombCut
        Bu2D0h_KSLL.MotherCut = self.B_LL_MotherCut 

#        from Configurables import LoKi__VertexFitter
#        lvf = LoKi__VertexFitter()
#        Bu2D0h_KSLL.addTool(lvf)
#        Bu2D0h_KSLL.ParticleCombiners.update({'':lvf, 'Offline':lvf}) 
        
        SelBu2D0h_KSLL = Selection("SelBu2D0h_KSLL", Algorithm=Bu2D0h_KSLL,
                                    RequiredSelections = [self.MergedD0_KShh_noPID_LL, MyStdNoPIDKaons_BDh_LL])
        
        self.Bu2D0h_LLSel_noPID=SelBu2D0h_KSLL



        ################
        # B, DD KS     #
        ################
    def Bu2D0h_KShh_KSDDAlg(self):
        '''
        B->D0K, D0->KS Pi Pi, KS DD selection
        '''
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units
        from PhysSelPython.Wrappers import Selection, DataOnDemand

        MyStdNoPIDKaons_BDh_DD = DataOnDemand(Location = 'Phys/StdNoPIDsKaons')

        Bu2D0h_KSDD = CombineParticles("Bu2D0h_KSDD")
        Bu2D0h_KSDD.DecayDescriptor = "[B+ -> D~0 K+]cc"

        Bu2D0h_KSDD.DaughtersCuts = { "D0" : "ALL",
                                      "K+" : self.B_DD_BachCut
                                      }
        Bu2D0h_KSDD.CombinationCut = self.B_DD_CombCut
        Bu2D0h_KSDD.MotherCut = self.B_DD_MotherCut

#        from Configurables import LoKi__VertexFitter
#        lvf = LoKi__VertexFitter()
#        Bu2D0h_KSDD.addTool(lvf)
#        Bu2D0h_KSDD.ParticleCombiners.update({'':lvf, 'Offline':lvf}) 

        SelBu2D0h_KSDD = Selection("SelBu2D0h_KSDD", Algorithm=Bu2D0h_KSDD,
                                    RequiredSelections = [self.MergedD0_KShh_noPID_DD, MyStdNoPIDKaons_BDh_DD])
        
        self.Bu2D0h_DDSel_noPID=SelBu2D0h_KSDD


_StrippingBu2D0h_D02KShh_NoPIDConf = StrippingBu2D0h_D02KShh_NoPIDConf()
