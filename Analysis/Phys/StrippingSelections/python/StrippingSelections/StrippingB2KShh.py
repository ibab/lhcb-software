
__author__ = 'Jussara Miranda'
__date__ = '02/04/2010'
__version__ = '$Revision: 1.2 $'

'''
Stripping selection for B->KShh' , B==(Bs,Bd); h,h'=(K+-,pi+-) 
'''
#################################################################
#  This strip is GOOD for both Bd and Bs to KSLL of KSDD and hh'(h,h'=k,pi)
#  Asymmetrical mass window, Bd: (-200,+280 MeV)
#  There are 2 lines KSLLhh and the KSDDhh (hh==K+-,pi+-) 
#################################################################

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingB2KShhConf(LHCbConfigurableUser):
    __slots__ = { 
           'B_Mlow'          :  200. 
        ,  'B_Mhigh'         :  280.   
        ,  'B_dira'          :  0.999   
        ,  'B_FDpvsv'        :  0.8   
        ,  'B_Vchi2'         :  15.   
        ,  'B_IPPTmax'       :  0.05   
        ,  'B_pointpt'       :  0.2   
        ,  'B_IPsumhh'       :  0.08 
	,  'TrackCHI2DOF'    :  10. 
        ,  'DOCAhh'          :  0.2   
        ,  'KS_Mwindow'      :  30.   
        ,  'B_IP_LL'         :  0.08   
        ,  'B_PTmed_LL'      :  500.   
        ,  'B_PTsum_LL'      :  3200.   
        ,  'B_FDchi2_LL'     :  20.   
        ,  'KS_FDCHI2opv_LL' :  40.    
        ,  'B_IP_DD'         :  0.1   
        ,  'B_PTmed_DD'      :  800.   
        ,  'B_PTsum_DD'      :  3800.
        ,  'B_FDchi2_DD'     :  10.   
        ,  'KS_FDCHI2opv_DD' :  20.   
        }

    _propertyDocDct = {
           'B_Mlow'          : """ KSpipi low mass window """
        ,  'B_Mhigh'         : """ KSpipi high mass window """    
        ,  'B_dira'          : """ LL B cos angle    """    
        ,  'B_FDpvsv'        : """ B flight distance to the closest PV """    
        ,  'B_Vchi2'         : """ B Vertex chi2 """    
        ,  'B_IPPTamax'      : """ IP of the B daughter with the higest PT """    
        ,  'B_pointpt'       : """ B BPVTRGPOINTINGWPT """
        ,  'B_IPsumhh'       : """ sum of hh IP's """   
	,  'TrackCHI2DOF'    : """ track chidof to all tracks""" 
        ,  'DOCAhh'          : """ DOCA of hh """   
        ,  'KS_Mwindow'      : """ KS mass window """    
        ,  'B_IP_LL'         : """ B IP- for candidates with KSLL """    
        ,  'B_PTmed_LL'      : """ B daughter with medimum PT - for candidates with KSLL"""    
        ,  'B_PTsum_LL'      : """ B daughter PT sum - for candidates  with KSLL"""
        ,  'B_FDchi2_LL'     : """ B flight distance - for candidates  with KSLL"""
        ,  'KS_FDCHI2opv_LL' : """ KS flight distance wrt CHI2 PV - for candidates  with KSLL"""
        ,  'B_IP_DD'         : """ B IP- for candidates with KSDD """    
        ,  'B_PTmed_DD'      : """ B daughter with medimum PT - for candidates with KSDD"""  
        ,  'B_PTsum_DD'      : """ B daughter PT sum - for candidates  with KSDD"""    
        ,  'B_FDchi2_DD'     : """ B flight distance - for candidates  with KSDD"""    
        ,  'KS_FDCHI2opv_DD' : """ KS flight distance CHI2 wrt PV - for candidates  with KSDD"""    

        }

    ###############################################
    # Create StrippingLines with these selections #
    ###############################################
    def B2KSLLhh( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        KSLLforB2KShh = self.KSLLforB2KShhAlg()
        B2KSLLhh = self.B2KSLLhhAlg()
        return StrippingLine('B2KSLLhh_line'
                             , prescale = 1
                             , algos = [KSLLforB2KShh,B2KSLLhh]
                            , postscale = 1
                             )
    def B2KSDDhh( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        KSDDforB2KShh = self.KSDDforB2KShhAlg()
        B2KSDDhh = self.B2KSDDhhAlg()
        return StrippingLine('B2KSDDhh_line'
                             , prescale = 1
                             , algos = [KSDDforB2KShh,B2KSDDhh]
                             , postscale = 1
                             )
    ########
    # KSLL # 
    ########
    def KSLLforB2KShhAlg(self):
        from Configurables import FilterDesktop
        import GaudiKernel.SystemOfUnits as Units
        KSLL_FilterCuts="(BPVVDCHI2 > %(KS_FDCHI2opv_LL)s)  \
	                 & (ADMASS('KS0')<%(KS_Mwindow)s *MeV ) \
			 & (CHILDCUT((TRCHI2DOF < %(TrackCHI2DOF)s),1)) \
			 & (CHILDCUT((TRCHI2DOF < %(TrackCHI2DOF)s),2)) "% self.getProps()

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
        KSDD_FilterCuts="(BPVVDCHI2 > %(KS_FDCHI2opv_DD)s) & (ADMASS('KS0')<%(KS_Mwindow)s *MeV)"% self.getProps()

	KSDDforB2KShh=FilterDesktop("StripKSDDforB2KShh")
	KSDDforB2KShh.InputLocations = ["StdLooseKsDD"]
	KSDDforB2KShh.Code=KSDD_FilterCuts
	
        return KSDDforB2KShh

    #############
    # B-KSLL hh # 
    #############
    def B2KSLLhhAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    
        
        BLL_CombCuts= " (AM>(5279-%(B_Mlow)s) *MeV)\
	               &(AM<(5279+%(B_Mhigh)s) *MeV)\
	               &(ANUM(PT > %(B_PTmed_LL)s *MeV)>=2) \
	               &((APT1+APT2+APT3)>%(B_PTsum_LL)s *MeV) \
	               &(AVAL_MAX(MIPDV(PRIMARY), PT)>%(B_IPPTmax)s) \
		       &(ADOCA(1,2)< %(DOCAhh)s)\
		       &((ACHILD(MIPCHI2DV(PRIMARY),1)+ACHILD(MIPCHI2DV(PRIMARY),2))>%(B_IPsumhh)s)"% self.getProps()

        BLL_MotherCuts="(VFASPF(VCHI2)<%(B_Vchi2)s)\
		       &(BPVTRGPOINTINGWPT<%(B_pointpt)s)\
		       &(BPVDIRA>%(B_dira)s)\
		       &(MIPDV(PRIMARY)<%(B_IP_LL)s)\
		       &(VFASPF(VMINVDDV(PRIMARY))>%(B_FDpvsv)s)\
		       &(BPVVDCHI2>%(B_FDchi2_LL)s)"% self.getProps()

	B2KSLLhh=CombineParticles("StripB2KSLLhh")
	B2KSLLhh.InputLocations = [ "StripKSLLforB2KShh","StdLooseKaons","StdLoosePions"]
	B2KSLLhh.DecayDescriptors = ["B0 ->  pi+ pi+ KS0 ", "B0 -> pi+ pi- KS0 ", "B0 -> pi- pi- KS0 ", \
	                             "B0 -> K+ K+ KS0 ", "B0 -> K+ K- KS0 ", "B0 -> K- K- KS0  ", \
				     "B0 -> K+ pi- KS0  ","B0 -> pi+ K- KS0 ", "B0 -> K+ pi+ KS0 ", \
				     "B0 -> K- pi- KS0 "]
	B2KSLLhh.DaughtersCuts = {"K+":"(TRCHI2DOF<%(TrackCHI2DOF)s)"% self.getProps() ,"pi+": "(TRCHI2DOF<%(TrackCHI2DOF)s)"% self.getProps()}
	B2KSLLhh.CombinationCut = BLL_CombCuts
	B2KSLLhh.MotherCut = BLL_MotherCuts
        return B2KSLLhh
    ##############
    #  B-KSDD hh #  
    ##############
    def B2KSDDhhAlg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    
#		      &(DOCA(1,2'LoKi::TrgDistanceCalculator')< %(DOCAhh)s)\
        
        BDD_CombCuts=" (AM>(5279-%(B_Mlow)s) *MeV)\
	              &(AM<(5279+%(B_Mhigh)s) *MeV)\
	              &(AVAL_MAX(MIPDV(PRIMARY), PT)>%(B_IPPTmax)s)\
		      &((APT1+APT2+APT3)>%(B_PTsum_DD)s *MeV)\
	              &(ANUM(PT > %(B_PTmed_DD)s *MeV)>=2)\
		      &(ADOCA(1,2)< %(DOCAhh)s)\
		      &((ACHILD(MIPCHI2DV(PRIMARY),1)+ACHILD(MIPCHI2DV(PRIMARY),2))> %(B_IPsumhh)s)"% self.getProps()

        BDD_MotherCuts="(VFASPF(VCHI2)<%(B_Vchi2)s)\
		       &(BPVTRGPOINTINGWPT<%(B_pointpt)s)\
		       &(BPVDIRA>%(B_dira)s)\
		       &(MIPDV(PRIMARY)<%(B_IP_DD)s)\
		       &(VFASPF(VMINVDDV(PRIMARY))>%(B_FDpvsv)s)\
		       &(BPVVDCHI2>%(B_FDchi2_DD)s)"% self.getProps()

	B2KSDDhh=CombineParticles("StripB2KSDDhh")
	B2KSDDhh.InputLocations = [ "StripKSDDforB2KShh","StdLooseKaons","StdLoosePions"]
	B2KSDDhh.DecayDescriptors = ["B0 ->  pi+ pi+ KS0 ", "B0 -> pi+ pi- KS0 ", "B0 -> pi- pi- KS0 ", \
	                             "B0 -> K+ K+ KS0 ", "B0 -> K+ K- KS0 ", "B0 -> K- K- KS0  ", \
				     "B0 -> K+ pi- KS0  ","B0 -> pi+ K- KS0 ", "B0 -> K+ pi+ KS0 ", \
				     "B0 -> K- pi- KS0 "]
	B2KSDDhh.DaughtersCuts = {"K+":"(TRCHI2DOF<%(TrackCHI2DOF)s)"% self.getProps() ,"pi+": "(TRCHI2DOF<%(TrackCHI2DOF)s)"% self.getProps()}
	B2KSDDhh.CombinationCut = BDD_CombCuts
	B2KSDDhh.MotherCut = BDD_MotherCuts
        return B2KSDDhh


    def getProps(self) :
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
