from TAlignment.TAlignmentConf import GAlign
GAlign().forceIterations = False
GAlign().MinIterations = 7
GAlign().MaxIterations = 7
GAlign().evtsPerRun = 3000
from Configurables import AlignTrTools
AlignTrTools().Tools = ["TAConfig", "Derivatives", "WriteAlignmentConditions" ]
#AlignTrTools.Configuration.AlignTrTools().applyConf()
#GAlign().addTool( TAConfig, name="TAConfig" )
#GAlign().TAConfig.addTool( Derivatives, name="Derivatives" )
#GAlign().TAConfig.addTool( Centipede, name="Centipede" )
#GAlign().TAConfig.addTool( MuonMeasurementProvider, name="MuonMeasurementProvider" )
#GAlign().TAConfig.addTool( TrackMasterExtrapolator, name="TrackMasterExtrapolator")
#GAlign().TAConfig.addTool( TrajPoca, name="TrajPoca" )
