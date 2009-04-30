##############################################################################
# File for running Velo alignment on FEST data
#
# Syntax is:
#
# gaudirun.py $ESCHEROPTS/VeloMillepedeAlign.py $ESCHEROPTS/FEST-Data.py 
#
# or
#
# $ESCHEROPTS/gaudiiter.py -n NUMITER -e NUMEVENTS $ESCHEROPTS/VeloMillepedeAlign.py $ESCHEROPTS/FEST-Data.py
#
##############################################################################
from Configurables import ( Escher, TrackSys, RecSysConf )

from Gaudi.Configuration import *
from GaudiConf.Configuration import *

from GaudiKernel.ProcessJobOptions import importOptions
#importOptions('$STDOPTS/PreloadUnits.opts')

#Escher().DataType  = "2008"
#Escher().DDDBtag   = "default"
#Escher().CondDBtag = "default"
#Escher().Detectors = ["Velo"]
Escher().Kalman = True
Escher().InputType = "DST"
Escher().Millepede = False
Escher().TrackContainer = "Rec/Track/Best" # Velo, VeloR, Long, Upstream, Downstream, Ttrack, Muon or Calo
Escher().EvtMax = 10000
#TrackSys().ExpertTracking += [ "kalmanSmoother" ]
#TrackSys().TrackExtraInfoAlgorithms = ['']
#RecSysConf().RecoSequence = ["VELO","TT","IT","OT","Tr","Vertex"]
RecSysConf().RecoSequence = ["VELO","Tr","Vertex"]
TrackSys().TrackPatRecAlgorithms=["Velo"]

from Configurables import (ProcessPhase,AlignSensors,VAlign,WriteAlignmentConditions,TrackVertexMonitor,PatPVOffline
                          )
from TrackFitter.ConfiguredFitters import *
from TrackFitter.ConfiguredFitters import (ConfiguredStraightLineFit)

def doMyAlignChanges():
    #veloAlign=AlignSensors("AlignSensors")
    veloAlign=VAlign("VAlign")
    GaudiSequencer("AlignSequence").Members = [veloAlign ,
                                               #WriteAlignmentConditions("WriteDetectors"),
                                               WriteAlignmentConditions("WriteGlobal"),
                                               WriteAlignmentConditions("WriteModules"),
                                               TrackVertexMonitor("TrackVertexMonitor")
                                               ]
    importOptions("$VELOALIGNMENTOPTS/VeloAlign.py")
    veloAlign.OutputLevel = 3
    PatPVOffline("PatPVOffline").OutputLevel = 3
    VAlign("VAlign").Box_DOF              = [1,1,1,1,1,1]                         
    VAlign("VAlign").Box_EQs              = [0,0,0,0,0,0]
    TrackVertexMonitor("TrackVertexMonitor").TrackContainer= "LHCb::TrackLocation::Velo"
    ###VAlign("VAlign").TrackContainer= "Rec/Track/Best"
    
    eventfitter = TrackEventFitter('FitVelo')                                   
    eventfitter.Fitter.ZPositions=[]
    eventfitter.Fitter.ApplyMaterialCorrections = False
    eventfitter.Fitter.Extrapolator.ApplyMultScattCorr = False
    eventfitter.Fitter.Extrapolator.ApplyEnergyLossCorr = False
    eventfitter.Fitter.Extrapolator.ApplyElectronEnergyLossCorr = False
    eventfitter.Fitter.StateAtBeamLine = False
    eventfitter.Fitter.MaxNumberOutliers = 0
    eventfitter.Fitter.NumberFitIterations = 10
    eventfitter.Fitter.UpdateTransport = True
    #importOptions( '$VELOALIGNMENTOPTS/AlignSensors.py' )

appendPostConfigAction(doMyAlignChanges)

ntupleDir='$ESCHEROPTS/'
ntupleOutputFile = ntupleDir+'EscherNtuple.root' 
histoOutputFile = ntupleDir+'EscherHisto.root'
HistogramPersistencySvc().OutputFile = histoOutputFile
NTupleSvc().Output=["FILE1 DATAFILE='"+ntupleOutputFile+"' TYP='ROOT' OPT='NEW'"]

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-TED_Data.py
##############################################################################



