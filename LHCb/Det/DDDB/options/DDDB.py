__author__ = "Marco Clemencic <Marco.Clemencic@cern.ch>"

from Gaudi.Configuration import *
from DDDB.Configuration import DDDBConf

DDDBConf(UseOracle = getConfigurable("LHCbApp").getProp("useOracleCondDB"))
