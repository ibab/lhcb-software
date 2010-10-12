from GaudiTest import RegexpReplacer
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
    LineSkipper(["SIMCOND_"]) + \
    LineSkipper(["DDDB_"]) + \
    RegexpReplacer(when = "XmlParserSvc        ERROR DOM",
        orig = r'entity .*conddb:',
        repl = r'entity conddb:')
 
