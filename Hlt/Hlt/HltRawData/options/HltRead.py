from Gaudi.Configuration import *
from Configurables import GaudiSequencer


# Add a sequence
HltReadSeq = GaudiSequencer("HltReadSeq")
HltReadSeq.MeasureTime = TRUE
ApplicationMgr().TopAlg += [ HltReadSeq ]

from Configurables import HltDecReportsReader,HltSelReportsReader,HltVertexReportsReader
HltDecReportsReader = HltDecReportsReader()
HltSelReportsReader = HltSelReportsReader()
HltVertexReportsReader = HltVertexReportsReader()
HltReadSeq.Members += [ HltDecReportsReader , HltSelReportsReader ,HltVertexReportsReader ]

HltDecReportsReader.OutputLevel = 3
HltSelReportsReader.OutputLevel = 3
HltVertexReportsReader.OutputLevel = 3


# redirect TES output to non-default location if run HltRead in the same job as HltWrite
# HltDecReportsReader.OutputHltDecReportsLocation = "Hlt/FromRawEvent/DecReports"
# HltSelReportsReader.OutputHltSelReportsLocation = "Hlt/FromRawEvent/SelReports"
# HltVertexReportsReader.OutputHltVertexReportsLocation = "Hlt/FromRawEvent/VertexReports"

