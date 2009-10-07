# Test, will fail to write a DST because the output mode is wrong
# Will fill some counters, and output the summary.xml
# These options are mostly copied from $GAUDIEXAMPLESROOT/python/CounterEx.py
from Gaudi.Configuration import *

importOptions("$XMLSUMMARYKERNELROOT/options/test-XMLSummary.py")

# Set up the OutputStream that fails to copy an event data file
FailCopyStream=InputCopyStream()
FailCopyStream.Output = "DATAFILE='PFN:PoolDst.root' TYP='POOL_ROOTTREE' OPT='READ'";

