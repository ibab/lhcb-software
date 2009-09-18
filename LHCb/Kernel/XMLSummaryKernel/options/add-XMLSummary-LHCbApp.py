# Minimum option to add the XMLSummary to your job
from Gaudi.Configuration import *

from Configurables import LHCbApp

LHCbApp().XMLSummary="summary.xml"
