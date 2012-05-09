"""
Decorators for DecayTreeTuples, to simplify life somewhat
"""

from Gaudi.Configuration import *
from Configurables import (DecayTreeTuple, EventTuple, MCDecayTreeTuple, TupleToolDecay, TupleToolMCTruth)

def __splitname__(self, tool):
  '''parse the string into the configurable and instance name'''
  name=None
  if '/' in tool: 
    name=tool.split('/')[-1]
    tool=tool.split('/')[0] 
  while('::') in tool:
    tool=tool.replace('::','__')
  if name is not None:
    while('::') in name:
      name=name.replace('::','__')
  return tool, name

  
def addTupleTool(self,tool,name=None):
  """Correctly adds a TupleTool to a DecayTreeTuple or Branch instance, so that the user doesn't need to do the logic themselves
  tool can be any TupleTool, either the bare class, instance, string with '::' or string with '__'
  name must be a string
  examples:
  mytuple.addTupleTool('LoKi::Hybrid::TupleTool/LoKi_J')
    mytuple.addTupleTool('LoKi::Hybrid::TupleTool')
    mytuple.addTupleTool(LoKi__Hybrid__TupleTool)
    mytuple.addTupleTool('LoKi__Hybrid__TupleTool')
    mytuple.addTupleTool(LoKi__Hybrid__TupleTool,'LoKi_J')
    mytuple.addTupleTool(LoKi__Hybrid__TupleTool('Shared_J'))
  """
  
  ############## Step 1: check you're not doing something stupid ######################
  import operator
  if name is not None:
    if type(name) is not str:
      raise TypeError, ('expected string for name, got '+str(type(name))+' instead')
  
  if type(tool) is not str and not operator.isCallable(tool):
    if 'getFullName' not in dir(tool):
      raise TypeError, ('tool instance must be a string or configurable, got '+str(type(tool))+' instead')
    mother,atype,aname=tool.splitName()
    if aname==atype and tool.__class__.__name__==aname and tool.isPublic():
      raise TypeError, ('You are trying to add a default public tool-configurable to your ntuple: '
                        +tool.getFullName()
                        +' This is dangerous so not allowed with addTTool.'
                        +' Either add it yourself manually, or, better, supply an instance name')
  if type(tool) is not str and operator.isCallable(tool):
    import GaudiKernel
    if type(tool) is not GaudiKernel.ConfigurableMeta.ConfigurableMeta:
      raise TypeError, ("Expected a bare configurable, got a "+str(type(tool))+" instead")
  #will fail here if you haven't supplied an ntuple!
  if 'ToolList' not in dir(self):
    raise TypeError, ("You are calling addTupleTool to something which hasn't got the ability to own TupleTools "+str(type(self)))
  
  if type(tool) is not str and not operator.isCallable(tool) and name is not None:
    mother,atype,aname=tool.splitName()
    if aname!=name:
      raise NameError, ('You have supplied an instance, but also specified a different name '+atype+' '+name +' '+aname+'. Supply the bare class or strings instead if an instance.')
    name=None
  if type(tool) is str and name is not None and '/' in tool:
    tool,name2= self.__splitname__(tool)
    if name2!=name:
      raise NameError, ('You have supplied two different names for '+tool+' '+name+' '+name2)
    name=None
  
  
  config=None
  if self.ToolList is None:
    self.ToolList=[]

  tooltype=''
  toolname=''
  toolinstance=''
  
  ################# Step 2: retrieve the configurable for the tool in question ############
  if type(tool) is str:
    tool,name2= self.__splitname__(tool)
    if name2 is not None:
      name=name2
    #will fail here if the configurable doesn't exist
    try:
      exec('from Configurables import '+tool)
      exec('config = '+tool)
    except ImportError:
      raise ImportError, ('The TupleTool '+tool+' does not exist, check the name and try again')
  else:
    config=tool
  
  
  
  ################# Step 3: add to Self ###################################################
  if name is None: self.addTool(config)
  else: self.addTool(config, name)
  
  ################# Step 4: add to ToolList and return the instance of the configurable ###
  instance=None
  
  if type(tool) is not str and not operator.isCallable(config):
    #if a configurable was supplied I need to find its name twice ...
    mother,tool,name=config.splitName()
    if(tool==name): tool=config.__class__.__name__
  
  if type(tool) is not str and operator.isCallable(config):
    #if a bare configurable was supplied I need to change the type to a string.. not easy to do that!
    import GaudiKernel
    tool= GaudiKernel.ConfigurableMeta.ConfigurableMeta.__repr__(tool)
    tool=tool.split("'")[-2]
    tool=tool.split(".")[-1]
  
  if name is None:
    exec('instance=self.'+tool)
  else: 
    exec('instance=self.'+name)

  #mother,tool,name=instance.splitName()
  
  if instance.getFullName() in self.ToolList:
    raise AttributeError, ('The tool '+instance.getFullName()+' was already added to the ToolList, remove and try again')
  elif instance.getFullName().split('/')[0]==instance.getFullName().split('/')[-1] and instance.getFullName().split('/')[0] in self.ToolList:
    raise AttributeError, ('The tool '+instance.getFullName().split('/')[-1]+' was already added to the ToolList, remove and try again')
  #elif (tool==name and name in self.ToolList):
  #  raise AttributeError, ('The tool '+tool+' was already added to the ToolList, remove and try again')
  
  if instance.getFullName().split('/')[0]==instance.getFullName().split('/')[-1]:
    self.ToolList.append(instance.getFullName().split('/')[0])
  else:
    self.ToolList.append(instance.getFullName())
  
  return instance

def addBranches(self, branches):
  """Simplified adding of branches a little bit
  takes a dictionary of {branch: decay descriptor}, returns a dictionary of {branch: configurable instances}"""
  if 'Branches' not in dir(self):
    raise TypeError, ("you're trying to add branches to something which doesn't support branching, "+str(type(self)))
  if type(branches) is not type({}):
    raise TypeError, ("expected a dictionary of branches, got a "+str(type(branches))+" instead")
  
  if self.Branches is None:
    self.Branches={}
  
  instances={}
  for branch in branches:
    #check for whitespace
    for char in ' \t\r\n':
      if char in branch:
        raise NameError, ("You have tried to add a branch named '"+branch+"',which contains whitespace. This is not permitted.")
    self.Branches[branch]=branches[branch]
    self.addTool(TupleToolDecay,branch)
    exec('instances[branch]=self.'+branch)
  
  return instances
 
for config in [DecayTreeTuple, EventTuple, MCDecayTreeTuple, TupleToolDecay, TupleToolMCTruth]:
  config.__splitname__=__splitname__
  config.addTupleTool=addTupleTool

for config in [DecayTreeTuple, EventTuple, MCDecayTreeTuple]:
  config.addBranches=addBranches

