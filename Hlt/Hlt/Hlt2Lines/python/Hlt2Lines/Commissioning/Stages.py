# Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Stage import Hlt2Stage

# The GEC
class CopyTracks(Hlt2Stage):
    def __init__(self):
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        self.__tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()
        super(CopyTracks, self).__init__("CopyTracks", [], [self.__tracks])
        
    def stage(self, stages, cuts):
        stages += self.dependencies(cuts)
        from Configurables import HltCopySelection_LHCb__Track_ as HltCopyTrackSelection
        return HltCopyTrackSelection('Hlt2ForwardDecision',
                                     InputSelection = 'TES:' + self.__tracks.outputSelection())

class IncidentGenerator(Hlt2Stage):
    def __init__(self):
        super(IncidentGenerator, self).__init__("Generator", [])

    def stage(self, stages, cuts):
        from Configurables import HltIncidentGenerator, HltSelReportsMaker
        HltSelReportsMaker().DebugIncident = 'RequestDebugEvent'
        return HltIncidentGenerator('Hlt2DebugEventDecision',
                                    Incident = HltSelReportsMaker().DebugIncident)

class ErrorCounter(Hlt2Stage):
    def __init__(self):
        super(ErrorCounter, self).__init__("Counter", [])

    def stage(self, stages, cuts):
        from DAQSys.Decoders import DecoderDB
        from Configurables import LoKi__HDRFilter as HDRFilter
        decoder = DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"]
        return HDRFilter('Hlt2ErrorEventHlt2Filter',
                         Code = "HLT_COUNT_ERRORBITS_RE('^Hlt2.*',0xffff) > 0",
                         Location = decoder.listOutputs()[0])
