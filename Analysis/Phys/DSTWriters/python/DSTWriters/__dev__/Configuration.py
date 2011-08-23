"""
DST-writing ConfigurableUser classes and default configuration parameter
sets for three standard use-cases. Every configuration is possible via
SelDSTWriter.

Exported symbols:

    * ConfigurableUsers:
       'SelDSTWriter'   - The base class doing all the work.
       'MicroDSTWriter' - special configuration of SelDSTWriter.
       'StripDSTWriter' - special configuration of SelDSTWriter.
    * default output stream parameters:
       'selDSTStreamConf'
       'microDSTStreamConf'
       'stripDSTStreamConf'
    * default sets of items for MicroDST partition.
       'selDSTElements'
       'microDSTElements'
       'stripDSTElements'

Example of useage:

from DSTWriters.__dev__.microdstelements import *
from DSTWriters.__dev__.Configuration import *

defaultElements = selDSTElements()

stripElements = stripDSTElements()

microElements = defaultElements + [CloneLHCbIDs(),
                                  ReFitAndClonePVs()]


streamConf = {'default' : stripDSTStreamConf(),
              muons.name(): microDSTStreamConf()}

conf = SelDSTWriter('SelDST',
                    StreamConf = streamConf,
                    MicroDSTElements = {'default' : stripElements,
                                        muons.name() : microElements},
                    OutputFileSuffix = 'Test',
                    SelectionSequences = [...])

"""

__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

__all__ = ('SelDSTWriter',
           'MicroDSTWriter',
           'StripDSTWriter',
           'selDSTStreamConf',
           'microDSTStreamConf',
           'stripMicroDSTStreamConf',
           'stripCalibMicroDSTStreamConf',
           'stripDSTStreamConf',
           'selDSTElements',
           'microDSTElements',
           'stripMicroDSTElements',
           'stripDSTElements')

from GaudiConf.Configuration import *
from Configurables import GaudiSequencer, InputCopyStream, OutputStream

from dstwriters     import DSTWriterSelectionSequence
from dstwriterutils import MicroDSTElementList
from seldst         import selDSTStreamConf, selDSTElements
from microdst       import microDSTStreamConf, microDSTElements, stripMicroDSTStreamConf, stripMicroDSTElements, stripCalibMicroDSTStreamConf
from stripdst       import stripDSTStreamConf, stripDSTElements

from copy import copy

class SelDSTWriter(ConfigurableUser) :
    """
    Write a DST for a set of selection sequences. Writes out the entire
    contents of the input DST file, plus optional extra items from the TES.
    """
    __slots__ = {
        "OutputFileSuffix"           : "Sel"
        , "SelectionSequences"       : []
        , "WriteFSR"                 : True
        , "StreamConf"               : { 'default' : selDSTStreamConf() }
        , "MicroDSTElements"         : { 'default' : selDSTElements()   }
        , "RootInTES"                : '/Event'
        }

    _propertyDocDct = {  
        "OutputFileSuffix"             : """Add to name of output DST file. Default 'Sel'"""
        , "SelectionSequences" : """ List of SelectionSequences defining the selections"""
        , "WriteFSR"           : """ Flags whether to write out an FSR """
        , "StreamConf"         : """ Output stream configuration """
        , "MicroDSTElements"   : """ List of callables setting up each element to be added to each SelectionSequence and copied to MicroDST partition."""
        , "RootInTES"          : """ RootInTES property for sequences and MicroDST output location."""
        }

    def _buildClonerList(self, selSeq) :
        '''
        Build list of callables that will be used by the BaseDSTWriter.
        '''
        clonerDict = copy(self.getProp('MicroDSTElements'))
        rootInTES = self.getProp('RootInTES')
        seqName = selSeq.name()
        clonerList = []
        if seqName in clonerDict.keys() :
            clonerList = clonerDict[seqName]
        else :
            clonerList = clonerDict['default']
        
        return MicroDSTElementList(branch = seqName,
                                   callables = clonerList,
                                   rootInTES = rootInTES)

    def _buildStreamConf(self, selSeq) :
        scDict = copy(self.getProp('StreamConf'))
        seqName = selSeq.name()
        sc = None
        if seqName in scDict.keys() :
            sc = scDict[seqName]
        else :
            sc = scDict['default']
        prefix = self.getProp('OutputFileSuffix')
        if prefix != '' :
            sc.filePrefix = self.getProp('OutputFileSuffix') + '.'
        return sc
    
    def sequence(self) :
        return GaudiSequencer(self.name() + "MainSeq",
                              ModeOR = True, 
                              ShortCircuit = False,
                              RootInTES = self.getProp('RootInTES'))

    def __apply_configuration__(self) :
        """
        BaseDSTWriter configuration. Creates a DSTWriterSelectionSequence for
        each input SelectionSequence and adds it to the main sequence.
        """
        log.info("Configuring BaseDSTWriter")
        for sel in self.getProp('SelectionSequences') :
            seq = DSTWriterSelectionSequence(selSequence = sel,
                                             outputStreamConfiguration = self._buildStreamConf(sel),
                                             extras = self._buildClonerList(sel),
                                             writeFSR = self.getProp('WriteFSR'))
            self.sequence().Members += [ seq.sequence() ]

class MicroDSTWriter(SelDSTWriter) :
    """
    Write a MicroDST for a set of selection sequences.
    """
    __slots__ = { "StreamConf"       : { 'default' : microDSTStreamConf() },
                  "MicroDSTElements" : { 'default' : microDSTElements() }   }

class StripDSTWriter(SelDSTWriter) :
    """
    Write a MicroDST for a set of selection sequences.
    """
    __slots__ = { "StreamConf"       : { 'default' : stripDSTStreamConf() },
                  "MicroDSTElements" : { 'default' : stripDSTElements() }   }
