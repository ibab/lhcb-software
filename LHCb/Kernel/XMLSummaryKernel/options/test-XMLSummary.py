# Test, will read and write a Dst file
# Will fill some counters, and output the summary.xml
from Gaudi.Configuration import *

from Configurables import GaudiSequencer, EventCountAlg
from Configurables import XMLSummarySvc
from Configurables import LHCbApp

#Run this other example, which reads/copies a DST
importOptions("$IOEXAMPLEROOT/tests/options/WriteDst.opts")

xmlsummary=XMLSummarySvc("CounterSummarySvc")
xmlsummary.OutputLevel=1

EvtAlg=EventCountAlg("TestCounter")

ApplicationMgr().TopAlg+=[EvtAlg]


LHCbApp().XMLSummary="summary.xml"
LHCbApp().DataType="DC06"
LHCbApp().Simulation=True
