from GaudiTest import RegexpReplacer
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
    RegexpReplacer(when = "XmlParserSvc        ERROR DOM",
        orig = r'Id=.*conddb:',
        repl = r'Id=conddb:')
 
