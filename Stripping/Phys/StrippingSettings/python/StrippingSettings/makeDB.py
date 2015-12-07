#!/usr/bin/env python

import os
import sys

'''
Generate a dbase module from a set of stripping line config
dictionaries.

Usage: 
  > python makeDB <Stripping> 
'''

import string

def my_import(name):
    mod = __import__(name)
    components = string.split(name, '.')
    for comp in components[1:]:
        mod = getattr(mod, comp)
    return mod

def generate_dbase( config ):
   
   _strippingmod = my_import( 'StrippingSettings.'+config )
   _modulenames  = _strippingmod.__all__

   _configs  = [] 
   
   for _name in _modulenames:
      _configs += [ my_import( 'StrippingSettings.'+config + '.'+_name ) ]
         
   from StrippingSettings.Utils import addModuleToDbase

   for _config in _configs :
      addModuleToDbase('stripping.tmp', _config, verbose=True)
      
  
if 2 == len(sys.argv):
   generate_dbase( sys.argv[1] )
