#!/usr/bin/env python
# ============================================================================
# @file GetNTuples.py
# @author Albert Puig Navarro (Albert.Puig@cern.ch)
# @date 2009-XII-15
# ============================================================================
"""Utility to extract the LFNs of the output tuples and put them in a file."""
import sys
import os
from optparse import OptionParser

def getTupleLFNs(job):
  """Generate a list of the LFNs of the ROOT Output Data of the given job.
  
  @param jobID: jobID to get the fmDSTs from
  @type jobID: int
  
  @return: list of strings
  
  """
  tupleList = []
  subjobs = job.subjobs.select(status='completed')
  for subjob in subjobs:
      for output in subjob.backend.getOutputDataLFNs():
          if os.path.splitext(output.name)[1] == '.root':
              tupleList.append('LFN:%s' %output.name)
  return tupleList
  
if __name__=='__main__':  
  usage = "usage: ganga %prog [options] output"
  parser = OptionParser(usage=usage)
  parser.add_option("-j", "--jobid", action="store", type="int", dest="jobid", default=-1, help="jobID to take the input data from")
  (options, args) = parser.parse_args()

  if options.jobid not in jobs.ids():
    print "Unknown jobID %s" %options.jobid
    sys.exit()
  if len(args) != 1:
    print "Wrong number of arguments"
    print usage
    sys.exit()
  list = getTupleLFNs(jobs(options.jobid))
  print "Writing ntuple LFNs to %s" %args[0]
  f = open(args[0], 'w')
  for line in list:
    f.write(line + '\n')
  f.close()

# EOF

