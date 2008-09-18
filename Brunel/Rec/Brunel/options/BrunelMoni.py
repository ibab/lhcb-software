# This file sets up the Brunel monitoring, independent of MC Truth
#-----------------------------------------------------------------

from GaudiKernel.ProcessJobOptions import importOptions
from Configurables import (ProcessPhase,RootHistCnv__PersSvc,
                           ApplicationMgr,AuditorSvc,SequencerTimerTool)

ProcessPhase('Moni').Context = 'Offline'

# ROOT persistency for histograms
importOptions('$STDOPTS/RootHist.opts')
RootHistCnv__PersSvc('RootHistCnv').ForceAlphaIds = True

# Use TimingAuditor for timing, suppress printout from SequencerTimerTool
ApplicationMgr().ExtSvc += [ 'ToolSvc', 'AuditorSvc' ]
ApplicationMgr().AuditAlgorithms = True
AuditorSvc().Auditors += [ 'TimingAuditor' ] 
SequencerTimerTool().OutputLevel = 4

#--------------------------------------------------------------
# Calo Monitoring algorithms
#--------------------------------------------------------------
importOptions('$CALOMONIDSTOPTS/CaloMonitor.opts')

#--------------------------------------------------------------
# Velo Monitoring algorithms
#--------------------------------------------------------------
importOptions('$VELORECMONITORSROOT/options/BrunelMoni_Velo.py')

#--------------------------------------------------------------
# Track Monitoring algorithms
#--------------------------------------------------------------
from TrackMonitors.ConfiguredTrackMonitors import ConfiguredTrackMonitorSequence
ConfiguredTrackMonitorSequence(Name='MoniTrackSeq')

#--------------------------------------------------------------
# Muon Monitoring algorithms
#--------------------------------------------------------------
importOptions("$MUONPIDCHECKERROOT/options/MuonPIDMonitor.opts")
