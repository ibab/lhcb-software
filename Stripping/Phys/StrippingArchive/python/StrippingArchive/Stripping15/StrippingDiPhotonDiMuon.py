#Stripping Lines for Exclusive Elastic DiMuon Processes.
#Electroweak Group (Convenor: Tara Shears)
#Adaptation of lines (to use line builders) originally written by James Keaveneyand Dermot Moran by Will Barter

# Accepts events that passed the relevant L0 trigger.


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


confdict_PP2PPMuMu={
    'PP2PPMuMuLinePrescale'    : 1.0 
    ,  'PP2PPMuMuLinePostscale'   : 1.0
    }

name = "PP2PPMuMu"

class PP2PPMuMuConf(LineBuilder) :

    __configuration_keys__ = ('PP2PPMuMuLinePrescale',
                              'PP2PPMuMuLinePostscale'                           
                              )
    
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name


                      
        self.PP2PPMuMu_line = StrippingLine(self._myname+"NoPVLine",
                                            prescale = config['PP2PPMuMuLinePrescale'],
                                            postscale = config['PP2PPMuMuLinePostscale'],
                                            checkPV = False,
                                            HLT = "HLT_PASS('Hlt2DiMuonNoPVDecision')"
                                            )
        
        self.registerLine(self.PP2PPMuMu_line)
       



