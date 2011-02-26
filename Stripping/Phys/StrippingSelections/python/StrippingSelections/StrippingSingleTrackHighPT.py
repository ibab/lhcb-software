#Stripping Lines for Study of Background to W->MuNu.
#Electroweak Group (Convenor: Tara Shears)
#Line designed by Simone Bifani (written by William Barter)

# Accepts events that passed the relevant HLT line. Mainly interested in high PT (now also a lower PT version)


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


confdict_SingleTrackHighPT={
    'SingleTrackHighPTPrescale'    : .1 
    ,  'SingleTrackHighPTPostscale'   : 1.0
    ,  'SingleTrackLowPTPrescale'    : .05 
    ,  'SingleTrackLowPTPostscale'   : 1.0
    }

name = "SingleTrackHighPT"

class SingleTrackHighPTConf(LineBuilder) :

    __configuration_keys__ = ('SingleTrackHighPTPrescale',
                              'SingleTrackHighPTPostscale',
                              'SingleTrackLowPTPrescale',
                              'SingleTrackLowPTPostscale' 
                              )
    
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name

                      
        self.SingleTrackHighPT_line = StrippingLine(self._myname+"SingleTrackHlt1TisHighPT",
                                          prescale = config['SingleTrackHighPTPrescale'],
                                          postscale = config['SingleTrackHighPTPostscale'],
                                          checkPV = False,
                                          HLT = "HLT_PASS('Hlt2SingleTrackHighPTDecision')"
                                          )
        
        self.registerLine(self.SingleTrackHighPT_line)

       
      
        self.SingleTrackLowPT_line = StrippingLine(self._myname+"SingleTrackHlt1TisLowPT",
                                          prescale = config['SingleTrackLowPTPrescale'],
                                          postscale = config['SingleTrackLowPTPostscale'],
                                          checkPV = False,
                                          HLT = "HLT_PASS('Hlt2SingleTrackLowPTDecision')"
                                          )
        
        self.registerLine(self.SingleTrackLowPT_line)
