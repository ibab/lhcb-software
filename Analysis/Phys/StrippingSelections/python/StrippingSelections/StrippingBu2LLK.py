__author__ = 'Patrick Koppenburg'
__date__ = '08/12/2009'
__version__ = '$Revision: 1.4 $'

"""
B->llK selections for B->eeK versus B->MuMuK
"""
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from StrippingSelections.Utils import checkConfig
from PhysSelPython.Wrappers import Selection, DataOnDemand
from Configurables import FilterDesktop, CombineParticles
from StrippingConf.StrippingLine import StrippingLine

defaultConfig = {
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

class StrippingBu2LLKConf(object):
    """
    Definition of B->LLK
    
    To use this in the stripping do:
    
    from StrippingSelections.StrippingBu2LLK import StrippingBu2LLKConf
    from StrippingSelections.StrippingBu2LLK import defaultConfig
    
    Bu2LLKConf = StrippingBu2LLKConf( config=defaultConfig )
    lines = Bu2LLKConf.lines()

    Warning : gives you a eeK (fisrt) and a mmK line (second). Do not append to same stream!
    you cana also use the _eeKLine() and _mmKLine() methods
    
    author : P. Koppenburg
    data   : 03/09/2011
    """
    
    __configuration_keys__ = (
                   'BFlightCHI2'        
                ,  'BDIRA'              
                ,  'BIPCHI2'            
                ,  'BVertexCHI2'        
                ,  'DiLeptonPT'         
                ,  'DiLeptonFDCHI2'     
                ,  'DiLeptonIPCHI2'     
                ,  'LeptonPT'           
                ,  'KaonIPCHI2'         
                ,  'KaonPT'
                ,  'JpsiMassWindow'
                ,  'HighMassBLowerMass'
                ,  'SignalBUpperMass'
                   )
    
    def __init__(self,name="Bu2LLK",config=None):
        """
        Constructor makes the selection objects but doesn't make the lines
        """
        checkConfig(StrippingBu2LLKConf.__configuration_keys__,config)
        
        self.name = name
        self.__Bu_ee__  = self._combineBu( config, '_ee')
        self.__Bu_mm__  = self._combineBu( config, '_mm' )
        self.__eeK__ = self._eeK()
        self.__mmK__ = self._mmK()
        
        self.eeKLine = None
        self.mmKLine = None
        
    def _eeK( self ) :
        """
        EEK 
        """
        _dielectron = DataOnDemand( Location="Phys/StdLooseDiElectron" )
        _kaons = DataOnDemand( Location="Phys/StdLooseKaons" )
        _sel = Selection( "Sel"+self.name+"_ee", RequiredSelections = [ _kaons, _dielectron ], Algorithm = self.__Bu_ee__ )
        return _sel
    
    def _mmK( self ) :
        """
        mumuK
        """
        _dimuon = DataOnDemand( Location="Phys/StdLooseDiMuon" )
        _kaons = DataOnDemand( Location="Phys/StdLooseKaons" )
        _sel = Selection( "Sel"+self.name+"_mm", RequiredSelections = [ _kaons, _dimuon ], Algorithm = self.__Bu_mm__ )
        return _sel

    def _combineBu(self, conf = None, trail=None):
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
        kaoncut = "(PT> %(KaonPT)s *MeV) & (MIPCHI2DV(PRIMARY)> %(KaonIPCHI2)s )" % conf
        #
        # lepton cut :(PT>1000*MeV)
        # leptoncut = "(PT> %(LeptonPT)s *MeV)" % self.getProps()
        #
        # dilepton mass cut 
        # llcombcut   = "(AM < %(SignalBUpperMass)s *MeV) & (APT < %(DiLeptonPT)s) & (MINTREE(ABSID<14,PT)>%(LeptonPT)s *MeV)" % self.getProps()
        #
        # dilepton cut (VFASPF(VCHI2/VDOF)<9)
        llcut   = "(MM>100*MeV) & " \
                  "(MM < %(SignalBUpperMass)s *MeV) & " \
                  "(PT > %(DiLeptonPT)s) & " \
                  "(MINTREE(ABSID<14,PT)>%(LeptonPT)s *MeV) & " \
                  "(VFASPF(VCHI2/VDOF)<9) & (BPVVDCHI2> %(DiLeptonFDCHI2)s ) & "\
                  "(BPVIPCHI2() > %(DiLeptonIPCHI2)s )"  % conf
        # 
        # B mass cuts : Hard-coded as we _need_ the full B mass window for the final fit. Nobody dare touch that!
        combcut = "(ADAMASS('B0')<600*MeV)"
        # 
        # B candidate cuts : ((VFASPF(VCHI2/VDOF)<3) & (BPVIPCHI2()<25) & (BPVDIRA>0.9998) & (BPVVDCHI2>50))
        bcut   = "((VFASPF(VCHI2/VDOF)< %(BVertexCHI2)s ) & (BPVIPCHI2()< %(BIPCHI2)s ) & (BPVDIRA> %(BDIRA)s ) & (BPVVDCHI2> %(BFlightCHI2)s ))" % conf
        
        ################################
        #
        # B -> J/psi K
        #
        StripBu2MuMuK = CombineParticles("StripBu2MuMuK"+trail)
        StripBu2MuMuK.DecayDescriptor = "[ B+ -> J/psi(1S) K+ ]cc" 
        StripBu2MuMuK.DaughtersCuts = { "J/psi(1S)" : llcut , "K+" : kaoncut }
        StripBu2MuMuK.CombinationCut = combcut
        StripBu2MuMuK.MotherCut = bcut
        return StripBu2MuMuK

    def lines( self ):
        """
        Creates and returns the line list for the default stripping
        
        @author P. Koppenburg
        @date 03/09/2010
        """
        return [ self._eeKLine(), self._mmKLine() ]
        
    def _eeKLine( self ):
        
        if self.eeKLine == None :
            self.eeKLine = StrippingLine ( self.name+"_ee",
                                           prescale = 1,
                                           algos = [ self.__eeK__ ] )
        return self.eeKLine

    def _mmKLine( self ):
        if self.mmKLine == None :
            self.mmKLine = StrippingLine ( self.name+"_mm",
                                           prescale = 1,
                                           algos = [ self.__mmK__ ] )
            
        return self.mmKLine
    

    
