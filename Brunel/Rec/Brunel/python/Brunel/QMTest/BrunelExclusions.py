from GaudiTest import LineSkipper, RegexpReplacer
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
  LineSkipper(["IODataManager       ERROR Referring to existing dataset"]) + \
  LineSkipper(["MagneticFieldSvc     INFO Opened magnetic field file"]) + \
  LineSkipper(["ConfigTarFileAc...   INFO  opening "]) + \
  LineSkipper(["LHCBCOND_"]) + \
  LineSkipper(["SIMCOND_"]) + \
  LineSkipper(["DDDB_"]) + \
  LineSkipper(["Memory for the event exceeds 3*sigma"]) + \
  LineSkipper(["Mean 'delta-memory' exceeds 3*sigma"]) + \
  LineSkipper(["BrunelInit.Brun...SUCCESS Exceptions/Errors/Warnings/Infos Statistics : 0/0/"]) + \
  LineSkipper(["| AfterMagnetRegion/"]) + \
  LineSkipper(["| MagnetRegion/"]) +\
  LineSkipper(["| BeforeMagnetRegion/"]) +\
  LineSkipper(["| DownstreamRegion/"]) +\
  LineSkipper(["MD5 sum:"]) + \
  RegexpReplacer(when = "DeFTDetector         INFO Current FT geometry version =   20",
                 orig = r'Current FT geometry version =   20', repl = r'==> Initialize DeFTDetector') +\
  RegexpReplacer(when = "RichHotPixels     WARNING Rich::HPDAnalysisAlg::",
                 orig = r'[01 ]{33}', repl = r'') +\
  RegexpReplacer(when = "RichHotPixels     SUCCESS  #WARNINGS   =",
                 orig = r'[01 ]{33}', repl = r'') +\
  RegexpReplacer(when = "Message = 'Fully suppressed     {",
                 orig = r'[01 ]{33}', repl = r'')
