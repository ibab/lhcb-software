# Test, will fail to write a DST because the output mode is wrong
# Will fill some counters, and output the summary.xml
# These options are mostly copied from $GAUDIEXAMPLESROOT/python/CounterEx.py
from Gaudi.Configuration import *

importOptions("$XMLSUMMARYKERNELROOT/options/test-XMLSummary.py")

# Set up the Event Selector for a file which does not exist
EventSelector().Input   = ["DATAFILE='PFN:YouCannotOpenMe.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

