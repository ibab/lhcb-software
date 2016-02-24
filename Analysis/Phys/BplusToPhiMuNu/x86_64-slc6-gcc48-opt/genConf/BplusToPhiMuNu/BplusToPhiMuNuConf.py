#Wed Feb 24 12:25:00 2016"""Automatically generated. DO NOT EDIT please"""
from GaudiKernel.Proxy.Configurable import *

class TupleToolBu2phimunu( ConfigurableAlgTool ) :
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
    'GlobalTimeOffset' : 0.0000000, # float
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'ContextService' : 'AlgContextSvc', # str
    'HistoProduce' : True, # bool
    'HistoPrint' : False, # bool
    'HistoCountersPrint' : True, # bool
    'HistoCheckForNaN' : True, # bool
    'HistoSplitDir' : False, # bool
    'HistoOffSet' : 0, # int
    'HistoTopDir' : '', # str
    'HistoDir' : 'AlgTool', # str
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
    'NTupleDir' : 'AlgTool', # str
    'EvtColsProduce' : False, # bool
    'EvtColsPrint' : False, # bool
    'EvtColSplitDir' : False, # bool
    'EvtColOffSet' : 0, # int
    'EvtColLUN' : 'EVTCOL', # str
    'EvtColTopDir' : '', # str
    'EvtColDir' : 'AlgTool', # str
    'ExtraName' : '', # str
    'Verbose' : False, # bool
    'MaxPV' : 100, # int
    'ParticlePath' : '/Event/Phys/StdAllNoPIDsPions/Particles', # str
    'VarSuffix' : '_Long', # str
  }
  _propertyDocDct = { 
    'MaxPV' : """ Maximal number of PVs considered """,
    'ExtraName' : """ prepend the name of any variable with this string """,
    'EvtColOffSet' : """ Offset for numerical N-tuple ID """,
    'EvtColDir' : """ Subdirectory for Event Tag Collections """,
    'EvtColSplitDir' : """ Split long directory names into short pieces """,
    'EvtColsPrint' : """ Print statistics for Event Tag Collections  """,
    'EvtColsProduce' : """ General switch to enable/disable Event Tag Collections """,
    'NTupleDir' : """ Subdirectory for N-Tuples """,
    'NTupleSplitDir' : """ Split long directory names into short pieces (suitable for HBOOK) """,
    'NTupleProduce' : """ General switch to enable/disable N-tuples """,
    'AutoStringIDPurgeMap' : """ Map of strings to search and replace when using the title as the basis of automatically generated literal IDs """,
    'UseSequencialNumericAutoIDs' : """ Flag to allow users to switch back to the old style of creating numerical automatic IDs """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'NTuplePrint' : """ Print N-tuple statistics """,
    'HistoOffSet' : """ OffSet for automatically assigned histogram numerical identifiers  """,
    'StatPrint' : """ Print the table of counters """,
    'NTupleLUN' : """ Logical File Unit for N-tuples """,
    'FormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'EvtColLUN' : """ Logical File Unit for Event Tag Collections """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'NTupleOffSet' : """ Offset for numerical N-tuple ID """,
    'ContextService' : """ The name of Algorithm Context Service """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'EvtColTopDir' : """ Top-level directory for Event Tag Collections """,
    'HistoPrint' : """ Switch on/off the printout of histograms at finalization """,
    'Verbose' : """ add extra variables for this tool """,
    'HistoCheckForNaN' : """ Switch on/off the checks for NaN and Infinity for histogram fill """,
    'HistoDir' : """ Histogram Directory """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'HistoProduce' : """ Switch on/off the production of histograms """,
    'HistoCountersPrint' : """ Switch on/off the printout of histogram counters at finalization """,
    'HistoSplitDir' : """ Split long directory names into short pieces (suitable for HBOOK) """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'HistoTopDir' : """ Top level histogram directory (take care that it ends with '/') """,
    'NTupleTopDir' : """ Top-level directory for N-Tuples """,
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ShortFormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'HeaderFor1DHistoTable' : """ The table header for printout of 1D histograms  """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(TupleToolBu2phimunu, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'BplusToPhiMuNu'
  def getType( self ):
      return 'TupleToolBu2phimunu'
  pass # class TupleToolBu2phimunu

class TupleToolBu2phimunu_2( ConfigurableAlgTool ) :
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
    'GlobalTimeOffset' : 0.0000000, # float
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'ContextService' : 'AlgContextSvc', # str
    'HistoProduce' : True, # bool
    'HistoPrint' : False, # bool
    'HistoCountersPrint' : True, # bool
    'HistoCheckForNaN' : True, # bool
    'HistoSplitDir' : False, # bool
    'HistoOffSet' : 0, # int
    'HistoTopDir' : '', # str
    'HistoDir' : 'AlgTool', # str
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
    'NTupleDir' : 'AlgTool', # str
    'EvtColsProduce' : False, # bool
    'EvtColsPrint' : False, # bool
    'EvtColSplitDir' : False, # bool
    'EvtColOffSet' : 0, # int
    'EvtColLUN' : 'EVTCOL', # str
    'EvtColTopDir' : '', # str
    'EvtColDir' : 'AlgTool', # str
    'ExtraName' : '', # str
    'Verbose' : False, # bool
    'MaxPV' : 100, # int
    'ParticlePath' : '/Event/Phys/StdAllNoPIDsPions/Particles', # str
    'VarSuffix' : '_Long', # str
  }
  _propertyDocDct = { 
    'MaxPV' : """ Maximal number of PVs considered """,
    'ExtraName' : """ prepend the name of any variable with this string """,
    'EvtColOffSet' : """ Offset for numerical N-tuple ID """,
    'EvtColDir' : """ Subdirectory for Event Tag Collections """,
    'EvtColSplitDir' : """ Split long directory names into short pieces """,
    'EvtColsPrint' : """ Print statistics for Event Tag Collections  """,
    'EvtColsProduce' : """ General switch to enable/disable Event Tag Collections """,
    'NTupleDir' : """ Subdirectory for N-Tuples """,
    'NTupleSplitDir' : """ Split long directory names into short pieces (suitable for HBOOK) """,
    'NTupleProduce' : """ General switch to enable/disable N-tuples """,
    'AutoStringIDPurgeMap' : """ Map of strings to search and replace when using the title as the basis of automatically generated literal IDs """,
    'UseSequencialNumericAutoIDs' : """ Flag to allow users to switch back to the old style of creating numerical automatic IDs """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'NTuplePrint' : """ Print N-tuple statistics """,
    'HistoOffSet' : """ OffSet for automatically assigned histogram numerical identifiers  """,
    'StatPrint' : """ Print the table of counters """,
    'NTupleLUN' : """ Logical File Unit for N-tuples """,
    'FormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'EvtColLUN' : """ Logical File Unit for Event Tag Collections """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'NTupleOffSet' : """ Offset for numerical N-tuple ID """,
    'ContextService' : """ The name of Algorithm Context Service """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'EvtColTopDir' : """ Top-level directory for Event Tag Collections """,
    'HistoPrint' : """ Switch on/off the printout of histograms at finalization """,
    'Verbose' : """ add extra variables for this tool """,
    'HistoCheckForNaN' : """ Switch on/off the checks for NaN and Infinity for histogram fill """,
    'HistoDir' : """ Histogram Directory """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'HistoProduce' : """ Switch on/off the production of histograms """,
    'HistoCountersPrint' : """ Switch on/off the printout of histogram counters at finalization """,
    'HistoSplitDir' : """ Split long directory names into short pieces (suitable for HBOOK) """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'HistoTopDir' : """ Top level histogram directory (take care that it ends with '/') """,
    'NTupleTopDir' : """ Top-level directory for N-Tuples """,
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ShortFormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'HeaderFor1DHistoTable' : """ The table header for printout of 1D histograms  """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(TupleToolBu2phimunu_2, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'BplusToPhiMuNu'
  def getType( self ):
      return 'TupleToolBu2phimunu_2'
  pass # class TupleToolBu2phimunu_2

class TupleToolSLTools( ConfigurableAlgTool ) :
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
    'GlobalTimeOffset' : 0.0000000, # float
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'ContextService' : 'AlgContextSvc', # str
    'HistoProduce' : True, # bool
    'HistoPrint' : False, # bool
    'HistoCountersPrint' : True, # bool
    'HistoCheckForNaN' : True, # bool
    'HistoSplitDir' : False, # bool
    'HistoOffSet' : 0, # int
    'HistoTopDir' : '', # str
    'HistoDir' : 'AlgTool', # str
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
    'NTupleDir' : 'AlgTool', # str
    'EvtColsProduce' : False, # bool
    'EvtColsPrint' : False, # bool
    'EvtColSplitDir' : False, # bool
    'EvtColOffSet' : 0, # int
    'EvtColLUN' : 'EVTCOL', # str
    'EvtColTopDir' : '', # str
    'EvtColDir' : 'AlgTool', # str
    'ExtraName' : '', # str
    'Verbose' : False, # bool
    'MaxPV' : 100, # int
    'VertexFitter' : 'LoKi::VertexFitter', # str
    'Bmass' : 5279.2900, # float
    'VertexCov' : False, # bool
    'MomCov' : False, # bool
  }
  _propertyDocDct = { 
    'MaxPV' : """ Maximal number of PVs considered """,
    'ExtraName' : """ prepend the name of any variable with this string """,
    'EvtColOffSet' : """ Offset for numerical N-tuple ID """,
    'EvtColDir' : """ Subdirectory for Event Tag Collections """,
    'EvtColSplitDir' : """ Split long directory names into short pieces """,
    'EvtColsPrint' : """ Print statistics for Event Tag Collections  """,
    'EvtColsProduce' : """ General switch to enable/disable Event Tag Collections """,
    'NTupleDir' : """ Subdirectory for N-Tuples """,
    'NTupleSplitDir' : """ Split long directory names into short pieces (suitable for HBOOK) """,
    'NTupleProduce' : """ General switch to enable/disable N-tuples """,
    'AutoStringIDPurgeMap' : """ Map of strings to search and replace when using the title as the basis of automatically generated literal IDs """,
    'UseSequencialNumericAutoIDs' : """ Flag to allow users to switch back to the old style of creating numerical automatic IDs """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'NTuplePrint' : """ Print N-tuple statistics """,
    'HistoOffSet' : """ OffSet for automatically assigned histogram numerical identifiers  """,
    'StatPrint' : """ Print the table of counters """,
    'NTupleLUN' : """ Logical File Unit for N-tuples """,
    'FormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'EvtColLUN' : """ Logical File Unit for Event Tag Collections """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'NTupleOffSet' : """ Offset for numerical N-tuple ID """,
    'ContextService' : """ The name of Algorithm Context Service """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'EvtColTopDir' : """ Top-level directory for Event Tag Collections """,
    'HistoPrint' : """ Switch on/off the printout of histograms at finalization """,
    'Verbose' : """ add extra variables for this tool """,
    'HistoCheckForNaN' : """ Switch on/off the checks for NaN and Infinity for histogram fill """,
    'HistoDir' : """ Histogram Directory """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'HistoProduce' : """ Switch on/off the production of histograms """,
    'HistoCountersPrint' : """ Switch on/off the printout of histogram counters at finalization """,
    'HistoSplitDir' : """ Split long directory names into short pieces (suitable for HBOOK) """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'HistoTopDir' : """ Top level histogram directory (take care that it ends with '/') """,
    'NTupleTopDir' : """ Top-level directory for N-Tuples """,
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ShortFormatFor1DHistoTable' : """ Format string for printout of 1D histograms """,
    'HeaderFor1DHistoTable' : """ The table header for printout of 1D histograms  """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(TupleToolSLTools, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'BplusToPhiMuNu'
  def getType( self ):
      return 'TupleToolSLTools'
  pass # class TupleToolSLTools
