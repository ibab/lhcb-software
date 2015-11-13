__author__ = 'Carlos Vazquez Sierra'
__date__ = '2015/05/18'

'''
Module for a quick collection of config dictionaries. After the environment is 
set run with:

    python collect_dicts.py <your WG>

It will create a file called `dictionaries.py` with all the configuration of the 
lines of your working group as specified by the users in their default_config

Modified by Andrea Contu 03/06/15
Modified by C.Khurewathanakul 15/06/15
'''

import re
import sys
import json

# Specify the name of your configuration
my_wg=sys.argv[1] #FOR LIAISONS


# NOTE: this will work only if you inserted correctly the 
# default_config dictionary in the code where your LineBuilder 
# is defined.
from StrippingSelections import buildersConf
confs = buildersConf(WGs=[ my_wg ])

dictlist = ['',
    ] # List the names of the modules you want the dictionaries of


## Prepare outputfile
fname = 'dictionaries.py'
with open(fname, 'w') as fout:
    ## Finally, json-dump each one.
    for name, config in sorted(confs.iteritems()):
        if name in dictlist:
            config.pop('NAME', None) # Strangely, some conf doesn't have 'NAME' 
            val   = json.dumps( config, sort_keys=True, indent=4 )
            val   = re.sub(r'\[\n\s+(\S+)\n\s+\]', r'[ \1 ]', val) # Pretty one-liner list
            val   = re.sub(r'false', 'False', val) # Capitalize boolean patch
            val   = re.sub(r'true', 'True', val) 
            val   = re.sub(r'null', 'None', val) # null-value patch
            entry = name + ' = ' + val + '\n\n'
            fout.write(entry)
            print '\tWrite: ', name
            dictlist.remove(name)
        else:
            print name,' omitted, not in specified list'
print 'Write successfully: '+fname
print 'Missing dictionaries: ',dictlist
## Double-check: Re-exec the entire output file.
execfile(fname)


#--------

# for k,mod in confs.iteritems():
#     if mod.has_key("WGs"):
#         print mod['WGs']
#         if my_wg== (mod["WGs"])[0] or my_wg==mod["WGs"]:
#             _dict[k]=mod


# def makestring(input):
#     if isinstance(input,basestring):
#         if "\'" not in input:
#             return "\'"+input+"\'"
#         else:
#             return "\""+input+"\""
#     else:
#         return str(input)

# _output = open('dictionaries.py','w')

# _ordkeys = ['BUILDERTYPE', 'STREAMS', 'WGs', 'CONFIG']

# _dictkeys=_dict.keys()
# _dictkeys.sort()


# for key in _dictkeys:
#     md=_dict[key]
#     #_output.write('\n'+key+' = '+s+'\n')
#     _output.write('\n'+key+' = {\n')
#     for k in _ordkeys:
#         if k!='CONFIG' and k!='STREAMS':
#             _output.write("    "+makestring(k)+" : "+makestring(md[k])+",\n")
#         elif k=='STREAMS':
#             if isinstance(md[k],basestring):
#                 _output.write("    "+makestring(k)+" : "+makestring(md[k])+",\n")
#             elif isinstance(md[k],dict):
#                 _output.write("    "+makestring(k)+" : {,\n")
#                 for streamdict in (md[k]).keys():
#                     _output.write("        "+makestring(streamdict)+" : "+ makestring((md[k])[streamdict]) + ",\n")
#                 _output.write("    },\n")

#             else:
#                 _output.write("    "+makestring(k)+" : "+str(md[k])+",\n")
#         else:
#             _output.write("    "+makestring(k)+" : { \n")
#             confdictkeys=((md[k])).keys()
#             confdictkeys.sort()
#             for kconf in confdictkeys:
#                 if isinstance(((md[k])[kconf]), basestring):
#                     _output.write("        "+makestring(kconf)+" : "+ makestring((md[k])[kconf]) +",\n")
#                 elif isinstance(((md[k])[kconf]), dict):
#                     _output.write("        "+makestring(kconf)+" : {\n")
#                     for cdict in ((md[k])[kconf]).keys():
#                         _output.write("            "+makestring(cdict)+" : "+ makestring(((md[k])[kconf])[cdict]) +",\n")
                         
#                     _output.write("        "+"},\n")
#                 else:
#                     sconf=str((md[k])[kconf])
#                     _output.write("        "+makestring(kconf)+" : "+ sconf +",\n")
#             _output.write("    } \n")
#     _output.write('}\n')

#from __init__ import _selections

#save_dicts = True # Set to False if does not want to save dictionaries in an output file!

#err_str = ''
#_dict = {}
#for _sel in _selections:
#  try:
#    _sModule = __import__( '%s' % _sel, globals(), locals(), ['default_config'])
#  except:
#    err_str += 'No default_config found for %s module! Skipping...\n' % _sel
#  try:
#    _name = _sModule.default_config['NAME']
#    del _sModule.default_config['NAME']
#    _dict[_name] = _sModule.default_config
#  except:
#    err_str += 'No NAME key found for default_config in %s module! Skipping...\n' % _sel

#if save_dicts:
#  _output = open('dictionaries.py','w')

#for key in _dict.keys():
#  print '\n'+key+' = '+str(_dict[key])+'\n'
#  if save_dicts:
#    _output.write('\n'+key+' = '+str(_dict[key])+'\n')

#if save_dicts:_output.write('\n'+key+' = '+str(_dict[key])+'\n')
#  _output.close()

#if len(err_str)>1:
#  print 'WARNINGS:\n'+err_str
