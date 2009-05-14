##############################################################################
#
# RICH PID Monitoring from data
#
from Configurables import RichPIDQCConf, DaVinci, GaudiSequencer
richSeq = GaudiSequencer("RichPIDMoniSeq")
RichPIDQCConf().setProp("CalibSequencer",richSeq)
#RichPIDQCConf().MCChecks = True                  # Enable MC checking as well
#RichPIDQCConf().PIDCalibrations = [ "KshortPiPi" ]  # The PID Calibration selections to run (default is all)
DaVinci().MoniSequence += [ richSeq ] # RICH sequence
