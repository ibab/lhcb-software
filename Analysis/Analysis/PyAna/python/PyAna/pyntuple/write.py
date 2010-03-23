'''
Helper functions for writing and opening ntuples.

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

__all__ = ('gzip_save', 'save')

def gzip_save(obj, filename = "file.ntp") :
    import gzip
    import cPickle
    file = gzip.open(filename, 'wb')
    cPickle.dump(obj, file, 2)
    file.close()

def save(obj, filename = "file.ntp") :
    import pickle
    file = open(filename, 'w')
    pickle.dump(obj, file)
    file.close()
