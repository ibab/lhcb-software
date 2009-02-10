"""
High level configuration tools for Gauss
"""
__version__ = "$Id: Configuration.py,v 1.7 2009-02-10 16:41:36 gcorti Exp $"
__author__  = "Gloria Corti <Gloria.Corti@cern.ch>"

from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from GaudiKernel import SystemOfUnits
from Configurables import ( CondDBCnvSvc, EventClockSvc, FakeEventTime,
                            CondDBEntityResolver )
from Configurables import ( GenInit, Generation, MinimumBias, Inclusive,
                            SignalPlain, SignalRepeatedHadronization,
                            SignalForcedFragmentation, StandAloneDecayTool,
                            Special,
                            PythiaProduction, HijingProduction,
                            CollidingBeams, FixedTarget,
                            BeamSpotSmearVertex, FlatZSmearVertex )
from Configurables import ( GiGaGeo, GiGaInputStream )
from Configurables import LHCbApp

##
## Set value of crossing angle for all generation methods 
def setCrossingAngle(angle):

    # All event methods with Pythia as production engine
    # Minimum bias
    Generation().addTool(MinimumBias,name="MinimumBias")
    Generation().MinimumBias.addTool(PythiaProduction,name="PythiaProduction")
    Generation().MinimumBias.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
    Generation().MinimumBias.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
    # Inclusive events
    Generation().addTool(Inclusive,name="Inclusive")
    Generation().Inclusive.addTool(PythiaProduction,name="PythiaProduction")
    Generation().Inclusive.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
    Generation().Inclusive.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
    # Signal plane (e.g. J/Psi)
    Generation().addTool(SignalPlain,"SignalPlain")
    Generation().SignalPlain.addTool(PythiaProduction,name="PythiaProduction")
    Generation().SignalPlain.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
    Generation().SignalPlain.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
    # Signal repeated hadronization (e.g. Bd, Bu, Bs, Lambda_b)
    Generation().addTool(SignalRepeatedHadronization,"SignalRepeatedHadronization")
    Generation().SignalRepeatedHadronization.addTool(PythiaProduction,name="PythiaProduction")
    Generation().SignalRepeatedHadronization.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
    Generation().SignalRepeatedHadronization.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
    # Signal forced fragmentation (e.g. Bc)
    Generation().addTool(SignalForcedFragmentation,"SignalForcedFragmentation")
    Generation().SignalForcedFragmentation.addTool(PythiaProduction,name="PythiaProduction")
    Generation().SignalForcedFragmentation.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
    Generation().SignalForcedFragmentation.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
    # Special signal (Bc with BcVegPy, Higgs, HiddenValley...)
    Generation().addTool(Special,"Special")
    Generation().Special.addTool(PythiaProduction,name="PythiaProduction")
    Generation().Special.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
    Generation().Special.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle
    # Only signal events
    Generation().addTool(StandAloneDecayTool,"StandAloneDecayTool")
    Generation().StandAloneDecayTool.addTool(PythiaProduction,name="PythiaProduction")
    Generation().StandAloneDecayTool.PythiaProduction.addTool(CollidingBeams,name="CollidingBeams")
    Generation().StandAloneDecayTool.PythiaProduction.CollidingBeams.HorizontalCrossingAngle = angle

## end setCrossingAngle
