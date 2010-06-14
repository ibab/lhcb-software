from os import environ
import GaudiKernel.SystemOfUnits as Units
from Gaudi.Configuration import *
from Configurables import DaVinci

importOptions ("$MVASELECTIONSROOT/options/grid/Bs2DsPiSelection_grid.py")
HistogramPersistencySvc().OutputFile = "BsDsPiSignal_MvaSelections_MC09.root";


#importOptions ("$MVASELECTIONSROOT/datafiles/Bs2DsPi/Bs2DsPi_MC09_04March2010_Signal_PFN.py" )
#importOptions ("$MVASELECTIONSROOT/datafiles/Backgr/BBar_MC09_StrippedFiles_pfn.py" )

