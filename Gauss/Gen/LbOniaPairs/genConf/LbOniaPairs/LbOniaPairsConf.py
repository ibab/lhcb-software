#Mon May 30 16:25:06 2011
"""Automatically generated. DO NOT EDIT please"""
from GaudiKernel.Proxy.Configurable import *

class OniaPairsProduction( ConfigurableAlgTool ) :
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
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'ContextService' : 'AlgContextSvc', # str
    'Commands' : [ 'pysubs msub 11 0' , 'pysubs msub 12 0' , 'pysubs msub 13 0' , 'pysubs msub 28 0' , 'pysubs msub 53 0' , 'pysubs msub 68 0' , 'pysubs msub 91 0' , 'pysubs msub 92 0' , 'pysubs msub 93 0' , 'pysubs msub 94 0' , 'pysubs msub 95 0' , 'pysubs msub 421 0' , 'pysubs msub 422 0' , 'pysubs msub 423 0' , 'pysubs msub 424 0' , 'pysubs msub 425 0' , 'pysubs msub 426 0' , 'pysubs msub 427 0' , 'pysubs msub 428 0' , 'pysubs msub 429 0' , 'pysubs msub 430 0' , 'pysubs msub 431 0' , 'pysubs msub 432 0' , 'pysubs msub 433 0' , 'pysubs msub 434 0' , 'pysubs msub 435 0' , 'pysubs msub 436 0' , 'pysubs msub 437 0' , 'pysubs msub 438 0' , 'pysubs msub 439 0' , 'pysubs msub 461 0' , 'pysubs msub 462 0' , 'pysubs msub 463 0' , 'pysubs msub 464 0' , 'pysubs msub 465 0' , 'pysubs msub 466 0' , 'pysubs msub 467 0' , 'pysubs msub 468 0' , 'pysubs msub 469 0' , 'pysubs msub 470 0' , 'pysubs msub 471 0' , 'pysubs msub 472 0' , 'pysubs msub 473 0' , 'pysubs msub 474 0' , 'pysubs msub 475 0' , 'pysubs msub 476 0' , 'pysubs msub 477 0' , 'pysubs msub 478 0' , 'pysubs msub 479 0' , 'pysubs msub 480 0' , 'pysubs msub 481 0' , 'pysubs msub 482 0' , 'pysubs msub 483 0' , 'pysubs msub 484 0' , 'pysubs msub 485 0' ], # list
    'PygiveCommands' : [  ], # list
    'PDTList' : [  ], # list
    'BeamToolName' : 'CollidingBeams', # str
    'WidthLimit' : 0.0015, # float
    'SLHADecayFile' : 'empty', # str
    'PDecayList' : [  ], # list
    'SLHASpectrumFile' : 'empty', # str
    'ValidateHEPEVT' : True, # bool
    'Inconsistencies' : 'HEPEVT_inconsistencies.out', # str
    'EnergyCM' : 7000.0, # float
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'StatPrint' : """ Print the table of counters """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'ContextService' : """ The name of Algorithm Context Service """,
    'EnergyCM' : """ CM-energy """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'ValidateHEPEVT' : """ The flag to force the validation (mother&daughter) of HEPEVT """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
    'Inconsistencies' : """ The file to dump HEPEVT inconsinstencies """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(OniaPairsProduction, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'LbOniaPairs'
  def getType( self ):
      return 'OniaPairsProduction'
  pass # class OniaPairsProduction
