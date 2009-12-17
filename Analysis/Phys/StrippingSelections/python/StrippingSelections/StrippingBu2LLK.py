__author__ = 'Patrick Koppenburg'
__date__ = '08/12/2009'
__version__ = '$Revision: 1.2 $'

"""
B->llK selections for B->eeK versus B->MuMuK
"""
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingBu2LLKConf(LHCbConfigurableUser):
    """
    Definition of B->LLK stripping
    """
    __slots__ = { 
                   'BFlightCHI2'        : 100       # adimentional 
                ,  'BDIRA'              : 0.999     # adimentional
                ,  'BIPCHI2'            : 25        # adimentional  
                ,  'BVertexCHI2'        : 16        # adimentional
                ,  'DiLeptonPT'         : 0         # MeV (not used)
                ,  'DiLeptonFDCHI2'     : 16        # adimentional
                ,  'DiLeptonIPCHI2'     : 9         # adimentional
                ,  'LeptonPT'           : 800       # MeV 
                ,  'KaonIPCHI2'         : 4         # adimentional
                ,  'KaonPT'             : 1400      # MeV 
                ,  'JpsiMassWindow'     : 500       # MeV (J/psi box mass window)
                ,  'HighMassBLowerMass' : 5400      # MeV (Lower bound of high-mass box)
                ,  'SignalBUpperMass'   : 5500      # MeV (Higher bound of signal box)
                   }

    def eeK( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        StripBu2MuMuK = self.combineBu()
        StripBu2eeK = StripBu2MuMuK.clone("StripBu2eeK")
        StripBu2eeK.InputLocations = [ "StdLooseDiElectron", "StdLooseKaons" ]
        return StrippingLine('Bu2eeK', prescale = 1, algos = [ StripBu2eeK ] )    
    
    def mmK( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        StripBu2MuMuK = self.combineBu()
        return StrippingLine('Bu2MuMuK', prescale = 1, algos = [ StripBu2MuMuK ] )

    def combineBu(self):
        """
        The thing that defines The B
        """        
        from Configurables import FilterDesktop, CombineParticles
        import GaudiKernel.SystemOfUnits as Units
        ################################
        #
        # All cuts
        #
        # Kaon cut : (PT>1400*MeV) & (MIPCHI2DV(PRIMARY)>4)
        kaoncut = "(PT> %(KaonPT)s *MeV) & (MIPCHI2DV(PRIMARY)> %(KaonIPCHI2)s )" % self.getProps()
        #
        # lepton cut :(PT>1000*MeV)
        # leptoncut = "(PT> %(LeptonPT)s *MeV)" % self.getProps()
        #
        # dilepton mass cut 
        # llcombcut   = "(AM < %(SignalBUpperMass)s *MeV) & (APT < %(DiLeptonPT)s) & (MINTREE(ABSID<14,PT)>%(LeptonPT)s *MeV)" % self.getProps()
        #
        # dilepton cut (VFASPF(VCHI2/VDOF)<9)
        llcut   = "(MM>100*MeV) & (MM < %(SignalBUpperMass)s *MeV) & (PT > %(DiLeptonPT)s) & (MINTREE(ABSID<14,PT)>%(LeptonPT)s *MeV) & (VFASPF(VCHI2/VDOF)<9) & (BPVVDCHI2> %(DiLeptonFDCHI2)s ) & (BPVIPCHI2() > %(DiLeptonIPCHI2)s )"  % self.getProps()
        # 
        # B mass cuts : Hard-coded as we _need_ the full B mass window for the final fit. Nobody dare touch that!
        combcut = "(ADAMASS('B0')<600*MeV)"
        # 
        # B candidate cuts : ((VFASPF(VCHI2/VDOF)<3) & (BPVIPCHI2()<25) & (BPVDIRA>0.9998) & (BPVVDCHI2>50))
        bcut   = "((VFASPF(VCHI2/VDOF)< %(BVertexCHI2)s ) & (BPVIPCHI2()< %(BIPCHI2)s ) & (BPVDIRA> %(BDIRA)s ) & (BPVVDCHI2> %(BFlightCHI2)s ))" % self.getProps()
        
        ################################
        #
        # B -> J/psi K
        #
        StripBu2MuMuK = CombineParticles("StripBu2MuMuK")
        StripBu2MuMuK.InputLocations = [ "StdLooseDiMuon", "StdLooseKaons" ]
        StripBu2MuMuK.DecayDescriptor = "[ B+ -> J/psi(1S) K+ ]cc" 
        StripBu2MuMuK.DaughtersCuts = { "J/psi(1S)" : llcut , "K+" : kaoncut }
        StripBu2MuMuK.CombinationCut = combcut
        StripBu2MuMuK.MotherCut = bcut
        return StripBu2MuMuK
        
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
