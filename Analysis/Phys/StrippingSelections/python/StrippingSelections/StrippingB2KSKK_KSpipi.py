
__author__ = 'Jussara Miranda'
__date__ = '16/03/2010'
__version__ = '$Revision: 1.1 $'

'''
Stripping selection for B->KShh , B==(Bs,Bd); h=(K,pi) 
'''
#################################################################
#  This strip is GOOD for both Bd and Bs to KSLL of KSDD and hh(h=k,pi)
#  Asymmetrical mass window, Bd: (-200,+280 MeV)
#  There are 4 lines KK and pipi for  KSLL and the KSDD; 
#################################################################

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingB2KSKK_KSpipiConf(LHCbConfigurableUser):
    __slots__ = { 
           'B_Mlow'      :  200. 
        ,  'B_Mhigh'     :  280.   
        ,  'B_dira'      :  0.999   
        ,  'B_FDpvsv'    :  0.8   
        ,  'B_Vchi2'     :  15.   
        ,  'B_IPPTmax'   :  0.05   
        ,  'B_pointpt'   :  0.2   
        ,  'B_IPsumhh'   :  0.   
        ,  'KS_Mwindow'  :  30.   
        ,  'B_IP_LL'     :  0.08   
        ,  'B_PTmed_LL'  :  500.   
        ,  'B_PTsum_LL'  :  3200.   
        ,  'B_FDchi2_LL' :  20.   
        ,  'B_IP_DD'     :  0.1   
        ,  'KS_FDopv_LL' :  5.    
        ,  'B_PTmed_DD'  :  800.   
        ,  'B_PTsum_DD'  :  3800.
        ,  'B_FDchi2_DD' :  10.   
        ,  'KS_FDopv_DD' :  80.   
        }

    _propertyDocDct = {
           'B_Mlow'      : """ KSpipi low mass window """
        ,  'B_Mhigh'     : """ KSpipi high mass window """    
        ,  'B_dira'      : """ LL B cos angle    """    
        ,  'B_FDpvsv'    : """ B flight distance to the closest PV """    
        ,  'B_Vchi2'     : """ B Vertex chi2 """    
        ,  'B_IPPTamax'  : """ IP of the B daughter with the higest PT """    
        ,  'B_pointpt'   : """ B BPVTRGPOINTINGWPT """
        ,  'B_IPsumhh'   : """ sum of hh IP's """   
        ,  'KS_Mwindow'  : """ KS mass window """    
        ,  'B_IP_LL'     : """ B IP- for candidates with KSLL """    
        ,  'B_PTmed_LL'  : """ B daughter with medimum PT - for candidates with KSLL"""    
        ,  'B_PTsum_LL'  : """ B daughter PT sum - for candidates  with KSLL"""
        ,  'B_FDchi2_LL' : """ B flight distance - for candidates  with KSLL"""
        ,  'KS_FDopv_LL' : """ KS flight distance wrt PV - for candidates  with KSLL"""
        ,  'B_IP_DD'     : """ B IP- for candidates with KSDD """    
        ,  'B_PTmed_DD'  : """ B daughter with medimum PT - for candidates with KSDD"""  
        ,  'B_PTsum_DD'  : """ B daughter PT sum - for candidates  with KSDD"""    
        ,  'B_FDchi2_DD' : """ B flight distance - for candidates  with KSDD"""    
        ,  'KS_FDopv_DD' : """ KS flight distance wrt PV - for candidates  with KSDD"""    

        }

    ###############################################
    # Create StrippingLines with these selections #
    ###############################################
    def B2KSLLKK( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        KSLLforB2KShh = self.KSLLforB2KShhAlg()
        B2KSLLKK = self.B2KSLLKKAlg()
        return StrippingLine('B2KSLLKK_line'
                             , prescale = 1
                             , algos = [KSLLforB2KShh,B2KSLLKK]
                            , postscale = 1
                             )
    def B2KSDDKK( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        KSDDforB2KShh = self.KSDDforB2KShhAlg()
        B2KSDDKK = self.B2KSDDKKAlg()
        return StrippingLine('B2KSDDKK_line'
                             , prescale = 1
                             , algos = [KSDDforB2KShh,B2KSDDKK]
                             , postscale = 1
                             )

    def B2KSLLpipi( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        KSLLforB2KShh = self.KSLLforB2KShhAlg()
        B2KSLLpipi = self.B2KSLLpipiAlg()
        return StrippingLine('B2KSLLpipi_line'
                             , prescale = 1
                             , algos = [KSLLforB2KShh,B2KSLLpipi]
                            , postscale = 1
                             )
    def B2KSDDpipi( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        KSDDforB2KShh = self.KSDDforB2KShhAlg()
        B2KSDDpipi = self.B2KSDDpipiAlg()
        return StrippingLine('B2KSDDpipi_line'
                             , prescale = 1
                             , algos = [KSDDforB2KShh,B2KSDDpipi]
                             , postscale = 1
                             )
    ########
    # KSLL # 
    ########
    def KSLLforB2KShhAlg(self):
        from Configurables import FilterDesktop
        import GaudiKernel.SystemOfUnits as Units
        KSLL_FilterCuts="(BPVVD > %(KS_FDopv_LL)s) & (ADMASS('KS0')<%(KS_Mwindow)s *MeV)"% self.getProps()

	KSLLforB2KShh=FilterDesktop("StripKSLLforB2KShh")
	KSLLforB2KShh.InputLocations = ["StdLooseKsLL"]
	KSLLforB2KShh.Code=KSLL_FilterCuts
	
        return KSLLforB2KShh
    ########
    # KSDD # 
    ########
    def KSDDforB2KShhAlg(self):
        from Configurables import FilterDesktop
        import GaudiKernel.SystemOfUnits as Units
        KSDD_FilterCuts="(BPVVD > %(KS_FDopv_DD)s) & (ADMASS('KS0')<%(KS_Mwindow)s *MeV)"% self.getProps()

	KSDDforB2KShh=FilterDesktop("StripKSDDforB2KShh")
	KSDDforB2KShh.InputLocations = ["StdLooseKsDD"]
	KSDDforB2KShh.Code=KSDD_FilterCuts
	
        return KSDDforB2KShh
    #############
    # B-KSLL KK # 
    #############
    def B2KSLLKKAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    
        
        BLL_CombCuts= "(AM>(5279-%(B_Mlow)s) *MeV)\
	               &(AM<(5279+%(B_Mhigh)s) *MeV)\
	               &(AVAL_MAX(MIPDV(PRIMARY), PT)>%(B_IPPTmax)s) \
	               &(ANUM(PT > %(B_PTmed_LL)s *MeV)>=2) \
	               &((APT1+APT2+APT3)>%(B_PTsum_LL)s *MeV) \
		       &((ACHILD(MIPCHI2DV(PRIMARY),1)+ACHILD(MIPCHI2DV(PRIMARY),2))>%(B_IPsumhh)s)"% self.getProps()

        BLL_MotherCuts="(BPVDIRA>%(B_dira)s)\
		       &(VFASPF(VMINVDDV(PRIMARY))>%(B_FDpvsv)s)\
		       &(VFASPF(VCHI2)<%(B_Vchi2)s)\
		       &(MIPDV(PRIMARY)<%(B_IP_LL)s)\
		       &(BPVTRGPOINTINGWPT<%(B_pointpt)s)\
		       &(BPVVDCHI2>%(B_FDchi2_LL)s)"% self.getProps()


	B2KSLLKK=CombineParticles("StripB2KSLLKK")
	B2KSLLKK.InputLocations = ["StdLooseKaons", "StripKSLLforB2KShh"]
	B2KSLLKK.DecayDescriptor = "B0 -> K+ K- KS0"
	B2KSLLKK.DaughtersCuts = {"K+": "TRCHI2DOF<20"}
	B2KSLLKK.CombinationCut = BLL_CombCuts
	B2KSLLKK.MotherCut = BLL_MotherCuts
        return B2KSLLKK
    ##############
    #  B-KSDD KK #  
    ##############
    def B2KSDDKKAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    
        
        BDD_CombCuts="(AM>(5279-%(B_Mlow)s) *MeV)\
	              &(AM<(5279+%(B_Mhigh)s) *MeV)\
	              &(AVAL_MAX(MIPDV(PRIMARY), PT)>%(B_IPPTmax)s)\
	              &(ANUM(PT > %(B_PTmed_DD)s *MeV)>=2)\
		      &((APT1+APT2+APT3)>%(B_PTsum_DD)s *MeV)\
		      &((ACHILD(MIPCHI2DV(PRIMARY),1)+ACHILD(MIPCHI2DV(PRIMARY),2))> %(B_IPsumhh)s)"% self.getProps()

        BDD_MotherCuts="(BPVDIRA>%(B_dira)s)\
		       &(VFASPF(VMINVDDV(PRIMARY))>%(B_FDpvsv)s)\
		       &(VFASPF(VCHI2)<%(B_Vchi2)s)\
		       &(MIPDV(PRIMARY)<%(B_IP_DD)s)\
		       &(BPVTRGPOINTINGWPT<%(B_pointpt)s)\
		       &(BPVVDCHI2>%(B_FDchi2_DD)s)"% self.getProps()

	B2KSDDKK=CombineParticles("StripB2KSDDKK")
	B2KSDDKK.InputLocations = ["StdLooseKaons", "StripKSDDforB2KShh"]
	B2KSDDKK.DecayDescriptor = "B0 -> K+ K- KS0"
	B2KSDDKK.DaughtersCuts = {"K+": "TRCHI2DOF<20"}
	B2KSDDKK.CombinationCut = BDD_CombCuts
	B2KSDDKK.MotherCut = BDD_MotherCuts
        return B2KSDDKK

    #############
    # B-KSLL pipi # 
    #############
    def B2KSLLpipiAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    
        
        BLL_CombCuts="(AM>(5279-%(B_Mlow)s) *MeV)\
	              &(AM<(5279+%(B_Mhigh)s) *MeV)\
	              &(AVAL_MAX(MIPDV(PRIMARY), PT)>%(B_IPPTmax)s) \
	              &(ANUM(PT > %(B_PTmed_LL)s *MeV)>=2) \
	              &((APT1+APT2+APT3)>%(B_PTsum_LL)s *MeV) \
		      &((ACHILD(MIPCHI2DV(PRIMARY),1)+ACHILD(MIPCHI2DV(PRIMARY),2))>%(B_IPsumhh)s)"% self.getProps()

        BLL_MotherCuts="(BPVDIRA>%(B_dira)s)\
		       &(VFASPF(VMINVDDV(PRIMARY))>%(B_FDpvsv)s)\
		       &(VFASPF(VCHI2)<%(B_Vchi2)s)\
		       &(MIPDV(PRIMARY)<%(B_IP_LL)s)\
		       &(BPVTRGPOINTINGWPT<%(B_pointpt)s)\
		       &(BPVVDCHI2>%(B_FDchi2_LL)s)"% self.getProps()


	B2KSLLpipi=CombineParticles("StripB2KSLLpipi")
	B2KSLLpipi.InputLocations = ["StdLoosePions", "StripKSLLforB2KShh"]
	B2KSLLpipi.DecayDescriptor = "B0 -> pi+ pi- KS0"
	B2KSLLpipi.DaughtersCuts = {"pi+": "TRCHI2DOF<10"}
	B2KSLLpipi.CombinationCut = BLL_CombCuts
	B2KSLLpipi.MotherCut = BLL_MotherCuts
        return B2KSLLpipi
    ##############
    #  B-KSDD pipi #  
    ##############
    def B2KSDDpipiAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    
        
        BDD_CombCuts="(AM>(5279-%(B_Mlow)s) *MeV)\
	              &(AM<(5279+%(B_Mhigh)s) *MeV)\
	              &(AVAL_MAX(MIPDV(PRIMARY), PT)>%(B_IPPTmax)s)\
	              &(ANUM(PT > %(B_PTmed_DD)s *MeV)>=2)\
		      &((APT1+APT2+APT3)>%(B_PTsum_DD)s *MeV)\
		      &((ACHILD(MIPCHI2DV(PRIMARY),1)+ACHILD(MIPCHI2DV(PRIMARY),2))> %(B_IPsumhh)s)"% self.getProps()

        BDD_MotherCuts="(BPVDIRA>%(B_dira)s)\
		       &(VFASPF(VMINVDDV(PRIMARY))>%(B_FDpvsv)s)\
		       &(VFASPF(VCHI2)<%(B_Vchi2)s)\
		       &(MIPDV(PRIMARY)<%(B_IP_DD)s)\
		       &(BPVTRGPOINTINGWPT<%(B_pointpt)s)\
		       &(BPVVDCHI2>%(B_FDchi2_DD)s)"% self.getProps()

	B2KSDDpipi=CombineParticles("StripB2KSDDpipi")
	B2KSDDpipi.InputLocations = ["StdLoosePions", "StripKSDDforB2KShh"]
	B2KSDDpipi.DecayDescriptor = "B0 -> pi+ pi- KS0"
	B2KSDDpipi.DaughtersCuts = {"pi+": "TRCHI2DOF<10"}
	B2KSDDpipi.CombinationCut = BDD_CombCuts
	B2KSDDpipi.MotherCut = BDD_MotherCuts
        return B2KSDDpipi

    def getProps(self) :
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
