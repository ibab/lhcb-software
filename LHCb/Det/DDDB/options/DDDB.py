__author__ = "Marco Clemencic <Marco.Clemencic@cern.ch>"

from Gaudi.Configuration import *
from GaudiConf.Configuration import LHCbApp
from DDDB.Configuration import DDDBConf

LHCbApp()
DDDBConf(UseOracle = getConfigurable("LHCbApp").getProp("useOracleCondDB"))
