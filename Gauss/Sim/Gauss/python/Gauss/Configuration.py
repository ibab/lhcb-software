"""
High level configuration tools for Gauss
"""
__version__ = "$Id: Configuration.py,v 1.4 2008-11-20 16:30:11 gcorti Exp $"
__author__  = "Gloria Corti <Gloria.Corti@cern.ch>"

from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from GaudiKernel.SystemOfUnits import GeV, ns, ms, mrad
from Configurables import ( CondDBCnvSvc, EventClockSvc, FakeEventTime,
                            CondDBEntityResolver )
from Configurables import ( GenInit, Generation, MinimumBias, PythiaProduction,
                            CollidingBeams )
from Configurables import ( GiGaGeo, GiGaInputStream )
