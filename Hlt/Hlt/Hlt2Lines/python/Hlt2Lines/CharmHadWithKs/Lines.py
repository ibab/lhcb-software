from D2HHKsLines import CharmHadD2HHKsLines
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadWithKsLines(Hlt2LinesConfigurableUser):
    def __apply_configuration__(self):
        CharmHadD2HHKsLines()
