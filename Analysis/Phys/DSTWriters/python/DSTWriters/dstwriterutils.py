''' Helpers functions and classes for DST and MicroDST writing.
'''
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

__all__ = ('ConfigurableList',
           'setCloneFilteredParticlesToTrue',
           'flattenList',
           'personaliseName',
           'dataLocations',
           'MicroDSTElement',
           'MicroDSTElementList')

def isIterable(obj) :
    '''
    Test if an object is iterable but not a string type.
    '''
    return hasattr(obj, "__iter__") and not isinstance(obj, basestring) 

def flattenList(sequence) :
    """flattenList(sequence) -> flat list
    Return a flat list containing all the non-iterable elements retrieved sequence and sequence's sub-sequences. 
    """
    flatList = []
    for member in sequence :
        if isIterable(member) :
            flatList.extend(flattenList(member))
        else :
            flatList.append(member)
    return flatList

def setCloneFilteredParticlesToTrue(algs) :
    '''
    Loop over algs and try to set attribute CloneFilteredParticles to True.
    If a member of algs is a GaudiSequencer, loop over its Members.
    '''
    for alg in algs :
        try :
            alg.CloneFilteredParticles = True
            #print "Setting", alg.name()+".CloneFilteredParticles = True"
        except :
            try :
                setCloneFilteredParticlesToTrue(alg.Members)
            except :
                pass

def personaliseName(obj, name) :
    '''
    Prepend a string to the name of named object obj.
    obj must have method obj.name().
    '''
    return name + "_" + obj.name()

def dataLocations(selSequence,extension,deepSearch=False) :
    """
    Add an extension to each of the locations in selSequence.oupputLocations().
    Returns a new list of locations. Removes trailing '/' and '/Particles'
    """
    loc = []
    for output in selSequence.outputLocations() :
        location = output
        if location.endswith('/Particles') :
            location = location[ : location.rfind('/Particles') ]
        location = (location+"/"+extension).replace("//", "/")
        if location.endswith('/') :
            location = location[ : -1 ]
        loc += [location]
    #if deepSearch :
    return loc

class ConfigurableList(object) :
    '''
    Return a list of plain configurables from a SelectionSequence style object.
    Intended to deal with difference in interfaces of SelectionSequence, StrippingStream, etc.
    '''
    _algos = { 'DEFAULT'         : lambda x : x.algorithms(),
               'StrippingStream' : lambda x : x.filterMembers(),
               'StrippingLine'   : lambda x : x.filterMembers(),
               'StrippingConf'   : lambda x : x.filterMembers()
               }
    def __init__(self, selection) :
        self._sel = selection
    def name(self) :
        return type(self._sel).__name__
    def flatList(self) :
        if self.name() in ConfigurableList._algos.keys() :
            return ConfigurableList._algos[self.name()](self._sel)
        else :
            return ConfigurableList._algos['DEFAULT'](self._sel)

class MicroDSTElementList(object) :
    """
    Wrap a set of MicroDST callables in a list-like interface.
    Set the TES branch of each to the input branch.
    Export the output location trunk of all the callables.
    Each callable must satisfy the following conditions:

    Data member 'branch' controlling the TES location
    to which the contained algorithms write data to.

    Attribute __call__(self, sel) accepting a SelectionSequenc-like object,
    returning suitably ordered list of suitably configured Configurables.
    
    """
    def __init__( self,
                  branch = 'MicroDST',
                  callables = [],
                  rootInTES = '/Event/' ) :
        self.callables = callables
        self.branch = branch
        self.output = [ ]
        output = rootInTES + '/' + branch + '#99'
        output = output.replace('//', '/')
        self.output += [output]
        # For MC add links. Never found for real data so just do it all the time...
        self.output += [output.replace('Event','Event/Link')]
        for c in self.callables :
            c.branch = self.branch
    def __getitem__(self, index) :
        return self.callables[index]

class MicroDSTElement(object) :
    '''
    Base class for MicroDSTElement callables.
    Stores the TES branch and makes dataLocations and personaliseName functions accessible.
    Derived classes must implement __call__(self, sel) method, where sel is a SelectionSequence.
    Method should return list of all configured algorithms necessary to produce the data to be cloned
    and the cloners.
    '''
    def __init__(self, branch = '') :
        self.branch = branch
        self.personaliseName = personaliseName
        self.dataLocations = dataLocations
    def setOutputPrefix(self,alg) :
        alg.OutputPrefix = self.branch
