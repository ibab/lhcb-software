################################################################################
##                                                                            ##
##   File: TestBandQLinesFromStrippingSettings.py                             ##
##   Authors: Lucio.Anderlini@fi.infn.it, Roberta.Cardinale@ge.infn.it        ##
##                                                                            ##
##   GaudiPython script used to test BandQ lines in Stripping21               ##
##   as defined in the StrippingSettings package                              ##
##                                                                            ##
################################################################################
from Gaudi.Configuration import *
from Configurables import DaVinci


def deNone(variable, default): 
  "Returns default if variable is None. Returns variable otherwise"
  if variable == None: return default
  return variable

################################################################################
## StrippingConfigurator
################################################################################

class StrippingConfigurator:
  "Configurator to run the Stripping"
  def __init__ (self 
                , strippingVersion          = None
                , WGs                       = None
                , trackingCuts              = None
                , useStrippingSettings      = None
                , TESPrefix                 = None
                ):

    self.strippingVersion     = deNone(strippingVersion   , 'stripping21')
    self.WGs                  = deNone(WGs                , ['BandQ'])
    self.trackingCuts         = deNone(trackingCuts       , {
                                    "Chi2Cut" : [ 0, 3 ],
                                    "CloneDistCut" : [5000, 9e+99 ] }
                                 )
    self.useStrippingSettings  = deNone(useStrippingSettings, True)
    self.TESPrefix             = deNone(TESPrefix, 'Strip')

    # Cache variables
    self._streams   = None
    self._algorithm = None

  def _getStreams(self):
    "return the stream objects"

    if self._streams != None: return self._streams

  # Tighten Trk Chi2 to <3
    from CommonParticles.Utils import DefaultTrackingCuts
    DefaultTrackingCuts().Cuts  = self.trackingCuts;

    from StrippingSettings.Utils import strippingConfiguration
    from StrippingSelections.Utils import buildStreams

    _config  = strippingConfiguration(self.strippingVersion)
    _streams = buildStreams(stripping = _config, WGs = self.WGs)
        
    self._streams = _streams
    return _streams

  def _getStrippingSequence(self, streams):
    "return the StrippingSequence"
    from StrippingConf.Configuration import StrippingConf, StrippingStream
    from Configurables import ProcStatusCheck
    _filterBadEvents = ProcStatusCheck()

    for stream in streams:
      for line in stream.lines:
        print stream.name(), ": ", line.name()


    _sc = StrippingConf(Streams = streams,
                        MaxCandidates = 2000,
                        AcceptBadEvents = False,
                        BadEventSelection = _filterBadEvents, 
                        TESPrefix = self.TESPrefix)

    return _sc
  
  def getAlgorithm(self):
    "Creates the StrippingConf algorithm and returns its sequence"
    if self._algorithm == None: 
      self._algorithm = self._getStrippingSequence (  self._getStreams() )

    return self._algorithm

  def getStrippingMonitors(self):
    _monitors = []
    from Configurables import StrippingMonitor
    for stream in _streams:
      for line in stream.lines:
        print stream.name(), ": ", line.name()
        strippingMonitorSeq = GaudiSequencer("SMSeq_"+line.name())
        monitor = StrippingMonitor("SM_"+line.name())
        monitor.Inputs = [ "/Event/Phys/"+line.name().replace("Stripping", "",1)+"/Particles"]
        _monitors += monitor

    return _monitors

################################################################################
## configTimeSvc
################################################################################
def configTimeSvc ():
  "configure TimingAuditor and SequencerTimerTool"
  from Configurables import TimingAuditor, SequencerTimerTool
  from Configurables import AuditorSvc, ChronoAuditor
  TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
  TimingAuditor().TIMER.NameSize = 60
  AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )



################################################################################
## PersistencyConfigurator
################################################################################
class PersistencyConfigurator:
  def __init__(self
               , strippingAlgorithm
               , enablePacking = True
               , alwaysSaveRaw = False
               , outputLocation = None
               ):
    self.enablePacking = enablePacking
    self.alwaysSaveRaw = alwaysSaveRaw
    self.outputLocation = deNone(outputLocation, '.')
    self.strippingAlgorithm = strippingAlgorithm

  def getWriter(self, mdstStreams):
    "Configure the dst and microdst Writer and return the algorithm"
    from DSTWriters.Configuration import (SelDSTWriter,
                                          stripDSTStreamConf,
                                          stripDSTElements,
                                          stripMicroDSTStreamConf,
                                          stripMicroDSTElements,
                                          stripCalibMicroDSTStreamConf
                                          )

    enablePacking = self.enablePacking
    mdstStreamConf = stripMicroDSTStreamConf(pack=enablePacking)
    mdstElements   = stripMicroDSTElements(pack=enablePacking)

    ###mdstStreams = ['Leptonic', 'Charm', 'PID', 'Bhadron']
    mdstElementsDict = dict()
    mdstConfigDicts  = dict()
    for mdstStream in mdstStreams:
      mdstConfigDicts[mdstStream] = mdstStreamConf
      mdstElementsDict[mdstStream]    = mdstElements
      

    SelDSTWriterElements = {'default': stripDSTElements(pack=enablePacking)}
    SelDSTWriterElements.update ( mdstElementsDict )

    vetoedRaw = []
    if not self.alwaysSaveRaw:
      for i in ['Calo', 'Rich', 'Other', 'Muon', 'DAQ'] :
        vetoedRaw += [ "/Event/{}/RawEvent".format(i) ]

  
    SelDSTWriterConf = dict()
    SelDSTWriterConf.update( mdstConfigDicts )
    SelDSTWriterConf.update( 
      {'default': stripDSTStreamConf(pack=enablePacking, vetoItems = vetoedRaw)}
    )

    if 'PID' in mdstStreams:
      SelDSTWriterConf.update (
        {'PID': stripCalibMicroDSTStreamConf(pack=enablePacking)}
      )


    return SelDSTWriter( "MyDSTWriter",
                         StreamConf = SelDSTWriterConf,
                         MicroDSTElements = SelDSTWriterElements,
                         OutputFileSuffix = self.outputLocation,
                         SelectionSequences = self.strippingAlgorithm.activeStreams()
                       )





####### ENTRY POINT  #########
def configureStrippingTest(id, WGs):
  MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"
      
  conf_stripping = StrippingConfigurator()
  conf_stripping.WGs = WGs
  stripping = conf_stripping.getAlgorithm().sequence()
  DaVinci().appendToMainSequence ( [ stripping ] )

  ### StrippingMonitor currently not released
  ### DaVinci().appendToMainSequence ( [ conf_stripping.getStrippingMonitors() ] )

  configTimeSvc()

  from Configurables import StrippingReport
  sr = StrippingReport(Selections = conf_stripping.getAlgorithm().selections())
  try:
    sr.Correlation = True
  except:
    pass
  DaVinci().appendToMainSequence ( [ sr ] )
  

  pc = PersistencyConfigurator( conf_stripping.getAlgorithm(), 
                                outputLocation = id
                              )

  mdststreams = ['Leptonic', 'PID', 'Charm', 'Bhadron']
  DaVinci().appendToMainSequence ( [ pc.getWriter(mdststreams).sequence() ] )

  DaVinci().DDDBtag  = "dddb-20120831"
  DaVinci().CondDBtag = "cond-20121008"


  DaVinci().Simulation = False
  DaVinci().DataType  = "2012"
  DaVinci().InputType = "DST"

  MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

  # database
  DaVinci().DDDBtag  = "dddb-20120831"
  DaVinci().CondDBtag = "cond-20121025"



###
###  configureStrippingTest('BandQ', ['BandQ'])
###  # input file
###  importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py") # MagDown sample
