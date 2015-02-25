## @file TypeHelper.py
#
# @brief python module to help with instantiating C++ types.
#
# @author Manuel Schiller <manuel.schiller@nikhef.nl>
# @date 2013-04-30
#
## @package TypeHelper
#
# @brief python module to help with instantiating C++ types.
#
# @author Manuel Schiller <manuel.schiller@nikhef.nl>
# @date 2013-04-30
#
import logging

import ROOT, ctypes
from ROOT import std

## map [(un)signed][bitwidth] to ctypes integer types
__itypes__ = [
        { 8: ctypes.c_int8, 16: ctypes.c_int16, 32: ctypes.c_int32, 64: ctypes.c_int64 },
        { 8: ctypes.c_uint8, 16: ctypes.c_uint16, 32: ctypes.c_uint32, 64: ctypes.c_uint64 }
        ]

## simple integer type introspection.
#
# @param s name of C++ integer type for which to find a matching python type
# @return an integer type with the same width as the one used by ROOT's C++
# interpreter.
def __getCType__(s):
    from ROOT import gInterpreter
    unsigned = 1 if 'unsigned' in s else 0
    return __itypes__[unsigned][8 * gInterpreter.ProcessLine('sizeof(%s);' % s)]

## dictionary of POD C/C++ type names to ctypes type
__typesPOD__ = {
        'bool':                 ctypes.c_bool,
        'char':                 __getCType__('char'),
        'unsigned char':        __getCType__('unsigned char'),
        'short':                __getCType__('short'),
        'unsigned short':       __getCType__('unsigned short'),
        'int':                  __getCType__('int'),
        'unsigned':             __getCType__('unsigned int'),
        'unsigned int':         __getCType__('unsigned int'),
        'long':                 __getCType__('long'),
        'unsigned long':        __getCType__('unsigned long'),
        'long long':            __getCType__('long long'),
        'unsigned long long':   __getCType__('unsigned long long'),
        'float':                ctypes.c_float,
        'double':               ctypes.c_double,

        'Bool_t':               ctypes.c_bool,
        'Char_t':               ctypes.c_int8,
        'UChar_t':              ctypes.c_uint8,
        'Short_t':              ctypes.c_int16,
        'UShort_t':             ctypes.c_uint16,
        'Int_t':                ctypes.c_int32,
        'UInt_t':               ctypes.c_uint32,
        'Long_t':               __getCType__('long'),
        'ULong_t':              __getCType__('unsigned long'),
        'Long64_t':             ctypes.c_int64,
        'ULong64_t':            ctypes.c_uint64,
        'Float16_t':            ctypes.c_float,
        'Float_t':              ctypes.c_float,
        'Double32_t':           ctypes.c_double,
        'Double_t':             ctypes.c_double
        }

## dictionary of C++ STL type names to corresponding pythonised type
__typesSTL__ = {
        'complex':              ROOT.std.complex,
        'deque':                ROOT.std.deque,
        'list':                 ROOT.std.list,
        'map':                  ROOT.std.map,
        'multimap':             ROOT.std.multimap,
        'multiset':             ROOT.std.multiset,
        'pair':                 ROOT.std.pair,
        'queue':                ROOT.std.queue,
        'set':                  ROOT.std.set,
        'stack':                ROOT.std.stack,
        'string':               ROOT.std.string,
        'vector':               ROOT.std.vector,
        'std::complex':         ROOT.std.complex,
        'std::deque':           ROOT.std.deque,
        'std::list':            ROOT.std.list,
        'std::map':             ROOT.std.map,
        'std::multimap':        ROOT.std.multimap,
        'std::multiset':        ROOT.std.multiset,
        'std::pair':            ROOT.std.pair,
        'std::queue':           ROOT.std.queue,
        'std::set':             ROOT.std.set,
        'std::stack':           ROOT.std.stack,
        'std::string':          ROOT.std.string,
        'std::vector':          ROOT.std.vector
        }

## name-type cache for templated types, populated on the fly
__othertemplates__ = {}

## name-type cache for non-templated types, populated on the fly
__othertypes__ = {}

## type factory cache
#
# this is far from perfect because ROOT rewrites type names behind our backs,
# still there are situations where this saves us some work...
__cache__ = {}

## return True if string s names a templated type.
# 
# @param s      string with name of type
# @return       True/False
def __istemplate__(s):
    return '<' in s or '>' in s

## returns True if string s names a plain old data (POD) type.
# 
# @param s      string with name of type
# @return       True/False
def __isPOD__(s):
    s = s.expandtabs(1).strip()
    return s in __typesPOD__

## returns True if string s names a type from the C++ STL.
#
#@param s       string with name of type
#@return        True/False
def __isSTL__(s):
    s = s.strip()
    return s in __typesSTL__

## converts a string s containing a C++ type name into a parsed
# representation.
# 
# non-templated types return a tuple ( typename, () ).
# 
# templated types return a tuple ( typename, ( typeargs ... ) ) where
# typeargs consists of one or more return values of __parsetype__.
# 
# @param s      name of type
# @return       tuple with parsed components of type
def __parsetype__(s):
    # normalise string
    s = s.expandtabs(1).strip()
    while '  ' in s:
        s = s.replace('  ', ' ')
    # if refer to a non-templated class, return its name
    if not __istemplate__(s):
        return (s, ( ) )
    # template, split into class name and template arguments 
    if (s.count('<') - s.count('>')) != 0:
        raise Exception('Unable to parse template parameter')
    idx1 = s.find('<')
    idx2 = s.rfind('>')
    if len(s) - 1 != idx2:
        raise Exception('Unable to parse template parameter')
    classname = s[0:idx1]
    s = s[idx1 + 1 : idx2].strip()

    t = ()
    idx1 = 0
    idx2 = 0
    while idx2 < len(s) - 1:
        # scan forward for a comma until the number of opening '<' is equal to
        # the number of closing '>'
        if ',' in s[idx2 : ]:
            while idx1 == idx2:
                idx2 = s.find(',', idx2)
                while 0 != (s.count('<', idx1, idx2) - s.count('>', idx1, idx2)):
                    idx2 += 1
                    idx2 = s.find(',', idx2)
                    if idx2 < idx1:
                        idx2 = len(s)
        else:
            idx2 = len(s)
        # have complete template argument list
        t += __parsetype__(s[idx1 : idx2])
        # move to the remainder of the string
        idx1 = idx2 + 1
        idx2 = idx2 + 1
    return classname, t 

## patch the pythonised version of C++ STL map-like objects for sane behaviour
# in python
def __patchmap__(t):
    # patch map, std::map to have a sane __getitem__ and a sane
    # iterator implementation (PyROOT's implementation is doing
    # something fishy at the moment)
    logging.info('patching %s for sane behaviour' % str(t))
    t.__getitem__ = lambda obj, idx: obj.at(idx)
    from veloview.giantrootfile.iter_tools import (
        RangeIter,
        ValueRangeIter,
        KeyRangeIter
    )
    t.__iter__ = lambda obj: KeyRangeIter(obj.begin(), obj.end())
    t.iterkeys = lambda obj: KeyRangeIter(obj.begin(), obj.end())
    t.itervalues = lambda obj: ValueRangeIter(obj.begin(), obj.end())
    t.iteritems = lambda obj: RangeIter(obj.begin(), obj.end())
    t.keys = lambda obj: list(obj.iterkeys())
    t.values = lambda obj: list(obj.itervalues())
    t.items = lambda obj: list(obj.iteritems())
    t.has_key = lambda obj, key: obj.end() != obj.find(key)

## returns an object which can be used to construct objects of the type
# given in the string typename.
#
# @param typename       string containing the C++ name of the type
# @return factory object which allows construction of said type
#
# usage example:
#
# @code
# # get the factory object
# vectdblfactory = getTypeFactory('std::vector<double>')
# # use it to construct a 32 element vector
# v = vectdblfactory(32, 3.14)
# # set element 7 to 2.79
# v[7] = 2.79
# @endcode
def getTypeFactory(typename):
    if type(typename) == str:
        typename = typename.expandtabs(1).strip()
        if typename in __cache__:
            return __cache__[typename]
        typetuple = __parsetype__(typename)
    elif type(typename) == tuple:
        typetuple = typename
    else:
        raise Exception('Wrong type: expect string!')
    t, a = typetuple
    if len(a):
        # template arguments
        al = ()
        while len(a):
            ta = a[0:2]
            a = a[2:]
            if () == ta[1] and __isPOD__(ta[0]): al += (ta[0]),
            else: al += (getTypeFactory(ta),)
        if __isSTL__(t):
            tmp = t
            t = __typesSTL__[t](*al)
            if 'map' == tmp or 'std::map' == tmp:
                __patchmap__(t)
        else:
            if t not in __othertemplates__:
                from ROOT import Template
                __othertemplates__[t] = Template(t)
            tmp = t
            t = __othertemplates__[t](*al)
            if 'VersionedObject' == tmp:
                __patchmap__(t)
    else:
        if __isPOD__(t): t = __typesPOD__[t]
        elif 'string' in t and t in __typesSTL__:
            t = __typesSTL__[t]
        else:
            if t not in __othertypes__:
                exec('from ROOT import %s' % t)
                __othertypes__[t] = eval('ROOT.%s' % t)
            t = __othertypes__[t]
    if type(typename) == str and typename not in __cache__:
        __cache__[typename] = t
    return t

# vim: sw=4:tw=78:ft=python
