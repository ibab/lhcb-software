from Gaudi.Configuration import *
from Configurables import ( Escher, TrackSys )
from Configurables import (VAlign,WriteAlignmentConditions)
from GaudiKernel.ProcessJobOptions import importOptions

# JobOptions file for Velo Alignment monitoring
# Uncomment if you want an nTuple to be created

VAlign().OutputLevel = 3

# Other outputs

NTupleSvc.Output = ["FILE1 DATAFILE='VeloAlign.root' OPT='NEW' TYP='ROOT'"]

VAlign().Monitor_Constants  = True   #  Alignment constants at different stages 
VAlign().Monitor_PV         = True   #  Primary vertex information
VAlign().Monitor_Overlaps   = True   #  Overlap tracks information
VAlign().Monitor_Tracks     = True   #  Track info (params, residuals,...)
VAlign().Monitor_Tracks_num = 0     #  How many track in the TrackInfo sample ?
VAlign().Monitor_Events     = True
