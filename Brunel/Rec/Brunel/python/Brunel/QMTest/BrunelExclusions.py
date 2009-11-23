from GaudiTest import LineSkipper
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
  LineSkipper(["INFO MuonIDAlg v"]) + \
  LineSkipper(["Delta Memory for the event exceeds 3*sigma"]) + \
  LineSkipper(["AfterMagnetRegion/T/lvT"]) + \
  LineSkipper(["BeforeMagnetRegion/Rich1/lvRich1AerogelContainerRight"]) +\
  LineSkipper(["BeforeMagnetRegion/Velo2Rich1/lvVelo2Rich1"]) +\
  LineSkipper(["BeforeMagnetRegion/Velo/RFFoil/lvRFDSSlope"]) +\
  LineSkipper(["BeforeMagnetRegion/Velo/RFFoil/lvRFSlope"]) +\
  LineSkipper(["BeforeMagnetRegion/Velo/RFFoil/lvRFDownStreamSection"]) +\
  LineSkipper(["BeforeMagnetRegion/Velo/lvVelo"]) +\
  LineSkipper(["BeforeMagnetRegion/Velo/PipeSections/lvVeloPipe"])
