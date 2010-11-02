''' Helpers functions and classes for sequence manipulation.
'''
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

__all__ = ('isiterable',
           'flattenList')
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
                                                        
