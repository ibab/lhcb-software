from Gaudi.Configuration import *
from GaudiConf import IOHelper
# input file
importOptions("$PRCONFIGOPTS/DaVinci/Stripping/Collision12-4TeV-Reco14-FULLDST.py")
# output file
IOHelper().outStream("RootDst.root","InputCopyStream")

