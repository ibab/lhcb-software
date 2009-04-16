##############################################################################
# $Id: DVMonitorWithBrunel.py,v 1.1 2009-04-16 07:20:16 pkoppenb Exp $
#
# syntax: gaudirun.py $DAVINCIMONITORSROOT/options/DVMonitorDst.py
#
# Author: Patrick Koppenburg <patrick.koppenburg@cern.ch>
#
##############################################################################
from DaVinci.Configuration import *
from Gaudi.Configuration import *
##############################################################################
#
# Reconstruction
#
from RecSys.Configuration import *
RecSysConf()
##############################################################################
#
# the stuff
#
importOptions( "$DAVINCIMONITORSROOT/options/DaVinciMonitoring.py") 
##############################################################################
#
# Histograms
#
DaVinci().HistogramFile = "DVMonitors.root"
##############################################################################
#
# Most of this will be configured from Dirac
#
##############################################################################
DaVinci().EvtMax = 1000
DaVinci().DataType = "2008" # Default is "DC06"
DaVinci().Simulation = True
DaVinci().InputType = "MDF"

def fullReco() :
    from Configurables import ( ProcessPhase, GaudiSequencer, RecSysConf )
    brunelSeq = GaudiSequencer("BrunelSequencer")
    # analysis and Phys init are not run with MDF
    GaudiSequencer("DaVinciInitSeq").Members += [ brunelSeq ] 
    brunelSeq.Members += [ "ProcessPhase/Init", "ProcessPhase/Reco", "ProcessPhase/Moni"]
    ProcessPhase("Init").DetectorList += [ "Brunel", "Calo"]
    GaudiSequencer("InitCaloSeq").Members += ["GaudiSequencer/CaloBanksHandler"]
    importOptions("$CALODAQROOT/options/CaloBankHandler.opts")
    configureMoni(False,False)
#
# stolen from Brunel. @todo Put it somewhere else
#
def configureMoni(expert,withMC):
    # Set up monitoring (i.e. not using MC truth)
    moniSeq = ["CALO","RICH","MUON","VELO","Tr","ST"]
    ProcessPhase("Moni").DetectorList += moniSeq

    # Histograms filled both in real and simulated data cases
    if "CALO" in moniSeq :
        importOptions('$CALOMONIDSTOPTS/CaloMonitor.opts')
    else :
        # Hack for units options, normally included by CaloMonitor.opts
        importOptions('$STDOPTS/PreloadUnits.opts')

    if "VELO" in moniSeq :
        importOptions('$VELORECMONITORSROOT/options/BrunelMoni_Velo.py')

    if "Tr" in moniSeq :
        from TrackMonitors.ConfiguredTrackMonitors import ConfiguredTrackMonitorSequence
        ConfiguredTrackMonitorSequence(Name='MoniTrSeq')

    if "MUON" in moniSeq :
        importOptions("$MUONPIDCHECKERROOT/options/MuonPIDMonitor.py")

    if "ST" in moniSeq :
        from Configurables import STClusterMonitor, GaudiSequencer
        GaudiSequencer( "MoniSTSeq" ).Members += [ STClusterMonitor("TTClusterMonitor"),
                                                   STClusterMonitor("ITClusterMonitor")]
        STClusterMonitor("TTClusterMonitor").DetType = "TT" ## default anyway 
        STClusterMonitor("ITClusterMonitor").DetType = "IT" 

    # Histograms filled only in real data case
    if not withMC:
        from Configurables import RichRecQCConf
        if "RICH" in moniSeq :
            from Configurables import GaudiSequencer
            RichRecQCConf().DataType = "MDF"
            RichRecQCConf().setProp("MoniSequencer", GaudiSequencer("MoniRICHSeq"))
            

appendPostConfigAction(fullReco)
