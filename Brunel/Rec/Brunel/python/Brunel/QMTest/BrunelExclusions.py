from GaudiTest import LineSkipper
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
  LineSkipper(["MagneticFieldSvc     INFO Map scaled by factor"]) + \
  LineSkipper(["MagneticFieldSvc     INFO Opened magnetic field file"]) + \
  LineSkipper(["ToolSvc.PatTTMa...   INFO  No B field detected."]) + \
  LineSkipper(["ConfigTarFileAc...   INFO  opening "]) + \
  LineSkipper(["Gas       INFO Refractive index update triggered : Pressure ="]) + \
  LineSkipper(["LHCBCOND_"]) + \
  LineSkipper(["SIMCOND_"]) + \
  LineSkipper(["DDDB_"]) + \
  LineSkipper(["INFO MuonIDAlg v"]) + \
  LineSkipper(["Memory for the event exceeds 3*sigma"]) + \
  LineSkipper(["Mean 'delta-memory' exceeds 3*sigma"]) + \
  LineSkipper(["BrunelInit.Brun...SUCCESS Exceptions/Errors/Warnings/Infos Statistics : 0/0/"]) + \
  LineSkipper(["AfterMagnetRegion/"]) + \
  LineSkipper(["MagnetRegion/"]) +\
  LineSkipper(["BeforeMagnetRegion/"]) +\
  LineSkipper(["DownstreamRegion"]) +\
  LineSkipper(["MD5 sum:"])
