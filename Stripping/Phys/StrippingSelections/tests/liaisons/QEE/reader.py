#!/usr/bin/env python

"""

Summarize info from merged job

"""

import os
import re
import sys

def valid(line):
  """Return True if this line looks like a stirppingline."""
  return '#accept' in line and 'Stripping' in line and 'Scaler' not in line and 'ExtraInfo' not in line and 'Stream' not in line

def process(line):
  name,count = re.findall(r'name="Stripping(\S+)#accept">(\d+)<', line)[0]
  return name, int(count)

def read_xml(src):
  counter = {}
  ntotal  = 0
  with open(src) as fin:
    for line in fin:
      if valid(line):
        name, count   = process(line)
        counter[name] = count
      if 'checkPVmin0/Events' in line:
        ntotal = int(re.findall(r'>(\d+)<', line)[0])

  for name,count in sorted(counter.iteritems()):
    frac = 1E4 * count / ntotal 
    print '{name:40} {count:6} / {ntotal:8} --> {frac:5.2f} %%'.format(**locals())

  print '\n[ %% for per 10k ]'

if __name__ == '__main__':
  src = 'summary.xml'
  if len(sys.argv)==2:
    jid = sys.argv[1]
    src = os.path.expandvars('$HOME/gangadir/workspace/$USER/LocalXML/%s/output/summary.xml')%jid
  read_xml(src)
