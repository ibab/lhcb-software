__author__ = 'Paul Schaack'
__date__ = '12/02/2011'
__version__ = '$Revision: 1.0 $'

__all__ = ( 'Bs2MuMuPhiConf' )

"""
Stripping selection for Bs -> Mu Mu Phi ( -> K K).
"""

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import  CombineParticles, FilterDesktop
from PhysSelPython.Wrappers import Selection, AutomaticData
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from LHCbKernel.Configuration import *  #check if needed


#################
#
#  Define Cuts here
#
#################


defaultConfig = {
    'BsIPCHI2'             : 9.0           # dimensionless
    , 'BsLT'               : 0.0002        # ns
    , 'BsVertexCHI2'       : 40.0          # dimensionless
    , 'KaonPIDK'           : 0             # dimensionless
    , 'KaonMINIPCHI2'      : 9.0           # dimensionless
    , 'MuonMINIPCHI2'      : 9.0           # dimensionless
}


#################
#
#  Make line here
#
#################

defaultName = "Bs2MuMuPhi"


class Bs2MuMuPhiConf(LineBuilder) :

    __configuration_keys__ = (
        'BsIPCHI2'
        , 'BsLT'
        , 'BsVertexCHI2'
        , 'KaonPIDK'
        , 'KaonMINIPCHI2'
        , 'MuonMINIPCHI2'
    )

    def __init__(self, name, config) :


        LineBuilder.__init__(self, name, config)

        self.name = name
        self.Muons = self.__Muons__(config)
        self.Kaons = self.__Kaons__(config)
        self.Bs = self.__Bs__(self.Muons, self.Kaons, config)

        self.line = StrippingLine(self.name+"Line",
                                  prescale = 1,
                                  algos = [ self.Bs ]
                                  )
        self.registerLine(self.line)



    def __Muons__(self, conf):
        """
        Filter muons from StdLooseMuons
        """  
        _muons = AutomaticData(Location = 'Phys/StdLooseMuons/Particles')
        _filter = FilterDesktop(Code = self.__MuonCuts__(conf))
        _sel = Selection("Selection_"+self.name+"_Muons",
                         RequiredSelections = [ _muons ] ,
                         Algorithm = _filter)

        return _sel
        
    def __Kaons__(self, conf):
        """
        Filter kaons from StdLooseKaons
        """  
        _kaons = AutomaticData(Location = 'Phys/StdLooseKaons/Particles')
        _filter = FilterDesktop(Code = self.__KaonCuts__(conf))
        _sel = Selection("Selection_"+self.name+"_Kaons",
                         RequiredSelections = [ _kaons ] ,
                         Algorithm = _filter)

        return _sel

    def __KaonCuts__(self, conf):
        """
        Returns the Kaon cut string
        """
        _KaonCuts = """
        (PIDK > %(KaonPIDK)s ) &
        (MIPCHI2DV(PRIMARY) > %(KaonMINIPCHI2)s )
        """ % conf
        return _KaonCuts

    def __MuonCuts__(self, conf):
        """
        Returns the Muon cut string
        """
        _MuonCuts = """
        (MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s )
        """ % conf
        return _MuonCuts

    def __BsCuts__(self, conf):
        """
        Returns the Bs cut string
        """
        _BsCuts = """
        (VFASPF(VCHI2) < %(BsVertexCHI2)s ) &
        (BPVLTIME( %(BsIPCHI2)s ) > %(BsLT)s *ns)
        """ % conf
        return _BsCuts

    def __Bs__(self, Muons, Kaons, conf):
        """
        Make and return a Bs selection
        """      
        _bs2KKmumu = CombineParticles()
        _bs2KKmumu.DecayDescriptor = "B_s0 -> K+ K- mu+ mu-"
        _bs2KKmumu.CombinationCut = "(ADAMASS('B_s0') < 1000.0 *MeV) & (AM12 < 1070.0 *MeV)"
        _bs2KKmumu.MotherCut = self.__BsCuts__(conf)
        
        SelBS2KKMUMU = Selection( "Selection_"+self.name,
                                  Algorithm = _bs2KKmumu,
                                  RequiredSelections = [ Muons, Kaons ] )
        return SelBS2KKMUMU

