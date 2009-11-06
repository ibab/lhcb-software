"""
Configurable for Boole output
"""
__version__ = "$Id: DigiConf.py,v 1.9 2009-11-06 14:00:36 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

__all__ = [
    'DigiConf'  ## the configurable, configures DIGI writing/packing/unpacking   
    ]

from Gaudi.Configuration import *
from Configurables import LHCbConfigurableUser
from SimConf import SimConf
import GaudiKernel.ProcessJobOptions

class DigiConf(LHCbConfigurableUser):
    
    __slots__ = {
         "DigiType"       : "Default"
       , "Writer"         : "NONE"
       , "TAEPrev"        : 0
       , "TAENext"        : 0
       , "SpilloverPaths" : []
       , "EnablePack"     : True
       , "EnableUnpack"   : True
         }

    _propertyDocDct = { 
        'DigiType'      : """ Type of digi, can be ['Minimal','Default','Extended'] """
       ,'Writer'        : """ Name of OutputStream writing the Digi file, if any """
       ,'TAEPrev'       : """ Number of Prev Time Alignment Events to write """
       ,'TAENext'       : """ Number of Next Time Alignment Events to write """
       ,'SpilloverPaths': """ Paths to write out when spillover is enabled """
       ,'EnablePack'    : """ Turn on/off packing of the DIGI data (where appropriate/available) """
       ,'EnableUnpack'  : """ Configure the SIM unpacking via the Data On Demand Service """
       }

    __used_configurables__ = [ SimConf ]

    KnownDigiTypes = ['Minimal','Default','Extended']
    KnownSpillPaths= [ "Prev", "PrevPrev", "Next" ]

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

        # get the write instance
        writer = OutputStream( self.getProp("Writer") )
        ApplicationMgr().OutStream.insert( 0, writer )
        writer.Preload = False

        self._defineOutputData( dType, writer )
        
        self._doWritePOOL( dType, writer )

    def addHeaders( self, writer ):

        # Boole header
        writer.ItemList += [ "/Event/MC/DigiHeader#1" ]

    def addMCDigitSummaries( self, writer ):

        simDir = "MC"
        if self.getProp("EnablePack") : simDir = "pSim"

        # Pack RICH summary info if not already done
        from Configurables import DataPacking__Pack_LHCb__MCRichDigitSummaryPacker_
        RichSumPack = DataPacking__Pack_LHCb__MCRichDigitSummaryPacker_("MCRichDigitSummaryPacker")
        DataOnDemandSvc().AlgMap["pSim/Rich/DigitSummaries"] = RichSumPack
        
        writer.ItemList += [ 
            # Digitization summaries
            "/Event/"+simDir+"/Rich/DigitSummaries#1"
            , "/Event/MC/Muon/DigitsInfo#1"
            ]

    def addMCParticleLinks( self, writer ):
        
        writer.ItemList += [ 
            # Links to MCParticles
            "/Event/Link/Trig/L0/Calo#1"
            , "/Event/Link/Trig/L0/FullCalo#1"
            , "/Event/Link/Raw/Velo/Clusters#1"
            , "/Event/Link/Raw/TT/Clusters#1"
            , "/Event/Link/Raw/IT/Clusters#1"
            , "/Event/Link/Raw/OT/Times#1"
            , "/Event/Link/Raw/Ecal/Digits#1"
            , "/Event/Link/Raw/Hcal/Digits#1"
            , "/Event/Link/Raw/Muon/Digits#1"
            , "/Event/MC/TrackInfo#1"
            ]

    def addMCHitLinks( self, writer ):

        writer.ItemList += [ 
            # Links to MCHits
            "/Event/Link/Raw/Velo/Clusters2MCHits#1"
            , "/Event/Link/Raw/TT/Clusters2MCHits#1"
            , "/Event/Link/Raw/IT/Clusters2MCHits#1"
            , "/Event/Link/Raw/OT/Times2MCHits#1"
            ]

    def _defineOutputData( self, dType, writer ):
        """
        Define content of the output dataset
        """

        # Pack pSim containers for the output if not on the input file
        DataOnDemandSvc().AlgMap["pSim/MCParticles"] = "PackMCParticle"
        DataOnDemandSvc().AlgMap["pSim/MCVertices"]  = "PackMCVertex"

        simDir = "MC"
        if self.getProp("EnablePack") : simDir = "pSim"

        # Headers propagated from Gauss
        SimConf().addHeaders(writer)

        # Start with minimal content (real data plus pileup info)
        self.addHeaders(writer)
        writer.ItemList += [
            # Real data simulation
            "/Event/DAQ/RawEvent#1" ]

        if dType == "Minimal":

            SimConf().addMCVertices(writer,['']) # Filtered, only primary vertices with no daughters

        else:
            # Standard DIGI content
                
            # Generator info
            SimConf().addGenInfo(writer)

            # General MC simulation information. Spillover added only to Extended format
            locations = [ '' ]
            if dType == "Extended":
                locations = SimConf().allEventLocations()
            SimConf().addGeneralSimInfo(writer,locations)

            # Summary info
            self.addMCDigitSummaries(writer)

            # MCParticle links
            self.addMCParticleLinks(writer)

            # MCHit Links
            self.addMCHitLinks(writer)

            # Add TAE RawEvents when enabled
            taePrev = self.getProp("TAEPrev")
            while taePrev > 0:
                writer.ItemList += ["/Event/Prev%s/DAQ/RawEvent#1"%taePrev]
                taePrev -= 1
            taeNext = self.getProp("TAENext")
            while taeNext>0:
                writer.ItemList += ["/Event/Next%s/DAQ/RawEvent#1"%taeNext]
                taeNext -= 1

        if dType == "Extended":
            
            # Add the sub detector MCHits
            SimConf().addSubDetSimInfo(writer)
            

    def _doWritePOOL( self, dType, writer ):
        """
        Write a file in POOL format
        """
        log.info( "%s.ItemList    = %s"%(self.getProp("Writer"),writer.ItemList) )
        log.info( "%s.OptItemList = %s"%(self.getProp("Writer"),writer.OptItemList) )

        # In Minimal case, need to kill some nodes
        if dType == "Minimal":
            from Configurables import EventNodeKiller
            nodeKiller = EventNodeKiller("POOLNodeKiller")
            ApplicationMgr().OutStream.insert( 0, nodeKiller )
            nodeKiller.Nodes += [ "Link", "pSim" ]
            simDir = "MC"
            if self.getProp("EnablePack") : simDir = "pSim"
            nodeKiller.Nodes += [ simDir+"/Velo", simDir+"/PuVeto", simDir+"/TT", simDir+"/IT",
                                  simDir+"/OT", simDir+"/Rich", simDir+"/Prs", simDir+"/Spd",
                                  simDir+"/Ecal", simDir+"/Hcal", simDir+"/Muon" ]
            nodeKiller.Nodes += self.KnownSpillPaths

    def _doUnpacking(self):

        from Configurables import DataPacking__Unpack_LHCb__MCRichDigitSummaryPacker_
        unp = DataPacking__Unpack_LHCb__MCRichDigitSummaryPacker_("MCRichDigitSummaryUnpacker")
        DataOnDemandSvc().AlgMap["MC/Rich/DigitSummaries"] = unp

    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOn()
        self.setOtherProps(SimConf(),["SpilloverPaths","EnableUnpack","EnablePack"])
        self._doWrite()
        if self.getProp("EnableUnpack") : self._doUnpacking()
        GaudiKernel.ProcessJobOptions.PrintOff()
