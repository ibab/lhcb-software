
__author__ = 'Regis Lefevre'
__date__ = '08/07/2010'
__version__ = '$Revision: 1.1 $'

'''
Stripping selection for B -> h h pi0
'''
#################################################################
#  This strip is used both for Bd -> pi pi pi0 and Bs -> K pi pi0
#  B Mass window : 4200 to 6400 MeV/c2 
#  2 lines : one for merged, one for resolved pi0
#################################################################

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingB2HHPi0Conf(LHCbConfigurableUser):
    __slots__ = {
        'PiMinPT'               : 300       # MeV
        ,'PiMinP'               : 2000      # MeV
        ,'PiMinTrackProb'       : 0.000001  # unitless
        ,'PiMinIPChi2'          : 25        # unitless
        ,'Pi0MinPT_M'           : 2500      # MeV
        ,'Pi0MinPT_R'           : 1500      # MeV
        ,'ResPi0MinMM'          : 115       # MeV
        ,'ResPi0MaxMM'          : 155       # MeV
        ,'ResPi0MinGamCL'       : 0.2       # unitless
        ,'BMinM'                : 4200      # MeV
        ,'BMaxM'                : 6400      # MeV
        ,'BMinPT_M'             : 3000      # MeV
        ,'BMinPT_R'             : 2500      # MeV
        ,'BMinVtxProb'          : 0.001     # unitless
        ,'BMaxIPChi2'           : 9         # unitless
        ,'BMinDIRA'             : 0.99995   # unitless
        ,'BMinVVDChi2'          : 64        # unitless
         }

    ###############################################
    # Create StrippingLines with these selections #
    ###############################################
    def B2HHPi0_Merged( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        B2HHPi0_Merged = self.B2HHPi0_Merged_Alg()
        return StrippingLine('B2HHPi0_Merged_line'
                             , prescale = 1
                             , algos = [B2HHPi0_Merged]
                             , postscale = 1
                             )

    def B2HHPi0_Resolved( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        B2HHPi0_Resolved = self.B2HHPi0_Resolved_Alg()
        return StrippingLine('B2HHPi0_Resolved_line'
                             , prescale = 1
                             , algos = [B2HHPi0_Resolved]
                             , postscale = 1
                             )

    ##########
    # Merged # 
    ##########
    def B2HHPi0_Merged_Alg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    
	B2HHPi0M=CombineParticles("StripB2HHPi0M")
	B2HHPi0M.InputLocations = ["StdLoosePions", "StdLooseMergedPi0"]
	B2HHPi0M.DecayDescriptor = "B0 -> pi+ pi- pi0"
	B2HHPi0M.DaughtersCuts = {
            "pi-"  : "(PT>%(PiMinPT)s *MeV) & (P>%(PiMinP)s *MeV) & (TRPCHI2>%(PiMinTrackProb)s) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2)s)" % self.getProps()
            ,"pi+" : "(PT>%(PiMinPT)s *MeV) & (P>%(PiMinP)s *MeV) & (TRPCHI2>%(PiMinTrackProb)s) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2)s)" % self.getProps()   
            ,"pi0" : "(PT>%(Pi0MinPT_M)s *MeV)" % self.getProps()   
            }
	B2HHPi0M.CombinationCut = "(AM>%(BMinM)s *MeV) & (AM<%(BMaxM)s *MeV)" % self.getProps()
	B2HHPi0M.MotherCut = "(PT>%(BMinPT_M)s *MeV) & (VFASPF(VPCHI2)>%(BMinVtxProb)s) & (BPVIPCHI2()<%(BMaxIPChi2)s) & (BPVDIRA>%(BMinDIRA)s) & (BPVVDCHI2>%(BMinVVDChi2)s)" % self.getProps()
        return B2HHPi0M

    ############
    # Resolved # 
    ############
    def B2HHPi0_Resolved_Alg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    
	B2HHPi0R=CombineParticles("StripB2HHPi0R")
	B2HHPi0R.InputLocations = ["StdLoosePions", "StdLooseResolvedPi0"]
	B2HHPi0R.DecayDescriptor = "B0 -> pi+ pi- pi0"
	B2HHPi0R.DaughtersCuts = {
            "pi-"  : "(PT>%(PiMinPT)s *MeV) & (P>%(PiMinP)s *MeV) & (TRPCHI2>%(PiMinTrackProb)s) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2)s)" % self.getProps()
            ,"pi+" : "(PT>%(PiMinPT)s *MeV) & (P>%(PiMinP)s *MeV) & (TRPCHI2>%(PiMinTrackProb)s) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2)s)" % self.getProps()   
            ,"pi0" : "(PT>%(Pi0MinPT_R)s *MeV) & (MM>%(ResPi0MinMM)s *MeV) & (MM<%(ResPi0MaxMM)s *MeV) & (CHILD(CL,1)>%(ResPi0MinGamCL)s) & (CHILD(CL,2)>%(ResPi0MinGamCL)s)" % self.getProps()   
            }
	B2HHPi0R.CombinationCut = "(AM>%(BMinM)s *MeV) & (AM<%(BMaxM)s *MeV)" % self.getProps()
	B2HHPi0R.MotherCut = "(PT>%(BMinPT_R)s *MeV) & (VFASPF(VPCHI2)>%(BMinVtxProb)s) & (BPVIPCHI2()<%(BMaxIPChi2)s) & (BPVDIRA>%(BMinDIRA)s) & (BPVVDCHI2>%(BMinVVDChi2)s)" % self.getProps()
        return B2HHPi0R


    ######
    ######
    ######
    def getProps(self) :
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
