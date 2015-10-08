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
                 orig = r'[01 ]{33}', repl = r'') +\
  RegexpReplacer(when = "ToolSvc.TrackIn...SUCCESS", orig = r' \[OTHitEfficiencyMonitor\]', repl = r'' ) + \
  RegexpReplacer(when = "ToolSvc.TrackSt...SUCCESS", orig = r' \[TrackV0Finder\]', repl = r'' ) + \
  RegexpReplacer(when = "ToolSvc.PatSeedFitSUCCESS", orig = r' \[TrackBestTrackCreator\]', repl = r'' ) + \
  RegexpReplacer(when = "ToolSvc.PatSeedFitSUCCESS", orig = r' \[ForwardHLT1FitterAlg\]', repl = r'' ) + \
  RegexpReplacer(when = "ToolSvc.TrackMa...SUCCESS", orig = r' \[TrackResChecker\]', repl = r'' ) + \
  RegexpReplacer(when = "ToolSvc.OTChann...SUCCESS", orig = r' \[MuonIDAlgLite\]', repl = r'' ) + \
  RegexpReplacer(when = "ToolSvc.OTChann...SUCCESS", orig = r' \[PatForwardHLT1\]', repl = r'' ) + \
  RegexpReplacer(when = "ToolSvc.OTChann...SUCCESS", orig = r' \[PatForward\]', repl = r'' )
