

__author__ = 'Susan Haines'
__date__ = '22/01/2010'
__version__ = '$Revision: 1.1 $'

'''
Bu->D0(KShh)h inclusive stripping selection using LoKi::Hybrid and python
configurables for new MC09 offline selection. No PID or error info used.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingBu2D0h_D02KShh_NoPIDNoErrorsConf(LHCbConfigurableUser):
    __slots__ = { 
        'BFlightDist_LL'        : 1.4  
        ,  'BDIRA_LL'           : 0.99985     
        ,  'BIP_LL'             : 0.06         
        ,  'BVertexCHI2_LL'     : 6.25
        ,  'BachIP_LL'          : 0.09
        ,  'BachPt_LL'          : 0.9  
        ,  'DVertexCHI2_LL'     : 4.0     
        ,  'DdaughterIP_LL'     : 0.08
        ,  'KSFlightDist_LL'    : 17.5
        ,  'KSVertexCHI2_LL'    : 16.
        ,  'KSdaughterPiIP_LL'  : 0.14
        ,  'BFlightDist_DD'     : 1.4
        ,  'BDIRA_DD'           : 0.99985    
        ,  'BIP_DD'             : 0.06         
        ,  'BVertexCHI2_DD'     : 9.
        ,  'BachIP_DD'          : 0.08
        ,  'BachPt_DD'          : 0.4  
        ,  'DVertexCHI2_DD'     : 16.        
        ,  'DdaughterIP_DD'     : 0.08
        ,  'KSFlightDist_DD'    : 175.
        ,  'KSVertexCHI2_DD'    : 16.
        ,  'KSdaughterPiIP_DD'  : 1.4
        }

    _propertyDocDct = {
        'BFlightDist_LL'      : """ LL B flight distance cut """   
        , 'BDIRA_LL'          : """ LL B cos angle cut         """
        ,  'BIP_LL'           : """ LL B IP cut         """       
        ,  'BVertexCHI2_LL'   : """ LL B vertex chi2 cut         """
        ,  'BachIP_LL'        : """ LL Bachelor K/pi IP cut         """
        ,  'BachPt_LL'        : """ LL Bachelor K/pi Pt cut         """
        ,  'DVertexCHI2_LL'   : """ LL D0 vertex chi2 cut         """
        ,  'DdaughterIP_LL'   : """ LL D daughter K/pi IP  cut         """
        ,  'KSFlightDist_LL'  : """ LL KS flight distance cut     """
        ,  'KSVertexCHI2_LL'  : """ LL KS vertex chi2 cut     """
        ,  'KSdaughterPiIP_LL' : """ LL KS daughter pion IP cut         """
        ,  'BFlightDist_DD'   : """ DD B flight distance cut """   
        ,  'BDIRA_DD'         : """ DD B cos angle cut         """
        ,  'BIP_DD'           : """ DD B IP cut         """       
        ,  'BVertexCHI2_DD'   : """ DD B vertex chi2 cut         """
        ,  'BachIP_DD'        : """ DD Bachelor K/pi IP cut         """
        ,  'BachPt_DD'        : """ DD Bachelor K/pi Pt cut         """
        ,  'DVertexCHI2_DD'   : """ DD D0 vertex chi2 cut         """
        ,  'DdaughterIP_DD'   : """ DD D daughter K/pi IP cut         """
        ,  'KSFlightDist_DD'  : """ DD KS flight distance cut     """
        ,  'KSVertexCHI2_DD'  : """ DD KS vertex chi2 cut     """
        ,  'KSdaughterPiIP_DD' : """ DD KS daughter pion IP cut         """


        }

        ###############################################
        # Create StrippingLines with these selections #
        ###############################################
    def Bu2D0h_KSLL( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        LLPionFilterForBu2D0h_D02KShh = self.LLPionFilterForBu2D0h_D02KShhAlg()
        KSLLForBu2D0h_D02KShh = self.KSLLForBu2D0h_D02KShhAlg()
        DForBu2D0h_D02KShh_KSLL = self.DForBu2D0h_D02KShh_KSLLAlg()
        Bu2D0h_KSLL = self.Bu2D0h_KSLLAlg()
        return StrippingLine('Bu2D0h_D02KShh_KSLL'
                             , prescale = 1
                             , algos = [LLPionFilterForBu2D0h_D02KShh, KSLLForBu2D0h_D02KShh, DForBu2D0h_D02KShh_KSLL, Bu2D0h_KSLL]
                             , postscale = 1
                             )
    def Bu2D0h_KSDD( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        DDPionFilterForBu2D0h_D02KShh = self.DDPionFilterForBu2D0h_D02KShhAlg()
        KSDDForBu2D0h_D02KShh = self.KSDDForBu2D0h_D02KShhAlg()
        DForBu2D0h_D02KShh_KSDD = self.DForBu2D0h_D02KShh_KSDDAlg()
        Bu2D0h_KSDD = self.Bu2D0h_KSDDAlg()
        return StrippingLine('Bu2D0h_D02KShh_KSDD'
                             , prescale = 1
                             , algos = [DDPionFilterForBu2D0h_D02KShh, KSDDForBu2D0h_D02KShh, DForBu2D0h_D02KShh_KSDD, Bu2D0h_KSDD]
                             , postscale = 1
                             )



        ###########################
        # KS daughters, long long # 
        ###########################
    def LLPionFilterForBu2D0h_D02KShhAlg(self):
        from Configurables import FilterDesktop
        import GaudiKernel.SystemOfUnits as Units

        LLPi_FilterCut = "(MIPDV(PRIMARY) > %(KSdaughterPiIP_LL)s) & (P < 100.*GeV)"% self.getProps() 

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

        DDPi_FilterCut = "(ISDOWN) & (P < 100.*GeV) & (MIPDV(PRIMARY) > %(KSdaughterPiIP_DD)s)"% self.getProps()

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

        KSLL_CombCut = "(ADAMASS('KS0') < 30.*MeV)"
        KSLL_MotherCut = "(ADMASS('KS0') < 20.*MeV) & (VFASPF(VCHI2/VDOF) < %(KSVertexCHI2_LL)s) & (BPVVD > %(KSFlightDist_LL)s)"% self.getProps()
        
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

        KSDD_CombCut = "(ADAMASS('KS0') < 40.*MeV)"
        KSDD_MotherCut = "(ADMASS('KS0') < 28.*MeV) & (VFASPF(VCHI2/VDOF) < %(KSVertexCHI2_DD)s) & (BPVVD > %(KSFlightDist_DD)s)"% self.getProps()

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
    def DForBu2D0h_D02KShh_KSLLAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        D_LL_DaughterCut ="(MIPDV(PRIMARY) >%(DdaughterIP_LL)s) & (P < 100.*GeV)" % self.getProps()
        D_LL_CombCut = "(AM > 1000.*MeV) & (AM < 2000.*MeV)"
        D_LL_MotherCut = "(M > 1060.*MeV) & (M < 1897.3*MeV) & (VFASPF(VCHI2/VDOF) < %(DVertexCHI2_LL)s)"% self.getProps()
        
        DForBu2D0h_D02KShh_KSLL = CombineParticles("StripDForBu2D0h_D02KShh_KSLL")
        DForBu2D0h_D02KShh_KSLL.DecayDescriptor = "[D0 -> KS0 pi+ pi-]cc"

        DForBu2D0h_D02KShh_KSLL.InputLocations =  ["StdNoPIDsPions",
                                                     "StripKSLLForBu2D0h_D02KShh"
                                                     ]

        DForBu2D0h_D02KShh_KSLL.DaughtersCuts  = {"pi+" : D_LL_DaughterCut
                                                   ,"KS0" : "ALL"
                                                   }
        DForBu2D0h_D02KShh_KSLL.CombinationCut =  D_LL_CombCut
        DForBu2D0h_D02KShh_KSLL.MotherCut      =  D_LL_MotherCut
        return DForBu2D0h_D02KShh_KSLL


        ###########################
        # D0, with KS DD daughter #
        ###########################
    def DForBu2D0h_D02KShh_KSDDAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        D_DD_DaughterCut = "(P < 100.*GeV) & (MIPDV(PRIMARY) > %(DdaughterIP_DD)s)"% self.getProps() 
        D_DD_CombCut = "(AM > 1000.*MeV) & (AM < 2000.*MeV)"
        D_DD_MotherCut = "(M > 1060.*MeV) & (M < 1904.*MeV) & (VFASPF(VCHI2/VDOF) < %(DVertexCHI2_DD)s)"% self.getProps()
        
        DForBu2D0h_D02KShh_KSDD = CombineParticles("StripDForBu2D0h_D02KShh_KSDD")
        DForBu2D0h_D02KShh_KSDD.DecayDescriptor = "[D0 -> KS0 pi+ pi-]cc"

        DForBu2D0h_D02KShh_KSDD.InputLocations =  ["StdNoPIDsPions",
                                                    "StripKSDDForBu2D0h_D02KShh"
                                                    ]

        DForBu2D0h_D02KShh_KSDD.DaughtersCuts  = {"pi+" : D_DD_DaughterCut
                                                   ,"KS0" : "ALL"
                                                   }
        DForBu2D0h_D02KShh_KSDD.CombinationCut =  D_DD_CombCut
        DForBu2D0h_D02KShh_KSDD.MotherCut      =  D_DD_MotherCut
        return DForBu2D0h_D02KShh_KSDD

        ################
        # B, LL KS     #
        ################
    def Bu2D0h_KSLLAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    

        B_LL_BachCut = "(PT > %(BachPt_LL)s *GeV) & (MIPDV(PRIMARY) > %(BachIP_LL)s) & (P < 100.*GeV)"% self.getProps()
        B_LL_CombCut = "(ADAMASS('B+') < 600.*MeV)"
        B_LL_MotherCut = "(ADMASS('B+') < 500.*MeV)  & (VFASPF(VCHI2/VDOF) < %(BVertexCHI2_LL)s) & (MIPDV(PRIMARY) < %(BIP_LL)s)  & (BPVDIRA > %(BDIRA_LL)s) & (BPVVD > %(BFlightDist_LL)s)"% self.getProps()
        
        Bu2D0h_KSLL = CombineParticles("StripBu2D0h_KSLL")
        Bu2D0h_KSLL.DecayDescriptor = "[B+ -> D~0 K+]cc"
        Bu2D0h_KSLL.InputLocations = ["StripDForBu2D0h_D02KShh_KSLL",
                                      "StdNoPIDsKaons"]


        Bu2D0h_KSLL.DaughtersCuts = { "D0" : "ALL",
                                      "K+" : B_LL_BachCut
                                    }
        Bu2D0h_KSLL.CombinationCut = B_LL_CombCut
        Bu2D0h_KSLL.MotherCut = B_LL_MotherCut
        return Bu2D0h_KSLL

        ################
        # B, DD KS     #
        ################
    def Bu2D0h_KSDDAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    

        B_DD_BachCut = "(PT > %(BachPt_DD)s*GeV) & (MIPDV(PRIMARY) > %(BachIP_DD)s) & (P < 100.*GeV)"% self.getProps()
        B_DD_CombCut = "(ADAMASS('B+') < 600.*MeV)"
        B_DD_MotherCut = "(ADMASS('B+') < 500.*MeV)  & (VFASPF(VCHI2/VDOF) < %(BVertexCHI2_DD)s) & (MIPDV(PRIMARY) < %(BIP_DD)s) & (BPVDIRA > %(BDIRA_DD)s) & (BPVVD > %(BFlightDist_DD)s)"% self.getProps()
        
        Bu2D0h_KSDD = CombineParticles("StripBu2D0h_KSDD")
        Bu2D0h_KSDD.DecayDescriptor = "[B+ -> D~0 K+]cc"
        Bu2D0h_KSDD.InputLocations = ["StripDForBu2D0h_D02KShh_KSDD",
                                     "StdNoPIDsKaons"]


        Bu2D0h_KSDD.DaughtersCuts = { "D0" : "ALL",
                                      "K+" : B_DD_BachCut
                                   }
        Bu2D0h_KSDD.CombinationCut =  B_DD_CombCut
        Bu2D0h_KSDD.MotherCut = B_DD_MotherCut
        return Bu2D0h_KSDD

    
    def getProps(self) :
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d


