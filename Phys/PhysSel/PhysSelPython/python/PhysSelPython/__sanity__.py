import DaVinciTools.DaVinciToolsConf
import LoKiCore.LoKiCoreConf
import LoKiHlt.LoKiHltConf
import LoKiArrayFunctors.LoKiArrayFunctorsConf
import DecayTreeTuple.DecayTreeTupleConf
import ParticleMaker.ParticleMakerConf
import TisTosTobbing.TisTosTobbingConf
from Configurables import DeterministicPrescaler, GaudiSequencer

from SaneConfigurables import sanitise

sanitise([DaVinciTools.DaVinciToolsConf,
          LoKiCore.LoKiCoreConf,
          LoKiArrayFunctors.LoKiArrayFunctorsConf,
          LoKiHlt.LoKiHltConf,
          LoKiArrayFunctors.LoKiArrayFunctorsConf,
          DecayTreeTuple.DecayTreeTupleConf,
          ParticleMaker.ParticleMakerConf,
          TisTosTobbing.TisTosTobbingConf,
          GaudiSequencer,
          DeterministicPrescaler])
