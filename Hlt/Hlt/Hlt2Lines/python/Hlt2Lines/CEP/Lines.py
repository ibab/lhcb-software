# -*- coding: utf-8 -*-
## @file
#  Set of Hlt2-lines for study of low multiplicity processes. Lines 
#  are organised according to L0 trigger dependence
#
#  @author Dan Johnson daniel.johnson@cern.ch
=============================================================================
""" Set of Hlt2-lines for study of low multiplicity processes. Lines
are organised according to L0 trigger dependence

"""
=============================================================================
__author__  = "Dan Johnson daniel.johnson@cern.ch"

from CEPHadronLines import CEPHadronLines
from CEPMuonLines import CEPMuonLines
from CEPPhotonLines import CEPPhotonLines
from CEPElectronLines import CEPElectronLines
from GaudiKernel.SystemOfUnits import GeV, MeV, mm

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
                                 , 'Hlt2MinKinBiasHadron'   : 0.01
                                 # Muon lines
                                 , 'Hlt2LowMultDiMuon'      : 1.0
                                 , 'Hlt2LowMultMuon'        : 0.1
                                 # Photon lines
                                 , 'Hlt2LowMultPhoton'      : 1.0
                                 # Electron lines
                                 , 'Hlt2LowMultElectron'    : 1.0
                                 , 'Hlt2LowMultMinKinBiasElectron': 0.05
                                 }
                  , 'Postscale' : {   'Hlt2LowMultL2pPi'       : 1.0
                                    , 'Hlt2LowMultL2pPiWS'     : 1.0
                                    , 'Hlt2LowMultD2KPi'       : 1.0
                                    , 'Hlt2LowMultD2KPiWS'     : 1.0
                                    , 'Hlt2LowMultD2KPiPi'     : 1.0
                                    , 'Hlt2LowMultD2KPiPiWS'   : 1.0
                                    , 'Hlt2LowMultD2KKPi'      : 1.0
                                    , 'Hlt2LowMultD2KKPiWS'    : 1.0
                                    , 'Hlt2LowMultD2K3Pi'      : 1.0
                                    , 'Hlt2LowMultD2K3PiWS'    : 1.0
                                    , 'Hlt2LowMultChiC2HH'     : 1.0
                                    , 'Hlt2LowMultChiC2HHWS'   : 1.0
                                    , 'Hlt2LowMultChiC2HHHH'   : 1.0
                                    , 'Hlt2LowMultChiC2HHHHWS' : 1.0
                                    , 'Hlt2LowMultChiC2PP'     : 1.0
                                    , 'Hlt2LowMultChiC2PPWS'   : 1.0
                                    , 'Hlt2LowMultLMR2HH'      : 1.0
                                    , 'Hlt2LowMultLMR2HHWS'    : 1.0
                                    , 'Hlt2LowMultLMR2HHHH'    : 1.0
                                    , 'Hlt2LowMultLMR2HHHHWS'  : 1.0
                                    , 'Hlt2MinKinBiasHadron'   : 1.0
                                    # Muon lines
                                    , 'Hlt2LowMultDiMuon'      : 1.0
                                    , 'Hlt2LowMultMuon'        : 1.0
                                    # Photon lines
                                    , 'Hlt2LowMultPhoton'      : 1.0
                                    # Electron lines
                                    , 'Hlt2LowMultElectron'    : 1.0
                                    , 'Hlt2LowMultMinKinBiasElectron': 1.0
                                    }
                    , 'HLT'       : "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')"
                    , 'Common'    : {'nBackTracksmax' :     1,
                                     'H_PTmin'        :     100.0 * MeV,
                                     'H_Pmin'         :     5000.0 * MeV,
                                     'H_TrkChi2max'   :     3.0,
                                     'K_PIDKmin'      :     0.0,
                                     'P_PIDpmin'      :     0.0}                
                   ,   'L0Channels'        : {'Hadron'  : ['DiHadron,lowMult'],
                                              'Muon'    : ['Muon,lowMult','DiMuon,lowMult'],
                                              'Photon'  : ['Photon,lowMult','DiEM,lowMult'],
                                              'Electron': ['Electron,lowMult','DiEM,lowMult']
                   }
} 
theseSlots.update(CEPHadronLines.localcuts())
theseSlots.update(CEPMuonLines.localcuts())
theseSlots.update(CEPPhotonLines.localcuts())
theseSlots.update(CEPElectronLines.localcuts())

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
    
    lines = {'Hadron'  : CEPHadronLines.locallines(),
             'Muon'    : CEPMuonLines.locallines(),
             'Photon'  : CEPPhotonLines.locallines(),
             'Electron': CEPElectronLines.locallines()
            }
    
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        for l0nick, lns in lines.iteritems():
            for linename, algos in self.algorithms(lns).iteritems():
                Hlt2Line(linename, prescale = self.prescale,
                         L0DU = self.__l0du(l0nick),
                         HLT1 = self.getProp('HLT'),
                         algos = algos, postscale = self.postscale) 
        
