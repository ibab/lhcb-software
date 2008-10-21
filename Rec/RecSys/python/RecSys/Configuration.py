
## @package RecSys
#  High level configuration tools for REC
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.3 2008-10-21 19:34:43 jonrob Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from LHCbKernel.Configuration import *
from TrackSys.Configuration   import *
from RichRecSys.Configuration import *
from Configurables import ( ProcessPhase, PatPVOffline )

## @class RecSysConf
#  Configurable for LHCb reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RecSysConf(LHCbConfigurableUser):

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
        "recoSequence" : []      # The Sub-detector sequencing. Default is all known
       ,"specialData"  : []      # Various special data processing options. See KnownSpecialData for all options
       ,"expertHistos":  False   # set to True to write out expert histos
       ,"context":     "Offline" # The context within which to run the reco sequences
        }

    ## Apply the configuration
    def applyConf(self):

        # Check the special data options
        for option in self.getProp("specialData"):
            if option not in self.KnownSpecialData:
                raise RuntimeError("Unknown SpecialData option '%s'"%option)
        
        # Phases
        if len(self.getProp("recoSequence")) == 0 :
            if "fieldOff" in self.getProp("specialData"):
                self.setProp("recoSequence",self.DefaultSubDetsFieldOff)
            else:
                self.setProp("recoSequence",self.DefaultSubDetsFieldOn)
        recoSeq = self.getProp("recoSequence")
        ProcessPhase("Reco").DetectorList += recoSeq

        # Primary Vertex
        if "Vertex" in recoSeq:
            GaudiSequencer("RecoVertexSeq").Members += [ PatPVOffline() ];
     
        # Tracking (Should make it more fine grained ??)
        DoTracking = False
        for seq in self.DefaultTrackingSubdets:
            if seq in recoSeq: DoTracking = True
        if DoTracking:
            trackConf = TrackSys()
            self.setOtherProps(trackConf,["expertHistos","specialData"])
            trackConf.applyConf()
            # delete config object (to allow reuse from defaults)
            del trackConf

        # RICH
        if "RICH" in recoSeq:
            richConf = RichRecSysConf()
            self.setOtherProps(richConf,["expertHistos","specialData","context"])
            richConf.applyConf(GaudiSequencer("RecoRICHSeq"))
            # delete config object (to allow reuse from defaults)
            del richConf

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
