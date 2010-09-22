# Test, will read and write a Dst file
# Will fill some counters, and output the summary.xml
# These options are mostly copied from $IOEXAMPLEROOT/tests/options/WriteDst.opts
from Gaudi.Configuration import *

from Configurables import EventCountAlg
from Configurables import XMLSummarySvc
from XMLSummaryKernel.Configuration import XMLSummary
#from Configurables import LHCbApp

#---------------------------------------------------------------------------
# Setup read and write of a POOL dst file
#---------------------------------------------------------------------------
ApplicationMgr().HistogramPersistency = "NONE";
ApplicationMgr().EvtMax = 5;
EventSelector().PrintFreq = 1;

# Set up the OutputStream that copies an event data file
ApplicationMgr().OutStream += ["InputCopyStream/InputCopyStream"]

#ApplicationMgr().OutStream += ["InputCopyStream/FailCopyStream"]
#FailCopyStream=InputCopyStream('FailCopyStream')
#FailCopyStream.Output = "DATAFILE='PFN:PoolDst.root' TYP='POOL_ROOTTREE' OPT='READ'";

# Set up POOL persistency
importOptions( "$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts" )

#Define the input file
importOptions( "$XMLSUMMARYKERNELROOT/options/PoolDst.py")

#---------------------------------------------------------------------------
# Add a counting algorithm
#---------------------------------------------------------------------------

EvtAlg=EventCountAlg("TestCounter")

ApplicationMgr().TopAlg+=[EvtAlg]

#---------------------------------------------------------------------------
# Setup XMLSummarySvc
#---------------------------------------------------------------------------

xmlsummary=XMLSummarySvc("CounterSummarySvc")
xmlsummary.OutputLevel=1
xmlsummary.UpdateFreq=1

XMLSummary().XMLSummary="summary.xml"
#LHCbApp().DataType="DC06"
#LHCbApp().Simulation=True
