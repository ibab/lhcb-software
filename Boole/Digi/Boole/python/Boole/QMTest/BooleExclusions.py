from GaudiTest import LineSkipper
from GaudiConf.QMTest.LHCbExclusions import preprocessor as LHCbPreprocessor

preprocessor = LHCbPreprocessor + \
    LineSkipper(["BooleInit.Boole...SUCCESS Exceptions/Errors/Warnings/Infos Statistics : 0/0/"]) + \
    LineSkipper(["SIMCOND_"]) + \
    LineSkipper(["DDDB_"])
 
