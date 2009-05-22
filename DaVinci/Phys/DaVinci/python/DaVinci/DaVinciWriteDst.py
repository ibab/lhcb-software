"""
Dst Writing for DaVinci
"""
__version__ = "$Id: DaVinciWriteDst.py,v 1.5 2009-05-22 15:24:20 pkoppenb Exp $"
__author__  = "Patrick Koppenburg <Patrick.Koppenburg@cern.ch>"

from Gaudi.Configuration import *
from Configurables import ( DstConf, DaVinci )

class DaVinciWriteDst(ConfigurableUser):
    __slots__ = {
         "DstFiles"           : {}            # Name and sequence of output files
       , "Items"              : []            # Additional items to write to DST
         }
    
    _propertyDocDct = {
        "DstFiles"           : """ Write name of output Dst file and sequence to be run
                                   e.g. DaVinci().DstFiles = { 'Name1.dst' : sequence1 ,
                                                               'Name2.dst' : sequence2 }
                                   where sequenceX are GaudiSequencers"""
       , "Items"              : """ Additional items to write to DST.
                                     Example = [ '/Event/Phys/StdUnbiasedJpsi2MuMu#2' ]
                                     Note the #2 !"""
        }
    __used_configurables__ = [ DstConf, DaVinci ]
    KnownDstNames = [ "dst", "DST" ]

    def __apply_configuration__(self):
        self.dstSequence()

    ############################################################################
    # WriteDst Stream Name
    #
    def streamName(self,filename):
        """
        Strip off extension from filename
        """
        f1 = filename.find('.')
        if (f1<=0):
            raise TypeError( "Invalid filename '%s'"%filename )
        head = filename[:f1]
        tail = filename[f1+1:]
        if ( tail not in self.KnownDstNames ):
            log.warning(filename+" has unexpected extension ``"+tail+"''. Expecting DST or dst.")
        return "DST"+head
    
            
    ############################################################################
    # Dst writing
    #
    def writeDst(self,filename,seq):
        """
        Write out a DST using a sequence
        """
        log.info("DaVinciWriteDst::writeDst "+filename)
        stream = self.streamName(filename)
        from Configurables import ApplicationMgr, InputCopyStream  
        writer = InputCopyStream( stream )
        ApplicationMgr().OutStream += [ writer ]
        writer.RequireAlgs = [ seq.getName() ]  # require the sequencer 
        writer.Output = "DATAFILE='PFN:" + filename + "' TYP='POOL_ROOTTREE' OPT='REC'"
        writer.OptItemList = self.getProp("Items")
# suggested by Marco. Say nothing produced fewer errors
#        writer.ItemList = [ '/Event/MC#999', '/Event/DAQ#999', '/Event/Rec#999', '/Event/Link#999',
#                            '/Event/pSim#999' ]
        writer.TakeOptionalFromTES = True
        writer.Preload = False          # True makes LoKi crash (why?)
        writer.PreloadOptItems = False  # True makes LoKi crash (why?)
#        writer.OutputLevel = 1 
        return 

    ################################################################################
    # Dst and ETC output sequence
    #
    def dstSequence(self):
        """
        Output Sequence
        """
        log.info("Configuring DST and ETC")
        if (len( self.getProp("DstFiles"))):
            self.setProp("Items", self.getProp("Items"))
        from Configurables import GaudiSequencer
        seq = GaudiSequencer("DstWriters")
        seq.IgnoreFilterPassed = True
        for f,s in self.getProp("DstFiles").iteritems():
            log.info("Will configure DST output file "+f)
            self.writeDst(f,s)
            seq.Members += [ s ]
        DaVinci().appendToMainSequence(seq)

