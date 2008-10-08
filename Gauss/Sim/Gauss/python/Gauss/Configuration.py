"""
High level configuration tools for Gauss
"""
__version__ = "$Id: Configuration.py,v 1.3 2008-10-08 16:52:38 gcorti Exp $"
__author__  = "Gloria Corti <Gloria.Corti@cern.ch>"

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import GeV, ns, ms, mrad
from Configurables import ( CondDBCnvSvc, EventClockSvc, FakeEventTime,
                            CondDBEntityResolver )
from Configurables import GiGaGeo
import GaudiKernel.ProcessJobOptions
from Configurables import ( GenInit, Generation, MinimumBias, PythiaProduction,
                            CollidingBeams )
