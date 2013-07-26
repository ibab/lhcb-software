##############################################################################
# File for running Brunel without SPD/PRS
##############################################################################

from Brunel.Configuration import *


from Configurables import LHCbApp, CondDB

CondDB().Upgrade = True

#if "Calo_NoSPDPRS" not in CondDB().AllLocalTagsByDataType:
#    CondDB().AllLocalTagsByDataType += ["Calo_NoSPDPRS"]
 
from Configurables import Brunel
for det in ["Spd", "Prs"]:
    if det in Brunel().Detectors:
        Brunel().Detectors.remove(det)

Brunel().DataType = "Upgrade"

