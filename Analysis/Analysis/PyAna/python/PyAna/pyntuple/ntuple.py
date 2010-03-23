'''
Simple NTuple class

Copyright (c) 2010 Juan Palacios juan.palacios.puyana@gmail.com
Subject to the Lesser GNU Public License - see < http://www.gnu.org/licenses/lgpl.html>
'''

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

__author__ = 'Juan Palacios <juan.palacios@nikhef.nl>'
__version__ = '1.0'

__all__ = ('NTuple')

class NTuple(object) :
    '''
    A simple NTuple class.
    Author: Juan Palacios

    Create an NTuple instance with a set of allowed tags or labels.
    Usage:
    from ntuple import NTuple
    from math import con, sin, pi
    # instantiate an ntuple with tags 'x', 'y' and 'z'
    nt = NTuple('x','y','z)
    # fill it with some values
    for i in xrange(-50,50) :
       nt.fill('var_x', 4+0.01*(i**2))
       nt.fill('s_x', sin(2*pi*i))
       nt.fill('c_x', cos(2*pi*i))
       nt.write()
     # access a column as a list. Apply a cut.
     sin_x = nt.column('s_x', lambda s : s > 0.)
     # merge two NTuples
     nt2 = NTuple('x','y','z)
     for i in xrange(200) :
        nt.fill('var_x', 0.05*(i**2))
        nt.fill('s_x', sin(2*pi*i))
        nt.fill('c_x', cos(2*pi*i))
        nt.write()
     nt3 = nt + nt1
    '''
    def __init__(self, *tags) :
        self._data = []
        self._newRow = True
        self._tags = tags
        self._event = 0

    def fill(self, tag, value) :
        if tag in self._tags :
            if self._newRow : 
                self._setRow()
                self._newRow=False
            setattr(self._data[self._event],tag,value)
        else : raise KeyError('Invalid Tag: '+ str(tag) )

    def row(self, event) :
        return self._data[event]

    def write(self) :
        self._event+=1
        self._newRow = True

    def _setRow(self) :
        self._data.append( Row() )

    def column(self, tag, cuts=None) :
        if not tag in self._tags : raise KeyError('Invalid Tag')
        if cuts == None : return [getattr(row,tag) for row in self._data]
        return [getattr(rw,tag) for rw in filter( cuts, [row for row in self._data])]

    def compatible(self, other) :
        return self._tags == other._tags

    def __add__(self, other) :
        if self.compatible(other) :
            tmp = NTuple(*self._tags)
            tmp._event = self._event+other._event
            tmp._data += self._data
            tmp._data += other._data
            return tmp
        else :
            raise KeyError('Incompatible Tags: '+ str(self._tags) + ' vs. ' + str(other._tags) )

class Row :
    pass

