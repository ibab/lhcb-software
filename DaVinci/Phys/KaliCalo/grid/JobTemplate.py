#!/usr/bin/env python
# ============================================================================
# @file JobTemplat.py
# @author Ricardo Vazquez Gomez (Ricardo.Vazquez.Gomez@cern.ch)
# @author Albert Puig Navarro (Albert.Puig@cern.ch)
# @date 2009-XII-12
# ============================================================================
"""Provide a base template for generating a Ganga Job for fmDST analysis."""

import Ganga.GPI as GangaAPI

Job = GangaAPI.Job
DaVinci = GangaAPI.DaVinci
Dirac = GangaAPI.Dirac
DiracSplitter = GangaAPI.DiracSplitter 

def createJob(name='KaliCalofmDST'):
  """Create the job with default options.
  
  @param name: name of the job
  @type name: string
  
  @return: Ganga Job
  
  """
  j = Job(application=DaVinci(), backend=Dirac())
  j.application.platform = 'slc4_ia32_gcc34'
  j.application.optsfile = ['/afs/cern.ch/lhcb/software/releases/ANALYSIS/ANALYSIS_v3r13/Phys/KaliCalo/python/KaliCalo/KaliPi0.py']
  j.application.masterpackage = 'Phys/KaliCalo'
  j.application.version = 'v24r7'
  j.name = name
  j.outputdata = ['KaliPi0_Tuples.root']
  j.splitter = DiracSplitter()
  return j
  
# EOF

