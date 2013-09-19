from GaudiTest import LineSkipper, RegexpReplacer
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
  LineSkipper(["IODataManager                   ERROR Referring to existing dataset"]) + \
  LineSkipper(["EventSelector                    INFO Stream:EventSelector.DataStreamTool"]) + \
  LineSkipper(["MagneticFieldSvc                 INFO Map scaled by factor"]) + \
  LineSkipper(["MagneticFieldSvc                 INFO Opened magnetic field file"]) + \
  LineSkipper(["ConfigTarFileAccessSvc           INFO  opening "]) + \
  LineSkipper(["INFO MuonIDAlg v"]) + \
  LineSkipper(["Memory for the event exceeds 3*sigma"]) + \
  LineSkipper(["Mean 'delta-memory' exceeds 3*sigma"]) + \
  LineSkipper(["AfterMagnetRegion/"]) + \
  LineSkipper(["MagnetRegion/"]) + \
  LineSkipper(["BeforeMagnetRegion/"]) + \
  LineSkipper(["DownstreamRegion"]) + \
  LineSkipper(["DaVinciInitAlg.DaVinciMemory"]) + \
  LineSkipper(["Trying to import module StrippingArchive.Stripping"]) + \
  LineSkipper(["MD5 sum:"]) + \
  LineSkipper(["LHCBCOND_"]) + \
  LineSkipper(["SIMCOND_"]) + \
  LineSkipper(["DDDB_"])
