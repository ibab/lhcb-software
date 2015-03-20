#!/usr/bin/env python
"""
Test to check that all the code in StrippingSelections does not cause breaks
"""

__author__ = 'Stefano Perazzini stefano.perazzini@cern.ch'


import subprocess, sys

if __name__ == '__main__' :

  proc = subprocess.Popen(['python','-c','from StrippingSelections import *'],stdout=subprocess.PIPE)

  out = proc.communicate()[0]

  summary = '\n'
  if out != '': 
    summary += out+'\n'
    wr = sys.stderr.write
  else:          
    summary += 'PASS\n'
    wr = sys.stdout.write

  wr(summary)
