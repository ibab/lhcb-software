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
  * Pt of the D0 daughters > 1 GeV (configurable)
  * IP chi2 of the D0 daughters > 1 (configurable)
  * D0 mass window 100 MeV
  * Chi2 of the D0 vertex < 25
  * Chi2 of the D0 life time fit < 25
  * D0 life time > 0.1 ps (configurable)
  * Track-Chi2/NDOF of the soft pion from D* < 5
  * M(D*) - M(D0) < 160 MeV
  * Chi2 of the D* vertex < 25
  * HLT = 'Hlt.*(MuMu|MBMicro|Muon).*Decision'

Data sample: $STRIPPINGSELECTIONSROOT/tests/data/RUN_81430_RealData+Reco08-Stripping12_90000000_SDST.py
Note: no prescales

StrippingReport                                                INFO Event 10000, Good event 9252
 |                                    *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*| *Errs*|*Incds*| *Slow*|
 |_StrippingGlobal_                                  |  2.2157|       205|       |   8.234|       |       |       |
 |_StrippingSequenceStreamTest_                      |  2.2157|       205|       |   8.225|       |       |       |
 |!StrippingD02MuMuLine                              |  0.0973|         9|  0.000|   4.811|      0|      0|     27|
 |!StrippingD02MuMuDstLine                           |  0.0324|         3|  0.000|   1.677|      0|      0|      0|
 |!StrippingD02MuMuClbrD02PiPiLine                   |  2.2049|       204|  0.000|   0.515|      0|      0|      0|
 |!StrippingD02MuMuClbrDstD02PiPiLine                |  0.6485|        60|  0.000|   0.489|      0|      0|      0|
 
#=================================================================================================
                         AlgorithmCorrelationsAlg.AlgorithmCorrelations
#=================================================================================================
    Algorithm                             Eff.       1       2       3       4       5       6  
-------------------------------------------------------------------------------------------------
  1 StrippingGlobal                       2.22% |  ###### 100.00% 100.00% 100.00% 100.00% 100.00%
  2 StrippingSequenceStreamTest           2.22% | 100.00%  ###### 100.00% 100.00% 100.00% 100.00%
  3 StrippingD02MuMuLine                  0.10% |   4.39%   4.39%  ###### 100.00%   3.92%   5.00%
  4 StrippingD02MuMuDstLine               0.03% |   1.46%   1.46%  33.33%  ######   1.47%   5.00%
  5 StrippingD02MuMuClbrD02PiPiLine       2.20% |  99.51%  99.51%  88.89% 100.00%  ###### 100.00%
  6 StrippingD02MuMuClbrDstD02PiPiLine    0.65% |  29.27%  29.27%  33.33% 100.00%  29.41%  ######
#=================================================================================================

"""

__all__ = ( 'D02MuMuConf',
            'default_config' )

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StandardParticles import StdNoPIDsPions, StdLooseMuons

from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class D02MuMuConf(LineBuilder) :
    __configuration_keys__ = ('MinDauPT',           # 1.0 GeV
                              'MinDauIPCHI2',       # 1.0
                              'MinD0LT',            # 0.1 ps
                              'D02MuMuPrescale',    # 1.0
                              'DstD02MuMuPrescale', # 1.0
                              'D02PiPiPrescale',    # 0.1
                              'DstD02PiPiPrescale'  # 0.1
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        D0DaughtersCutsMuons = \
          "(TRCHI2DOF < 5) & (PT > %(MinDauPT)s * GeV) & (MIPCHI2DV() > %(MinDauIPCHI2)s)" % config
        D0DaughtersCutsPion = \
          "(TRCHI2DOF < 5) & (PT > %(MinDauPT)s * GeV) & (MIPCHI2DV() > %(MinDauIPCHI2)s)" % config
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
          "AM - AM1 < 165 * MeV"
        DstarMotherCut = \
          "(VFASPF(VCHI2) < 25) & " + \
          "((M - M1) < 160 * MeV)"

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
        D02MuMuSelection = Selection(name + "_D02MuMuSelection", Algorithm = CombineD0, RequiredSelections = [ StdLooseMuons ])
        DstD02MuMuSelection = Selection(name + "_DstD02MuMuSelection", Algorithm = CombineDst, RequiredSelections = [ StdLooseMuons, StdNoPIDsPions, D02MuMuSelection ])
        D02PiPiSelection = Selection(name + "_D02PiPiSelection", Algorithm = CombineD0, RequiredSelections = [ StdNoPIDsPions ])
        DstD02PiPiSelection = Selection(name + "_DstD02PiPiSelection", Algorithm = CombineDst, RequiredSelections = [ StdNoPIDsPions, D02PiPiSelection ])

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



default_config = {'MinDauPT'            : 1.0, # 1.0 GeV
                  'MinDauIPCHI2'        : 1.0,
                  'MinD0LT'             : 0.1, # ps
                  'D02MuMuPrescale'     : 1.0,
                  'DstD02MuMuPrescale'  : 1.0,
                  'D02PiPiPrescale'     : 0.1,
                  'DstD02PiPiPrescale'  : 0.1
                  }
