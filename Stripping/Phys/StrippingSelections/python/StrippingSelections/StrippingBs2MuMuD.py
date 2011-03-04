__author__ = 'Paul Schaack'
__date__ = '25/10/2010'
__version__ = '$Revision: 1.0 $'

"""
Stripping selection for Bs -> Mu Mu D ( -> K pi).
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from Configurables import CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import Selection, AutomaticData
from StrippingUtils.Utils import checkConfig


#################
#
#  Define Cuts here
#
#################


defaultConfig = {
    'BsIPCHI2'             : 9.0           # dimensionless
    , 'BsLT'               : 0.0002        # ns
    , 'BsVertexCHI2'       : 40.0          # dimensionless
    , 'DMINIPCHI2'         : 9.0           # dimensionless
    , 'MuonMINIPCHI2'      : 9.0           # dimensionless
}

#################
#
#  Make line here
#
#################

class StrippingBs2MuMuDConf(object):
    __config_keys__ = (
        'BsIPCHI2'
        , 'BsLT'
        , 'BsVertexCHI2'
        , 'DMINIPCHI2'
        , 'MuonMINIPCHI2'
    )

    def __init__(self, config, name="Bs2MuMuD"):
        """
        Creates Bs Selection object
        """
        checkConfig(StrippingBs2MuMuDConf.__config_keys__, config)

        self.name = name
        self.Dimuon = self.__Dimuon__(config)
        self.D = self.__D__(config)
        self.Bs = self.selBs(self.Dimuon, self.D, config)
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


    def __D__(self, conf):
        """
        Filter Kstar from StdLooseD02HH.py
        """
        _D = AutomaticData( Location="Phys/StdLooseD02KPi" )
        _filter = FilterDesktop("Filter_"+self.name+"_D",
                                Code = "(MIPCHI2DV(PRIMARY) > %(DMINIPCHI2)s )" % conf )
        _sel = Selection("Selection"+self.name+"_D",
                         RequiredSelections = [ _D ] ,
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

    def selBs(self, Dimuon, D, conf):
        """
        Make and return a Bs selection
        """      
        _bs2mumuD = CombineParticles("Combine_"+self.name)
        _bs2mumuD.DecayDescriptor = "B_s0 -> D0 J/psi(1S)"
        _bs2mumuD.CombinationCut = "(ADAMASS('B_s0') < 500.0 *MeV)"
        _bs2mumuD.MotherCut = self.__BsCuts__(conf)
        
        SelBS2MUMUD = Selection("Selection_"+self.name,
                                Algorithm = _bs2mumuD,
                                RequiredSelections = [ Dimuon, D] )
        return SelBS2MUMUD

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

