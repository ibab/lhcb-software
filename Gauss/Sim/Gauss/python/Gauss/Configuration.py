"""
High level configuration tools for Gauss
"""
__version__ = "$Id: Configuration.py,v 1.5 2008-12-07 16:30:25 gcorti Exp $"
__author__  = "Gloria Corti <Gloria.Corti@cern.ch>"

from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from GaudiKernel import SystemOfUnits
from Configurables import ( CondDBCnvSvc, EventClockSvc, FakeEventTime,
                            CondDBEntityResolver )
from Configurables import ( GenInit, Generation, MinimumBias, PythiaProduction,
                            CollidingBeams )
from Configurables import ( GiGaGeo, GiGaInputStream )
