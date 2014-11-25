#Stripping Lines for Low Multiplicity Processes.
#Electroweak Group (Convenor: Tara Shears)
#Written by Will Barter

# Accepts events with few tracks. Used to study background.


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


confdict_ExclusiveDiMuon={
    'ExclusiveDiMuonPrescale'    : 1.0
    ,  'ExclusiveDiMuonPostscale'   : 1.0
    }

name = "ExclusiveDiMuon"

class ExclusiveDiMuonConf(LineBuilder) :

    __configuration_keys__ = ('ExclusiveDiMuonPrescale',
                              'ExclusiveDiMuonPostscale',
                              )

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name

        ExclusiveDiMuonNumTracksGEC = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) <= 5) & (recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) >= 1)",
                                       'Preambulo' : ["from LoKiTracks.decorators import *"]}



        self.ExclusiveDiMuon_line = StrippingLine(self._myname+"BackgroundLine",
                                                  prescale = config['ExclusiveDiMuonPrescale'],
                                                  postscale = config['ExclusiveDiMuonPostscale'],
                                                  checkPV = False,
                                                  HLT = "HLT_PASS('Hlt1MBNoBiasDecision')",
                                                  FILTER =  ExclusiveDiMuonNumTracksGEC
                                                  )

        self.registerLine(self.ExclusiveDiMuon_line)


