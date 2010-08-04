"""
Write a DST for a single selection sequence. Writes out the entire
contents of the input DST
"""
__version__ = "$Id: BaseDSTWriter.py,v 1.5 2010-08-04 09:31:28 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer

from DSTWriters.dstwriters import DSTWriterSelectionSequence, baseDSTWriterConf
from DSTWriters.dstwriterutils import MicroDSTElementList

from copy import copy

class BaseDSTWriter(ConfigurableUser) :
    """
    Write a DST for a set of selection sequences. Writes out the entire
    contents of the input DST file, plus optional extra items from the TES.
    """
    __slots__ = {
        "OutputFileSuffix"           : "Sel"
        , "SelectionSequences"       : []
        , "ExtraItems"               : []
        , "WriteFSR"                 : True
        , "StreamConf"               : baseDSTWriterConf
        , "MicroDSTElements" : []
        }

    _propertyDocDct = {  
        "OutputFileSuffix"             : """Add to name of output DST file. Default 'Sel'"""
        , "SelectionSequences" : """ Name of SelectionSequence that defines the selection"""
        , "ExtraItems"         : """ Extra TES locations to be written. Default: []"""
        , "WriteFSR"           : """ Flags whether to write out an FSR """
        , "StreamConf"         : """ Output stream configuration """
        , "MicroDSTElements"   : """ List of callables setting up each element to be copied to MicroDST partition."""
        }

    def buildClonerList(self, selSeq) :
        '''
        Build list of callables that will be used by the BaseDSTWriter.
        '''
        clonerList = copy(self.getProp('MicroDSTElements'))
        return MicroDSTElementList(branch = selSeq.name(), callables = clonerList)

    def sequence(self) :
        return GaudiSequencer(self.name() + "MainSeq",
                              ModeOR = True, 
                              ShortCircuit = False)

    def __apply_configuration__(self) :
        """
        BaseDSTWriter configuration. Creates a DSTWriterSelectionSequence for
        each input SelectionSequence and adds it to the main sequence.
        """
        log.info("Configuring BaseDSTWriter")
        sc = copy(self.getProp('StreamConf'))
        sc.filePrefix = self.getProp('OutputFileSuffix')
        sc.extraItems = self.getProp('ExtraItems')
        for sel in self.getProp('SelectionSequences') :
            _sc = copy(sc)
            seq = DSTWriterSelectionSequence(selSequence = sel,
                                             outputStreamConfiguration = _sc,
                                             extras = self.buildClonerList(sel))
            self.sequence().Members += [ seq.sequence() ]
