from GaudiTest import LineSkipper, RegexpReplacer
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
    LineSkipper(["BooleInit.Boole...SUCCESS Exceptions/Errors/Warnings/Infos Statistics : 0/0/"]) + \
    LineSkipper(["SIMCOND_"]) + \
    LineSkipper(["DDDB_"]) + \
    RegexpReplacer(when = "ToolSvc.OTChann...SUCCESS", orig = r' \[OTFillRawBuffer\]', repl = r'' ) + \
    RegexpReplacer(when = "ToolSvc.OTChann...SUCCESS", orig = r' \[OTTimeChecker\]', repl = r'' )
