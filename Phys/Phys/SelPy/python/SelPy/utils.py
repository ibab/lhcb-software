"""
General helpers for maniplating selections and lists.
"""
__author__ = "Juan PALACIOS palacios@physik.uzh.ch"

__all__ = ( 'flatSelectionList',
            'treeSelectionList',
            'flattenList',
            'isIterable',
            'removeDuplicates'   )

def treeSelectionList(selection) :
    """
    Return a nested list with all the selections needed by a selection, including itself.
    """
    _selList = [treeSelectionList(sel) for sel in selection.requiredSelections] + [selection]
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
                                                                        

                                
