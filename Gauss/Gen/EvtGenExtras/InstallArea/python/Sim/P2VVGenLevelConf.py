#Tue Jan 20 10:46:15 2009
"""Automatically generated. DO NOT EDIT please"""
from GaudiKernel.Proxy.Configurable import *

class P2VVTupleTool( ConfigurableAlgorithm ) :
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
    'ContextService' : 'AlgContextSvc', # str
    'RegisterForContextService' : True, # bool
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
    'NTupleName' : 'P2VVTuple', # str
    'MotherID' : 511, # int
    'DaughterOne' : 321, # int
    'DaughterTwo' : 211, # int
  }
  _propertyDocDct = { 
    'UseSequencialNumericAutoIDs' : """ Flag to allow users to switch back to the old style of creating numerical automatic IDs """,
    'HistoTopDir' : """ Top level histogram directory (take care that it ends with '/') """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'HistoCheckForNaN' : """ Swicth on/off the checks for NaN and Infinity for histogram fill """,
    'HistoDir' : """ Histogram Directory """,
    'HistoProduce' : """ Swith on/off the production of histograms  """,
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'FormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
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
      super(P2VVTupleTool, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'P2VVGenLevel'
  def getType( self ):
      return 'P2VVTupleTool'
  pass # class P2VVTupleTool

class P2VVAngleCalculator( ConfigurableAlgTool ) :
  __slots__ = { 
    'MonitorService' : 'MonitorSvc', # str
    'OutputLevel' : 7, # int
    'AuditTools' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'AuditFinalize' : False, # bool
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
    'ContextService' : 'AlgContextSvc', # str
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(P2VVAngleCalculator, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'P2VVGenLevel'
  def getType( self ):
      return 'P2VVAngleCalculator'
  pass # class P2VVAngleCalculator
