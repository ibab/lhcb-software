from TAlignment.TAlignmentConf import GAlign
GAlign().forceIterations = False
#GAlign().MinIterations = 7
GAlign().MaxIterations = 0
GAlign().OutputLevel = 4
#GAlign().evtsPerRun = 3000
from Configurables import AlignTrTools
AlignTrTools().Tools = ["TAConfig", "Derivatives", "WriteAlignmentConditions" ]
from Configurables import (ApplicationMgr, NTupleSvc)
ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]
tupleFile = "Alignment.root"
tuple = "FILE1 DATAFILE='"+tupleFile+"' TYP='ROOT' OPT='NEW'"
NTupleSvc().Output = [ tuple ]

#AlignTrTools.Configuration.AlignTrTools().applyConf()
#GAlign().addTool( TAConfig, name="TAConfig" )
#GAlign().TAConfig.addTool( Derivatives, name="Derivatives" )
#GAlign().TAConfig.addTool( Centipede, name="Centipede" )
#GAlign().TAConfig.addTool( MuonMeasurementProvider, name="MuonMeasurementProvider" )
#GAlign().TAConfig.addTool( TrackMasterExtrapolator, name="TrackMasterExtrapolator")
#GAlign().TAConfig.addTool( TrajPoca, name="TrajPoca" )
