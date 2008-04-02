#Wed Apr  2 14:32:02 2008
"""Automatically generated. DO NOT EDIT please"""
from GaudiKernel.Proxy.Configurable import *

class MuonHitmapTool( ConfigurableAlgTool ) :
  __slots__ = { 
    'MonitorService' : 'MonitorSvc', # str
    'OutputLevel' : 7, # int
    'AuditTools' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditFinalize' : False, # bool
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0, # float
    'StatTableHeader' : '     Counter     |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' %|-15.15s|%|17t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : '*%|-15.15s|%|17t||%|10d| |%|11.5g| |(%|#9.7g| +- %|-#9.7g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'ContextService' : 'AlgContextSvc', # str
    'HistoProduce' : True, # bool
    'HistoPrint' : False, # bool
    'HistoCheckForNaN' : True, # bool
    'HistoSplitDir' : False, # bool
    'HistoOffSet' : 0, # int
    'HistoTopDir' : '', # str
    'HistoDir' : 'AlgTool', # str
    'FullDetail' : False, # bool
    'MonitorHistograms' : True, # bool
    'FormatFor1DHistoTable' : '| %2$-30.30s | %3$=7d |%8$11.5g | %10$-11.5g|%12$11.5g |%14$11.5g |', # str
    'ShortFormatFor1DHistoTable' : ' | %1$-15.15s %2%', # str
    'HeaderFor1DHistoTable' : '|   Title                        |    #    |     Mean   |    RMS     |  Skewness  |  Kurtosis  |', # str
    'UseSequencialNumericAutoIDs' : False, # bool
    'AutoStringIDPurgeMap' : { '/' : '=SLASH=' }, # list
  }
  _propertyDocDct = { 
    'UseSequencialNumericAutoIDs' : """ Flag to allow users to switch back to the old style of creating numerical automatic IDs """,
    'HistoTopDir' : """ Top level histogram directory (take care that it ends with '/') """,
    'HistoCheckForNaN' : """ Swicth on/off the checks for NaN and Infinity for histogram fill """,
    'HistoDir' : """ Histogram Directory """,
    'HistoProduce' : """ Swith on/off the production of histograms  """,
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'FormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'HistoSplitDir' : """ Split long directory names into short pieces (suitable for HBOOK) """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'HistoPrint' : """ Switch on/off the printout of histograms at finalization """,
    'HeaderFor1DHistoTable' : """ The table header for printout of 1D histograms  """,
    'ShortFormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'AutoStringIDPurgeMap' : """ Map of strings to search and replace when using the title as the basis of automatically generated literal IDs """,
    'HistoOffSet' : """ OffSet for automatically assigned histogram numerical identifiers  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(MuonHitmapTool, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'L0MuonMonitor'
  def getType( self ):
      return 'MuonHitmapTool'
  pass # class MuonHitmapTool

class MuonChannelsHistTool( ConfigurableAlgTool ) :
  __slots__ = { 
    'MonitorService' : 'MonitorSvc', # str
    'OutputLevel' : 7, # int
    'AuditTools' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditFinalize' : False, # bool
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0, # float
    'StatTableHeader' : '     Counter     |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' %|-15.15s|%|17t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : '*%|-15.15s|%|17t||%|10d| |%|11.5g| |(%|#9.7g| +- %|-#9.7g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'ContextService' : 'AlgContextSvc', # str
    'HistoProduce' : True, # bool
    'HistoPrint' : False, # bool
    'HistoCheckForNaN' : True, # bool
    'HistoSplitDir' : False, # bool
    'HistoOffSet' : 0, # int
    'HistoTopDir' : '', # str
    'HistoDir' : 'AlgTool', # str
    'FullDetail' : False, # bool
    'MonitorHistograms' : True, # bool
    'FormatFor1DHistoTable' : '| %2$-30.30s | %3$=7d |%8$11.5g | %10$-11.5g|%12$11.5g |%14$11.5g |', # str
    'ShortFormatFor1DHistoTable' : ' | %1$-15.15s %2%', # str
    'HeaderFor1DHistoTable' : '|   Title                        |    #    |     Mean   |    RMS     |  Skewness  |  Kurtosis  |', # str
    'UseSequencialNumericAutoIDs' : False, # bool
    'AutoStringIDPurgeMap' : { '/' : '=SLASH=' }, # list
  }
  _propertyDocDct = { 
    'UseSequencialNumericAutoIDs' : """ Flag to allow users to switch back to the old style of creating numerical automatic IDs """,
    'HistoTopDir' : """ Top level histogram directory (take care that it ends with '/') """,
    'HistoCheckForNaN' : """ Swicth on/off the checks for NaN and Infinity for histogram fill """,
    'HistoDir' : """ Histogram Directory """,
    'HistoProduce' : """ Swith on/off the production of histograms  """,
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'FormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'HistoSplitDir' : """ Split long directory names into short pieces (suitable for HBOOK) """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'HistoPrint' : """ Switch on/off the printout of histograms at finalization """,
    'HeaderFor1DHistoTable' : """ The table header for printout of 1D histograms  """,
    'ShortFormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'AutoStringIDPurgeMap' : """ Map of strings to search and replace when using the title as the basis of automatically generated literal IDs """,
    'HistoOffSet' : """ OffSet for automatically assigned histogram numerical identifiers  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(MuonChannelsHistTool, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'L0MuonMonitor'
  def getType( self ):
      return 'MuonChannelsHistTool'
  pass # class MuonChannelsHistTool

class L0MuonOnlineMonitor( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'AuditAlgorithms' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditExecute' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditBeginRun' : False, # bool
    'AuditEndRun' : False, # bool
    'MonitorService' : 'MonitorSvc', # str
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0, # float
    'StatTableHeader' : '     Counter     |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' %|-15.15s|%|17t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : '*%|-15.15s|%|17t||%|10d| |%|11.5g| |(%|#9.7g| +- %|-#9.7g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'ContextService' : 'AlgContextSvc', # str
    'RegisterForContextService' : True, # bool
    'HistoProduce' : True, # bool
    'HistoPrint' : False, # bool
    'HistoCheckForNaN' : True, # bool
    'HistoSplitDir' : False, # bool
    'HistoOffSet' : 0, # int
    'HistoTopDir' : '', # str
    'HistoDir' : 'DefaultName', # str
    'FullDetail' : False, # bool
    'MonitorHistograms' : True, # bool
    'FormatFor1DHistoTable' : '| %2$-30.30s | %3$=7d |%8$11.5g | %10$-11.5g|%12$11.5g |%14$11.5g |', # str
    'ShortFormatFor1DHistoTable' : ' | %1$-15.15s %2%', # str
    'HeaderFor1DHistoTable' : '|   Title                        |    #    |     Mean   |    RMS     |  Skewness  |  Kurtosis  |', # str
    'UseSequencialNumericAutoIDs' : False, # bool
    'AutoStringIDPurgeMap' : { '/' : '=SLASH=' }, # list
    'TimeSlots' : [ -7 , -6 , -5 , -4 , -3 , -2 , -1 , 0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 ], # list
    'Quarters' : [ 0 , 1 , 2 , 3 ], # list
    'Stations' : [ 0 , 1 , 2 , 3 , 4 ], # list
    'Regions' : [ 0 , 1 , 2 , 3 , 4 ], # list
  }
  _propertyDocDct = { 
    'UseSequencialNumericAutoIDs' : """ Flag to allow users to switch back to the old style of creating numerical automatic IDs """,
    'HistoTopDir' : """ Top level histogram directory (take care that it ends with '/') """,
    'HistoCheckForNaN' : """ Swicth on/off the checks for NaN and Infinity for histogram fill """,
    'HistoDir' : """ Histogram Directory """,
    'HistoProduce' : """ Swith on/off the production of histograms  """,
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'FormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'HistoSplitDir' : """ Split long directory names into short pieces (suitable for HBOOK) """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'HistoPrint' : """ Switch on/off the printout of histograms at finalization """,
    'HeaderFor1DHistoTable' : """ The table header for printout of 1D histograms  """,
    'ShortFormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'AutoStringIDPurgeMap' : """ Map of strings to search and replace when using the title as the basis of automatically generated literal IDs """,
    'HistoOffSet' : """ OffSet for automatically assigned histogram numerical identifiers  """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(L0MuonOnlineMonitor, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'L0MuonMonitor'
  def getType( self ):
      return 'L0MuonOnlineMonitor'
  pass # class L0MuonOnlineMonitor

class L0MuonMonitor( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'AuditAlgorithms' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditExecute' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditBeginRun' : False, # bool
    'AuditEndRun' : False, # bool
    'MonitorService' : 'MonitorSvc', # str
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0, # float
    'StatTableHeader' : '     Counter     |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' %|-15.15s|%|17t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : '*%|-15.15s|%|17t||%|10d| |%|11.5g| |(%|#9.7g| +- %|-#9.7g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'ContextService' : 'AlgContextSvc', # str
    'RegisterForContextService' : True, # bool
    'HistoProduce' : True, # bool
    'HistoPrint' : False, # bool
    'HistoCheckForNaN' : True, # bool
    'HistoSplitDir' : False, # bool
    'HistoOffSet' : 0, # int
    'HistoTopDir' : '', # str
    'HistoDir' : 'DefaultName', # str
    'FullDetail' : False, # bool
    'MonitorHistograms' : True, # bool
    'FormatFor1DHistoTable' : '| %2$-30.30s | %3$=7d |%8$11.5g | %10$-11.5g|%12$11.5g |%14$11.5g |', # str
    'ShortFormatFor1DHistoTable' : ' | %1$-15.15s %2%', # str
    'HeaderFor1DHistoTable' : '|   Title                        |    #    |     Mean   |    RMS     |  Skewness  |  Kurtosis  |', # str
    'UseSequencialNumericAutoIDs' : False, # bool
    'AutoStringIDPurgeMap' : { '/' : '=SLASH=' }, # list
    'Extension' : '', # str
    'TimeSlots' : [  ], # list
  }
  _propertyDocDct = { 
    'UseSequencialNumericAutoIDs' : """ Flag to allow users to switch back to the old style of creating numerical automatic IDs """,
    'HistoTopDir' : """ Top level histogram directory (take care that it ends with '/') """,
    'HistoCheckForNaN' : """ Swicth on/off the checks for NaN and Infinity for histogram fill """,
    'HistoDir' : """ Histogram Directory """,
    'HistoProduce' : """ Swith on/off the production of histograms  """,
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'FormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'HistoSplitDir' : """ Split long directory names into short pieces (suitable for HBOOK) """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'HistoPrint' : """ Switch on/off the printout of histograms at finalization """,
    'HeaderFor1DHistoTable' : """ The table header for printout of 1D histograms  """,
    'ShortFormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'AutoStringIDPurgeMap' : """ Map of strings to search and replace when using the title as the basis of automatically generated literal IDs """,
    'HistoOffSet' : """ OffSet for automatically assigned histogram numerical identifiers  """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(L0MuonMonitor, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'L0MuonMonitor'
  def getType( self ):
      return 'L0MuonMonitor'
  pass # class L0MuonMonitor
