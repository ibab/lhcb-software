__author__ = 'Paul Schaack'
__date__ = '25/10/2010'
__version__ = '$Revision: 1.0 $'

"""
Stripping selection for Bs -> Mu Mu Kstar ( -> K pi).
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from Configurables import CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import Selection, AutomaticData
from StrippingSelections.Utils import checkConfig


#################
#
#  Define Cuts here
#
#################


defaultConfig = {
    'BsIPCHI2'             : 9.0           # dimensionless
    , 'BsLT'               : 0.0002        # ns
    , 'BsVertexCHI2'       : 40.0          # dimensionless
    , 'KstarMINIPCHI2'      : 9.0           # dimensionless    
    , 'MuonMINIPCHI2'      : 9.0           # dimensionless
}

#################
#
#  Make line here
#
#################

class StrippingBs2MuMuKstarConf(object):
    __config_keys__ = (
        'BsIPCHI2'
        , 'BsLT'
        , 'BsVertexCHI2'
        , 'KstarMINIPCHI2'
        , 'MuonMINIPCHI2'
    )

    def __init__(self, config, name="Bs2MuMuKstar"):
        """
        Creates Bs Selection object
        """
        checkConfig(StrippingBs2MuMuKstarConf.__config_keys__, config)

        self.name = name
        self.Dimuon = self.__Dimuon__(config)
        self.Kstar = self.__Kstar__(config)
        self.Bs = self.selBs(self.Dimuon, self.Kstar, config)
        self.line = None


    def __Dimuon__(self, conf):
        """
        Filter Dimuon 
        """
        _muons = AutomaticData(Location = 'Phys/StdLooseMuons')

        muonCuts = "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s )" % conf        

        _Dimuon = CombineParticles("Dimuon")
        _Dimuon.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
        _Dimuon.DaughtersCuts = { "mu+" : muonCuts,
                                 "mu-" : muonCuts
                                 }
        _Dimuon.CombinationCut = "(ADOCACHI2CUT(30, ''))"
        _Dimuon.MotherCut = "(VFASPF(VCHI2) < 25)"

        _sel =  Selection ("Selection"+self.name+"_Dimuon",
                            Algorithm = _Dimuon,
                            RequiredSelections = [_muons])
        return _sel


    def __Kstar__(self, conf):
        """
        Filter Kstar from StdLooseKstar.py
        """
        _kstar = AutomaticData( Location="Phys/StdLooseKstar2Kpi" )
        _filter = FilterDesktop("Filter_"+self.name+"_Kstar",
                                Code = "(MIPCHI2DV(PRIMARY) > %(KstarMINIPCHI2)s )" % conf )
        _sel = Selection("Selection"+self.name+"_Kstar",
                         RequiredSelections = [ _kstar ] ,
                         Algorithm = _filter )
        return _sel


    def __BsCuts__(self, conf):
        """
        Returns the Bs cut string
        """
        _BsCuts = """
        (VFASPF(VCHI2) < %(BsVertexCHI2)s ) &
        (BPVLTIME( %(BsIPCHI2)s ) > %(BsLT)s *ns)
        """ % conf
        return _BsCuts

    def selBs(self, Dimuon, Kstar, conf):
        """
        Make and return a Bs selection
        """      
        _bs2mumukstar = CombineParticles("Combine_"+self.name)
        _bs2mumukstar.DecayDescriptor = "B_s0 -> K*(892)0 J/psi(1S)"
        _bs2mumukstar.CombinationCut = "(ADAMASS('B_s0') < 500.0 *MeV)"
        _bs2mumukstar.MotherCut = self.__BsCuts__(conf)
        
        SelBS2MUMUKSTAR = Selection( "Selection_"+self.name,
                                     Algorithm = _bs2mumukstar,
                                     RequiredSelections = [ Dimuon, Kstar] )
        return SelBS2MUMUKSTAR

    def lines(self):
        """
        Return signal line
        """
        if None == self.line:
            self.line = StrippingLine(self.name+"_line",
                                      prescale = 1,
                                      algos = [ self.Bs ]
                                      )
        return [ self.line ] 

