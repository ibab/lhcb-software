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
        from Configurables import HltIncidentGenerator, Hlt2SelReportsMaker
        Hlt2SelReportsMaker().DebugIncident = 'RequestDebugEvent'
        return HltIncidentGenerator('Hlt2DebugEventDecision',
                                    Incident = Hlt2SelReportsMaker().DebugIncident)
