#!/usr/bin/env python

__author__ = 'Carlos Vazquez Sierra'
__date__ = '2015/05/18'

'''
Module for a quick collection of config dictionaries. After the environment is 
set run with:

    ./collect_dicts.py <your WG>

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

## Prepare outputfile
fname = 'dictionaries.py'
with open(fname, 'w') as fout:
    ## Finally, json-dump each one.
    for name, config in sorted(confs.iteritems()):
        config.pop('NAME', None) # Strangely, some conf doesn't have 'NAME' 
        val   = json.dumps( config, sort_keys=True, indent=4 )
        val   = re.sub(r'\[\n\s+(\S+)\n\s+\]', r'[ \1 ]', val) # Pretty one-liner list
        val   = re.sub(r'false', 'False', val) # Capitalize boolean patch
        val   = re.sub(r'true', 'True', val) 
        val   = re.sub(r'null', 'None', val) # null-value patch
        entry = name + ' = ' + val + '\n\n'
        fout.write(entry)
        print 'Write: ', name
print 'Write successfully: '+fname

## Double-check: Re-exec the entire output file.
execfile(fname)
