"""
Output for DaVinci
"""
__version__ = "$Id: DaVinciOutput.py,v 1.3 2009-02-06 17:34:24 pkoppenb Exp $"
__author__  = "Patrick Koppenburg <Patrick.Koppenburg@cern.ch>"

from Gaudi.Configuration import *
from Configurables import ( DstConf )

class DaVinciOutput(ConfigurableUser):
    __slots__ = {
         "Items"              : []            # Items to write to DST
    ,    "RemovedItems"       : [ "Vertex/V0"]            # Items not to write to DST
    ,    "PackType"           : "NONE"        # Packing type
        }

    _propertyDocDct = { }
    __used_configurables__ = [ DstConf ]
    KnownDstNames = [ "dst", "DST" ]

    def __apply_configuration__(self):
        log.info("Do nothing")

    ############################################################################
    # Output Stream Name
    #
    def streamName(self,filename):
        """
        Strip off extension from filename
        """
        f1 = filename.find('.')
        if (f1<=0):
            raise TypeError( "Invalid finename '%s'"%filename )
        head = filename[:f1]
        tail = filename[f1+1:]
        if ( tail not in self.KnownDstNames ):
            log.warning(filename+" has unexpected extension ``"+tail+"''. Expecting DST or dst.")
        return "DST"+head
    
    ############################################################################
    # Reshuffle Items
    def defineItems(self):
        """
        Define TES locations to be put on the DST
        Ensure backward compatibility with previous versions of Brunel
        What works is to declare all mandatory items in the Dst as
        optional. If they are there they get copied.
        """
        # 1 get items from DstConf
        items = []
        optItems = []
#        DstConf()._defineOutputData( "DST", self.getProp("PackType"), items, optItems )
        # 2. merge all items.
        items.extend(optItems)
        itemsTMP = items
        # 3. remove to be removed items
        for r in self.getProp("RemovedItems") :
            for i in itemsTMP :
                 if (i.find(r)>=0):
                    log.info("Removing "+i+" from Items")
                    items.remove(i)
        # 4. add user items
        items.extend(self.getProp("Items"))
        print "# will write", items
        self.setProp("Items",items)
        
    ############################################################################
    # Configure Writer
    def configureWriter(self):
        """
        Configure Writer:
        """
        #
        # This is hack : DstWriter wants a sequencer to be defined,
        # even if I don't want it
        #
        if (self.getProp("PackType")!="NONE"):  
            DstConf().PackSequencer = GaudiSequencer("PackDST")
        self.defineItems()
    
    ############################################################################
    # Dst writing
    #
    def writeDst(self,filename,seq):
        """
        Write out a DST using a sequence
        """
        log.info("DaVinciOutput::writeDst "+filename)
        stream = self.streamName(filename)
        # InputCopyStream ignores the rest on the TES
        from Configurables import ApplicationMgr, InputCopyStream   
        writer = InputCopyStream( stream )
        ApplicationMgr().OutStream += [ writer ]
        writer.RequireAlgs = [ seq.getName() ]  # require the sequencer 
        writer.Output = "DATAFILE='PFN:" + filename + "' TYP='POOL_ROOTTREE' OPT='REC'"
        writer.OptItemList = self.getProp("Items")
# suggetsed by Marco. Say nothing produced fewer errors
#        writer.ItemList = [ '/Event/MC#999', '/Event/DAQ#999', '/Event/Rec#999', '/Event/Link#999',
#                            '/Event/pSim#999' ]
        writer.TakeOptionalFromTES = True
        writer.Preload = True
        writer.PreloadOptItems = True
#        writer.OutputLevel = 1 
        return 

    ############################################################################
    # ETC writing
    #
    def writeETC(self,filename,seq):
        """
        Write out a ETC using a sequence
        """
        log.info("DaVinciOutput::writeETC "+filename)
        log.warning("DaVinciOutput::writeETC not yet implemented")
        return seq

