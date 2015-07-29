#!/usr/bin/env python

"""
Summarize info from merged job
"""

import re

def process(line):
  name,count = re.findall(r'name="(\S+)_TIMING.*>(\d+)<', line)[0]
  return name, int(count)

def read_xml():
  counter = {}
  ntotal  = 0
  with open('summary.xml') as fin:
    for line in fin:
      if '_TIMING/#accept' in line and 'Stripping' in line:
        name, count   = process(line)
        counter[name] = count
      if 'checkPVmin0/Events' in line:
        ntotal = int(re.findall(r'>(\d+)<', line)[0])

  for name,count in sorted(counter.iteritems()):
    frac = 1E4 * count / ntotal 
    print '{name:40} {count:3} / {ntotal:6} --> {frac:5.2f} %%'.format(**locals())

if __name__ == '__main__':
  read_xml()
