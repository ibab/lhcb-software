#Stripping Lines for Low Multiplicity Processes.
#Electroweak Group (Convenor: Tara Shears)
#Adaptation of lines (to use line builders) originally designed by Dermot Moran by Will Barter

# Accepts events that passed the relevant HLT line.


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


confdict_LowMult={
    'LowMultPrescale'    : 1.0 
    ,  'LowMultPostscale'   : 1.0
    }

name = "LowMult"

class LowMultConf(LineBuilder) :

    __configuration_keys__ = ('LowMultPrescale',
                              'LowMultPostscale'                           
                              )
    
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name


                      
        self.LowMult_line = StrippingLine(self._myname+"LowMult",
                                          prescale = config['LowMultPrescale'],
                                          postscale = config['LowMultPostscale'],
                                          checkPV = False,
                                          HLT = "HLT_PASS('Hlt2LowMultDecision')"
                                          )
        
        self.registerLine(self.LowMult_line)

       
        self.LowMultMuon_line = StrippingLine(self._myname+"LowMultMuon",
                                          prescale = config['LowMultPrescale'],
                                          postscale = config['LowMultPostscale'],
                                          checkPV = False,
                                          HLT = "HLT_PASS('Hlt2LowMultMuonDecision')"
                                          )
        
        self.registerLine(self.LowMultMuon_line)


        self.LowMultElectron_line = StrippingLine(self._myname+"LowMultElectron",
                                          prescale = config['LowMultPrescale'],
                                          postscale = config['LowMultPostscale'],
                                          checkPV = False,
                                          HLT = "HLT_PASS('Hlt2LowMultElectronDecision')"
                                          )
        
        self.registerLine(self.LowMultElectron_line)


        self.LowMultHadron_line = StrippingLine(self._myname+"LowMultHadron",
                                          prescale = config['LowMultPrescale'],
                                          postscale = config['LowMultPostscale'],
                                          checkPV = False,
                                          HLT = "HLT_PASS('Hlt2LowMultHadronDecision')"
                                          )
        
        self.registerLine(self.LowMultHadron_line)


        self.LowMultPhoton_line = StrippingLine(self._myname+"LowMultPhoton",
                                          prescale = config['LowMultPrescale'],
                                          postscale = config['LowMultPostscale'],
                                          checkPV = False,
                                          HLT = "HLT_PASS('Hlt2LowMultPhotonDecision')"
                                          )
        
        self.registerLine(self.LowMultPhoton_line)


        self.LowMultPP2PPMuMu_line = StrippingLine(self._myname+"LowMultPP2PPMuMu",
                                          prescale = config['LowMultPrescale'],
                                          postscale = config['LowMultPostscale'],
                                          checkPV = False,
                                          HLT = "HLT_PASS('Hlt2diPhotonDiMuonDecision')"
                                          )
        
        self.registerLine(self.LowMultPP2PPMuMu_line)
        
      
