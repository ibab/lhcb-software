#!/usr/bin/env python
# ============================================================================
# @file FirstPass.py
# @author Ricardo Vazquez Gomez (Ricardo.Vazquez.Gomez@cern.ch)
# @author Albert Puig Navarro (Albert.Puig@cern.ch)
# @date 2009-XII-12
# ============================================================================
"""Send a job to run over DSTs and generate fmDST. Must be run from Ganga, or
otherwise JobTemplate won't work."""

from optparse import OptionParser
import os
import sys

# Hack
sys.path.append('.')
from JobTemplate import createJob

if __name__=='__main__':
  usage = "usage: ganga %prog [options] optionfile1 optionfile2 ..."
  parser = OptionParser(usage=usage)
  parser.add_option("-n", "--name", action="store", type="string", dest="name", default="fmDSTFirstPass", help="name of the job")
  parser.add_option("-s", "--submit", action="store_true", dest="submit", default=False, help="submit job after creating it")
  (options, args) = parser.parse_args()
  
  gridProxy.renew()
  print "Creating job with name %s" %options.name
  j = createJob(options.name)
  paths = []
  if not args:
    paths = ['LFN:/lhcb/MC/MC09/DST/00005694/0000/00005694_00000012_1.dst']
    j.inputdata = LHCbDataset(paths)
  else:
    # All arguments are considered as extra option files (e.g. datasets)
    for arg in args:
      paths.append(os.path.abspath(os.path.expandvars(arg)))
    j.application.optsfile.extend(paths)
  j.outputdata.files.append('KaliPi0.fmDST')
  j.application.extraopts = "kali.PrintFreq=100000;"
  if options.submit:
    j.submit()
    print "Your JobID is %s" %j.id

# EOF

