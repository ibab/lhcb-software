# Test, will read and write a Dst file
# Will fill some counters, and output the summary.xml
# These options are mostly copied from $IOEXAMPLEROOT/tests/options/WriteDst.opts
from Gaudi.Configuration import *

from Configurables import EventCountAlg, LHCbApp
from Configurables import XMLSummarySvc
from XMLSummaryKernel.Configuration import XMLSummary
#from Configurables import LHCbApp

#---------------------------------------------------------------------------
# Setup read and write of a ROOT dst file
#---------------------------------------------------------------------------
ApplicationMgr().HistogramPersistency = "NONE";
ApplicationMgr().EvtMax = 5;
EventSelector().PrintFreq = 1;

# Set up persistency
LHCbApp()

#Define the input file and output file to copy
importOptions( "$XMLSUMMARYKERNELROOT/options/RootDst.py")

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

