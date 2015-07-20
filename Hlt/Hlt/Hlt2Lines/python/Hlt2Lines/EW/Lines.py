#!/usr/bin/env python
# -*- coding: utf-8 -*- 
#======================================================================================
""" Owned by the QEE group, this is a set of Hlt2Lines for a range of analysis. All of
these are either single or di- lepton lines, enabling a very varied physics
program ranging from testing the SM parameters to searches for exotic particles """

__version__ = "$Revision: $"
__author__  = "Karlis Dreimanis k.dreimanis@liverpool.ac.uk"
__date__    = "2015-07-20"
#======================================================================================

from GaudiKernel.SystemOfUnits import GeV, MeV, mm 
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
class EWLines(Hlt2LinesConfigurableUser) :

    __slots__ = {'Prescale' : {'Hlt2EWDiMuonZ'                : 1.000,
                               'Hlt2EWDiMuonDY1'              :  0.02,
                               'Hlt2EWDiMuonDY2'              :   0.3,
                               'Hlt2EWDiMuonDY3'              : 1.000,
                               'Hlt2EWDiMuonDY4'              : 1.000,
                               'Hlt2EWSingleMuonLowPt'        : 0.002,
                               'Hlt2EWSingleMuonHighPt'       :	  0.1,
                               'Hlt2EWSingleMuonVHighPt'      : 1.000,
                               'Hlt2EWDiElectronDY'           : 1.000,
                               'Hlt2EWDiElectronHighMass'     : 1.000,
                               'Hlt2EWSingleElectron'         : 1.000,
                               'Hlt2EWSingleElectronLowPt'    : 0.001,
                               'Hlt2EWSingleElectronHighPt'   :  0.01,
                               'Hlt2EWSingleElectronVHighPt'  : 1.000,
                               'Hlt2EWSingleTauHighPt2Prong'  :   0.2,
                               'Hlt2EWSingleTauHighPt3Prong'  :   0.5},

                 'Common' :                  {},
                 #=====DiMuon=====
                 'DiMuonZ' :                 {'MinMass'        : 40000 * MeV,
                                              'Pt'             :     0 * MeV},
                 
                 'DiMuonDY1' :               {'MinMass'        :  2500 * MeV,
                                              'MinExcMass'     :  3000 * MeV,
                                              'MaxExcMass'     :  3200 * MeV,
                                              'MuPt'           :   800 * MeV,
                                              'Pt'             :     0 * MeV,
                                              'TkChi2'         :    10},
                 
                 'DiMuonDY2' :               {'MinMass'        :  5000 * MeV,
                                              'MuPt'           :  1000 * MeV,
                                              'Pt'             :     0 * MeV,
                                              'TkChi2'         :    10},
                 
                 'DiMuonDY3' :               {'MinMass'        : 10000 * MeV,
                                              'MuPt'           :     0 * MeV,
                                              'TkChi2'         :    10},
                 
                 'DiMuonDY4' :               {'MinMass'        : 20000 * MeV,
                                              'MuPt'           :     0 * MeV,
                                              'TkChi2'         :    10},
                 #=====SingleMuon=====
                 'SingleMuonLowPt' :         {'Pt'             :  4800 * MeV,
                                              'TkChi2'         :    10},

                 'SingleMuonHighPt' :        {'Pt'             : 10000 * MeV},
                 
                 'SingleMuonVHighPt' :       {'Pt'             : 12500 * MeV},
                 #=====DiElectron=====
                 'DiElectronDY' :            {'L0Req'          : "L0_CHANNEL('Electron')",
                                              'Hlt1Req'        : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
                                              'MinMass'        : 10000 * MeV,
                                              'MaxMass'        : 1e+10 * MeV,
                                              'VtxChi2'        :    25,
                                              'Pt'             :  -999 * MeV,
                                              'ElecPt'         :  1000 * MeV,
                                              'ElecPIDe'       :   1.5,
                                              'ElecTkChi2'     :    10,
                                              'PrsMin'         :    50,
                                              'EcalMin'        :   0.1,
                                              'HcalMax'        :  0.05},

                 'DiElectronHighMass' :      {'L0Req'          : "L0_CHANNEL('Electron')",
                                              'Hlt1Req'        : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
                                              'MinMass'        : 20000 * MeV,
                                              'VtxChi2'        :    25,
                                              'TkChi2'         :    10,
                                              'Pt'             :  -999 * MeV,
                                              'ElecPt'         : 10000 * MeV,
                                              'PrsMin'         :    50,
                                              'EcalMin'        :   0.1,
                                              'HcalMax'        :  0.05},
                 #=====SingleElectron=====
                 'SingleElectron' :          {'L0Req'          : "L0_CHANNEL('Electron')",
                                              'Hlt1Req'        : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
                                              'Pt'             : 10000 * MeV,
                                              'IP'             :  0.05 * mm,
                                              'PIDe'           :     4,  #unused?
                                              'IPChi2'         :    -1,
                                              'TkChi2'         :     5,
                                              'PrsMin'         :    50,
                                              'EcalMin'        :   0.1,
                                              'HcalMax'        :  0.05},
                 
                 'SingleElectronLowPt' :     {'L0Req'          : "L0_CHANNEL('Electron')",
                                              'Hlt1Req'        : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
                                              'Pt'             :  4800 * MeV,
                                              'PIDe'           :     4,  #unused?
                                              'TkChi2'         :     5,
                                              'PrsMin'         :    50,
                                              'EcalMin'        :   0.1,
                                              'HcalMax'        :  0.05},

                 'SingleElectronHighPt'   :  {'L0Req'          : "L0_CHANNEL('Electron')",
                                              'Hlt1Req'        : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
                                              'Pt'             : 10000 * MeV,
                                              'PrsMin'         :    50,
                                              'EcalMin'        :   0.1,
                                              'HcalMax'        :  0.05,
                                              'TkChi2'         :    20},
                 
                 'SingleElectronVHighPt'   : {'L0Req'          : "L0_CHANNEL('Electron')",
                                              'Hlt1Req'        : "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
                                              'Pt'             : 15000 * MeV,
                                              'PrsMin'         :    50,
                                              'EcalMin'        :   0.1,
                                              'HcalMax'        :  0.05,
                                              'TkChi2'         :    20},
                 ##tau makers
                 'TauTrkFilter'   :          {'trk_PT'          :    2*GeV,
                                              'trk_TRCHI2DOF_MAX' : 3
                                              },
                 'TauRhoCombiner' :          {'RHO_M_MIN'       : 0*GeV,
                                              'RHO_M_MAX'       : 2*GeV,
                                              'RHO_PT_MIN'      : 0*GeV,
                                              'PI_PT_MIN'       : 2*GeV
                                              },
                 'Tau3PiCombiner' :          {'sumPT'          :     8 * GeV,
                                              'PT'             :     8 * GeV,
                                              'childPT'        :     2 * GeV,
                                              'maxchildPT'     :     5 * GeV,
                                              'FDCHI2_MIN'     :    10,
                                              'FDT_MIN'        :   0.5,
                                              'VCHI2_NDOF_MAX' :    20,
                                              'DOCA_MAX'       :   0.1,
                                              'CORRM_MIN'      :   1.2 * GeV,
                                              'CORRM_MAX'      :   2.0 * GeV},

                 'TauPiPi0Combiner' :        {'sumPT'          :     8 * GeV,
                                              'PT'             :     8 * GeV,
                                              'childPT'        :     5 * GeV,
                                              'maxchildPT'     :     5 * GeV,
                                              'M_MIN'          :     0 * GeV,
                                              'M_MAX'          :   1.5 * GeV},
                 ## single tau
                 'SingleTauHighPt2Prong' :   {'PT'             :    15 * GeV},
                 'SingleTauHighPt3Prong' :   {'PT'             :    12 * GeV}

                 }                               

    def __apply_configuration__(self) :
        from Stages import (DiMuonZFilter,DiMuonDY1Filter,DiMuonDY2Filter,DiMuonDY3Filter,DiMuonDY4Filter,
                            SingleMuonLowPtFilter,SingleMuonHighPtFilter,SingleMuonVHighPtFilter,
                            DiElectronDYFilter,DiElectronHighMassFilter,
                            SingleElectronFilter,SingleElectronLowPtFilter,SingleElectronHighPtFilter,SingleElectronVHighPtFilter, TauTrkFilter, TauRhoCombiner, HighPTTauCombiner, SingleTauFilter)
        from Inputs import (MergedPi0s, ResolvedPi0s, BiKalmanFittedMuons, BiKalmanFittedElectrons, BiKalmanFittedPions)
        #create the taus first
        tau_trkfilter     = TauTrkFilter("TauTrkFilter", [BiKalmanFittedPions])
        tau_rhocombiner   = TauRhoCombiner("TauRhoCombiner", [tau_trkfilter])
        taus_3prong       = HighPTTauCombiner("Tau3PiCombiner",0, [tau_rhocombiner, tau_trkfilter])
        taus_2prong       = HighPTTauCombiner("TauPiPi0Combiner",1, [tau_trkfilter, MergedPi0s, ResolvedPi0s])
        singletaus_3prong = SingleTauFilter("SingleTauHighPt3Prong",0, [taus_3prong])
        singletaus_2prong = SingleTauFilter("SingleTauHighPt2Prong",1, [taus_2prong])

        stages = {'DiMuonZ'                 : [DiMuonZFilter('DiMuonZ')],
                  'DiMuonDY1'               : [DiMuonDY1Filter('DiMuonDY1')],
                  'DiMuonDY2'               : [DiMuonDY2Filter('DiMuonDY2')],
                  'DiMuonDY3'               : [DiMuonDY3Filter('DiMuonDY3')],
                  'DiMuonDY4'               : [DiMuonDY4Filter('DiMuonDY4')],
                  'SingleMuonLowPt'         : [SingleMuonLowPtFilter('SingleMuonLowPt')],
                  'SingleMuonHighPt'        : [SingleMuonHighPtFilter('SingleMuonHighPt')],
                  'SingleMuonVHighPt'       : [SingleMuonVHighPtFilter('SingleMuonVHighPt')],
                  'DiElectronDY'            : [DiElectronDYFilter('DiElectronDY')],
                  'DiElectronHighMass'      : [DiElectronHighMassFilter('DiElectronHighMass')],
                  'SingleElectron'          : [SingleElectronFilter('SingleElectron')],
                  'SingleElectronLowPt'     : [SingleElectronLowPtFilter('SingleElectronLowPt')],
                  'SingleElectronHighPt'    : [SingleElectronHighPtFilter('SingleElectronHighPt')],
                  'SingleElectronVHighPt'   : [SingleElectronVHighPtFilter('SingleElectronVHighPt')],
                  'SingleTauHighPt3Prong'   : [singletaus_3prong],
                  'SingleTauHighPt2Prong'   : [singletaus_2prong]}
        
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltDecodeRaw import DecodeL0CALO
        for (nickname, algos) in self.algorithms(stages).iteritems():
            cuts = self.getProp(nickname)
            linename = 'EW' + nickname
            Hlt2Line(linename, prescale = self.prescale,L0DU = cuts.get('L0Req', None), HLT1 = cuts.get('Hlt1Req', None), algos = algos, postscale = self.postscale)
