from GaudiTest import LineSkipper, RegexpReplacer
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
  LineSkipper(["IODataManager       ERROR Referring to existing dataset"]) + \
  LineSkipper(["EventSelector        INFO Stream:EventSelector.DataStreamTool"]) + \
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
  LineSkipper(["| AfterMagnetRegion/"]) + \
  LineSkipper(["| MagnetRegion/"]) +\
  LineSkipper(["| BeforeMagnetRegion/"]) +\
  LineSkipper(["| DownstreamRegion/"]) +\
  LineSkipper(["MD5 sum:"]) + \
  LineSkipper(["INFO ANNPID : Tune=MC12TuneV2    TrackType"]) + \
  RegexpReplacer(when = "RichHotPixels     WARNING Rich::HPDAnalysisAlg::",
                 orig = r'[01 ]{33}', repl = r'') +\
  RegexpReplacer(when = "RichHotPixels     SUCCESS  #WARNINGS   =",
                 orig = r'[01 ]{33}', repl = r'') +\
  RegexpReplacer(when = "Message = 'Fully suppressed     {",
                 orig = r'[01 ]{33}', repl = r'') +\
  RegexpReplacer(when = "RichRawDataDBCheckWARNING Rich::DAQ::DataDBCheck::",
                 orig = r'[01 ]{33}', repl = r'') +\
  RegexpReplacer(when = "RichRawDataDBCheckSUCCESS  #WARNINGS   =",
                 orig = r'[01 ]{33}', repl = r'') +\
  RegexpReplacer(when = "EventID Mismatch : HPD L0ID=",
                 orig = r'[01 ]{33}', repl = r'')
