"""
Configuration for the high-PT displaced muon line (for exotic long-lived particle decays with neutralino, and jet studies)
"""
__author__  = "Pieter David"
__date__    = "2014-08-28"
__all__     = ("LLP2MuXConf", "default_config")

from LHCbKernel.Configuration import *
from GaudiKernel import SystemOfUnits as units

from PhysSelPython.Wrappers import AutomaticData, Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop

default_config= {
    'NAME'        : 'LLP2MuX',
    'BUILDERTYPE' : 'LLP2MuXConf',
    'STREAMS'     : [ 'EW'  ],
    'WGs'         : [ 'QEE' ],
    'CONFIG'      : { 
        "MinPT"     : 12.00*units.GeV,
        "MinIP"     :  0.25*units.mm,
        "L0DU"      : "L0_CHANNEL('Muon')",
        "HLT1"      : "HLT_PASS_RE('Hlt1.*SingleMuonHighPTDecision')",
        "HLT2"      : "HLT_PASS_RE('Hlt2.*SingleMuon.*High.*Decision')",  # Prepare for new Hlt2EW name
        'RawEvents' : ["Trigger","Muon","Calo","Rich"], ## FIXME "Velo" and "Tracker"
    },
}

class LLP2MuXConf(LineBuilder):
    __configuration_keys__ = default_config['CONFIG'].keys()

    def __init__( self, name, config ):
        LineBuilder.__init__( self, name, config )

        allLooseMuons = AutomaticData("Phys/StdAllLooseMuons/Particles")

        muonSel = Selection( "%sMuonSelection" % self.name(),
                    RequiredSelections = [ allLooseMuons ],
                    Algorithm = FilterDesktop( Code = "( PT > %(MinPT)f ) & ( MIPDV('') > %(MinIP)f )" % config )
                    )

        testLine = StrippingLine( "%sHighPTHighIPMuonLine" % self.name(),
                        selection = muonSel,
                        L0DU = config['L0DU'],
                        HLT1 = config['HLT1'],
                        HLT2 = config['HLT2'],
                        RequiredRawEvents = config['RawEvents'],
                        )

        self.registerLine(testLine)
