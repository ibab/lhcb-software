'''Modules to create a complete DST-writing sequence given a selection and some configuration parameters.'''

__all__ = ('DSTWriterSelectionSequence',
           'SelDSTWriter')

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

from Configurables import GaudiSequencer, RecordStream

from copy import copy

class DSTWriterSelectionSequence(object) :
    '''
    Extends a SelectionSequence to write a DST.
    Takes a SelectionSequence as an input. Other arguments include
    the output stream configuration and a MicroDSTExtras, which
    has a list of callables that populate the relevant MicroDST
    locations plut their output location trunk.
    '''
    def __init__(self,
                 selSequence,
                 outputStreamConfiguration,
                 writeFSR    = True,
                 extras      = None,
                 persistency = None) :
        
        from GaudiConf import IOHelper
        
        outputStreamConfiguration.name = selSequence.name()
        
        #self.stream = outputStream(outputStreamConfiguration)
        #self.fsrStream = fsrStream(outputStreamConfiguration)
        
        conf = outputStreamConfiguration
        dstName = conf.filePrefix + conf.name + conf.extension
        
        ioh=IOHelper(persistency,persistency)
        
        writer = conf.streamType(conf.name+'_OStream')
        writer.OptItemList += conf.extraItems
        if len(conf.vetoItems) > 0 : writer.TESVetoList += conf.vetoItems

        #writer.OutputLevel = 1
        
        algs = ioh.outputAlgs( filename=dstName, writer=writer, writeFSR=writeFSR )
        
        self.name = "DSTWriter"+selSequence.name()
        
        self.algos = [selSequence.sequence()]
        
        self.mainSeq = None
        
        if extras is not None:
            for _algs in [x(selSequence) for x in extras] :
                self.algos += _algs

        # Extract Raw Events to keep for this stream, line by line.
        if outputStreamConfiguration.selectiveRawEvent :
            try :
                rawEvents = selSequence.getRequiredRawEvents()
                rawEvPropName = 'AlgDependentItemList'
                if hasattr(writer,rawEvPropName) :
                    writer.setProp( rawEvPropName, rawEvents )
                else:
                    print "WARNING : Output Writer", writer.name(), "does not have property", rawEvPropName
            except :
                raise Exception( "Sequence '" + selSequence.name() + "' does not have support for selective raw event saving. **NO** Raw Event banks will be saved. To save them, set selectiveRawEvent=False in your SelDSTWriter stream Configuration." )

        # Collects the data available on the input file, for subsequent DST streams.
        # Must be run once per event, before *any* TES address killing.
        # Only one instance needed, so run an instance here that is not stream specific.
        from Configurables import FixInputCopyStream
        self.algos.append( FixInputCopyStream() )

        if outputStreamConfiguration.killTESAddressHistory :
            from Configurables import AddressKillerAlg
            self.algos.append( AddressKillerAlg( name = "KillTESAddresses_"+conf.name ) )
        
        for alg in algs:
            if ioh.detectStreamType(alg) in ["FSR"]:
                #FSR stream
                if writeFSR:
                    self.algos = [ alg ] + self.algos
            else:
                #event data stream
                self.algos += [alg]
                if extras is not None:
                    alg.OptItemList += extras.output
        
    def sequence(self, sequencerType = GaudiSequencer) :
        if self.mainSeq == None :
            self.mainSeq = sequencerType(self.name, Members = copy(self.algos))
        return self.mainSeq
