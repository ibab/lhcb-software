#Tue Sep 23 15:09:20 2008
"""Automatically generated. DO NOT EDIT please"""
from GaudiKernel.Proxy.Configurable import *

class LoKi__ODINFilter( ConfigurableAlgorithm ) :
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
    'Factory' : 'LoKi::Hybrid::HltFactory/HltFactory::PUBLIC', # str
    'Code' : 'ODIN_NONE', # str
    'Location' : 'DAQ/ODIN', # str
  }
  _propertyDocDct = { 
    'Code' : """ The Bender/Python code to be used """,
    'Factory' : """ The type/name of LoKiBender "hybrid" factory """,
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'Location' : """ TES location of LHCb::ODIN object """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(LoKi__ODINFilter, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'LoKiHlt'
  def getType( self ):
      return 'LoKi::ODINFilter'
  pass # class LoKi__ODINFilter

class LoKi__L0Filter( ConfigurableAlgorithm ) :
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
    'Factory' : 'LoKi::Hybrid::HltFactory/HltFactory::PUBLIC', # str
    'Code' : 'L0_NONE', # str
    'Location' : 'Trig/L0/L0DUReport', # str
  }
  _propertyDocDct = { 
    'Code' : """ The Bender/Python code to be used """,
    'Factory' : """ The type/name of LoKiBender "hybrid" factory """,
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'Location' : """ TES location of LHCb::L0DUReport object """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(LoKi__L0Filter, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'LoKiHlt'
  def getType( self ):
      return 'LoKi::L0Filter'
  pass # class LoKi__L0Filter

class LoKi__Hybrid__HltFactory( ConfigurableAlgTool ) :
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
    'ShowCode' : False, # bool
    'Modules' : [ 'LoKiHlt.decorators' ], # list
    'Actor' : 'LoKi.Hybrid.HltEngine()', # str
    'Lines' : [  ], # list
  }
  _propertyDocDct = { 
    'ShowCode' : """ Flag to display the prepared python code """,
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'Actor' : """ The processing engine """,
    'Lines' : """ Additional Python lines to be executed """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'Modules' : """ Python modules to be imported """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(LoKi__Hybrid__HltFactory, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'LoKiHlt'
  def getType( self ):
      return 'LoKi::Hybrid::HltFactory'
  pass # class LoKi__Hybrid__HltFactory

class LoKi__HDRFilter( ConfigurableAlgorithm ) :
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
    'Factory' : 'LoKi::Hybrid::HltFactory/HltFactory::PUBLIC', # str
    'Code' : 'HLT_NONE', # str
    'Location' : 'Hlt/DecReports', # str
  }
  _propertyDocDct = { 
    'Code' : """ The Bender/Python code to be used """,
    'Factory' : """ The type/name of LoKiBender "hybrid" factory """,
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'Location' : """ TES location of LHCb::HltDecReports object """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(LoKi__HDRFilter, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'LoKiHlt'
  def getType( self ):
      return 'LoKi::HDRFilter'
  pass # class LoKi__HDRFilter
