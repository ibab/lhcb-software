from GaudiTest import LineSkipper, RegexpReplacer
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
    LineSkipper(["BooleInit.Boole...SUCCESS Exceptions/Errors/Warnings/Infos Statistics : 0/0/"]) + \
    LineSkipper(["SIMCOND_"]) + \
    LineSkipper(["DDDB_"]) + \
    RegexpReplacer(when = "DeFTDetector         INFO Current FT geometry version =   20",
                 orig = r'Current FT geometry version =   20', repl = r'==> Initialize DeFTDetector')
 
