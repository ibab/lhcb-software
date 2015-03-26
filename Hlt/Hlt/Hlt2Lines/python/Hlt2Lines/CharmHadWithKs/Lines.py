from D2HHKsLines import CharmHadD2HHKsLines
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

KsLines = CharmHadD2HHKsLines()

class CharmHadWithKsLines(Hlt2LinesConfigurableUser):
    __slots__ = KsLines.localcuts()

    def __apply_configuration__(self):
        stages = KsLines.locallines()

        from HltLine.HltLine import Hlt2Line
        for nickname, algos in self.algorithms(stages).iteritems():
            Hlt2Line(nickname, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
