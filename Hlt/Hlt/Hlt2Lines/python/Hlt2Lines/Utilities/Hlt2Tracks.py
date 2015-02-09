from Hlt2Stage import Hlt2Stage
class Hlt2Tracks(Hlt2Stage):
    def __init__(self, name = "Hlt2Tracks"):
        Hlt2Stage.__init__(self, name, inputs = [])

    def clone(self, name):
        return Hlt2Tracks(name)

    def stage(self, cuts):
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking as Hlt2LongTracking
        return Hlt2LongTracking().hlt2PrepareTracks()
