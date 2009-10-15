#!/usr/bin/env python
"""
Module which implements different text matchers
"""
import re

_exponentexpr = "(e|E)[+-]?\d+"
_fracexpr = "\.\d+"
_intpartexpr = "\d+"
_pointfloatexpr = "(%s)?%s|(%s)\." % (_intpartexpr, _fracexpr, _intpartexpr)
_exponentfloatexpr = "(%s|%s)%s" % (_intpartexpr, 
                                    _pointfloatexpr, 
                                    _exponentexpr)
_floatnumberexpr = "(%s|%s)" % (_pointfloatexpr, _exponentfloatexpr)

 
_decnumberexpr = "(?=[^\.])([1-9][0-9]*|0)(?=[^\.])"
#_intnumberexpr = "([1-9][0-9]*|0)"
_hexnumberexpr = "0[xX][0-9a-fA-F]+"
#_intnumberexpr = "[^.:](\d+)[^.]"
_octnumberexpr = "0[1-7]+[0-7]*"
_intnumberexpr = "(%s|%s|%s)" % (_hexnumberexpr, _octnumberexpr, _decnumberexpr)

class Match(object):
    """ Generic match interface """
    def __init__(self, customexpr=None):
        if customexpr:
            self._matchexpr = re.compile(customexpr)
    def getNext(self, textinput):
        """ get next match """
        for m in self._matchexpr.finditer(textinput):
            yield m.group()

class KeyWordMatch(Match):
    """ Generic keyword match """
    def __init__(self, keywordlist=None):
        self._keylist = keywordlist
        keywexpr = "|".join(keywordlist)
        super(KeyWordMatch, self).__init__(keywexpr)


class MatchedNumber(object):
    def __init__(self, relprecision=None):
        pass

class NumberMatch(Match):
    """ generic number match with separators """
    def __init__(self, numberexpr, relprecision=None, absprecision=None):
        super(NumberMatch, self).__init__("(?P<number>%s)(?=\W)" % numberexpr)
        self._relprec = relprecision
        self._ablsprec = absprecision
    def getNext(self, textinput):
        for m in self._matchexpr.finditer(textinput):
            yield m.group("number")

  
class MatchedFloat(float):
    """ Wrapper for floating point number with precision """
    def __init__(self, stringrepr, relprecision=None):
        super(MatchedFloat, self).__init__(stringrepr)
        self.value = float(stringrepr)
        if relprecision == None:
            self.prec = 0.0
        else:
            self.prec = relprecision
    def __cmp__(self, other):
        isequal = False
        allowedsep = abs(self.value*self.prec)
        allowedsep += abs(other.value*other.prec)
        if abs(self.value-other.value) < allowedsep:
            isequal = True
        return isequal


class FloatMatch(NumberMatch):
    """ Generic floating point number match """
    def __init__(self, relprecision=None, absprecision=None):
        super(FloatMatch, self).__init__(_floatnumberexpr, relprecision, absprecision)
    def getNext(self, textinput):
        """ get next match and convert it to float """
        for i in super(FloatMatch, self).getNext(textinput):
            yield MatchedFloat(i)


class MatchedInt(int):
    """ Wrapper for floating point number with precision """
    def __init__(self, stringrepr, relprecision=None):
        super(MatchedInt, self).__init__(stringrepr)
        self.value = int(stringrepr)
        if relprecision == None:
            self.prec = 0.0
        else:
            self.prec = relprecision
    def __cmp__(self, other):
        isequal = False
        allowedsep = abs(self.value*self.prec)
        allowedsep += abs(other.value*other.prec)
        if abs(self.value-other.value) < int(allowedsep):
            isequal = True
        return isequal

class IntMatch(NumberMatch):
    """ Generic integer number match """
    def __init__(self, relprecision=None, absprecision=None):
        super(IntMatch, self).__init__(_intnumberexpr, relprecision, absprecision)
    def getNext(self, textinput):
        """ get next match and convert it to int """
        for i in super(IntMatch, self).getNext(textinput):
            yield MatchedInt(i)


class MatchedDec(int):
    """ Wrapper for floating point number with precision """
    def __init__(self, stringrepr, relprecision=None):
        super(MatchedDec, self).__init__(stringrepr)
        self.value = int(stringrepr)
        if relprecision == None:
            self.prec = 0.0
        else:
            self.prec = relprecision
    def __cmp__(self, other):
        isequal = False
        allowedsep = abs(self.value*self.prec)
        allowedsep += abs(other.value*other.prec)
        if abs(self.value-other.value) < int(allowedsep):
            isequal = True
        return isequal

class DecMatch(NumberMatch):
    """ Generic integer number match """
    def __init__(self, relprecision=None, absprecision=None):
        super(DecMatch, self).__init__(_decnumberexpr, relprecision, absprecision)
    def getNext(self, textinput):
        """ get next match and convert it to int """
        for i in super(DecMatch, self).getNext(textinput):
            yield MatchedDec(i)

class MatchedHex(int):
    """ Wrapper for floating point number with precision """
    def __init__(self, stringrepr, relprecision=None):
        super(MatchedHex, self).__init__(eval(stringrepr))
        self.value = hex(eval(stringrepr))
        if relprecision == None:
            self.prec = 0.0
        else:
            self.prec = relprecision
    def __cmp__(self, other):
        isequal = False
        allowedsep = abs(self.value*self.prec)
        allowedsep += abs(other.value*other.prec)
        if abs(self.value-other.value) < int(allowedsep):
            isequal = True
        return isequal
    
class HexMatch(NumberMatch):
    """ Generic integer number match """
    def __init__(self, relprecision=None, absprecision=None):
        super(HexMatch, self).__init__(_hexnumberexpr, relprecision, absprecision)
    def getNext(self, textinput):
        """ get next match and convert it to int """
        for i in super(HexMatch, self).getNext(textinput):
            yield MatchedHex(i)

class MatchedOct(int):
    """ Wrapper for floating point number with precision """
    def __init__(self, stringrepr, relprecision=None):
        super(MatchedOct, self).__init__(eval(stringrepr))
        self.value = oct(eval(stringrepr))
        if relprecision == None:
            self.prec = 0.0
        else:
            self.prec = relprecision
    def __cmp__(self, other):
        isequal = False
        allowedsep = abs(self.value*self.prec)
        allowedsep += abs(other.value*other.prec)
        if abs(self.value-other.value) < int(allowedsep):
            isequal = True
        return isequal
    
class OctMatch(NumberMatch):
    """ Generic integer number match """
    def __init__(self, relprecision=None, absprecision=None):
        super(OctMatch, self).__init__(_octnumberexpr, relprecision, absprecision)
    def getNext(self, textinput):
        """ get next match and convert it to int """
        for i in super(OctMatch, self).getNext(textinput):
            yield MatchedOct(i)

 
