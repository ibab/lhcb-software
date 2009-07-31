from GaudiTest import LineSkipper
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
               LineSkipper(["--- Reader", "--- CFMlpANN", "INFO MuonIDAlg v" ])
