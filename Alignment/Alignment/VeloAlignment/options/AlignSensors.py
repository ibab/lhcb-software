from Gaudi.Configuration import *
from Configurables import ( Escher )
from GaudiKernel.ProcessJobOptions import importOptions
from Configurables import (AlignSensors, TrackMasterFitter, TrackMasterExtrapolator,
                            TrackKalmanFilter, TrackSimpleExtraSelector )

AlignSensors("AlignSensors").OutputLevel	  = 3
AlignSensors("AlignSensors").MinModule          = 5
AlignSensors("AlignSensors").TrackLocation      = "Rec/Track/Best" #"Rec/Track/Velo"
AlignSensors("AlignSensors").PrintInfo		= False
AlignSensors("AlignSensors").SelectedEvent 	= 1 #Choose event number to display
AlignSensors("AlignSensors").OutputNtuple	= False
AlignSensors("AlignSensors").MinNResiduals	= 200
AlignSensors("AlignSensors").KalmanResidual     = False
AlignSensors("AlignSensors").ACDC               = True
AlignSensors("AlignSensors").MinDeltaSig        = 1 # minimal significance of alignment delta required for update

AlignSensors("AlignSensors").addTool(TrackMasterFitter, name="Fitter")
AlignSensors("AlignSensors").Fitter.NumberFitIterations = 1
AlignSensors("AlignSensors").Fitter.addTool(TrackMasterExtrapolator, "Extrapolator")
AlignSensors("AlignSensors").Fitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
AlignSensors("AlignSensors").Fitter.addTool(TrackKalmanFilter, "NodeFitter")
AlignSensors("AlignSensors").Fitter.Extrapolator.addTool(TrackSimpleExtraSelector, "ExtraSelector")
AlignSensors("AlignSensors").Fitter.NodeFitter.addTool(TrackMasterExtrapolator, "Extrapolator")
AlignSensors("AlignSensors").Fitter.NodeFitter.Extrapolator.ExtraSelector = "TrackSimpleExtraSelector"
AlignSensors("AlignSensors").Fitter.Extrapolator.addTool(TrackSimpleExtraSelector, name= "TrackLinearExtrapolator")
AlignSensors("AlignSensors").Fitter.Extrapolator.ApplyEnergyLossCorr = False


##################
#Never change these options:
AlignSensors("AlignSensors").NTupleSplitDir     =False
################

from Configurables import (WriteAlignmentConditions)

writeDetectors= WriteAlignmentConditions("WriteDetectors")
writeDetectors.topElement = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
writeDetectors.depths = [ 4 ]
writeDetectors.outputFile = "Detectors.xml"


