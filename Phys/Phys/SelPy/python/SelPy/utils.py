"""
General helpers for maniplating selections and lists.
"""
__author__ = "Juan PALACIOS palacios@physik.uzh.ch"

__all__ = ( 'flatSelectionList',
            'treeSelectionList',
            'flattenList',
            'isIterable',
            'removeDuplicates',
            'update_dict_overlap',
            'NamedObject',
            'UniquelyNamedObject',
            'NameError',
            'NonEmptyInputLocations',
            'IncompatibleInputLocations', )

class NamedObject(object) :

    def __init__(self, name) :
        self._name = name

    def name(self) :
        return self._name

class UniquelyNamedObject(NamedObject) :
    __used_names = []
    def __init__(self, name) :
        if name in UniquelyNamedObject.__used_names :
            raise NameError('Name ' + name + ' has already been used. Pick a new one.')
        NamedObject.__init__(self, name)
        UniquelyNamedObject.__used_names.append(name)

class SelectionBase(object) :

    def __init__(self, algorithm, outputLocation, requiredSelections) :
        self._algorithm = algorithm
        self._outputLocation = outputLocation
        self._requiredSelections = list(requiredSelections)

    def algorithm(self) :
        return self._algorithm

    def outputLocation(self) :
        return self._outputLocation

    def requiredSelections(self) :
        return self._requiredSelections
    
def treeSelectionList(selection) :
    """
    Return a nested list with all the selections needed by a selection, including itself.
    """
    _selList = [treeSelectionList(sel) for sel in selection.requiredSelections()] + [selection]
    return _selList

def isIterable(obj) :
    '''
    Test if an object is iterable but not a string type.
    '''
    return hasattr(obj, "__iter__") and not isinstance(obj, basestring)

def flattenList(tree) :
    """flattenList(tree) -> flat list
    Return a flat list containing all the non-iterable elements retrieved tree and tree's sub-trees.
    """
    flatList = []
    for member in tree :
        if isIterable(member) :
            flatList.extend(flattenList(member))
        else :
            flatList.append(member)
    return flatList

def removeDuplicates(obj_list) :
    """
    Remove all but the first instance of an object from the a list.
    Remove all NoneType instances.
    """
    clean_list = []
    for obj in obj_list :
        if obj not in clean_list and obj != None :
            clean_list.append(obj)
    return clean_list

def flatSelectionList(selection) :
    """
    Return a flat selection list containing all the selections required
    by selection, in the correct order of execution. Duplicates are removed,
    keeping the first one.
    """
    return removeDuplicates(flattenList(treeSelectionList(selection)))

def update_dict_overlap(dict0, dict1) :
    """
    Replace entries from dict0 with those from dict1 that have
    keys present in dict0.
    """
    overlap_keys = filter(dict1.has_key, dict0.keys())
    result = dict(dict0)
    for key in overlap_keys : result[key] = dict1[key]
    return result


def compatibleSequences( seq0, seq1) :
    if len(seq0) != len(seq1) :
        return False
    for x in seq0 :
        if x not in seq1 :
            return False
    return True


def connectToRequiredSelections(selection, inputSetter) :
    """
    Make selection get input data from its requiredSelections via an inputSetter method (str).
    """
    _outputLocations = [sel.outputLocation() for sel in selection.requiredSelections()]
    _outputLocations = filter(lambda s : s != '', _outputLocations)
    configurable = selection.algorithm()

    if inputSetter and hasattr(configurable, inputSetter) :
        _inputLocations = getattr(configurable, inputSetter)
        if len(_inputLocations) != 0 :
            if not compatibleSequences(_outputLocations,
                                       _inputLocations) :
                raise IncompatibleInputLocations('InputLocations of input algorithm incompatible with RequiredSelections!'\
                                                 '\nInputLocations: '+str(_inputLocations)+\
                                                 '\nRequiredSelections: '+str(_outputLocations))

    if inputSetter :
        configurable.__setattr__(inputSetter, list(_outputLocations))

                                                                        
class NameError(Exception) :
    pass

class NonEmptyInputLocations(Exception) :
    pass
class IncompatibleInputLocations(Exception) :
    pass

                                
