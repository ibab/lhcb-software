"""
Configuration for the high-PT displaced muon line (for exotic long-lived particle decays with neutralino, and jet studies)
"""
__author__ = "Pieter David"
__date__ = "2014-08-28"
__all__ = ("LLP2MuXConf", "default_config")

from LHCbKernel.Configuration import *
from GaudiKernel import SystemOfUnits as units

from PhysSelPython.Wrappers import AutomaticData, Selection
from StrippingConf.StrippingLine import StrippingLine, bindMembers
from StrippingUtils.Utils import LineBuilder

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles

default_config= {
    'NAME'        : 'LLP2MuX',
    'WGs'         : ['QEE'],
    'STREAMS'         : ['EW'],
    'BUILDERTYPE' : 'LLP2MuXConf',
    'CONFIG'      : { "MinPT"  : 12.00*units.GeV
                     ,"MinIP"  :  0.25*units.mm
                    }
}

class LLP2MuXConf(LineBuilder):
    """
    """
    __configuration_keys__ = ("MinPT", "MinIP")

    def __init__( self, name, config ):
        LineBuilder.__init__( self, name, config )

        allLooseMuons = AutomaticData("Phys/StdAllLooseMuons/Particles")

        muonSel = Selection( "%sMuonSelection" % self.name()
                    , RequiredSelections = [ allLooseMuons ]
                    , Algorithm = FilterDesktop( Code = "( PT > %(MinPT)f ) & ( MIPDV('') > %(MinIP)f )" % config )
                    )

        testLine = StrippingLine( "%sHighPTHighIPMuonLine" % self.name()
                        , selection = muonSel
                        , L0DU = "L0_CHANNEL('Muon')"
                        , HLT  = "HLT_PASS('Hlt1SingleMuonHighPTDecision') & HLT_PASS('Hlt2SingleMuonHighPTDecision')"
                        , RequiredRawEvents = ["Trigger","Muon","Calo","Rich"] ## FIXME "Velo" and "Tracker"
                        )
        self.registerLine(testLine)
