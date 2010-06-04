from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/earlyData.py")

from Gaudi.Configuration import EventSelector
from Configurables import Brunel

Brunel().DataType = "2010"

# file of selected large events
EventSelector().Input = [
  "DATAFILE='root:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/testFiles/run_69669_large_2ev.mdf?svcClass=default' SVC='LHCb::MDFSelector'"
, "DATAFILE='root:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/testFiles/run_70122_large_1ev.mdf?svcClass=default' SVC='LHCb::MDFSelector'"
, "DATAFILE='root:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/testFiles/bug67364.mdf?svcClass=default' SVC='LHCb::MDFSelector'"
    ]
