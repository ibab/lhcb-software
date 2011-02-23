#!/usr/bin/env python

__author__ = 'Alexandr Kozlinskiy'
__email__ = 'akozlins@cern.ch'
__date__ = '31/08/2010'
__version__ = '$Revision: 1.2 $'

"""

Stripping lines for D* and D0 with D0 -> mu mu.
Control channel: D0 -> pi+ pi- (prescaled)

Cuts:
  * Particles input: StdLooseMuons, StdNoPIDsPions, StdNoPIDsKaons
  * Track-Chi2/NDOF of the D0 daughters < 5
  * Pt of the D0 daughters > 1 GeV
  * D0 mass window 100 MeV
  * Chi2 of the D0 vertex < 25
  * Chi2 of the D0 life time fit < 25
  * D0 life time > 0.1 ps
  * Track-Chi2/NDOF of the soft pion from D* < 5
  * M(D*) - M(D0) < 155 MeV
  * Chi2 of the D* vertex < 25
  * HLT = 'Hlt.*(MuMu|MBMicro|Muon).*Decision'



Data sample: $STRIPPINGSELECTIONSROOT/tests/data/RUN_81430_RealData+Reco08-Stripping12_90000000_SDST.py
Note: no prescales

StrippingReport                                                INFO Event 10000, Good event 9252
 |                                    *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*| *Errs*|*Incds*| *Slow*|
 |_StrippingGlobal_                                  |  3.3182|       307|       |   5.889|       |       |       |
 |_StrippingSequenceStreamTest_                      |  3.3182|       307|       |   5.884|       |       |       |
 |!StrippingD02MuMuLine                              |  0.1189|        11|  0.000|   3.638|      0|      0|     17|
 |!StrippingD02MuMuDstLine                           |  0.0324|         3|  0.000|   1.131|      0|      0|      0|
 |!StrippingD02MuMuClbrD02PiPiLine                   |  3.3074|       306|  0.000|   0.338|      0|      0|      0|
 |!StrippingD02MuMuClbrDstD02PiPiLine                |  0.6701|        62|  0.000|   0.318|      0|      0|      0|
 
=================================================================================================
                         AlgorithmCorrelationsAlg.AlgorithmCorrelations
=================================================================================================
    Algorithm                             Eff.       1       2       3       4       5       6  
-------------------------------------------------------------------------------------------------
  1 StrippingGlobal                       3.32% |  ###### 100.00% 100.00% 100.00% 100.00% 100.00%
  2 StrippingSequenceStreamTest           3.32% | 100.00%  ###### 100.00% 100.00% 100.00% 100.00%
  3 StrippingD02MuMuLine                  0.12% |   3.58%   3.58%  ###### 100.00%   3.27%   4.84%
  4 StrippingD02MuMuDstLine               0.03% |   0.98%   0.98%  27.27%  ######   0.98%   4.84%
  5 StrippingD02MuMuClbrD02PiPiLine       3.31% |  99.67%  99.67%  90.91% 100.00%  ###### 100.00%
  6 StrippingD02MuMuClbrDstD02PiPiLine    0.67% |  20.20%  20.20%  27.27% 100.00%  20.26%  ######
=================================================================================================

"""

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class D02MuMuConf(LineBuilder) :
    __configuration_keys__ = ('MinDauPT',           # 1.0 GeV
                              'MinD0LT',            # 0.1 ps
                              'D02MuMuPrescale',    # 1.0
                              'DstD02MuMuPrescale', # 1.0
                              'D02PiPiPrescale',    # 0.05
                              'DstD02PiPiPrescale'  # 0.05
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        muons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")
        pions = DataOnDemand(Location = "Phys/StdNoPIDsPions/Particles")

        D0DaughtersCutsMuons = \
          "(TRCHI2DOF < 5) & (PT > %(MinDauPT)s * GeV)" % config
        D0DaughtersCutsPion = \
          "(TRCHI2DOF < 5) & (PT > %(MinDauPT)s * GeV)" % config
        D0CombinationCut = \
          "(ADAMASS('D0') < 110 * MeV)"
        D0MotherCut = \
          "(VFASPF(VCHI2) < 25) & " + \
          "(ADMASS('D0') < 100 * MeV) & " + \
          "(BPVLTFITCHI2() < 25) & " + \
          "(BPVLTIME() > %(MinD0LT)s * ps)" % config

        DstarDaughtersCutsPion = \
          "(TRCHI2DOF < 5)"
        DstarCombinationCut = \
          "AM - AM1 < 160 * MeV"
        DstarMotherCut = \
          "(VFASPF(VCHI2) < 25) & " + \
          "((M - M1) < 155 * MeV)"

        # D0 combine
        from Configurables import CombineParticles
        CombineD0 = CombineParticles( name + "_CombineD0" )
        CombineD0.DecayDescriptors = [ "[D0 -> mu+ mu-]cc", "[D0 -> pi+ pi-]cc" ]
        CombineD0.DaughtersCuts = { "mu+" : D0DaughtersCutsMuons, "pi+" : D0DaughtersCutsPion }
        CombineD0.CombinationCut = D0CombinationCut
        CombineD0.MotherCut = D0MotherCut

        # D* combine
        CombineDst = CombineParticles( name + "_CombineDst" )
        CombineDst.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
        CombineDst.DaughtersCuts = { "pi+" : DstarDaughtersCutsPion }
        CombineDst.CombinationCut = DstarCombinationCut
        CombineDst.MotherCut = DstarMotherCut

        # D0 and D* selections
        from PhysSelPython.Wrappers import Selection
        D02MuMuSelection = Selection(name + "_D02MuMuSelection", Algorithm = CombineD0, RequiredSelections = [ muons ])
        DstD02MuMuSelection = Selection(name + "_DstD02MuMuSelection", Algorithm = CombineDst, RequiredSelections = [ muons, pions, D02MuMuSelection ])
        D02PiPiSelection = Selection(name + "_D02PiPiSelection", Algorithm = CombineD0, RequiredSelections = [ pions ])
        DstD02PiPiSelection = Selection(name + "_DstD02PiPiSelection", Algorithm = CombineDst, RequiredSelections = [ pions, D02PiPiSelection ])

        self.lineD02MuMu = StrippingLine(name + "Line",
          algos = [ D02MuMuSelection ],
          prescale = config['D02MuMuPrescale'],
          HLT = "(HLT_PASS_RE('Hlt.*(MuMu|MBMicro|Muon).*Decision'))")

        self.lineDstD02MuMu = StrippingLine(name + "Dst" + "Line",
          algos = [ DstD02MuMuSelection ],
          prescale = config['DstD02MuMuPrescale'],
          HLT = "(HLT_PASS_RE('Hlt.*(MuMu|MBMicro|Muon).*Decision'))")

        self.lineD02PiPi = StrippingLine(name + "ClbrD02PiPi" + "Line",
          algos = [ D02PiPiSelection ],
          prescale = config['D02PiPiPrescale'],
          HLT = "(HLT_PASS_RE('Hlt.*(MuMu|MBMicro|Muon).*Decision'))")

        self.lineDstD02PiPi = StrippingLine(name + "ClbrDstD02PiPi" + "Line",
          algos = [ DstD02PiPiSelection ],
          prescale = config['DstD02PiPiPrescale'],
          HLT = "(HLT_PASS_RE('Hlt.*(MuMu|MBMicro|Muon).*Decision'))")

        self.registerLine(self.lineD02MuMu)
        self.registerLine(self.lineDstD02MuMu)
        self.registerLine(self.lineD02PiPi)
        self.registerLine(self.lineDstD02PiPi)
