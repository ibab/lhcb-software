"""
Configurable for Boole output
"""
__version__ = "$Id: DigiConf.py,v 1.13 2009-12-17 15:16:11 cattanem Exp $"
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
       , "PackSequencer"  : None
       , "Detectors"      : ['Velo','TT','IT','OT','Rich','Tr','Calo','Muon','L0']
       , "DataType"       : ""
       , "OutputName"     : ""
       , "WriteFSR"       : False
         }

    _propertyDocDct = { 
        'DigiType'      : """ Type of digi, can be ['Minimal','Default','Extended'] """
       ,'Writer'        : """ Name of OutputStream writing the Digi file, if any """
       ,'TAEPrev'       : """ Number of Prev Time Alignment Events to write """
       ,'TAENext'       : """ Number of Next Time Alignment Events to write """
       ,'SpilloverPaths': """ Paths to write out when spillover is enabled """
       ,'EnablePack'    : """ Turn on/off packing of the DIGI data (where appropriate/available) """
       ,'EnableUnpack'  : """ Configure the SIM unpacking via the Data On Demand Service """
       ,'PackSequencer' : """ Sequencer in which to run the packing algorithms """
       ,'Detectors'     : """ Active subdetectors """
       ,'DataType'      : """ Flag for backward compatibility with old data """
       ,'OutputName'    : """ Name of the output file """
       ,'WriteFSR'      : """ Flags whether to write out an FSR """
       }

    __used_configurables__ = [ SimConf ]

    KnownDigiTypes = ['Minimal','Default','Extended']
    KnownSpillPaths= [ "Prev", "PrevPrev", "Next", "NextNext" ]

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

        if self.getProp( "EnablePack" ):
            if not hasattr( self, "PackSequencer" ):
                raise TypeError( "Packing requested but PackSequencer not defined" )

        from GaudiConf.IOHelper import IOHelper
        writer = OutputStream( self.getProp("Writer") )
        writer.Preload = False

        # Set a default file name if not already set
        if not writer.isPropertySet("Output") :
            outputFile = self.getProp("OutputName")
            outputFile = "PFN:"+outputFile + '.digi'
        else:
            outputFile = IOHelper().undressFile( writer.getProp("Output") )

        # Add to the ApplicationMgr with correct output persistency
        algs = IOHelper().outputAlgs( outputFile, "OutputStream/"+self.getProp("Writer"), self.getProp("WriteFSR") )
        for alg in algs:
            ApplicationMgr().OutStream.insert( 0, alg )

        self._defineOutputData( dType, writer )
        
        self._doWritePOOL( dType, writer )

    def addHeaders( self, writer ):

        # Boole header
        writer.ItemList += [ "/Event/MC/DigiHeader#1" ]

    def addMCDigitSummaries( self, writer ):
        simDir = "MC"
        if (
            (self.getProp("EnablePack"))
            and
            ("Rich" in self.getProp("Detectors"))
            ):
            simDir = "pSim"
            packDigi = self.getProp("PackSequencer")
            from Configurables import DataPacking__Pack_LHCb__MCRichDigitSummaryPacker_
            RichSumPack = DataPacking__Pack_LHCb__MCRichDigitSummaryPacker_("MCRichDigitSummaryPacker")
            packDigi.Members += [ RichSumPack ]
            # Kill the unpacked Rich node
            from Configurables import EventNodeKiller
            nodeKiller = EventNodeKiller("MCRichNodeKiller")
            nodeKiller.Nodes += [ "MC/Rich" ]
            packDigi.Members += [ nodeKiller ]

        # Digitization summaries
        if "Rich" in self.getProp("Detectors"):
            writer.ItemList += ["/Event/"+simDir+"/Rich/DigitSummaries#1"]
        if "Muon" in self.getProp("Detectors"):
            writer.ItemList += ["/Event/MC/Muon/DigitsInfo#1"]

    def addMCParticleLinks( self, writer ):
        
        # Links to MCParticles
        dets = self.getProp("Detectors")
        if 'Velo' in dets :
            writer.ItemList += ["/Event/Link/Raw/Velo/Clusters#1"]
        if 'VP' in dets :
            writer.ItemList += ["/Event/Link/Raw/VP/Digits#1"]
        if 'L0' in dets :
            writer.OptItemList += ["/Event/Link/Trig/L0/Calo#1"]
            writer.OptItemList += ["/Event/Link/Trig/L0/FullCalo#1"]
        if 'TT' in dets :
            writer.ItemList += ["/Event/Link/Raw/TT/Clusters#1"]
        if 'UT' in dets :
            writer.ItemList += ["/Event/Link/Raw/UT/Clusters#1"]
        if 'IT' in dets :
            writer.ItemList += ["/Event/Link/Raw/IT/Clusters#1"]
        if 'OT' in dets :
            writer.ItemList += ["/Event/Link/Raw/OT/Times#1"]
        if 'FT' in dets :
            writer.ItemList += ["/Event/Link/Raw/FT/Clusters#1"]
        if 'Muon' in dets :
            writer.ItemList += ["/Event/Link/Raw/Muon/Digits#1"]

    def addMCTrackInfo( self, writer ):
        
        # Information for Tracking efficiencies and ghost rates
        dets = self.getProp("Detectors")
        if 'Tr' in dets :
            writer.ItemList += ["/Event/MC/TrackInfo#1"]

    def addMCCaloInfo( self, writer ):
        # For calo matching in DaVinci
        dets = self.getProp("Detectors")
        if 'Calo' in dets :
            writer.ItemList += ["/Event/Link/Raw/Ecal/Digits#1"]
            writer.ItemList += ["/Event/Link/Raw/Hcal/Digits#1"]
                    
    def addMCHitLinks( self, writer ):

        # Links to MCHits
        dets = self.getProp("Detectors")
        if 'Velo' in dets :
            writer.ItemList += ["/Event/Link/Raw/Velo/Clusters2MCHits#1"]
        if 'VP' in dets :
            writer.ItemList += ["/Event/Link/Raw/VP/Digits2MCHits#1"]
        if 'TT' in dets :
            writer.ItemList += ["/Event/Link/Raw/TT/Clusters2MCHits#1"]
        if 'UT' in dets :
            writer.ItemList += ["/Event/Link/Raw/UT/Clusters2MCHits#1"]
        if 'IT' in dets :
            writer.ItemList += ["/Event/Link/Raw/IT/Clusters2MCHits#1"]
        if 'OT' in dets :
            writer.ItemList += ["/Event/Link/Raw/OT/Times2MCHits#1"]
        if 'FT' in dets :
            writer.ItemList += ["/Event/Link/Raw/FT/Clusters2MCHits#1"]
            
            

    def _defineOutputData( self, dType, writer ):
        """
        Define content of the output dataset
        """
        simDir = "MC"
        if self.getProp("EnablePack") :
            simDir = "pSim"
            packDigi = self.getProp("PackSequencer")
            from Configurables import PackMCVertex
            mcVertPacker = PackMCVertex()
            packDigi.Members += [mcVertPacker]
            if dType != "Minimal":
                from Configurables import PackMCParticle
                mcPartPacker = PackMCParticle()
                packDigi.Members += [mcPartPacker]

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
            self.addMCCaloInfo(writer)

            # Reconstructible Info
            self.addMCTrackInfo(writer)

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

            # Enable packing of MCHits in main event (for old data)
            if self.getProp( "EnablePack" ):
                SimConf().PackingSequencers[''] =  self.getProp("PackSequencer")
            
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
            nodeKiller.Nodes += [ "Link" ]
            simDir = "MC"
            if self.getProp("EnablePack") : simDir = "pSim"
            nodeKiller.Nodes += [ simDir+"/MCParticles" ]
            nodeKiller.Nodes += [ simDir+"/Velo", simDir+"/PuVeto", simDir+"/TT", simDir+"/IT",
                                  simDir+"/OT", simDir+"/Rich", simDir+"/Prs", simDir+"/Spd",
                                  simDir+"/Ecal", simDir+"/Hcal", simDir+"/Muon",
                                  simDir+"VP", simDir+"UT", simDir+"FT"
                                  ]
            nodeKiller.Nodes += self.KnownSpillPaths

    def _doUnpacking(self):
        if "Rich" in self.getProp("Detectors"):
            from Configurables import DataPacking__Unpack_LHCb__MCRichDigitSummaryPacker_
            unp = DataPacking__Unpack_LHCb__MCRichDigitSummaryPacker_("MCRichDigitSummaryUnpacker")
            DataOnDemandSvc().AlgMap["MC/Rich/DigitSummaries"] = unp

    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOn()
        self.setOtherProps(SimConf(),["SpilloverPaths","EnableUnpack","EnablePack","DataType"])
        self._doWrite()
        if self.getProp("EnableUnpack") : self._doUnpacking()
        GaudiKernel.ProcessJobOptions.PrintOff()
