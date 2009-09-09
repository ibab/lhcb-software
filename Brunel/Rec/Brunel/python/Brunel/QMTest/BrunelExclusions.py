from GaudiTest import LineSkipper
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
  LineSkipper(["--- Reader", "--- CFMlpANN", "INFO MuonIDAlg v" ]) + \
  LineSkipper(["AfterMagnetRegion/T/lvT"]) + \
  LineSkipper(["BeforeMagnetRegion/Velo2Rich1/lvVelo2Rich1"]) +\
  LineSkipper(["BeforeMagnetRegion/Velo/RFFoil/lvRFDownStreamSection"]) +\
  LineSkipper(["BeforeMagnetRegion/Velo/PipeSections/lvVeloPipe"])
