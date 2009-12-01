from GaudiTest import LineSkipper
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
  LineSkipper(["INFO MuonIDAlg v"]) + \
  LineSkipper(["Delta Memory for the event exceeds 3*sigma"]) + \
  LineSkipper(["AfterMagnetRegion/"]) + \
  LineSkipper(["MagnetRegion/"]) +\
  LineSkipper(["BeforeMagnetRegion/"]) +\
  LineSkipper(["DownstreamRegion"])
