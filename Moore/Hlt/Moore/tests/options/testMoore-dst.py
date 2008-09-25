#!/usr/bin/env gaudirun.py
#
# Minimal file for running Moore from python prompt
# Syntax is:
#   gaudirun.py ../options/Moore.py
# or just
#   ../options/Moore.py
#
from Gaudi.Configuration import *
from Moore.Configuration import *

#//---------------------------------------------------------------------------
#// Histogram output
#//---------------------------------------------------------------------------
ApplicationMgr().HistogramPersistency = 'ROOT'
HistogramPersistencySvc().OutputFile = 'Moore_minbias.root'

#---------------------------------------------------------------------------
# Number of events to process, optionally skipping some events
#---------------------------------------------------------------------------

Moore().hltType = 'PA+VE+LU+MU+HA+EL+PH'
Moore().oldStyle = False

files = [ 'castor:/castor/cern.ch/user/s/snies/mdf/DC06_L0_v1_lumi2_MuonHadron_40000ev_1.mdf',
          'castor:/castor/cern.ch/user/s/snies/mdf/DC06_L0_v1_lumi2_MuonHadron_40000ev_2.mdf' ]

filetype = files[0][-3:].upper()
Moore().inputType = filetype
extensions = { 'RAW' : "' SVC='LHCb::MDFSelector'",
               'MDF' : "' SVC='LHCb::MDFSelector'",
               'DST' : "' TYP='POOL_ROOTTREE' OPT='READ'" }
EventSelector().Input = [ "DATAFILE='PFN:"+ f + extensions[ filetype ] for f in files ]
EventSelector().PrintFreq = 100

Moore().EvtMax = 2000
Moore().applyConf()

print Moore()
