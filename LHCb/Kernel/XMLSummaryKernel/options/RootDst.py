from Gaudi.Configuration import *
from GaudiConf import IOHelper
IOHelper().inputFiles( [
  'PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/Brunel/v34r5/00001820_00000001.dst?svcClass=default'
], clear=True)

IOHelper().outStream("RootDst.root","InputCopyStream")

