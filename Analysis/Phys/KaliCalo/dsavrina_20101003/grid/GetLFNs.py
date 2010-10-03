#!/usr/bin/env python
# ============================================================================
# @file GetLFNs.py
# @author Albert Puig Navarro (Albert.Puig@cern.ch)
# @date 2009-XII-16
# ============================================================================
"""Get OutputData LFN of a Ganga Job matching a certain regular expression and
write them into a file"""

import sys
import os
import re
from optparse import OptionParser

def getLFNs(job, regex):
  """Generate a list of the LFNs of the ROOT Output Data of the given job
  by selecting them using the given regular expression.

  Example:
  >> getTupleLFNs(job, 'fmDST')
  ['LFN:/grid/KaliCalo.fmDST']
  
  @param job: job to get the output data from
  @type job: GangaJob
  @param regex: regular expression to use
  @type regex: string
  
  @return: list of strings
  
  """
  lfnList = []
  myre = re.compile(regex)
  subjobs = job.subjobs.select(status='completed')
  for subjob in subjobs:
      for output in subjob.backend.getOutputDataLFNs():
          if myre.search(output.name):
              lfnList.append('LFN:%s' %output.name)
  return lfnList
  
if __name__=='__main__':  
  # Warning: Can only be used using ganga!
  usage = "usage: ganga %prog [options] output"
  parser = OptionParser(usage=usage)
  parser.add_option("-j", "--jobid", action="store", type="int", dest="jobid", default=-1, help="jobID to take the output data from")
  parser.add_option("-r", "--regex", action="store", type="str", dest="regex", default=".*", help="regular expression to filter outputdata results")
  (options, args) = parser.parse_args()

  if options.jobid not in jobs.ids():
    print "Unknown jobID %s" %options.jobid
    sys.exit()
  if len(args) != 1:
    print "Wrong number of arguments"
    print usage
    sys.exit()
  list = getLFNs(jobs(options.jobid), options.regex)
  f = open(args[0], 'w')
  for line in list:
    f.write(line + '\n')
  f.close()

# EOF
  
