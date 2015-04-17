# -*- coding: utf-8 -*-
## @file
#  Set of Hlt2-lines for study of low multiplicity processes. Lines 
#  are organised according to L0 trigger dependence
#
#  @author Dan Johnson daniel.johnson@cern.ch
#=============================================================================
""" Set of Hlt2-lines for study of low multiplicity processes. Lines
are organised according to L0 trigger dependence

"""
#=============================================================================
__author__  = "Dan Johnson daniel.johnson@cern.ch"

from HadronLines import CEPHadronLines
from MuonLines import CEPMuonLines
from PhotonLines import CEPPhotonLines
from ElectronLines import CEPElectronLines
from GaudiKernel.SystemOfUnits import GeV, MeV, mm

_CEPHadronLines = CEPHadronLines();
_CEPMuonLines = CEPMuonLines();
_CEPPhotonLines = CEPPhotonLines();
_CEPElectronLines = CEPElectronLines();

theseSlots =      { 'Prescale' : { 'Hlt2LowMultL2pPi'       : 1.0
                                 , 'Hlt2LowMultL2pPiWS'     : 0.1
                                 , 'Hlt2LowMultD2KPi'       : 1.0
                                 , 'Hlt2LowMultD2KPiWS'     : 0.1
                                 , 'Hlt2LowMultD2KPiPi'     : 1.0
                                 , 'Hlt2LowMultD2KPiPiWS'   : 0.1
                                 , 'Hlt2LowMultD2KKPi'      : 1.0
                                 , 'Hlt2LowMultD2KKPiWS'    : 0.1
                                 , 'Hlt2LowMultD2K3Pi'      : 1.0
                                 , 'Hlt2LowMultD2K3PiWS'    : 1.0
                                 , 'Hlt2LowMultChiC2HH'     : 1.0
                                 , 'Hlt2LowMultChiC2HHWS'   : 0.1
                                 , 'Hlt2LowMultChiC2HHHH'   : 1.0
                                 , 'Hlt2LowMultChiC2HHHHWS' : 0.1
                                 , 'Hlt2LowMultChiC2PP'     : 1.0
                                 , 'Hlt2LowMultChiC2PPWS'   : 0.1
                                 , 'Hlt2LowMultLMR2HH'      : 0.05
                                 , 'Hlt2LowMultLMR2HHWS'    : 0.05
                                 , 'Hlt2LowMultLMR2HHHH'    : 0.05
                                 , 'Hlt2LowMultLMR2HHHHWS'  : 0.05
                                 , 'Hlt2LowMultLMR2HH_mediumPS'      : 0.05
                                 , 'Hlt2LowMultLMR2HHWS_mediumPS'    : 0.05
                                 , 'Hlt2LowMultLMR2HHHH_mediumPS'    : 0.05
                                 , 'Hlt2LowMultLMR2HHHHWS_mediumPS'  : 0.05
                                 , 'Hlt2LowMultLMR2HH_heavyPS'      : 0.05
                                 , 'Hlt2LowMultLMR2HHWS_heavyPS'    : 0.05
                                 , 'Hlt2LowMultLMR2HHHH_heavyPS'    : 0.05
                                 , 'Hlt2LowMultLMR2HHHHWS_heavyPS'  : 0.05
                                 , 'Hlt2LowMultHadron_noTrFilt' : 0.01
                                 # Muon lines
                                 , 'Hlt2LowMultDiMuon'      : 1.0
                                 , 'Hlt2LowMultDiMuon_PS'   : 1.0
                                 , 'Hlt2LowMultMuon'        : 0.1
                                 # Photon lines
                                 , 'Hlt2LowMultDiPhoton'    : 1.0
                                 , 'Hlt2LowMultDiPhoton_HighMass'    : 1.0
                                 , 'Hlt2LowMultPi0'         : 1.0
                                 # Electron lines
                                 , 'Hlt2LowMultDiElectron'    : 1.0
                                 , 'Hlt2LowMultDiElectron_noTrFilt': 0.05
                                 # Technical lines
                                 , 'Hlt2LowMultTechnical_NoBias'        : 0.003 # Aim for 1 Hz from 300 Hz non-bb crossings
                                 , 'Hlt2LowMultTechnical_MinBias'       : 0.1
                                 }
                    , 'HLT'               :   {"Hadron"     : "HLT_PASS_RE('Hlt1CEPDecision')",
                                               "Muon"       : "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')",
                                               "Photon"     : "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')",
                                               "Electron"   : "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')",
                                               "TechnicalNoBias"  : "HLT_PASS_RE('Hlt1LumiDecision')"
                                               }
                    , 'Common'    : {'H_PTmin'        :     100.0 * MeV,
                                     'H_Pmin'         :     5000.0 * MeV,
                                     'H_TrkChi2max'   :     3.0}
                    , 'L0Channels'         : {'Hadron'  : ['DiHadron,lowMult'],
                                              'Muon'    : ['Muon,lowMult','DiMuon,lowMult'],
                                              'Photon'  : ['Photon,lowMult','DiEM,lowMult'],
                                              'Electron': ['Electron,lowMult','DiEM,lowMult']
                                             }
                    , 'Technical_L0'  : {"NoBias"          : "(L0_DATA('Spd(Mult)') < 100)" ,
                                         "MinBias"         : "(L0_DATA('Spd(Mult)') < 100) & ( (L0_DATA('Spd(Mult)') > 2) | (L0_DATA('Electron(Et)') > 5) | (L0_DATA('Photon(Et)') > 5) | (L0_DATA('Hadron(Et)') > 10) )"}
                    , 'Technical_ODIN': {"ALL"             : "(~(ODIN_BXTYP == LHCb.ODIN.BeamCrossing))" }
} 
theseSlots.update(_CEPHadronLines.localcuts())
theseSlots.update(_CEPMuonLines.localcuts())
theseSlots.update(_CEPPhotonLines.localcuts())
theseSlots.update(_CEPElectronLines.localcuts())

from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
class CEPLines(Hlt2LinesConfigurableUser) :
    __slots__ = theseSlots
    def __l0du(self, nickname):
        import collections
        l0 = self.getProp( 'L0Channels' )[nickname]
        if isinstance(l0, collections.Iterable) and not isinstance(l0, basestring):
            return "|".join(["L0_CHANNEL('%s')" % chan for chan in l0])
        else:
            return l0
    
    def __apply_configuration__(self) :
        # Set up the standard lines
        lines = {'Hadron'     : _CEPHadronLines.locallines(),
                 'Muon'       : _CEPMuonLines.locallines(),
                 'Photon'     : _CEPPhotonLines.locallines(),
                 'Electron'   : _CEPElectronLines.locallines()
                }
        from HltLine.HltLine import Hlt2Line
        for l0nick, lns in lines.iteritems():
            for linename, algos in self.algorithms(lns).iteritems():
                  Hlt2Line(linename, prescale = self.prescale,
                           L0DU = self.__l0du(l0nick),
                           HLT1 = self.getProp('HLT')[l0nick],
                           algos = algos) 
        # Add three additional technical lines
        Hlt2Line("LowMultTechnical_NoBias", prescale = self.prescale,
                 HLT1 = self.getProp('HLT')["TechnicalNoBias"],
                 ODIN = self.getProp('Technical_ODIN')["ALL"],
                 L0DU = self.getProp('Technical_L0')["NoBias"])
        Hlt2Line("LowMultTechnical_MinBias", prescale = self.prescale,
                 HLT1 = self.getProp('HLT')["TechnicalNoBias"],
                 ODIN = self.getProp('Technical_ODIN')["ALL"],
                 L0DU = self.getProp('Technical_L0')["MinBias"])
        
