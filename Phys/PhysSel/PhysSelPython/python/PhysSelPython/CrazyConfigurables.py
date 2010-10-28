"""Module containing a list of important, crazy configurables that should be cured in the Configurable Hospital.
Exports configurables as list via method configurables().
Add a configurable to list with method addConfigurable(arg).
"""

__author__ = 'Juan Palacios juan.palacios@physik.uzh.ch'

__all__ = ('parientList')

import DaVinciTools.DaVinciToolsConf
import LoKiCore.LoKiCoreConf
import LoKiHlt.LoKiHltConf
import LoKiArrayFunctors.LoKiArrayFunctorsConf
import DecayTreeTuple.DecayTreeTupleConf
import ParticleMaker.ParticleMakerConf
import TisTosTobbing.TisTosTobbingConf
from Configurables import DeterministicPrescaler, GaudiSequencer

from PhysSelPython.ConfigurableHospital import memoized_sanitise

_configurableList = [DaVinciTools.DaVinciToolsConf,
                                         LoKiCore.LoKiCoreConf,
                                         LoKiArrayFunctors.LoKiArrayFunctorsConf,
                                         LoKiHlt.LoKiHltConf,
                                         LoKiArrayFunctors.LoKiArrayFunctorsConf,
                                         DecayTreeTuple.DecayTreeTupleConf,
                                         ParticleMaker.ParticleMakerConf,
                                         TisTosTobbing.TisTosTobbingConf,
                                         GaudiSequencer,
                                         DeterministicPrescaler]

def configurables() :
    return _configurableList

def addConfigurable(conf) :
    if not conf in _configurableList :
        _configurableList += [conf]
