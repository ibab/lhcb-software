
## @package RecConf
#  High level configuration tools for REC
#  @author Rob Lambert
#  @date   21/05/2010

__version__ = "$Id: Configuration.py,v 1.34 2010-05-05 09:58:07 ggiacomo Exp $"
__author__  = "Rob Lambert"
            
from LHCbKernel.Configuration import *
from TrackSys.Configuration   import *
from RichRecSys.Configuration import *
from GlobalReco.Configuration import *

from CaloReco.Configuration   import OffLineCaloRecoConf 
from CaloPIDs.Configuration   import OffLineCaloPIDsConf

from Configurables import ( ProcessPhase, CaloMoniDstConf, RichRecQCConf,
                            VeloRecMonitors, GlobalRecoChecks,
                            MuonTrackMonitorConf, MuonIDAlg )

## @class RecSysConf
#  Configurable for LHCb reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RecSysConf(LHCbConfigurableUser):

    ## Name used for RICH reconstruction configurable
    richRecConfName = "RichOfflineRec"

    ## Possible used Configurables
    __used_configurables__ = [ GlobalRecoConf      ,
                               TrackSys            ,
                               OffLineCaloPIDsConf ,
                               OffLineCaloRecoConf ,
                               (RichRecSysConf,richRecConfName)
                               ]

    ## Default tracking Sub-detector processing sequence
    DefaultTrackingSubdets = ["Decoding", "VELO","TT","IT","OT","Tr","Vertex"]
    ## Default reconstruction sequence for field-on data
    DefaultSubDetsFieldOn  = DefaultTrackingSubdets+["RICH","CALO","MUON","PROTO","SUMMARY"]
    ## Default reconstruction sequence for field-off data
    DefaultSubDetsFieldOff = DefaultTrackingSubdets+["CALO","RICH","MUON","PROTO","SUMMARY"]
    ## List of known special data processing options
    KnownSpecialData = [ "cosmics", "veloOpen", "fieldOff", "beamGas",
                         "microBiasTrigger","upgrade" ]

    ## Steering options
    __slots__ = {
        "RecoSequence" : None    # The Sub-detector sequencing. Default is all known
       ,"SpecialData"  : []      # Various special data processing options. See KnownSpecialData for all options
       ,"Detectors"    : ['Velo','TT','IT','OT','Rich','Tr','Calo','Muon','L0']
       ,"Histograms"   : "OfflineFull" # Type of histograms
       ,"Context"      : "Offline"     # The context within which to run the reco sequences
       ,"OutputType"   : ""            # some sequences are different for RDST
       ,"DataType"     : ""            # Type of data, propagated from application
       ,"OutputLevel"  : INFO          # The printout level to use
       ,"Simulation"   : False         # Simulated data
        }

    def expertHistos(self): return self.getProp("Histograms") == "Expert"

    ## Apply the configuration
    def applyConf(self):

        # Check the special data options
        for option in self.getProp("SpecialData"):
            if option not in self.KnownSpecialData:
                raise RuntimeError("Unknown SpecialData option '%s'"%option)
        
        # Phases
        if not self.isPropertySet("RecoSequence"):
            if "fieldOff" in self.getProp("SpecialData"):
                self.setProp("RecoSequence",self.DefaultSubDetsFieldOff)
            else:
                self.setProp("RecoSequence",self.DefaultSubDetsFieldOn)
        recoSeq = self.getProp("RecoSequence")
        from Configurables import ProcessPhase
        ProcessPhase("Reco").DetectorList += recoSeq

        # Primary Vertex and V0 finding
        if "Vertex" in recoSeq:
            from Configurables import PatPVOffline, TrackV0Finder
            pvAlg = PatPVOffline()
            if "2009" == self.getProp("DataType"):
                from PatPV import PVConf 
                PVConf.VLoosePV().configureAlg()
            elif ( "veloOpen" in self.getProp("SpecialData") or
                    "microBiasTrigger" in self.getProp("SpecialData") ):
                from PatPV import PVConf
                PVConf.LoosePV().configureAlg()
            elif ( not ( self.getProp("Simulation") and
                         self.getProp("DataType") in ["2008","2009","2010","MC09","Upgrade"] ) ): 
                from PatPV import PVConf
                PVConf.StandardPV().configureAlg()
            GaudiSequencer("RecoVertexSeq").Members += [ pvAlg ];
            if self.getProp( "OutputType" ).upper() == "RDST":
                # Velo tracks not copied to Rec/Track/Best for RDST 
                from Configurables import PVOfflineTool
                pvAlg.addTool( PVOfflineTool() )
                pvAlg.PVOfflineTool.InputTracks = [ "Rec/Track/Best",
                                                    "Rec/Track/PreparedVelo" ]
            trackV0Finder = TrackV0Finder()
            GaudiSequencer("RecoVertexSeq").Members += [ trackV0Finder ]

        ## Upgrade type?
        if self.getProp("DataType") == 'Upgrade' :
             specialDataList = self.getProp("SpecialData")
             specialDataList.append("upgrade")
             self.setProp("SpecialData",specialDataList)


        # Tracking (Should make it more fine grained ??)
        DoTracking = False
        for seq in self.DefaultTrackingSubdets :
            if seq in recoSeq: DoTracking = True
        if DoTracking:
            trackConf = TrackSys()
            self.setOtherProps(trackConf,["SpecialData","OutputType"])
            trackConf.ExpertHistos = self.expertHistos()

        # RICH
        if "RICH" in recoSeq:

            # The main sequence
            seq = GaudiSequencer("RecoRICHSeq")
            # Create the top level Conf object and set some general options
            richConf = RichRecSysConf(self.richRecConfName)
            self.setOtherProps(richConf,["SpecialData","Context","OutputLevel","Simulation","DataType"])
            # Set the sequencer the RICH reco algs should be added to
            richConf.RecoSequencer = seq
            # Input Tracks (would be better to not hard code this. Get from TrackSys() or DstConf())
            richConf.trackConfig().InputTracksLocation = "Rec/Track/Best"
            # Output PID Location (Same again. maybe get this location from DstConf() 
            richConf.RichPIDLocation = "Rec/Rich/PIDs"
            # Printout
            import GaudiKernel.ProcessJobOptions
            GaudiKernel.ProcessJobOptions.PrintOn()
            log.debug(richConf)
            GaudiKernel.ProcessJobOptions.PrintOff()
            
        # CALO
        if "CALO" in recoSeq:
            
            seq  = GaudiSequencer ( 'RecoCALOSeq' )
            reco = GaudiSequencer ( 'CaloRecoSeq' )
            pids = GaudiSequencer ( 'CaloPIDsSeq' )
            seq.Members = [ reco , pids ]
            
            caloConf = OffLineCaloRecoConf(
                Sequence           = reco                           ,
                Context            = self.getProp('Context')        ,
                OutputLevel        = self.getProp('OutputLevel')    ,
                ##
                UseTracks          = True                           ,
                EnableRecoOnDemand = False 
                )
            
            caloPIDs = OffLineCaloPIDsConf (
                Sequence           = pids                           ,
                Context            = self.getProp('Context')        ,
                OutputLevel        = self.getProp('OutputLevel')    ,
                EnablePIDsOnDemand = False                          ,
                DataType           = self.getProp ('DataType')      
                )


        # MUON
        if "MUON" in recoSeq:
            from MuonID import ConfiguredMuonIDs
            from Configurables import RawBankReadoutStatusConverter,RawBankReadoutStatusFilter
            cm=ConfiguredMuonIDs.ConfiguredMuonIDs(data=self.getProp("DataType"),
                                                   specialData=self.getProp("SpecialData"))
            MuonIDSeq=cm.getMuonIDSeq()
            RawBankReadoutStatusConverter("MuonProcStatus").System="Muon"
            RawBankReadoutStatusConverter("MuonProcStatus").BankTypes=["Muon"]
            RawBankReadoutStatusFilter("MuonROFilter").BankType=13
            RawBankReadoutStatusFilter("MuonROFilter").RejectionMask=2067            
            GaudiSequencer("RecoMUONSeq").Members += [ "MuonRec",
                                                       "RawBankReadoutStatusConverter/MuonProcStatus", 
                                                       "RawBankReadoutStatusFilter/MuonROFilter",  MuonIDSeq ]
           
            if self.getProp("DataType") == 'Upgrade' and "VELOPIX" in recoSeq:
                from RecoUpgrade import RecoTrackingUpgrade
                RecoTrackingUpgrade.ConfigVeloPixProvider( MuonIDAlg().myMuIDTool.fitter )
                

        # PROTO
        if "PROTO" in recoSeq:
            self.setOtherProps(GlobalRecoConf(),["DataType","SpecialData",
                                                 "Context","OutputLevel"])
            GlobalRecoConf().RecoSequencer = GaudiSequencer("RecoPROTOSeq")
            

        # SUMMARY
        if "SUMMARY" in recoSeq:
            from Configurables import RecSummaryAlg
            summary = RecSummaryAlg("RecSummary")
            # make a new list of uppered detectors
            dets = []
            for det in self.getProp("Detectors"):
                dets.append(det.upper())
            summary.Detectors = dets 
            GaudiSequencer("RecoSUMMARYSeq").Members += [summary]

## @class RecMoniConf
#  Configurable for LHCb reconstruction monitoring (without MC truth)
#  @author Marco Cattaneo (Marco.Cattaneo@cern.ch)
#  @date   15/10/2009
class RecMoniConf(LHCbConfigurableUser):

    ## Name used for RICH Monitoring configurable
    richMoniConfName = "OfflineRichMoni"

    ## Possible used Configurables
    __used_configurables__ = [ CaloMoniDstConf,
                               (RichRecQCConf,richMoniConfName),
                               VeloRecMonitors,
                               MuonTrackMonitorConf,
                               GlobalRecoChecks ]

    ## Configurables that must be configured before us
    __queried_configurables__ = [ RecSysConf, TrackSys ]

    ## Steering options
    __slots__ = {
        "MoniSequence" : None
       ,"Histograms"   : "OfflineFull" # Type of histograms
       ,"CheckEnabled" : False
       ,"OutputLevel"  : INFO 
       ,"Context"      : "Offline"
       ,"DataType"     : ""
       ,"Simulation"   : False         # Simulated data
        }

    _propertyDocDct = { 
        'MoniSequence' : """ List of subdetectors to monitor, default is all known """
       ,'Histograms'   : """ Type of histograms """
       ,'CheckEnabled' : """ Flags whether a check sequence (with MC truth) is also enabled (default False) """
       ,'OutputLevel'  : """ The printout level to use (default INFO) """
       ,'Context'      : """ The context within which to run (default 'Offline') """
       ,'DataType'     : """ Data type, propagated from the application """
       ,'Simulation'   : """ Is it simulated data? """
       }

    ## Known monitoring sequences, all run by default
    KnownMoniSubdets        = ["GENERAL","CALO","RICH","MUON","VELO","Tr","OT","ST","PROTO","Hlt"] 
    KnownExpertMoniSubdets  = KnownMoniSubdets+["TT","IT"]

    def expertHistos(self): return self.getProp("Histograms") == "Expert"
    
    ## Apply the configuration
    def applyConf(self):

        # Set up monitoring (i.e. not using MC truth)
        if not self.isPropertySet("MoniSequence"):
            if self.expertHistos():
                moniSeq = self.KnownExpertMoniSubdets
            else:
                moniSeq = self.KnownMoniSubdets
            self.MoniSequence = moniSeq
        else:
            for seq in self.getProp("MoniSequence"):
                if self.expertHistos():
                    if seq not in self.KnownExpertMoniSubdets:
                        log.warning("Unknown subdet '%s' in MoniSequence"%seq)
                else:
                    if seq not in self.KnownMoniSubdets:
                        log.warning("Unknown subdet '%s' in MoniSequence"%seq)
        moniSeq = self.getProp("MoniSequence")
        from Configurables import ProcessPhase
        ProcessPhase("Moni").DetectorList += moniSeq

        # Histograms filled both in real and simulated data cases
        if "GENERAL" in moniSeq :

            # Enable ChronoAuditor
            chronoAuditor = "ChronoAuditor"
            if chronoAuditor not in AuditorSvc().Auditors :
                AuditorSvc().Auditors += [ chronoAuditor ]
            ChronoAuditor().Enable = True
            # Turn off most output
            ChronoStatSvc().ChronoPrintOutTable = False
            ChronoStatSvc().PrintUserTime       = False
            ChronoStatSvc().PrintSystemTime     = False
            
            from Configurables import GaudiSequencer, RecProcessingTimeMoni
            seq = GaudiSequencer( "MoniGENERALSeq")

            # Overall time
            overallTime = RecProcessingTimeMoni("OverallEventProcTime")
            overallTime.Algorithms = ["PhysicsSeq"]
            seq.Members += [overallTime]

            # Tracking
            trackTime = RecProcessingTimeMoni("TrackEventProcTime")
            trackTime.Algorithms = ["RecoTrSeq","RecoVELOSeq","RecoDecodingSeq"]
            seq.Members += [trackTime]

            # Vertex
            vertTime = RecProcessingTimeMoni("VertexEventProcTime")
            vertTime.Algorithms = ["RecoVertexSeq"]
            seq.Members += [vertTime]

            # RICH
            richTime = RecProcessingTimeMoni("RichEventProcTime")
            richTime.Algorithms = ["RecoRICHSeq"]
            seq.Members += [richTime]

            # CALO
            caloTime = RecProcessingTimeMoni("CaloEventProcTime")
            caloTime.Algorithms = ["RecoCALOSeq"]
            seq.Members += [caloTime]

            # MUON
            muonTime = RecProcessingTimeMoni("MuonEventProcTime")
            muonTime.Algorithms = ["RecoMUONSeq"]
            seq.Members += [muonTime]

            # PROTO
            protoTime = RecProcessingTimeMoni("ProtoEventProcTime")
            protoTime.Algorithms = ["RecoPROTOSeq"]
            seq.Members += [protoTime]

            # ProcStat Abort rates
            from Configurables import ProcStatAbortMoni
            seq.Members += [ProcStatAbortMoni()]
        
        if "CALO" in moniSeq :
            from Configurables import GaudiSequencer
            seq = GaudiSequencer( "MoniCALOSeq")
            caloMoni = CaloMoniDstConf( Sequence    = seq,
                                        OutputLevel = self.getProp('OutputLevel'),
                                        Context = 'Offline' )
            self.setOtherProps(caloMoni,["Histograms"])

        if "VELO" in moniSeq :
            from Configurables import GaudiSequencer
            self.setOtherProps(VeloRecMonitors(),["Histograms","OutputLevel"])
            VeloRecMonitors().setProp("MoniSequence", GaudiSequencer("MoniVELOSeq"))
            #importOptions('$VELORECMONITORSROOT/options/BrunelMoni_Velo.py')

        if "Tr" in moniSeq :
            from TrackMonitors.ConfiguredTrackMonitors import ConfiguredTrackMonitorSequence
            ConfiguredTrackMonitorSequence(Name='MoniTrSeq')

        if "OT" in moniSeq :
            from TrackMonitors.ConfiguredTrackMonitors import ConfiguredOTMonitorSequence
            ConfiguredOTMonitorSequence(Name='MoniOTSeq')

        if "MUON" in moniSeq :
            from MuonPIDChecker import ConfigureMuonPIDChecker as mmuon
            mydata =  self.getProp("DataType")
            mymonitconf = mmuon.ConfigureMuonPIDChecker(data = mydata)
            mymonitconf.configure(UseMC = False, HistosLevel = self.getProp("Histograms")) 

            mymtmconf = MuonTrackMonitorConf()
            self.setOtherProps(mymtmconf,["Histograms","OutputLevel","DataType"])
            mymtmconf.setProp("Sequencer", GaudiSequencer("MoniMUONSeq"))

        if "ST" in moniSeq :
            from Configurables import ST__STClusterMonitor, GaudiSequencer
            GaudiSequencer( "MoniSTSeq" ).Members += [ ST__STClusterMonitor("TTClusterMonitor"),
                                                       ST__STClusterMonitor("ITClusterMonitor")]
            ST__STClusterMonitor("TTClusterMonitor").DetType = "TT" ## default anyway 
            ST__STClusterMonitor("ITClusterMonitor").DetType = "IT"

        if "PROTO" in moniSeq :
            from Configurables import ( ChargedProtoParticleMoni, GaudiSequencer,
                                        ANNGlobalPID__ChargedProtoANNPIDMoni )
            seq = GaudiSequencer( "MoniPROTOSeq" )
            seq.Members += [ ChargedProtoParticleMoni("ChargedProtoPMoni"),
                             ANNGlobalPID__ChargedProtoANNPIDMoni("ChargedProtoANNPIDMoni") ]
            if self.expertHistos() and not self.getProp("CheckEnabled") :
                self.setOtherProps(GlobalRecoChecks(),["OutputType"])
                exSeq = GaudiSequencer("ExpertProtoMoni")
                seq.Members += [exSeq]
                GlobalRecoChecks().Sequencer = exSeq

        # If checking is enabled, all Rich histograms are booked in check sequence
        if "RICH" in moniSeq and not self.getProp("CheckEnabled"):
            from Configurables import GaudiSequencer
            conf = RichRecQCConf(self.richMoniConfName)
            self.setOtherProps(conf,["Histograms","Context","OutputLevel","DataType","Simulation"])
            conf.setProp("MoniSequencer", GaudiSequencer("MoniRICHSeq"))
            conf.setProp("WithMC", False)
            conf.RichPIDLocation = "Rec/Rich/PIDs"
            import GaudiKernel.ProcessJobOptions
            GaudiKernel.ProcessJobOptions.PrintOn()
            log.debug(conf)
            GaudiKernel.ProcessJobOptions.PrintOff()
        
        if "Hlt" in moniSeq :
            # Do not run Hlt monitoring on DataTypes without HltDecReports 
            if self.getProp("DataType") not in ["2008","DC06","MC09"] and not self.getProp("Simulation") :
                from HltMonitors.ConfiguredHltMonitors import ConfiguredHltMonitorSequence
                ConfiguredHltMonitorSequence(Name='MoniHltSeq')

        # Expert histograms
        if self.expertHistos():
            if "TT" in moniSeq :
                from Configurables import ST__STClusterMonitor
                clusMoni = ST__STClusterMonitor("TTClusterMonitor")
                clusMoni.FullDetail = True
                GaudiSequencer("MoniTTSeq").Members += [clusMoni]
            if "IT" in moniSeq :
                from Configurables import ST__STClusterMonitor
                clusMoni = ST__STClusterMonitor("ITClusterMonitor")
                clusMoni.FullDetail = True
                clusMoni.DetType = "IT"
                GaudiSequencer("MoniITSeq").Members += [clusMoni]
