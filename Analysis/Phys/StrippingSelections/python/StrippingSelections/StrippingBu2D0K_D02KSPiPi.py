

__author__ = 'Susan Haines'
__date__ = '22/01/2010'
__version__ = '$Revision: 1.2 $'

'''
Bu->D0(KSPiPi)K stripping selection using LoKi::Hybrid and python
configurables for new MC09 offline selection.
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingBu2D0K_D02KSPiPiConf(LHCbConfigurableUser):
    __slots__ = { 
        'BFlightCHI2_LL'        : 144.  
        ,  'BDIRA_LL'           : 0.99985     
        ,  'BIPCHI2_LL'         : 16.         
        ,  'BVertexCHI2_LL'     : 9.
        ,  'BachKIPCHI2_LL'     : 9.
        ,  'BachKPt_LL'         : 0.9  
        ,  'DVertexCHI2_LL'     : 6.25     
        ,  'DdaughterPiIPCHI2_LL' : 9.
        ,  'KSFlightCHI2_LL'    : 4.
        ,  'KSVertexCHI2_LL'    : 20.25
        ,  'KSdaughterPiIPCHI2_LL' : 4.
        ,  'BFlightCHI2_DD'     : 81.
        ,  'BDIRA_DD'           : 0.9998    
        ,  'BIPCHI2_DD'         : 25.         
        ,  'BVertexCHI2_DD'     : 12.25
        ,  'BachKIPCHI2_DD'     : 4.
        ,  'BachKPt_DD'         : 0.4  
        ,  'DVertexCHI2_DD'     : 16.        
        ,  'DdaughterPiIPCHI2_DD' : 4.
        ,  'KSFlightCHI2_DD'    : 4.
        ,  'KSVertexCHI2_DD'    : 20.25
        ,  'KSdaughterPiIPCHI2_DD' : 4.
        }

    _propertyDocDct = {
        'BFlightCHI2_LL'     : """ LL B flight distance chi2 cut """   
        , 'BDIRA_LL'            : """ LL B cos angle cut         """
        ,  'BIPCHI2_LL'         : """ LL B IP chi2 cut         """       
        ,  'BVertexCHI2_LL'     : """ LL B vertex chi2 cut         """
        ,  'BachKIPCHI2_LL'     : """ LL Bachelor kaon IP chi2 cut         """
        ,  'BachKPt_LL'         : """ LL Bachelor kaon Pt cut         """
        ,  'DVertexCHI2_LL'     : """ LL D0 vertex chi2 cut         """
        ,  'DdaughterPiIPCHI2_LL'     : """ LL D daughter pion IP chi2 cut         """
        ,  'KSFlightCHI2_LL'    : """ LL KS flight distance chi2 cut     """
        ,  'KSVertexCHI2_LL'    : """ LL KS vertex chi2 cut     """
        ,  'KSdaughterPiIPCHI2_LL'    : """ LL KS daughter pion IP chi2 cut         """
        ,  'BFlightCHI2_DD'     : """ DD B flight distance chi2 cut """   
        ,  'BDIRA_DD'           : """ DD B cos angle cut         """
        ,  'BIPCHI2_DD'         : """ DD B IP chi2 cut         """       
        ,  'BVertexCHI2_DD'     : """ DD B vertex chi2 cut         """
        ,  'BachKIPCHI2_DD'     : """ DD Bachelor kaon IP chi2 cut         """
        ,  'BachKPt_DD'         : """ DD Bachelor kaon Pt cut         """
        ,  'DVertexCHI2_DD'     : """ DD D0 vertex chi2 cut         """
        ,  'DdaughterPiIPCHI2_DD'     : """ DD D daughter pion IP chi2 cut         """
        ,  'KSFlightCHI2_DD'    : """ DD KS flight distance chi2 cut     """
        ,  'KSVertexCHI2_DD'    : """ DD KS vertex chi2 cut     """
        ,  'KSdaughterPiIPCHI2_DD'    : """ DD KS daughter pion IP chi2 cut         """


        }

    ###############################################
    # Create StrippingLines with these selections #
    ###############################################
    def Bu2D0K_KSLL( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        LLPionFilterForBu2D0K_D02KSPiPi = self.LLPionFilterForBu2D0K_D02KSPiPiAlg()
        KSLLForBu2D0K_D02KSPiPi = self.KSLLForBu2D0K_D02KSPiPiAlg()
        DForBu2D0K_D02KSPiPi_KSLL = self.DForBu2D0K_D02KSPiPi_KSLLAlg()
        Bu2D0K_KSLL = self.Bu2D0K_KSLLAlg()
        return StrippingLine('Bu2D0K_D02KSPiPi_KSLL'
                             , prescale = 1
                             , algos = [LLPionFilterForBu2D0K_D02KSPiPi, KSLLForBu2D0K_D02KSPiPi, DForBu2D0K_D02KSPiPi_KSLL, Bu2D0K_KSLL]
                             , postscale = 1
                             )
    
    def Bu2D0K_KSDD( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        DDPionFilterForBu2D0K_D02KSPiPi = self.DDPionFilterForBu2D0K_D02KSPiPiAlg()
        KSDDForBu2D0K_D02KSPiPi = self.KSDDForBu2D0K_D02KSPiPiAlg()
        DForBu2D0K_D02KSPiPi_KSDD = self.DForBu2D0K_D02KSPiPi_KSDDAlg()
        Bu2D0K_KSDD = self.Bu2D0K_KSDDAlg()
        return StrippingLine('Bu2D0K_D02KSPiPi_KSDD'
                             , prescale = 1
                             , algos = [DDPionFilterForBu2D0K_D02KSPiPi, KSDDForBu2D0K_D02KSPiPi, DForBu2D0K_D02KSPiPi_KSDD, Bu2D0K_KSDD]
                             , postscale = 1
                             )



        ###########################
        # KS daughters, long long # 
        ###########################
    def LLPionFilterForBu2D0K_D02KSPiPiAlg(self):
        from Configurables import FilterDesktop
        import GaudiKernel.SystemOfUnits as Units

        LLPi_FilterCut = "(BPVIPCHI2() > %(KSdaughterPiIPCHI2_LL)s) & (P < 100.*GeV)"% self.getProps() 
        
        LLPionFilterForBu2D0K_D02KSPiPi = FilterDesktop("StripLLPionFilterForBu2D0K_D02KSPiPi")
        LLPionFilterForBu2D0K_D02KSPiPi.InputLocations = ["StdLoosePions"]
        LLPionFilterForBu2D0K_D02KSPiPi.Code = LLPi_FilterCut
        return LLPionFilterForBu2D0K_D02KSPiPi
    
        #######################################
        # KS daughters, downstream downstream #
        #######################################
    def DDPionFilterForBu2D0K_D02KSPiPiAlg(self):
        from Configurables import FilterDesktop
        import GaudiKernel.SystemOfUnits as Units

        DDPi_FilterCut = "(ISDOWN) & (BPVIPCHI2() > %(KSdaughterPiIPCHI2_DD)s) & (P < 100.*GeV) & ((-PIDK) > 0.)"% self.getProps()
        
        DDPionFilterForBu2D0K_D02KSPiPi = FilterDesktop("StripDDPionFilterForBu2D0K_D02KSPiPi")
        DDPionFilterForBu2D0K_D02KSPiPi.InputLocations = ["StdNoPIDsDownPions"]
        DDPionFilterForBu2D0K_D02KSPiPi.Code = DDPi_FilterCut
        return DDPionFilterForBu2D0K_D02KSPiPi
        
        ###########################
        # KS, long long daughters #
        ###########################
    def KSLLForBu2D0K_D02KSPiPiAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        KSLL_CombCut = "(ADAMASS('KS0') < 40.*MeV)"
        KSLL_MotherCut = "(ADMASS('KS0') < 30.*MeV) & (VFASPF(VCHI2/VDOF) < %(KSVertexCHI2_LL)s) & (BPVVDCHI2 > %(KSFlightCHI2_LL)s)"% self.getProps()

        KSLLForBu2D0K_D02KSPiPi = CombineParticles("StripKSLLForBu2D0K_D02KSPiPi")
        KSLLForBu2D0K_D02KSPiPi.InputLocations = ["StripLLPionFilterForBu2D0K_D02KSPiPi"]
        KSLLForBu2D0K_D02KSPiPi.DecayDescriptor = "KS0 -> pi+ pi-"
        KSLLForBu2D0K_D02KSPiPi.DaughtersCuts = {"pi+": "ALL"}
        KSLLForBu2D0K_D02KSPiPi.CombinationCut = KSLL_CombCut
        KSLLForBu2D0K_D02KSPiPi.MotherCut = KSLL_MotherCut
        return KSLLForBu2D0K_D02KSPiPi
        
        #######################################
        # KS, downstream downstream daughters #
        #######################################
    def KSDDForBu2D0K_D02KSPiPiAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        KSDD_CombCut = "(ADAMASS('KS0') < 50.*MeV)"
        KSDD_MotherCut = "(ADMASS('KS0') < 42.*MeV) & (VFASPF(VCHI2/VDOF) < %(KSVertexCHI2_DD)s) & (BPVVDCHI2 > %(KSFlightCHI2_DD)s)"% self.getProps()
        
        KSDDForBu2D0K_D02KSPiPi = CombineParticles("StripKSDDForBu2D0K_D02KSPiPi")
        KSDDForBu2D0K_D02KSPiPi.InputLocations = ["StripDDPionFilterForBu2D0K_D02KSPiPi"]
        KSDDForBu2D0K_D02KSPiPi.DecayDescriptor = "KS0 -> pi+ pi-" 
        KSDDForBu2D0K_D02KSPiPi.DaughtersCuts = {"pi+" : "ALL"}
        KSDDForBu2D0K_D02KSPiPi.CombinationCut = KSDD_CombCut
        KSDDForBu2D0K_D02KSPiPi.MotherCut = KSDD_MotherCut
        return KSDDForBu2D0K_D02KSPiPi

        ###########################
        # D0, with KS LL daughter #
        ###########################
    def DForBu2D0K_D02KSPiPi_KSLLAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        D_LL_DaughterPiCut = "(BPVIPCHI2() > %(DdaughterPiIPCHI2_LL)s) & (P < 100.*GeV) & ((-PIDK) > -11.)"% self.getProps()
        D_LL_CombCut = "(ADAMASS('D0') < 60.*MeV)" 
        D_LL_MotherCut = "(ADMASS('D0') < 50.*MeV) & (VFASPF(VCHI2/VDOF) < %(DVertexCHI2_LL)s)"% self.getProps()
        
        DForBu2D0K_D02KSPiPi_KSLL = CombineParticles("StripDForBu2D0K_D02KSPiPi_KSLL")
        DForBu2D0K_D02KSPiPi_KSLL.DecayDescriptor = "[D0 -> KS0 pi+ pi-]cc"
        DForBu2D0K_D02KSPiPi_KSLL.InputLocations =  ["StdLoosePions",
                                                     "StripKSLLForBu2D0K_D02KSPiPi"
                                                     ]

        DForBu2D0K_D02KSPiPi_KSLL.DaughtersCuts  = {"pi+" : D_LL_DaughterPiCut
                                                   ,"KS0" : "ALL"
                                                   }
        DForBu2D0K_D02KSPiPi_KSLL.CombinationCut =  D_LL_CombCut
        DForBu2D0K_D02KSPiPi_KSLL.MotherCut      =  D_LL_MotherCut
        return DForBu2D0K_D02KSPiPi_KSLL




        ###########################
        # D0, with KS DD daughter #
        ###########################
    def DForBu2D0K_D02KSPiPi_KSDDAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        D_DD_DaughterPiCut = "(BPVIPCHI2() > %(DdaughterPiIPCHI2_DD)s) & (P < 100.*GeV) & ((-PIDK) > 0.)"% self.getProps()
        D_DD_CombCut = "(ADAMASS('D0') < 70.*MeV)" 
        D_DD_MotherCut = "(ADMASS('D0') < 60.*MeV) & (VFASPF(VCHI2/VDOF) < %(DVertexCHI2_DD)s)"% self.getProps()
        
        DForBu2D0K_D02KSPiPi_KSDD = CombineParticles("StripDForBu2D0K_D02KSPiPi_KSDD")
        DForBu2D0K_D02KSPiPi_KSDD.DecayDescriptor = "[D0 -> KS0 pi+ pi-]cc"
        DForBu2D0K_D02KSPiPi_KSDD.InputLocations =  ["StdLoosePions",
                                                    "StripKSDDForBu2D0K_D02KSPiPi"
                                                    ]

        DForBu2D0K_D02KSPiPi_KSDD.DaughtersCuts  = {"pi+" : D_DD_DaughterPiCut 
                                                   ,"KS0" : "ALL"
                                                   }
        DForBu2D0K_D02KSPiPi_KSDD.CombinationCut =  D_DD_CombCut
        DForBu2D0K_D02KSPiPi_KSDD.MotherCut      =  D_DD_MotherCut
        return DForBu2D0K_D02KSPiPi_KSDD

        ################
        # B, LL KS     #
        ################
    def Bu2D0K_KSLLAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    

        B_LL_BachKCut = "(PT > %(BachKPt_LL)s *GeV ) & (BPVIPCHI2() > %(BachKIPCHI2_LL)s ) & (P < 100.*GeV) & (PIDK > -4.) & ((PIDK - PIDp) > -11.)"% self.getProps()
        B_LL_CombCut = "(ADAMASS('B+') < 600.*MeV)"
        B_LL_MotherCut = "(ADMASS('B+') < 500.*MeV)  & (VFASPF(VCHI2/VDOF)<%(BVertexCHI2_LL)s) & (BPVIPCHI2() < %(BIPCHI2_LL)s)  & (BPVDIRA > %(BDIRA_LL)s) & (BPVVDCHI2 > %(BFlightCHI2_LL)s)" % self.getProps()

        Bu2D0K_KSLL = CombineParticles("StripBu2D0K_KSLL")
        Bu2D0K_KSLL.DecayDescriptor = "[B+ -> D~0 K+]cc"
        Bu2D0K_KSLL.InputLocations = ["StripDForBu2D0K_D02KSPiPi_KSLL",
                                      "StdLooseKaons"]

        Bu2D0K_KSLL.DaughtersCuts = { "D0" : "ALL",
                                      "K+" : B_LL_BachKCut
                                    }
        Bu2D0K_KSLL.CombinationCut =  B_LL_CombCut
        Bu2D0K_KSLL.MotherCut = B_LL_MotherCut 
        return Bu2D0K_KSLL


        ################
        # B, DD KS     #
        ################
    def Bu2D0K_KSDDAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        B_DD_BachKCut = "(PT > %(BachKPt_DD)s *GeV) & (BPVIPCHI2() > %(BachKIPCHI2_DD)s) & (P < 100.*GeV) & (PIDK > -2.5) & ((PIDK - PIDp) > -7.5)"% self.getProps()
        B_DD_CombCut = "(ADAMASS('B+') < 600.*MeV)"
        B_DD_MotherCut = "(ADMASS('B+') < 500.*MeV)  & (VFASPF(VCHI2/VDOF) < %(BVertexCHI2_DD)s) & (BPVIPCHI2() < %(BIPCHI2_DD)s) & (BPVDIRA > %(BDIRA_DD)s) & (BPVVDCHI2 > %(BFlightCHI2_DD)s)"% self.getProps()


        Bu2D0K_KSDD = CombineParticles("StripBu2D0K_KSDD")
        Bu2D0K_KSDD.DecayDescriptor = "[B+ -> D~0 K+]cc"
        Bu2D0K_KSDD.InputLocations = ["StripDForBu2D0K_D02KSPiPi_KSDD",
                                     "StdLooseKaons"]

        Bu2D0K_KSDD.DaughtersCuts = { "D0" : "ALL",
                                      "K+" : B_DD_BachKCut
                                      }
        Bu2D0K_KSDD.CombinationCut = B_DD_CombCut
        Bu2D0K_KSDD.MotherCut = B_DD_MotherCut
        return Bu2D0K_KSDD



    def getProps(self) :
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
