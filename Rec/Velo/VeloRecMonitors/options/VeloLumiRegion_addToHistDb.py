#!/usr/bin/env gaudirun.py

from Gaudi.Configuration import *
from Configurables import AddNewHistToDB
app = ApplicationMgr()
    
for side in ['ASide','CSide']:
    #----- 1D histograms from LumiRegionMonitoring
    add1DHistAlg = AddNewHistToDB("add"+side+"1DHistogramsAlg")
    add1DHistAlg.OutputLevel = INFO 
    app.TopAlg.append(add1DHistAlg)
    # set task and algorithm name of the histograms
    add1DHistAlg.TaskName      = "VeloBeamMon"
    add1DHistAlg.AlgorithmName = side+"LumiRegionMonitor"
    # setting the type for all histograms
    #   H1D, H2D, P1D, P2D, CNT
    add1DHistAlg.HistoType  = "H1D"
    # this is a list with the names of all histograms that should be created.
    add1DHistAlg.HistoNames = [ "LumiRegionX", "LumiRegionY", "LumiRegionZ", \
                                "LumiRegionX_cumu", "LumiRegionY_cumu", "LumiRegionZ_cumu", \
                                "LumiRegionX_trend", "LumiRegionY_trend", "LumiRegionZ_trend", \
                                "LumiRegionXZ_trend", "LumiRegionYZ_trend",\
                                "Ntrks", "Ntrks_used" ]
    print "adding:",add1DHistAlg.HistoNames
    
    #----- 1D profiles from LumiRegionMonitoring
    add1DProfAlg = AddNewHistToDB("add"+side+"1DProfilesAlg")
    add1DProfAlg.OutputLevel = INFO 
    app.TopAlg.append(add1DProfAlg)
    # set task and algorithm name of the histograms
    add1DProfAlg.TaskName      = "VeloBeamMon"
    add1DProfAlg.AlgorithmName = side+"LumiRegionMonitor"
    # setting the type for all histograms
    #   H1D, H2D, P1D, P2D, CNT
    add1DProfAlg.HistoType  = "P1D"
    # this is a list with the names of all histograms that should be created.
    add1DProfAlg.HistoNames = [ "LumiRegionX_vs_BunchId", "LumiRegionY_vs_BunchId", "LumiRegionZ_vs_BunchId" ]
    print "adding: ",add1DProfAlg.HistoNames

#----- 1D histograms from HltVeloOccMon
add1DHistAlg = AddNewHistToDB("addHltVeloOcc1DHistogramsAlg")
add1DHistAlg.OutputLevel = INFO 
app.TopAlg.append(add1DHistAlg)
# set task and algorithm name of the histograms
add1DHistAlg.TaskName      = "Hlt1VeloOccMon"
add1DHistAlg.AlgorithmName = "VeloLiteClusterMonitor"
# setting the type for all histograms
#   H1D, H2D, P1D, P2D, CNT
add1DHistAlg.HistoType  = "H1D"
# this is a list with the names of all histograms that should be created.
for side in ['ASide','CSide']:
    add1DHistAlg.HistoNames += [ "nClusters_"+side+"_LargeRange", "nClusters_"+side+"_SmallRange" ]
print "adding:",add1DHistAlg.HistoNames
    
#----- 1D profiles from LumiRegionMonitoring
add1DProfAlg = AddNewHistToDB("addHltVeloOcc1DProfilesAlg")
add1DProfAlg.OutputLevel = INFO 
app.TopAlg.append(add1DProfAlg)
# set task and algorithm name of the histograms
add1DProfAlg.TaskName      = "Hlt1VeloOccMon"
add1DProfAlg.AlgorithmName = "VeloLiteClusterMonitor"
# setting the type for all histograms
#   H1D, H2D, P1D, P2D, CNT
add1DProfAlg.HistoType  = "P1D"
# this is a list with the names of all histograms that should be created.
for side in ['ASide','CSide']:
    add1DProfAlg.HistoNames += [ "nClustersVsZ_"+side, "nClustersVsBunchId_"+side ]
print "adding: ",add1DProfAlg.HistoNames
