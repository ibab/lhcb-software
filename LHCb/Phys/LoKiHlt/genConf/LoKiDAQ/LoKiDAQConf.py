#Fri Sep 19 17:09:54 2008
"""Automatically generated. DO NOT EDIT please"""
from GaudiKernel.Proxy.Configurable import *

class LoKi__Hybrid__DAQFactory( ConfigurableAlgTool ) :
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
    'Modules' : [ 'LoKiDAQ.decorators' ], # list
    'Actor' : 'LoKi.Hybrid.DAQEngine()', # str
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
      super(LoKi__Hybrid__DAQFactory, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'LoKiDAQ'
  def getType( self ):
      return 'LoKi::Hybrid::DAQFactory'
  pass # class LoKi__Hybrid__DAQFactory
