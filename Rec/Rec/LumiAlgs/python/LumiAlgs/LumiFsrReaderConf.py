# options to run mdf writer for lumi writer
__version__ = "$Id: LumiFsrReaderConf.py,v 1.1 2010-01-08 17:10:11 panmanj Exp $"
__author__  = "Jaap Panman"


from os import environ, path

from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from LHCbKernel.Configuration import *

from Configurables import GaudiSequencer as Sequence
from Configurables import ( LHCbConfigurableUser, LHCbApp )

from Configurables import createODIN, DumpFSR

import GaudiKernel.ProcessJobOptions

def _ext(name) : return path.splitext(name)[-1].lstrip('.')

def _file(f) :
    extensions = { 'RAW' : "' SVC='LHCb::MDFSelector'",
                   'MDF' : "' SVC='LHCb::MDFSelector'",
                   'DST' : "' TYP='POOL_ROOTTREE' OPT='READ'" }
    if f.lstrip().startswith('DATAFILE'): return f
    else: return "DATAFILE='PFN:"+ f + extensions[ _ext(f).upper() ] 

def _sequenceAppender( seq ) :
    return lambda x : seq.Members.append( x )

class LumiFsrReaderConf(LHCbConfigurableUser):
  ## Possible used Configurables
  __used_configurables__ = [ LHCbApp,
                             ]

  __slots__ = {
    "EvtMax":             -1      # Maximum number of events to process
    , "SkipEvents":        0
    , "DataType":          '2010' # Data type
    , "outputFile" :       ''     # output filename
    , "inputFiles" :       [ ]    # input
    , "asciiFile"  :       ''     # ascii output filename for FSR dump
    , "userAlgorithms":    [ ]    # put here user algorithms to add
    , "OutputLevel" :      ERROR  #
    }   


  def _createReader(self, sequence):
    '''
    create reader sequence
    '''
    # create ODIN by hand - just to get the events
    #odin =  createODIN ('createODIN')
    #sequence.Members+=[ odin ]

    # dump FSR
    dump =  DumpFSR ('dumpFSR', AsciiFileName = self.getProp('asciiFile'),
                     OutputLevel = self.getProp("OutputLevel") )
    sequence.Members+=[ dump ]

  def _configureInput(self):
    # POOL Persistency
    importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

    files = self.getProp('inputFiles')
    if len(files) > 0 :
        EventSelector().Input = [ _file(f) for f in files ]

  def __apply_configuration__(self):

    GaudiKernel.ProcessJobOptions.PrintOff()
    # EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )

    # forward some settings...
    self.setOtherProps( LHCbApp(), ['EvtMax','SkipEvents','DataType'] )

    # instantiate the sequencer
    mainSeq = GaudiSequencer("LumiSeq")

    # activate the sequence
    appMgr=ApplicationMgr(TopAlg=[],
                          HistogramPersistency='NONE',
                          )
    ApplicationMgr().TopAlg += [ mainSeq ]
    
    # add user algorithms at the end
    if self.getProp("userAlgorithms"):
        for userAlg in self.getProp("userAlgorithms"):
            ApplicationMgr().TopAlg += [ userAlg ]

    # input
    self._configureInput()

    # configure the main sequence
    self._createReader(mainSeq)

