from GaudiTest import LineSkipper
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
  LineSkipper(["FileRecordCnvSvc     INFO File sharing enabled. Do not retire files"]) + \
  LineSkipper(["DEBUG Releasing service 'UpdateManagerSvc'"]) + \
  LineSkipper(["LHCBCOND_"]) + \
  LineSkipper(["DDDB_"])
