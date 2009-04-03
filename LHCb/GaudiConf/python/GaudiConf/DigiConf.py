"""
Configurable for Boole output
"""
__version__ = "$Id: DigiConf.py,v 1.2 2009-04-03 15:01:40 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions

class DigiConf(ConfigurableUser):
    __slots__ = {
         "DigiType"       : "Default"
       , "Writer"         : "NONE"
       , "TAEPrev"        : 0
       , "TAENext"        : 0
       , "SpilloverPaths" : []
         }

    _propertyDocDct = { 
        'DigiType'      : """ Type of digi, can be ['Minimal','Default','Extended'] """
       ,'Writer'        : """ Name of OutputStream writing the Digi file, if any """
       ,'TAEPrev'       : """ Number of Prev Time Alignment Events to write """
       ,'TAENext'       : """ Number of Next Time Alignment Events to write """
       ,'SpilloverPaths': """ Paths to write out when spillover is enabled """
       }

    KnownDigiTypes = ['Minimal','Default','Extended']

    def _doWrite( self ):
        """
        Define the file content and write it out
        """
        if self.getProp( "Writer" ) == "NONE":
            log.info( "No Writer defined for DIGI" )
            return
        
        dType = self.getProp( "DigiType" ).capitalize()
        if dType not in self.KnownDigiTypes:
            raise TypeError( "Unknown DIGI type '%s'"%dType )

        items    = []
        optItems = []
        self._defineOutputData( dType, items, optItems )
        
        self._doWritePOOL( dType, items, optItems )
            

    def _defineOutputData( self, dType, items, optItems ):
        """
        Define content of the output dataset
        """
        # Pack pSim containers for the output if not on the input file
        DataOnDemandSvc().AlgMap["pSim/MCParticles"] = "PackMCParticle"
        DataOnDemandSvc().AlgMap["pSim/MCVertices"]  = "PackMCVertex"

        # Start with minimal content (real data plus pileup info)
        items += [ 
            # Objects propagated from Gauss
              "/Event/Gen/Header#1"
            , "/Event/MC/Header#1"

            # Boole header
            , "/Event/MC/DigiHeader#1"

            # Real data simulation
            , "/Event/DAQ/RawEvent#1" ]

        if dType == "Minimal":
            items += [ "/Event/MC/Vertices#1" ] # Filtered, only primary vertices with no daughters

        else:
            # Standard DIGI content
            items += [ 
                # Objects propagated from Gauss
                  "/Event/Gen/Collisions#1"
                , "/Event/Gen/HepMCEvents#1"
                , "/Event/pSim/MCParticles#1"
                , "/Event/pSim/MCVertices#1"
 
                # Digitization summaries
                , "/Event/MC/Rich/DigitSummaries#1"
                , "/Event/MC/Muon/DigitsInfo#1"

                # Links to MCParticles
                , "/Event/Link/Trig/L0/Calo#1"
                , "/Event/Link/Trig/L0/FullCalo#1"
                , "/Event/Link/Raw/Velo/Clusters#1"
                , "/Event/Link/Raw/TT/Clusters#1"
                , "/Event/Link/Raw/IT/Clusters#1"
                , "/Event/Link/Raw/OT/Times#1"
                , "/Event/Link/Raw/Ecal/Digits#1"
                , "/Event/Link/Raw/Hcal/Digits#1"
                , "/Event/Link/Raw/Muon/Digits#1"
                , "/Event/MC/TrackInfo#1"

                # Links to MCHits
                , "/Event/Link/Raw/Velo/Clusters2MCHits#1"
                , "/Event/Link/Raw/TT/Clusters2MCHits#1"
                , "/Event/Link/Raw/IT/Clusters2MCHits#1"
                , "/Event/Link/Raw/OT/Times2MCHits#1" ]

            # Spillover event structure, copied from .sim if present
            for spill in self.getProp("SpilloverPaths"):
                optItems += [ "/Event/%s/MC/Header#1"%spill ]

            # Add TAE RawEvents when enabled
            taePrev = self.getProp("TAEPrev")
            while taePrev > 0:
                items += ["/Event/Prev%s/DAQ/RawEvent#1"%taePrev]
                taePrev -= 1
            taeNext = self.getProp("TAENext")
            while taeNext>0:
                items += ["/Event/Next%s/DAQ/RawEvent#1"%taeNext]
                taeNext -= 1

        if dType == "Extended":
            # Add the MCHits
            items += [
                  "/Event/MC/PuVeto/Hits#1"
                , "/Event/MC/Velo/Hits#1"
                , "/Event/MC/TT/Hits#1"
                , "/Event/MC/IT/Hits#1"
                , "/Event/MC/OT/Hits#1"
                , "/Event/MC/Rich/Hits#1"
                , "/Event/MC/Prs/Hits#1"
                , "/Event/MC/Spd/Hits#1"
                , "/Event/MC/Ecal/Hits#1"
                , "/Event/MC/Hcal/Hits#1"
                , "/Event/MC/Muon/Hits#1" ]
            

    def _doWritePOOL( self, dType, items, optItems ):
        """
        Write a file in POOL format
        """
        writer = OutputStream( self.getProp("Writer") )
        ApplicationMgr().OutStream.insert( 0, writer )
        writer.Preload = False
        writer.ItemList += items
        writer.OptItemList += optItems
        log.info( "%s.ItemList=%s"%(self.getProp("Writer"),items) )
        log.info( "%s.OptItemList=%s"%(self.getProp("Writer"),optItems) )

        # In Minimal case, need to kill some nodes
        if dType == "Minimal":
            from Configurables import EventNodeKiller
            nodeKiller = EventNodeKiller("POOLNodeKiller")
            ApplicationMgr().OutStream.insert( 0, nodeKiller )
            nodeKiller.Nodes += [ "Link", "pSim" ]
            nodeKiller.Nodes += [ "MC/Velo", "MC/PuVeto", "MC/TT", "MC/IT", "MC/OT", "MC/Rich", "MC/Prs", "MC/Spd", "MC/Ecal", "MC/Hcal", "MC/Muon" ]
            nodeKiller.Nodes += self.getProp("SpilloverPaths")

    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOn()
        self._doWrite()
        GaudiKernel.ProcessJobOptions.PrintOff()
