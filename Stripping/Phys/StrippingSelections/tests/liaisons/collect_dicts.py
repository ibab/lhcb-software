__author__ = 'Carlos Vazquez Sierra'
__date__ = '2015/05/18'

'''
Module for a quick collection of config dictionaries. Run once in the corresponding WG folder:
StrippingSelections/python/StrippingSelections/StrippingWG/
'''

from __init__ import _selections

save_dicts = True # Set to False if does not want to save dictionaries in an output file!

_dict = {}
for _sel in _selections:
  _sModule = __import__( '%s' % _sel, globals(), locals(), ['default_config'])
  _name = _sModule.default_config['NAME']
  del _sModule.default_config['NAME']
  _dict[_name] = _sModule.default_config

if save_dicts:
  _output = open('dictionaries.py','w')

for key in _dict.keys():
  print '\n'+key+' = '+str(_dict[key])+'\n'
  if save_dicts:
    _output.write('\n'+key+' = '+str(_dict[key])+'\n')

if save_dicts:
  _output.close()

