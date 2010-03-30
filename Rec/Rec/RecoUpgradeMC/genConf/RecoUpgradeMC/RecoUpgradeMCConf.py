#Tue Mar 30 08:59:39 2010
"""Automatically generated. DO NOT EDIT please"""
from GaudiKernel.Proxy.Configurable import *

class VeloPixChecker( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'AuditAlgorithms' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'AuditExecute' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditBeginRun' : False, # bool
    'AuditEndRun' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'MonitorService' : 'MonitorSvc', # str
    'RegisterForContextService' : True, # bool
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0, # float
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'VetoObjects' : [  ], # list
    'RequireObjects' : [  ], # list
    'HistoProduce' : True, # bool
    'HistoPrint' : False, # bool
    'HistoCheckForNaN' : True, # bool
    'HistoSplitDir' : False, # bool
    'HistoOffSet' : 0, # int
    'HistoTopDir' : '', # str
    'HistoDir' : 'DefaultName', # str
    'FullDetail' : False, # bool
    'MonitorHistograms' : True, # bool
    'FormatFor1DHistoTable' : '| %2$-45.45s | %3$=7d |%8$11.5g | %10$-11.5g|%12$11.5g |%14$11.5g |', # str
    'ShortFormatFor1DHistoTable' : ' | %1$-25.25s %2%', # str
    'HeaderFor1DHistoTable' : '|   Title                                       |    #    |     Mean   |    RMS     |  Skewness  |  Kurtosis  |', # str
    'UseSequencialNumericAutoIDs' : False, # bool
    'AutoStringIDPurgeMap' : { '/' : '=SLASH=' }, # list
    'NTupleProduce' : True, # bool
    'NTuplePrint' : True, # bool
    'NTupleSplitDir' : False, # bool
    'NTupleOffSet' : 0, # int
    'NTupleLUN' : 'FILE1', # str
    'NTupleTopDir' : '', # str
    'NTupleDir' : 'DefaultName', # str
    'EvtColsProduce' : False, # bool
    'EvtColsPrint' : False, # bool
    'EvtColSplitDir' : False, # bool
    'EvtColOffSet' : 0, # int
    'EvtColLUN' : 'EVTCOL', # str
    'EvtColTopDir' : '', # str
    'EvtColDir' : 'DefaultName', # str
    'NtupleName' : 'Tracks', # str
    'InputTracks' : 'Rec/Track/VeloPix', # str
  }
  _propertyDocDct = { 
    'EvtColSplitDir' : """ Split long directory names into short pieces """,
    'NTupleDir' : """ Subdirectory for N-Tuples """,
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'NTupleLUN' : """ Logical File Unit for N-tuples """,
    'EvtColTopDir' : """ Top-level directory for Event Tag Collections """,
    'EvtColLUN' : """ Logical File Unit for Event Tag Collections """,
    'NTupleSplitDir' : """ Split long directory names into short pieces (suitable for HBOOK) """,
    'HeaderFor1DHistoTable' : """ The table header for printout of 1D histograms  """,
    'StatPrint' : """ Print the table of counters """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'HistoDir' : """ Histogram Directory """,
    'NTupleTopDir' : """ Top-level directory for N-Tuples """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'EvtColsPrint' : """ Print statistics for Event Tag Collections  """,
    'HistoTopDir' : """ Top level histogram directory (take care that it ends with '/') """,
    'AutoStringIDPurgeMap' : """ Map of strings to search and replace when using the title as the basis of automatically generated literal IDs """,
    'NTuplePrint' : """ Print N-tuple statistics """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'ShortFormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'EvtColsProduce' : """ General switch to enable/disable Event Tag Collections """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'EvtColOffSet' : """ Offset for numerical N-tuple ID """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'EvtColDir' : """ Subdirectory for Event Tag Collections """,
    'HistoSplitDir' : """ Split long directory names into short pieces (suitable for HBOOK) """,
    'HistoProduce' : """ Swith on/off the production of histograms  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'HistoPrint' : """ Switch on/off the printout of histograms at finalization """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'FormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'NTupleOffSet' : """ Offset for numerical N-tuple ID """,
    'HistoOffSet' : """ OffSet for automatically assigned histogram numerical identifiers  """,
    'UseSequencialNumericAutoIDs' : """ Flag to allow users to switch back to the old style of creating numerical automatic IDs """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
    'HistoCheckForNaN' : """ Swicth on/off the checks for NaN and Infinity for histogram fill """,
    'NTupleProduce' : """ General switch to enable/disable N-tuples """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(VeloPixChecker, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'RecoUpgradeMC'
  def getType( self ):
      return 'VeloPixChecker'
  pass # class VeloPixChecker

class TrackAssociatorUpgrade( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'AuditAlgorithms' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'AuditExecute' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditBeginRun' : False, # bool
    'AuditEndRun' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'MonitorService' : 'MonitorSvc', # str
    'RegisterForContextService' : True, # bool
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0, # float
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'VetoObjects' : [  ], # list
    'RequireObjects' : [  ], # list
    'TracksInContainer' : 'Rec/Track/Best', # str
    'LinkerOutTable' : '', # str
    'FractionOK' : 0.7, # float
    'DecideUsingMuons' : False, # bool
    'UseVeloPix' : False, # bool
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'StatPrint' : """ Print the table of counters """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(TrackAssociatorUpgrade, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'RecoUpgradeMC'
  def getType( self ):
      return 'TrackAssociatorUpgrade'
  pass # class TrackAssociatorUpgrade

class PatLHCbIDUp2MCParticle( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'AuditAlgorithms' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'AuditExecute' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditBeginRun' : False, # bool
    'AuditEndRun' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'MonitorService' : 'MonitorSvc', # str
    'RegisterForContextService' : True, # bool
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0, # float
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'VetoObjects' : [  ], # list
    'RequireObjects' : [  ], # list
    'TargetName' : 'Pat/LHCbID', # str
    'LinkOT' : True, # bool
    'LinkIT' : True, # bool
    'LinkTT' : True, # bool
    'LinkVELO' : True, # bool
    'LinkVELOPIX' : False, # bool
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'StatPrint' : """ Print the table of counters """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(PatLHCbIDUp2MCParticle, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'RecoUpgradeMC'
  def getType( self ):
      return 'PatLHCbIDUp2MCParticle'
  pass # class PatLHCbIDUp2MCParticle

class PatLHCbIDUp2MCHit( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'AuditAlgorithms' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'AuditExecute' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditBeginRun' : False, # bool
    'AuditEndRun' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'MonitorService' : 'MonitorSvc', # str
    'RegisterForContextService' : True, # bool
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0, # float
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'VetoObjects' : [  ], # list
    'RequireObjects' : [  ], # list
    'TargetName' : 'Pat/LHCbIDMCHit', # str
    'LinkVELOPIX' : False, # bool
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'StatPrint' : """ Print the table of counters """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(PatLHCbIDUp2MCHit, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'RecoUpgradeMC'
  def getType( self ):
      return 'PatLHCbIDUp2MCHit'
  pass # class PatLHCbIDUp2MCHit

class CheatedVeloPixPat( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'AuditAlgorithms' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'AuditExecute' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditBeginRun' : False, # bool
    'AuditEndRun' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'MonitorService' : 'MonitorSvc', # str
    'RegisterForContextService' : True, # bool
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0, # float
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'VetoObjects' : [  ], # list
    'RequireObjects' : [  ], # list
    'HistoProduce' : True, # bool
    'HistoPrint' : False, # bool
    'HistoCheckForNaN' : True, # bool
    'HistoSplitDir' : False, # bool
    'HistoOffSet' : 0, # int
    'HistoTopDir' : '', # str
    'HistoDir' : 'DefaultName', # str
    'FullDetail' : False, # bool
    'MonitorHistograms' : True, # bool
    'FormatFor1DHistoTable' : '| %2$-45.45s | %3$=7d |%8$11.5g | %10$-11.5g|%12$11.5g |%14$11.5g |', # str
    'ShortFormatFor1DHistoTable' : ' | %1$-25.25s %2%', # str
    'HeaderFor1DHistoTable' : '|   Title                                       |    #    |     Mean   |    RMS     |  Skewness  |  Kurtosis  |', # str
    'UseSequencialNumericAutoIDs' : False, # bool
    'AutoStringIDPurgeMap' : { '/' : '=SLASH=' }, # list
    'NTupleProduce' : True, # bool
    'NTuplePrint' : True, # bool
    'NTupleSplitDir' : False, # bool
    'NTupleOffSet' : 0, # int
    'NTupleLUN' : 'FILE1', # str
    'NTupleTopDir' : '', # str
    'NTupleDir' : 'DefaultName', # str
    'EvtColsProduce' : False, # bool
    'EvtColsPrint' : False, # bool
    'EvtColSplitDir' : False, # bool
    'EvtColOffSet' : 0, # int
    'EvtColLUN' : 'EVTCOL', # str
    'EvtColTopDir' : '', # str
    'EvtColDir' : 'DefaultName', # str
    'outputTracksLocation' : 'Rec/Track/VeloPix', # str
    'MinimalMCHitForTrack' : 2, # int
  }
  _propertyDocDct = { 
    'EvtColSplitDir' : """ Split long directory names into short pieces """,
    'NTupleDir' : """ Subdirectory for N-Tuples """,
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'NTupleLUN' : """ Logical File Unit for N-tuples """,
    'EvtColTopDir' : """ Top-level directory for Event Tag Collections """,
    'EvtColLUN' : """ Logical File Unit for Event Tag Collections """,
    'NTupleSplitDir' : """ Split long directory names into short pieces (suitable for HBOOK) """,
    'HeaderFor1DHistoTable' : """ The table header for printout of 1D histograms  """,
    'StatPrint' : """ Print the table of counters """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'HistoDir' : """ Histogram Directory """,
    'NTupleTopDir' : """ Top-level directory for N-Tuples """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'EvtColsPrint' : """ Print statistics for Event Tag Collections  """,
    'HistoTopDir' : """ Top level histogram directory (take care that it ends with '/') """,
    'AutoStringIDPurgeMap' : """ Map of strings to search and replace when using the title as the basis of automatically generated literal IDs """,
    'NTuplePrint' : """ Print N-tuple statistics """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'ShortFormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'EvtColsProduce' : """ General switch to enable/disable Event Tag Collections """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'EvtColOffSet' : """ Offset for numerical N-tuple ID """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'EvtColDir' : """ Subdirectory for Event Tag Collections """,
    'HistoSplitDir' : """ Split long directory names into short pieces (suitable for HBOOK) """,
    'HistoProduce' : """ Swith on/off the production of histograms  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'HistoPrint' : """ Switch on/off the printout of histograms at finalization """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'FormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'NTupleOffSet' : """ Offset for numerical N-tuple ID """,
    'HistoOffSet' : """ OffSet for automatically assigned histogram numerical identifiers  """,
    'UseSequencialNumericAutoIDs' : """ Flag to allow users to switch back to the old style of creating numerical automatic IDs """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
    'HistoCheckForNaN' : """ Swicth on/off the checks for NaN and Infinity for histogram fill """,
    'NTupleProduce' : """ General switch to enable/disable N-tuples """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(CheatedVeloPixPat, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'RecoUpgradeMC'
  def getType( self ):
      return 'CheatedVeloPixPat'
  pass # class CheatedVeloPixPat
