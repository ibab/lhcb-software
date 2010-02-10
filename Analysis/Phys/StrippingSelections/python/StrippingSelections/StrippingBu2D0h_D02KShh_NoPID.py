

__author__ = 'Susan Haines'
__date__ = '10/02/2010'
__version__ = '$Revision: 1.1 $'

'''
Bu->D0(KShh)h stripping selection using LoKi::Hybrid and python
configurables for new MC09 offline selection, only PID cut on Ks DD daughter pions.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingBu2D0h_D02KShh_NoPIDConf(LHCbConfigurableUser):
    __slots__ = { 
        'BFlightCHI2_LL'        : 144.  
        ,  'BDIRA_LL'           : 0.99985     
        ,  'BIPCHI2_LL'         : 16.         
        ,  'BVertexCHI2_LL'     : 9.
        ,  'BachIPCHI2_LL'     : 9.
        ,  'BachPt_LL'         : 0.9  
        ,  'DVertexCHI2_LL'     : 6.25     
        ,  'DdaughterIPCHI2_LL' : 9.
        ,  'KSFlightCHI2_LL'    : 4.
        ,  'KSVertexCHI2_LL'    : 20.25
        ,  'KSdaughterPiIPCHI2_LL' : 4.
        ,  'BFlightCHI2_DD'     : 81.
        ,  'BDIRA_DD'           : 0.9998    
        ,  'BIPCHI2_DD'         : 25.         
        ,  'BVertexCHI2_DD'     : 12.25
        ,  'BachIPCHI2_DD'     : 4.
        ,  'BachPt_DD'         : 0.4  
        ,  'DVertexCHI2_DD'     : 16.        
        ,  'DdaughterIPCHI2_DD' : 4.
        ,  'KSFlightCHI2_DD'    : 4.
        ,  'KSVertexCHI2_DD'    : 20.25
        ,  'KSdaughterPiIPCHI2_DD' : 4.
        }

    _propertyDocDct = {
        'BFlightCHI2_LL'     : """ LL B flight distance chi2 cut """   
        , 'BDIRA_LL'            : """ LL B cos angle cut         """
        ,  'BIPCHI2_LL'         : """ LL B IP chi2 cut         """       
        ,  'BVertexCHI2_LL'     : """ LL B vertex chi2 cut         """
        ,  'BachIPCHI2_LL'     : """ LL Bachelor pion/kaon IP chi2 cut         """
        ,  'BachPt_LL'         : """ LL Bachelor pion/kaon Pt cut         """
        ,  'DVertexCHI2_LL'     : """ LL D0 vertex chi2 cut         """
        ,  'DdaughterIPCHI2_LL'     : """ LL D daughter pion/kaon IP chi2 cut         """
        ,  'KSFlightCHI2_LL'    : """ LL KS flight distance chi2 cut     """
        ,  'KSVertexCHI2_LL'    : """ LL KS vertex chi2 cut     """
        ,  'KSdaughterPiIPCHI2_LL'    : """ LL KS daughter pion IP chi2 cut         """
        ,  'BFlightCHI2_DD'     : """ DD B flight distance chi2 cut """   
        ,  'BDIRA_DD'           : """ DD B cos angle cut         """
        ,  'BIPCHI2_DD'         : """ DD B IP chi2 cut         """       
        ,  'BVertexCHI2_DD'     : """ DD B vertex chi2 cut         """
        ,  'BachIPCHI2_DD'     : """ DD Bachelor pion/kaon IP chi2 cut         """
        ,  'BachPt_DD'         : """ DD Bachelor kaon Pt cut         """
        ,  'DVertexCHI2_DD'     : """ DD D0 vertex chi2 cut         """
        ,  'DdaughterIPCHI2_DD'     : """ DD D daughter pion/kaon IP chi2 cut         """
        ,  'KSFlightCHI2_DD'    : """ DD KS flight distance chi2 cut     """
        ,  'KSVertexCHI2_DD'    : """ DD KS vertex chi2 cut     """
        ,  'KSdaughterPiIPCHI2_DD'    : """ DD KS daughter pion IP chi2 cut         """


        }

    ###############################################
    # Create StrippingLines with these selections #
    ###############################################
    def Bu2D0h_KSPiPi_KSLL( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        B2Dh_KSLL_StrippingNumTracksGEC = self.GEC_NumTracks_LL()
        LLPionFilterForBu2D0h_D02KShh = self.LLPionFilterForBu2D0h_D02KShhAlg()
        KSLLForBu2D0h_D02KShh = self.KSLLForBu2D0h_D02KShhAlg()
        DForBu2D0h_D02KSPiPi_KSLL = self.DForBu2D0h_D02KSPiPi_KSLLAlg()
        Bu2D0h_KSPiPi_KSLL = self.Bu2D0h_KSPiPi_KSLLAlg()
        return StrippingLine('Bu2D0h_D02KSPiPi_KSLL'
                             , prescale = 1
                             , algos = [B2Dh_KSLL_StrippingNumTracksGEC, LLPionFilterForBu2D0h_D02KShh, KSLLForBu2D0h_D02KShh, DForBu2D0h_D02KSPiPi_KSLL, Bu2D0h_KSPiPi_KSLL]
                             , postscale = 1
                             ) 
    
    def Bu2D0h_KSPiPi_KSDD( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        B2Dh_KSDD_StrippingNumTracksGEC = self.GEC_NumTracks_DD()
        DDPionFilterForBu2D0h_D02KShh = self.DDPionFilterForBu2D0h_D02KShhAlg()
        KSDDForBu2D0h_D02KShh = self.KSDDForBu2D0h_D02KShhAlg()
        DForBu2D0h_D02KSPiPi_KSDD = self.DForBu2D0h_D02KSPiPi_KSDDAlg()
        Bu2D0h_KSPiPi_KSDD = self.Bu2D0h_KSPiPi_KSDDAlg()
        return StrippingLine('Bu2D0h_D02KSPiPi_KSDD'
                             , prescale = 1
                             , algos = [B2Dh_KSDD_StrippingNumTracksGEC, DDPionFilterForBu2D0h_D02KShh, KSDDForBu2D0h_D02KShh, DForBu2D0h_D02KSPiPi_KSDD, Bu2D0h_KSPiPi_KSDD]
                             , postscale = 1
                             ) 

    def GEC_NumTracks_LL( self ):
        from Configurables import LoKi__VoidFilter as VoidFilter# Need the Void filter for the GEC
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        modules =  CoreFactory('CoreFactory').Modules
        for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)

        # Define the GEC on number of tracks, needed in order to control
        # the time for the combinatorics (D in particular)
        B2Dh_KSLL_StrippingNumTracksGEC = VoidFilter('B2Dh_KSLL_StrippingNumTracksGEC'
                                           , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )"
                                           )
        return B2Dh_KSLL_StrippingNumTracksGEC

    def GEC_NumTracks_DD( self ):
        from Configurables import LoKi__VoidFilter as VoidFilter# Need the Void filter for the GEC
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        modules =  CoreFactory('CoreFactory').Modules
        for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)

        # Define the GEC on number of tracks, needed in order to control
        # the time for the combinatorics (D in particular)
        B2Dh_KSDD_StrippingNumTracksGEC = VoidFilter('B2Dh_KSDD_StrippingNumTracksGEC'
                                           , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )"
                                           )
        return B2Dh_KSDD_StrippingNumTracksGEC


        ###########################
        # KS daughters, long long # 
        ###########################
    def LLPionFilterForBu2D0h_D02KShhAlg(self):
        from Configurables import FilterDesktop
        import GaudiKernel.SystemOfUnits as Units

        LLPi_FilterCut = "(BPVIPCHI2() > %(KSdaughterPiIPCHI2_LL)s) & (P < 100.*GeV)"% self.getProps() 
        
        LLPionFilterForBu2D0h_D02KShh = FilterDesktop("StripLLPionFilterForBu2D0h_D02KShh")
        LLPionFilterForBu2D0h_D02KShh.InputLocations = ["StdNoPIDsPions"]
        LLPionFilterForBu2D0h_D02KShh.Code = LLPi_FilterCut
        return LLPionFilterForBu2D0h_D02KShh
    
        #######################################
        # KS daughters, downstream downstream #
        #######################################
    def DDPionFilterForBu2D0h_D02KShhAlg(self):
        from Configurables import FilterDesktop
        import GaudiKernel.SystemOfUnits as Units

        DDPi_FilterCut = "(ISDOWN) & (BPVIPCHI2() > %(KSdaughterPiIPCHI2_DD)s) & (P < 100.*GeV) & ((-PIDK) > 0.)"% self.getProps()
        
        DDPionFilterForBu2D0h_D02KShh = FilterDesktop("StripDDPionFilterForBu2D0h_D02KShh")
        DDPionFilterForBu2D0h_D02KShh.InputLocations = ["StdNoPIDsDownPions"]
        DDPionFilterForBu2D0h_D02KShh.Code = DDPi_FilterCut
        return DDPionFilterForBu2D0h_D02KShh
        
        ###########################
        # KS, long long daughters #
        ###########################
    def KSLLForBu2D0h_D02KShhAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        KSLL_CombCut = "(AMINDOCA('LoKi::TrgDistanceCalculator')<2.4) & (ADAMASS('KS0') < 40.*MeV)"
        KSLL_MotherCut = "(ADMASS('KS0') < 30.*MeV) & (VFASPF(VCHI2/VDOF) < %(KSVertexCHI2_LL)s) & (BPVVDCHI2 > %(KSFlightCHI2_LL)s)"% self.getProps()

        KSLLForBu2D0h_D02KShh = CombineParticles("StripKSLLForBu2D0h_D02KShh")
        KSLLForBu2D0h_D02KShh.InputLocations = ["StripLLPionFilterForBu2D0h_D02KShh"]
        KSLLForBu2D0h_D02KShh.DecayDescriptor = "KS0 -> pi+ pi-"
        KSLLForBu2D0h_D02KShh.DaughtersCuts = {"pi+": "ALL"}
        KSLLForBu2D0h_D02KShh.CombinationCut = KSLL_CombCut
        KSLLForBu2D0h_D02KShh.MotherCut = KSLL_MotherCut
        return KSLLForBu2D0h_D02KShh
        
        #######################################
        # KS, downstream downstream daughters #
        #######################################
    def KSDDForBu2D0h_D02KShhAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        KSDD_CombCut = "(AMINDOCA('LoKi::TrgDistanceCalculator')<24.) & (ADAMASS('KS0') < 50.*MeV)"
        KSDD_MotherCut = "(ADMASS('KS0') < 42.*MeV) & (VFASPF(VCHI2/VDOF) < %(KSVertexCHI2_DD)s) & (BPVVDCHI2 > %(KSFlightCHI2_DD)s)"% self.getProps()
        
        KSDDForBu2D0h_D02KShh = CombineParticles("StripKSDDForBu2D0h_D02KShh")
        KSDDForBu2D0h_D02KShh.InputLocations = ["StripDDPionFilterForBu2D0h_D02KShh"]
        KSDDForBu2D0h_D02KShh.DecayDescriptor = "KS0 -> pi+ pi-" 
        KSDDForBu2D0h_D02KShh.DaughtersCuts = {"pi+" : "ALL"}
        KSDDForBu2D0h_D02KShh.CombinationCut = KSDD_CombCut
        KSDDForBu2D0h_D02KShh.MotherCut = KSDD_MotherCut
        return KSDDForBu2D0h_D02KShh

        ###########################
        # D0, with KS LL daughter #
        ###########################
    def DForBu2D0h_D02KSPiPi_KSLLAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        D_LL_DaughterCut = "(BPVIPCHI2() > %(DdaughterIPCHI2_LL)s) & (P < 100.*GeV)"% self.getProps()
        D_LL_CombCut = "(AMAXDOCA('LoKi::TrgDistanceCalculator') < 1.2) & (AM > 1020.*MeV) & (AM < 1940.*MeV)"
        D_LL_MotherCut = "(M > 1060.*MeV) & (M < 1897.3*MeV) & (VFASPF(VCHI2/VDOF) < %(DVertexCHI2_LL)s)"% self.getProps()

        DForBu2D0h_D02KSPiPi_KSLL = CombineParticles("StripDForBu2D0h_D02KSPiPi_KSLL")
        DForBu2D0h_D02KSPiPi_KSLL.DecayDescriptor = "[D0 -> KS0 pi+ pi-]cc"
        DForBu2D0h_D02KSPiPi_KSLL.InputLocations =  ["StdNoPIDsPions",
                                                     "StripKSLLForBu2D0h_D02KShh"
                                                     ]

        DForBu2D0h_D02KSPiPi_KSLL.DaughtersCuts  = {"pi+" : D_LL_DaughterCut
                                                   ,"KS0" : "ALL"
                                                   }
        DForBu2D0h_D02KSPiPi_KSLL.CombinationCut =  D_LL_CombCut
        DForBu2D0h_D02KSPiPi_KSLL.MotherCut      =  D_LL_MotherCut
        return DForBu2D0h_D02KSPiPi_KSLL




        ###########################
        # D0, with KS DD daughter #
        ###########################
    def DForBu2D0h_D02KSPiPi_KSDDAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        D_DD_DaughterCut = "(BPVIPCHI2() > %(DdaughterIPCHI2_DD)s) & (P < 100.*GeV)"% self.getProps()
        D_DD_CombCut = "(AMAXDOCA('LoKi::TrgDistanceCalculator') < 7.2) & (AM > 1020.*MeV) & (AM < 1950.*MeV)"
        D_DD_MotherCut = "(M > 1060.*MeV) & (M < 1904.*MeV) & (VFASPF(VCHI2/VDOF) < %(DVertexCHI2_DD)s)"% self.getProps()
        
        DForBu2D0h_D02KSPiPi_KSDD = CombineParticles("StripDForBu2D0h_D02KSPiPi_KSDD")
        DForBu2D0h_D02KSPiPi_KSDD.DecayDescriptor = "[D0 -> KS0 pi+ pi-]cc"
        DForBu2D0h_D02KSPiPi_KSDD.InputLocations =  ["StdNoPIDsPions",
                                                    "StripKSDDForBu2D0h_D02KShh"
                                                    ]

        DForBu2D0h_D02KSPiPi_KSDD.DaughtersCuts  = {"pi+" : D_DD_DaughterCut 
                                                   ,"KS0" : "ALL"
                                                   }
        DForBu2D0h_D02KSPiPi_KSDD.CombinationCut =  D_DD_CombCut
        DForBu2D0h_D02KSPiPi_KSDD.MotherCut      =  D_DD_MotherCut
        return DForBu2D0h_D02KSPiPi_KSDD

        ################
        # B, LL KS     #
        ################
    def Bu2D0h_KSPiPi_KSLLAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    

        B_LL_BachCut = "(BPVIPCHI2() > %(BachIPCHI2_LL)s ) & (PT > %(BachPt_LL)s *GeV ) &  (P < 100.*GeV)"% self.getProps()
        B_LL_CombCut = "(ADAMASS('B+') < 600.*MeV)"
        B_LL_MotherCut = "(ADMASS('B+') < 500.*MeV)  & (VFASPF(VCHI2/VDOF)<%(BVertexCHI2_LL)s) & (BPVIPCHI2() < %(BIPCHI2_LL)s)  & (BPVDIRA > %(BDIRA_LL)s) & (BPVVDCHI2 > %(BFlightCHI2_LL)s)" % self.getProps()

        Bu2D0h_KSLL = CombineParticles("StripBu2D0h_KSLL")
        Bu2D0h_KSLL.DecayDescriptor = "[B+ -> D~0 K+]cc"
        Bu2D0h_KSLL.InputLocations = ["StripDForBu2D0h_D02KSPiPi_KSLL",
                                      "StdNoPIDsKaons"]

        Bu2D0h_KSLL.DaughtersCuts = { "D0" : "ALL",
                                      "K+" : B_LL_BachCut
                                    }
        Bu2D0h_KSLL.CombinationCut =  B_LL_CombCut
        Bu2D0h_KSLL.MotherCut = B_LL_MotherCut 
        return Bu2D0h_KSLL


        ################
        # B, DD KS     #
        ################
    def Bu2D0h_KSPiPi_KSDDAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        B_DD_BachCut = "(BPVIPCHI2() > %(BachIPCHI2_DD)s) & (PT > %(BachPt_DD)s *GeV) &  (P < 100.*GeV)"% self.getProps()
        B_DD_CombCut = "(ADAMASS('B+') < 600.*MeV)"
        B_DD_MotherCut = "(ADMASS('B+') < 500.*MeV)  & (VFASPF(VCHI2/VDOF) < %(BVertexCHI2_DD)s) & (BPVIPCHI2() < %(BIPCHI2_DD)s) & (BPVDIRA > %(BDIRA_DD)s) & (BPVVDCHI2 > %(BFlightCHI2_DD)s)"% self.getProps()


        Bu2D0h_KSDD = CombineParticles("StripBu2D0h_KSDD")
        Bu2D0h_KSDD.DecayDescriptor = "[B+ -> D~0 K+]cc"
        Bu2D0h_KSDD.InputLocations = ["StripDForBu2D0h_D02KSPiPi_KSDD",
                                     "StdNoPIDsKaons"]

        Bu2D0h_KSDD.DaughtersCuts = { "D0" : "ALL",
                                      "K+" : B_DD_BachCut
                                      }
        Bu2D0h_KSDD.CombinationCut = B_DD_CombCut
        Bu2D0h_KSDD.MotherCut = B_DD_MotherCut
        return Bu2D0h_KSDD



    def getProps(self) :
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
