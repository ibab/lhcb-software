from D2HHHLines import CharmHadD2HHHLines
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadLines(Hlt2LinesConfigurableUser):
    def __apply_configuration__(self):
        CharmHadD2HHHLines()
