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
from Configurables import CaloProcessor

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
                               CaloProcessor,
                               (RichRecSysConf,richRecConfName)
                               ]

    ## Default tracking Sub-detector processing sequence (Run I )
    DefaultTrackingSubdets = ["VELO","TT","IT","OT","Tr","Vertex"]
    ## Default tracking Sub-detector processing sequence (Run II)
    DefaultTrackingSubdetsRun2 = ["VELO","TT","IT","OT","TrHLT1","Vertex", "TrHLT2"]
    ## Default reconstruction sequence for field-on data
    DefaultSubDetsFieldOn  = ["Decoding"] + DefaultTrackingSubdets+["RICH","CALO","MUON","PROTO","SUMMARY"]
    ## Default reconstruction sequence for field-on data (Run II)
    DefaultSubDetsFieldOnRun2  = ["Decoding"] + DefaultTrackingSubdetsRun2 + ["RICH","CALO","MUON","PROTO","SUMMARY"]
    ## List of known special data processing options
    KnownSpecialData = [ "cosmics", "veloOpen", "fieldOff", "beamGas", "microBiasTrigger", "pA", "pGun"]

    ## Steering options
    __slots__ = {
        "RecoSequence" : None    # The Sub-detector sequencing. Default is all known
       ,"SpecialData"  : []      # Various special data processing options. See KnownSpecialData for all options
       ,"Detectors"    : ['Velo','TT','IT','OT','Rich','Tr','Calo','Muon','L0']
       ,"Histograms"   : "OfflineFull" # Type of histograms
       ,"Context"      : "Offline"     # The context within which to run the reco sequences
       ,"OutputType"   : ""            # DST type, not used for now, propagated to TrackSys
       ,"DataType"     : ""            # Type of data, propagated from application
       ,"OutputLevel"  : INFO          # The printout level to use
       ,"Simulation"   : False         # Simulated data
       ,"OnlineMode"   : False         # Online mode
       ,"SkipTracking" : False         # Skip the tracking sequence
        }

    ## Initialisation. Needed to set RICH option before applyConf method starts.
    def __init__(self, name=Configurable.DefaultName, *args, **kwargs):
        LHCbConfigurableUser.__init__(self, name=Configurable.DefaultName,  *args, **kwargs)
        # Activate the RICH processing mode where Long, Downstream and Upstream
        # tracks are processed independently of each other
        richConf = RichRecSysConf(self.richRecConfName)
        richConf.setTrackGroups( [ ["Forward","Match"], ["VeloTT"], ["KsTrack"] ] )

    def expertHistos(self): return self.getProp("Histograms") == "Expert"

    ## Apply the configuration
    def applyConf(self):

        # Check the special data options
        for option in self.getProp("SpecialData"):
            if option not in self.KnownSpecialData:
                raise RuntimeError("Unknown SpecialData option '%s'"%option)
        
        # Phases
        if not self.isPropertySet("RecoSequence"):
            self.setProp("RecoSequence",self.DefaultSubDetsFieldOn)
            if self.getProp("DataType") is "2015":
                self.setProp("RecoSequence",self.DefaultSubDetsFieldOnRun2)
        
        recoSeq = self.getProp("RecoSequence")
        if self.getProp("SkipTracking"):
            for det in self.DefaultTrackingSubdets:
                if det in recoSeq:
                    recoSeq.remove(det)
        from Configurables import ProcessPhase
        ProcessPhase("Reco").DetectorList += recoSeq

        # Primary Vertex and V0 finding
        if "Vertex" in recoSeq  and self.getProp("DataType") is not "2015":
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
                         self.getProp("DataType") in ["2008","2009","2010","MC09"] ) ):
                # Default setting uses beam spot constraint from DB, available from 2011. Prior to 2011 stability of beam spot is not certain
                from PatPV import PVConf
                PVConf.StandardPV().configureAlg()
                
            # MC with particle gun cannot reconstruct PV, hence need to introduce one by hand
            if "pGun" in self.getProp("SpecialData"):
                from Configurables import PGPrimaryVertex
                pgPV = PGPrimaryVertex()
                GaudiSequencer("RecoVertexSeq").Members += [ pgPV ]
            else:
                GaudiSequencer("RecoVertexSeq").Members += [ pvAlg ];
                
            trackV0Finder = TrackV0Finder()
            GaudiSequencer("RecoVertexSeq").Members += [ trackV0Finder ]

        # for Run 2, we run a different algorithm and don't want to have
        # the V0 finder in the vertex sequence (which is now after HLT1)
        if "Vertex" in recoSeq and self.getProp("DataType") is "2015":
            from Configurables import PatPV3D, PVOfflineTool, LSAdaptPV3DFitter
            pvAlg = PatPV3D("PatPV3D")
            ## this should go in a configuration file when we know what to use
            pvAlg.addTool(PVOfflineTool,"PVOfflineTool")
            pvAlg.PVOfflineTool.addTool(LSAdaptPV3DFitter, "LSAdaptPV3DFitter")
            pvAlg.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
            pvAlg.PVOfflineTool.LSAdaptPV3DFitter.UseFittedTracks = True
            pvAlg.PVOfflineTool.LSAdaptPV3DFitter.AddMultipleScattering = False
            pvAlg.PVOfflineTool.LSAdaptPV3DFitter.TrackErrorScaleFactor = 1.0
            pvAlg.PVOfflineTool.LSAdaptPV3DFitter.MinTracks = 4
            pvAlg.PVOfflineTool.LSAdaptPV3DFitter.trackMaxChi2 = 12.0
            pvAlg.PVOfflineTool.UseBeamSpotRCut = True
            pvAlg.PVOfflineTool.BeamSpotRCut = 0.2
            pvAlg.PVOfflineTool.InputTracks = [ "Rec/Track/FittedHLT1VeloTracks" ]
            pvAlg.OutputVerticesName = "Rec/Vertex/Primary"
            pvAlg.PrimaryVertexLocation = "Rec/Vertex/PrimaryVertices"
            
            GaudiSequencer("RecoVertexSeq").Members += [ pvAlg ];
            GaudiSequencer("RecoVertexSeq").IgnoreFilterPassed = True

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
            
            # Create the top level Conf object 
            richConf = RichRecSysConf(self.richRecConfName)
            
            # set some general options
            self.setOtherProps(richConf,["SpecialData","Context","OutputLevel",
                                         "Simulation","DataType","OnlineMode"])
            
            # Set the sequencer the RICH reco algs should be added to
            richConf.RecoSequencer = seq
            
            # Input Tracks (would be better to not hard code this. Get from TrackSys() or DstConf())
            richConf.trackConfig().InputTracksLocation = "Rec/Track/Best"
            
            # Output PID Location (Same again. maybe get this location from DstConf())
            richConf.RichPIDLocation = "Rec/Rich/PIDs"
            
            # Printout
            import GaudiKernel.ProcessJobOptions
            GaudiKernel.ProcessJobOptions.PrintOn()
            log.debug(richConf)
            GaudiKernel.ProcessJobOptions.PrintOff()
            
        # CALO
        if "CALO" in recoSeq:
            import GaudiKernel.ProcessJobOptions            
            seq  = GaudiSequencer ( 'RecoCALOSeq' )
            caloConf=CaloProcessor(
                Context            = self.getProp('Context')       ,
                OutputLevel        = self.getProp('OutputLevel')    ,
                UseTracks          = True                           ,
                EnableOnDemand     = False                          ,
                DataType           = self.getProp ('DataType')      
                )
            GaudiKernel.ProcessJobOptions.PrintOn()
            seq.Members = [caloConf.caloSequence()]
            GaudiKernel.ProcessJobOptions.PrintOff()
            


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
       ,"Detectors"    : ['Velo','TT','IT','OT','Rich','Tr','Calo','Muon','L0']
       ,"OnlineMode"   : False         # Online mode
       ,"SkipTracking" : False         # Skip the tracking detectors
        }

    _propertyDocDct = { 
        'MoniSequence' : """ List of subdetectors to monitor, default is all known """
       ,'Histograms'   : """ Type of histograms """
       ,'CheckEnabled' : """ Flags whether a check sequence (with MC truth) is also enabled (default False) """
       ,'OutputLevel'  : """ The printout level to use (default INFO) """
       ,'Context'      : """ The context within which to run (default 'Offline') """
       ,'DataType'     : """ Data type, propagated from the application """
       ,'Simulation'   : """ Is it simulated data? """
       ,'OnlineMode'   : """ Online Brunel mode """
       ,'SkipTracking' : """ Skip the tracking detectors """
       }

    ## Known monitoring sequences, all run by default
    #KnownMoniSubdets        = ["GENERAL","CALO","RICH","MUON","VELO","Tr","OT","ST","PROTO","Hlt"]
    #KnownExpertMoniSubdets  = KnownMoniSubdets+["TT","IT"]
    KnownMoniSubdets        = ["CALO","RICH","MUON","VELO","OT","ST"]
    KnownMoniGeneral        = ["GENERAL","Tr","PROTO","Hlt"]
    KnownMoniSubdets        = KnownMoniSubdets + KnownMoniGeneral
    KnownExpertMoniSubdets  = KnownMoniSubdets+["TT","IT"]
    

    def expertHistos(self): return self.getProp("Histograms") == "Expert"
    
    ## Apply the configuration
    def applyConf(self):
        # Build list of existing detectors
        dets = []
        for det in self.getProp("Detectors"):
            dets.append(det.upper())
        # general monitoring
        for det in self.KnownMoniGeneral:
            dets.append(det)
        # For compatibility reasons
        if [det for det in ['IT', 'TT'] if det in dets]:
            dets.append("ST")
        if [det for det in ['SPD', 'PRS', 'ECAL', 'HCAL'] if det in dets]:
            dets.append("CALO")
        if [det for det in ['RICH1', 'RICH2'] if det in dets]:
            dets.append("RICH")
            
        # Set up monitoring (i.e. not using MC truth)
        moniSeq = []
        if not self.isPropertySet("MoniSequence"):
            # Build default monitoring
            if self.expertHistos():
                moniDets = self.KnownExpertMoniSubdets
            else:
                moniDets = self.KnownMoniSubdets
            # create overlap
            for det in moniDets:
                if det in dets:
                    moniSeq.append(det)
            self.MoniSequence = moniSeq
        else:
            for seq in self.getProp("MoniSequence"):
                if self.expertHistos():
                    if seq not in self.KnownExpertMoniSubdets:
                        log.warning("Unknown subdet '%s' in MoniSequence"%seq)
                    if seq not in dets:
                        log.warning("Detector unknown '%s'"%seq)    
                else:
                    if seq not in self.KnownMoniSubdets:
                        log.warning("Unknown subdet '%s' in MoniSequence"%seq)
                    if seq not in dets:
                        log.warning("Detector unknown '%s'"%seq)    
            moniSeq = self.getProp("MoniSequence")

        if self.getProp("SkipTracking"):
            for det in ['VELO','IT','TT','ST','OT']:
                if det in moniSeq:
                    moniSeq.remove(det)        
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
            import GaudiKernel.ProcessJobOptions
            GaudiKernel.ProcessJobOptions.PrintOn()
            from Configurables import GaudiSequencer
            seq = GaudiSequencer( "MoniCALOSeq")
            noSPDPRS = True
            if [det for det in ['SPD','PRS'] if det in dets]:
                noSPDPRS = False
            caloMoni = CaloMoniDstConf( MonitorSequence    = seq,
                                        OutputLevel = self.getProp('OutputLevel'),
                                        Context = 'Offline',
                                        NoSpdPrs = noSPDPRS)
            caloMoni.printConf()
            self.setOtherProps(caloMoni,["Histograms"])
            GaudiKernel.ProcessJobOptions.PrintOff()

        if "VELO" in moniSeq :
            from Configurables import GaudiSequencer
            self.setOtherProps(VeloRecMonitors(),["Histograms","OutputLevel"])
            VeloRecMonitors().setProp("MoniSequence", GaudiSequencer("MoniVELOSeq"))
            #importOptions('$VELORECMONITORSROOT/options/BrunelMoni_Velo.py')   
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
            GaudiSequencer( "MoniSTSeq" ).Members += [ ST__STClusterMonitor("TTClusterMonitor")]
            ST__STClusterMonitor("TTClusterMonitor").DetType = "TT" ## default anyway 
            GaudiSequencer( "MoniSTSeq" ).Members += [ ST__STClusterMonitor("ITClusterMonitor")]
            ST__STClusterMonitor("ITClusterMonitor").DetType = "IT"

        if "Tr" in moniSeq :
            from TrackMonitors.ConfiguredTrackMonitors import ConfiguredTrackMonitorSequence
            ConfiguredTrackMonitorSequence(Name='MoniTrSeq')


        if "PROTO" in moniSeq :
            from Configurables import ( ChargedProtoParticleMoni, GaudiSequencer,
                                        ANNGlobalPID__ChargedProtoANNPIDMoni )
            seq = GaudiSequencer( "MoniPROTOSeq" )
            seq.Members += [ ChargedProtoParticleMoni("ChargedProtoPMoni"),
                             ANNGlobalPID__ChargedProtoANNPIDMoni("ChargedProtoANNPIDMoni") ]
            if self.expertHistos() and not self.getProp("CheckEnabled") :
                exSeq = GaudiSequencer("ExpertProtoMoni")
                seq.Members += [exSeq]
                GlobalRecoChecks().Sequencer = exSeq

        # If checking is enabled, all Rich histograms are booked in check sequence
        if "RICH" in moniSeq and not self.getProp("CheckEnabled"):
            from Configurables import GaudiSequencer
            conf = RichRecQCConf(self.richMoniConfName)
            self.setOtherProps(conf,["Histograms","Context","OutputLevel",
                                     "DataType","Simulation","OnlineMode"])
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
