
## @package RecSys
#  High level configuration tools for REC
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.4 2008-11-14 17:11:19 jonrob Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from LHCbKernel.Configuration import *
from TrackSys.Configuration   import *
from RichRecSys.Configuration import *
from Configurables import ProcessPhase

## @class RecSysConf
#  Configurable for LHCb reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RecSysConf(LHCbConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ TrackSys, RichRecSysConf ]

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

        # Primary Vertex
        if "Vertex" in recoSeq:
            from Configurables import PatPVOffline
            GaudiSequencer("RecoVertexSeq").Members += [ PatPVOffline() ];

        # Tracking (Should make it more fine grained ??)
        DoTracking = False
        for seq in self.DefaultTrackingSubdets:
            if seq in recoSeq: DoTracking = True
        if DoTracking:
            trackConf = TrackSys()
            self.setOtherProps(trackConf,["ExpertHistos","SpecialData"])

        # RICH
        if "RICH" in recoSeq:
            richConf = RichRecSysConf()
            self.setOtherProps(richConf,["SpecialData","Context"])
            richConf.RecoSequencer = GaudiSequencer("RecoRICHSeq")
            
        # CALO
        if "CALO" in recoSeq:
            importOptions("$CALORECOOPTS/Brunel.opts")
            importOptions("$CALOPIDSOPTS/PhotonPDF.opts")

        # MUON
        if "MUON" in recoSeq:
            GaudiSequencer("RecoMUONSeq").Members += [ "MuonRec", "MuonID" ]
            importOptions("$MUONIDROOT/options/MuonID.opts")

        # PROTO
        if "PROTO" in recoSeq: 
            importOptions("$GLOBALRECOOPTS/Reco.opts")
