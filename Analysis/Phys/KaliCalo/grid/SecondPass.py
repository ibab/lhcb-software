#!/usr/bin/env python
# ============================================================================
# @file FirstPass.py
# @author Ricardo Vazquez Gomez (Ricardo.Vazquez.Gomez@cern.ch)
# @author Albert Puig Navarro (Albert.Puig@cern.ch)
# @date 2009-XII-12
# ============================================================================
"""Send a job to run over fmDST."""

import sys
import os
from optparse import OptionParser

# Hack
sys.path.append('.')
from JobTemplate import createJob

def getFmDSTLFNs(job):
  """Generate a list of the LFNs of the fmDST Output Data of the given job.
  
  @param jobID: jobID to get the fmDSTs from
  @type jobID: int
  
  @return: list of strings
  
  """
  fmDSTList = []
  subjobs = job.subjobs.select(status='completed')
  for subjob in subjobs:
      for output in subjob.backend.getOutputDataLFNs():
          if os.path.splitext(output.name)[1] == '.fmDST':
              fmDSTList.append('LFN:%s' %output.name)
  return fmDSTList
  
if __name__=='__main__':
  usage = "usage: ganga %prog [options]"
  parser = OptionParser(usage=usage)
  parser.add_option("-n", "--name", action="store", type="string", dest="name", default="fmDSTSecondPass", help="name of the job")
  parser.add_option("-j", "--jobid", action="store", type="int", dest="jobid", default=-1, help="jobID to take the input data from")
  parser.add_option("-f", "--fmDST", action="store_true", dest="fmDST", default=False, help="keep the fmDST in the outputdata")
  parser.add_option("-s", "--submit", action="store_true", dest="submit", default=False, help="submit job after creating it")
  (options, args) = parser.parse_args()
  
  print "Creating job with name %s" %options.name
  gridProxy.renew()
  if not options.jobid in jobs.ids():
    print "Job %s is unknown" %options.jobid
    sys.exit()

  j = createJob(options.name)
  oldjob = jobs(jobID)
  j.inputdata = LHCbDataset(oldjob)
  j.application.extraopts = "kali.FirstPass = False"
  if options.fmDST:
    j.outputdata.files.append('KaliPi0.fmDST')
  if options.submit:
    j.submit()

# EOF

