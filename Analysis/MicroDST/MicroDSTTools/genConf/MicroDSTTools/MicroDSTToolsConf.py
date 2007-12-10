#Mon Dec 10 10:27:09 2007
"""Automatically generated. DO NOT EDIT please"""
from GaudiKernel.Configurable import *

class VertexCloner( ConfigurableAlgTool ) :
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
    'InputLocation' : '', # str
    'OutputPrefix' : 'microDST', # str
    'ICloneParticle' : 'ParticleCloner', # str
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(VertexCloner, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'MicroDSTTools'
  def getType( self ):
      return 'VertexCloner'
  pass # class VertexCloner

class RecVertexClonerWithTracks( ConfigurableAlgTool ) :
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
    'InputLocation' : '', # str
    'OutputPrefix' : 'microDST', # str
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(RecVertexClonerWithTracks, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'MicroDSTTools'
  def getType( self ):
      return 'RecVertexClonerWithTracks'
  pass # class RecVertexClonerWithTracks

class RecVertexCloner( ConfigurableAlgTool ) :
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
    'InputLocation' : '', # str
    'OutputPrefix' : 'microDST', # str
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(RecVertexCloner, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'MicroDSTTools'
  def getType( self ):
      return 'RecVertexCloner'
  pass # class RecVertexCloner

class ParticleCloner( ConfigurableAlgTool ) :
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
    'InputLocation' : '', # str
    'OutputPrefix' : 'microDST', # str
    'ICloneVertex' : 'VertexCloner', # str
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(ParticleCloner, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'MicroDSTTools'
  def getType( self ):
      return 'ParticleCloner'
  pass # class ParticleCloner

class MCVertexCloner( ConfigurableAlgTool ) :
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
    'InputLocation' : '', # str
    'OutputPrefix' : 'microDST', # str
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(MCVertexCloner, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'MicroDSTTools'
  def getType( self ):
      return 'MCVertexCloner'
  pass # class MCVertexCloner

class MCParticleCloner( ConfigurableAlgTool ) :
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
    'InputLocation' : '', # str
    'OutputPrefix' : 'microDST', # str
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'EfficiencyRowFormat' : """ The format for the regular row in the outptu Stat-table """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ContextService' : """ The name of Algorithm Context Service """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(MCParticleCloner, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'MicroDSTTools'
  def getType( self ):
      return 'MCParticleCloner'
  pass # class MCParticleCloner
