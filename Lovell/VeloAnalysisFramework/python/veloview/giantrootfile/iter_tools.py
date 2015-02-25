## @file IterTools.py
#
# @brief convenience python classes to deal with C++ STL type iterators
#
# @author Manuel Schiller <manuel.schiller@nikhef.nl>
# @date 2013-05-03
#
## @package IterTools
#
# @brief convenience python classes to deal with C++ STL type iterators
#
# @author Manuel Schiller <manuel.schiller@nikhef.nl>
# @date 2013-05-03
#

## iterate over the elements in a given range
#
# @author Manuel Schiller <manuel.schiller@nikhef.nl>
# @date 2013-05-03
#
# example:
# @code
# import ROOT
# v = std.vector('int')(100, 42)
# # print elements 5 to 9 (inclusive) of v
# print tuple(RangeIter(v.begin() + 5, v.begin() + 10)
# @endcode
class RangeIter:
    ## constructor
    #
    # @param begin      (C++) iterator pointing to beginning of range
    # @param end        (C++) iterator pointing one behind end of range
    def __init__(self, begin, end):
        if type(begin) != type(end): raise Exception('Incompatible types!')
        self.__begin__ = begin
        self.__end__ = end

    ## get next object
    #
    # raises StopIteration exception when the end of the range is reached
    #
    # @return next object in range
    def next(self):
        if self.__end__ == self.__begin__: raise StopIteration()
        obj = self.__begin__.__deref__()
        self.__begin__.__preinc__()
        return obj

    ## return the object itself
    def __iter__(self):
        return self

## iterate over the keys in a given range
#
# @author Manuel Schiller <manuel.schiller@nikhef.nl>
# @date 2013-05-03
#
# example:
# @code
# import ROOT
# m = std.map('int', 'float')()
#
# # somehow fill m
# 
# # print first 10 keys in m
# print tuple(KeyRangeIter(m.begin(), m.begin() + 10)
# @endcode
class KeyRangeIter:
    ## constructor
    #
    # @param begin      (C++) iterator pointing to beginning of range
    # @param end        (C++) iterator pointing one behind end of range
    def __init__(self, begin, end):
        if type(begin) != type(end): raise Exception('Incompatible types!')
        if (begin != end):
            # non-empty range, can thus check if the underlying iterator points
            # to a pair (for an empty range, it does not matter)
            try:
                begin.__deref__().first
                begin.__deref__().second
            except:
                raise Exception(
                        'Iterator does not point to (key, value) pair!')
        self.__begin__ = begin
        self.__end__ = end

    ## get next key
    #
    # raises StopIteration exception when the end of the range is reached
    #
    # @return next key in range
    def next(self):
        if self.__end__ == self.__begin__: raise StopIteration()
        obj = self.__begin__.__deref__().first
        self.__begin__.__preinc__()
        return obj

    ## return the object itself
    def __iter__(self):
        return self

## iterate over the values in a given range
#
# @author Manuel Schiller <manuel.schiller@nikhef.nl>
# @date 2013-05-03
#
# example:
# @code
# import ROOT
# m = std.map('int', 'float')()
#
# # somehow fill m
# 
# # print first 10 values in m
# print tuple(ValueRangeIter(m.begin(), m.begin() + 10)
# @endcode
class ValueRangeIter:
    ## constructor
    #
    # @param begin      (C++) iterator pointing to beginning of range
    # @param end        (C++) iterator pointing one behind end of range
    def __init__(self, begin, end):
        if type(begin) != type(end): raise Exception('Incompatible types!')
        if (begin != end):
            # non-empty range, can thus check if the underlying iterator points
            # to a pair (for an empty range, it does not matter)
            try:
                begin.__deref__().first
                begin.__deref__().second
            except:
                raise Exception(
                        'Iterator does not point to (key, value) pair!')
        self.__begin__ = begin
        self.__end__ = end

    ## get next value
    #
    # raises StopIteration exception when the end of the range is reached
    #
    # @return next value in range
    def next(self):
        if self.__end__ == self.__begin__: raise StopIteration()
        obj = self.__begin__.__deref__().second
        self.__begin__.__preinc__()
        return obj

    ## return the object itself
    def __iter__(self):
        return self

# vim: tw=78:sw=4:ft=python
