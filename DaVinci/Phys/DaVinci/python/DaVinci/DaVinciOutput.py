"""
High level configuration tools for LHCb applications
"""
__version__ = "$Id: DaVinciOutput.py,v 1.1 2009-02-04 18:48:01 pkoppenb Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration import *
from Configurables import ( DstConf )

class DaVinciOutput(ConfigurableUser):
    __slots__ = {
         "Items"              : []            # Mandatory items to write to DST
    ,    "OptItems"           : []            # Optional items to write to DST
    ,    "RemovedItems"       : []            # Items not to write to DST
        }

    _propertyDocDct = { }
    __used_configurables__ = [ DstConf ]
    KnownDstNames = [ "dst", "DST" ]
    UnsureItems = [ "/Vertex/V0" ]

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
    # ensure backward compatibility with previous versions of Brunel
    #
    def reshuffleItems(self, items, optItems):
        itemsTMP = items
        optItemsTMP = optItems
        for r in self.RemovedItems :
            for i in itemsTMP :
                 if (i.find(r)>=0):
                    log.info("Removing "+i+" from Items")
                    items.remove(i)
            for i in optItemsTMP :
                 if (i.find(r)>=0):
                    log.info("Removing "+i+" from optional Items")
                    optItems.remove(i)
        itemsTMP = items
        for u in self.UnsureItems :
            for i in itemsTMP :
                 if (i.find(u)>=0):
                    log.info("Moving "+i+" to optional items")
                    items.remove(i)
                    optItems.append(i)
        self.setProp("Items", items)
        self.setProp("OptItems", optItems)
    
    ############################################################################
    # Configure Writer
    #
    def configureWriter(self):
        pType = "NONE" # @todo packing not supported yet
        if (pType!="NONE"):  # packing not supprted yet
            DstConf().PackSequencer = seq
        items = self.getProp("Items")
        optItems = self.getProp("OptItems")
        DstConf()._defineOutputData( "DST", pType, items, optItems )
        self.reshuffleItems(items, optItems)
    
    ############################################################################
    # Dst writing
    #
    def writeDst(self,filename,seq):
        """
        Write out a DST using a sequence
        """
        log.info("DaVinciOutput::writeDst "+filename)
        stream = self.streamName(filename)
        from Configurables import ApplicationMgr, OutputStream
        writer = OutputStream( stream )
        writer.RequireAlgs = [ seq ]
        ApplicationMgr().OutStream += [ writer ]
        writer.RequireAlgs = [ seq.getName() ]  # require the sequencer 
        writer.Output = "DATAFILE='PFN:" + filename + "' TYP='POOL_ROOTTREE' OPT='REC'"
        writer.Preload = False
        writer.ItemList = self.getProp("Items")
        writer.OptItemList = self.getProp("OptItems")
        print self.getProp("OptItems")
        return seq

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

