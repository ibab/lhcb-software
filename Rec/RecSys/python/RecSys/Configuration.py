
## @package RecSys
#  High level configuration tools for REC
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.12 2009-07-01 18:42:41 polye Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from LHCbKernel.Configuration import *
from TrackSys.Configuration   import *
from RichRecSys.Configuration import *
from GlobalReco.Configuration import *
from Configurables import ProcessPhase

## @class RecSysConf
#  Configurable for LHCb reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RecSysConf(LHCbConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ GlobalRecoConf, TrackSys, RichRecSysConf ]

    ## Default tracking Sub-detector processing sequence
    DefaultTrackingSubdets = ["VELO","TT","IT","OT","Tr","Vertex"]
    ## Default reconstruction sequence for field-on data
    DefaultSubDetsFieldOn  = DefaultTrackingSubdets+["RICH","CALO","MUON","PROTO"]
    ## Default reconstruction sequence for field-off data
    DefaultSubDetsFieldOff = DefaultTrackingSubdets+["CALO","RICH","MUON","PROTO"]
    ## List of known special data processing options
    KnownSpecialData = [ "cosmics", "veloOpen", "fieldOff" ]
    
    ## Steering options
    __slots__ = {
        "RecoSequence" : []      # The Sub-detector sequencing. Default is all known
       ,"SpecialData"  : []      # Various special data processing options. See KnownSpecialData for all options
       ,"ExpertHistos":  False   # set to True to write out expert histos
       ,"Context":     "Offline" # The context within which to run the reco sequences
       ,"OutputType": ""         # some sequences are different for RDST
       ,"DataType": ""           # Type of data, propagated from application
       ,"OutputLevel" : INFO     # The printout level to use
        }

    ## Apply the configuration
    def applyConf(self):

        # Check the special data options
        for option in self.getProp("SpecialData"):
            if option not in self.KnownSpecialData:
                raise RuntimeError("Unknown SpecialData option '%s'"%option)
        
        # Phases
        if len(self.getProp("RecoSequence")) == 0 :
            if "fieldOff" in self.getProp("SpecialData"):
                self.setProp("RecoSequence",self.DefaultSubDetsFieldOff)
            else:
                self.setProp("RecoSequence",self.DefaultSubDetsFieldOn)
        recoSeq = self.getProp("RecoSequence")
        ProcessPhase("Reco").DetectorList += recoSeq

        # Primary Vertex and V0 finding
        if "Vertex" in recoSeq:
            from Configurables import PatPVOffline, TrackV0Finder
            pvAlg = PatPVOffline()
            GaudiSequencer("RecoVertexSeq").Members += [ pvAlg ];
            if self.getProp( "OutputType" ).upper() == "RDST":
                # Velo tracks not copied to Rec/Track/Best for RDST 
                from Configurables import PVOfflineTool
                pvAlg.addTool( PVOfflineTool() )
                pvAlg.PVOfflineTool.InputTracks = [ "Rec/Track/Best", "Rec/Track/PreparedVelo" ]
            trackV0Finder = TrackV0Finder()
            GaudiSequencer("RecoVertexSeq").Members += [ trackV0Finder ]
            # the TrackV0Finder use simplified material for extra/interpolation
            trackV0Finder.Extrapolator.MaterialLocator='SimplifiedMaterialLocator'
            trackV0Finder.Interpolator.Extrapolator.MaterialLocator='SimplifiedMaterialLocator'

        # Tracking (Should make it more fine grained ??)
        DoTracking = False
        for seq in self.DefaultTrackingSubdets:
            if seq in recoSeq: DoTracking = True
        if DoTracking:
            trackConf = TrackSys()
            self.setOtherProps(trackConf,["ExpertHistos","SpecialData","OutputType"])

        # RICH
        if "RICH" in recoSeq:
            richConf = RichRecSysConf()
            self.setOtherProps(richConf,["SpecialData","Context","OutputLevel"])
            richConf.RecoSequencer = GaudiSequencer("RecoRICHSeq")
            
        # CALO
        if "CALO" in recoSeq:
            importOptions("$CALORECOOPTS/Brunel.opts")
            if self.getProp("DataType") == "DC06":
                importOptions("$CALORECOOPTS/CaloPIDs-DC06.opts")
            else:
                importOptions("$CALORECOOPTS/CaloPIDs.opts")
            importOptions("$CALOPIDSOPTS/PhotonPDF.opts")
            
        # MUON
        if "MUON" in recoSeq:
            from MuonID import ConfiguredMuonIDs
            cm=ConfiguredMuonIDs.ConfiguredMuonIDs(data=self.getProp("DataType"))
            MuonIDSeq=cm.getMuonIDSeq()
            GaudiSequencer("RecoMUONSeq").Members += [ "MuonRec", MuonIDSeq ]

        # PROTO
        if "PROTO" in recoSeq:
            self.setOtherProps(GlobalRecoConf(),["SpecialData","Context","OutputLevel"])
            GlobalRecoConf().RecoSequencer = GaudiSequencer("RecoPROTOSeq")
