
__author__ = 'Regis Lefevre'
__date__ = '03/09/2010'
__version__ = '$Revision: 1.1 $'

'''
Stripping selection for D0 -> K pi pi0 
'''
####################################################################
# Stripping selection for D0 -> K pi pi0 
#  2 lines : one for merged, one for resolved pi0
####################################################################

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingD02KPiPi0Conf(LHCbConfigurableUser):
    __slots__ = {
        'TrackMinPT'          : 300       # MeV
        ,'TrackMinPT_R'       : 600       # MeV  (>= TrackMinPT else no effect)
        ,'TrackMinTrackProb'  : 0.000001  # unitless
        ,'TrackMinIPChi2'     : 16        # unitless
        ,'Pi0MinPT_M'         : 2000      # MeV
        ,'Pi0MinPT_R'         : 1000      # MeV
        ,'ResPi0MinGamCL'     : 0.2       # unitless
        ,'D0MinM'             : 1300      # MeV
        ,'D0MaxM'             : 2400      # MeV
        ,'D0MinVtxProb'       : 0.001     # unitless
        ,'D0MaxIPChi2'        : 9         # unitless
        ,'D0MinDIRA'          : 0.9999    # unitless
        ,'D0MinVVDChi2'       : 64        # unitless
         }

    ###############################################
    # Create StrippingLines with these selections #
    ###############################################
    def D02KPiPi0_Merged( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        D02KPiPi0_Ks    = self.D02KPiPi0_Ks_Alg()
        D02KPiPi0_Merged = self.D02KPiPi0_Merged_Alg()
        return StrippingLine('D02KPiPi0_Merged_line'
                             , prescale = 1
                             , algos = [D02KPiPi0_Ks, D02KPiPi0_Merged]
                             , postscale = 1
                             )

    def D02KPiPi0_Resolved( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        D02KPiPi0_Ks      = self.D02KPiPi0_Ks_Alg()
        D02KPiPi0_Resolved = self.D02KPiPi0_Resolved_Alg()
        return StrippingLine('D02KPiPi0_Resolved_line'
                             , prescale = 1
                             , algos = [D02KPiPi0_Ks, D02KPiPi0_Resolved]
                             , postscale = 1
                             )

    ################
    # K- pi+ comb. # 
    ################

    def D02KPiPi0_Ks_Alg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    
	D02KPiPi0_Ks=CombineParticles("StripD02KPiPi0_Ks")
	D02KPiPi0_Ks.InputLocations = ["StdTightPions", "StdTightKaons"]
	D02KPiPi0_Ks.DecayDescriptor = "[K*(892)~0 -> K- pi+]cc"
	D02KPiPi0_Ks.DaughtersCuts = {
            "K-"   : "(PT>%(TrackMinPT)s *MeV) & (TRPCHI2>%(TrackMinTrackProb)s) & (MIPCHI2DV(PRIMARY)>%(TrackMinIPChi2)s)"  % self.getProps()
            ,"pi+" : "(PT>%(TrackMinPT)s *MeV) & (TRPCHI2>%(TrackMinTrackProb)s) & (MIPCHI2DV(PRIMARY)>%(TrackMinIPChi2)s)" % self.getProps()   
            }
	D02KPiPi0_Ks.CombinationCut = "AALL" % self.getProps()
	D02KPiPi0_Ks.MotherCut = "(VFASPF(VPCHI2)>%(D0MinVtxProb)s) & (BPVVDCHI2>%(D0MinVVDChi2)s)" % self.getProps()
        return D02KPiPi0_Ks

    ##########
    # Merged # 
    ##########
    
    def D02KPiPi0_Merged_Alg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    
	D02KPiPi0M=CombineParticles("StripD02KPiPi0M")
	D02KPiPi0M.InputLocations = ["StripD02KPiPi0_Ks", "StdLooseMergedPi0"]
	D02KPiPi0M.DecayDescriptor = "[D0 -> K*(892)~0 pi0]cc"
	D02KPiPi0M.DaughtersCuts = {
            "K*(892)~0" : "ALL" % self.getProps()
            ,"pi0"      : "(PT>%(Pi0MinPT_M)s *MeV)" % self.getProps()   
            }
	D02KPiPi0M.CombinationCut = "(AM>%(D0MinM)s *MeV) & (AM<%(D0MaxM)s *MeV)" % self.getProps()
	D02KPiPi0M.MotherCut = "(BPVIPCHI2()<%(D0MaxIPChi2)s) & (BPVDIRA>%(D0MinDIRA)s)" % self.getProps()
        return D02KPiPi0M

    ############
    # Resolved # 
    ############
    def D02KPiPi0_Resolved_Alg(self):
        from Configurables import CombineParticles
        import GaudiKernel.SystemOfUnits as Units    
	D02KPiPi0R=CombineParticles("StripD02KPiPi0R")
	D02KPiPi0R.InputLocations = ["StripD02KPiPi0_Ks", "StdLooseResolvedPi0"]
	D02KPiPi0R.DecayDescriptor = "[D0 -> K*(892)~0 pi0]cc"
	D02KPiPi0R.DaughtersCuts = {
            "K*(892)~0" : "(CHILD(PT,1)>%(TrackMinPT_R)s *MeV) & (CHILD(PT,2)>%(TrackMinPT_R)s *MeV)" % self.getProps()     
            ,"pi0"      : "(PT>%(Pi0MinPT_R)s *MeV) & (CHILD(CL,1)>%(ResPi0MinGamCL)s) & (CHILD(CL,2)>%(ResPi0MinGamCL)s)" % self.getProps()
            }
	D02KPiPi0R.CombinationCut = "(AM>%(D0MinM)s *MeV) & (AM<%(D0MaxM)s *MeV)" % self.getProps()
	D02KPiPi0R.MotherCut = "(BPVIPCHI2()<%(D0MaxIPChi2)s) & (BPVDIRA>%(D0MinDIRA)s)" % self.getProps()
        return D02KPiPi0R

    ######
    ######
    ######
    def getProps(self) :
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
