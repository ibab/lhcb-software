from GaudiTest import LineSkipper
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
  LineSkipper(["MagneticFieldSvc     INFO Map scaled by factor"]) + \
  LineSkipper(["MagneticFieldSvc     INFO Opened magnetic field file"]) + \
  LineSkipper(["INFO MuonIDAlg v"]) + \
  LineSkipper(["Delta Memory for the event exceeds 3*sigma"]) + \
  LineSkipper(["AfterMagnetRegion/"]) + \
  LineSkipper(["MagnetRegion/"]) +\
  LineSkipper(["BeforeMagnetRegion/"]) +\
  LineSkipper(["DownstreamRegion"]) +\
  LineSkipper(["MD5 sum:"])
