"""
Usage: SetupBender; ipython checkFT.py streamname dst_file
(Example: ipython checkFT.py Dimuon 00047192_00000021_1.dimuon.dst)
"""

import sys
import GaudiPython as GP
from GaudiConf import IOHelper
from Configurables import (
  LHCbApp,
  ApplicationMgr,
  DataOnDemandSvc,
  DecodeRawEvent,
  CondDB,
  DaVinci
)

## Copypasted from LHCb Starterkit:
## (http://lhcb.github.io/first-analysis-steps/05-interactive-dst.html)

appConf = ApplicationMgr()
dv = DaVinci()
dv.DataType = '2015'
dre = DecodeRawEvent()
dre.DataOnDemand = True
lhcbApp = LHCbApp()
dv.PrintFreq = 1000000 # Only results from FT checking loop.
lhcbApp.Simulation = False
CondDB().Upgrade = True
dv.IgnoreDQFlags = True

inputFiles = [sys.argv[-1]]
stream = str(sys.argv[-2])
IOHelper('ROOT').inputFiles(inputFiles)

appMgr = GP.AppMgr()
evt = appMgr.evtsvc()
appMgr.run(1)

def advance(decision):
    """Advance until stripping decision is true, returns
    number of events by which we advanced"""
    n = 0
    while True:
        appMgr.run(1)
        if not evt['/Event/Rec/Header']:
            print '**** Reached end of input files!'
            break
        n += 1
        dec=evt['/Event/Strip/Phys/DecReports']
        if dec.hasDecisionName('Stripping{0}Decision'.format(decision)):
            break
    return n

## Dictionaries with list of lines (B2CC, REPLACE with your WG lines):

sLines = { 'Leptonic': ['BetaSBu2JpsiKPrescaledLine',
                        'BetaSBs2JpsiPhiPrescaledLine',
                        'BetaSBd2JpsiKstarPrescaledLine',
                        'BetaSBd2JpsiKsPrescaledLine',
                        'BetaSBd2JpsiKsLDDetachedLine',
                        'BetaSBd2JpsiPi0DetachedLine',
                        'BetaSBd2JpsiPi0PrescaledLine',
                        'BetaSBs2JpsiKstarWideLine',
                        'BetaSLambdab2JpsiLambdaUnbiasedLine',
                        'BetaSBs2JpsieePhiFromTracksLine',
                        'BetaSBs2JpsieePhiDetachedLine',
                        'BetaSBs2JpsieePhiLine',
                        'Bd2JpsieeKSBd2JpsieeKSFromTracksPrescaledLine',
                        'Bd2JpsieeKSBd2JpsieeKSFromTracksDetachedLine',
                        'Bd2JpsieeKSBd2JpsieeKSDetachedLine',
                        'Bd2JpsieeKSBd2JpsieeKstarFromTracksPrescaledLine',
                        'Bd2JpsieeKSBd2JpsieeKstarFromTracksDetachedLine',
                        'Bd2JpsieeKSBu2JpsieeKFromTracksDetachedLine'],
           'Bhadron': ['Bs2EtacPhiBDT_KsKPiLine',
                       'Bs2EtacPhiBDTLine'],
           'Dimuon' : ['BetaSBu2JpsiKDetachedLine',
                       'BetaSBd2JpsiKstarDetachedLine',
                       'BetaSBs2JpsiPhiDetachedLine',
                       'BetaSJpsi2MuMuLine',
                       'BetaSBd2JpsiKsDetachedLine'] }

## Check for each line:

print '\n****** RESULTS:'
for line in sLines[stream]:
  advance(line)
  precand = evt['/Event/'+stream+'/Phys/'+line]
  if precand: 
    cand = evt['/Event/'+stream+'/Phys/'+line+'/FlavourTags']
    if cand:
      print '*** Stripping'+line+' ('+stream+') has stored FT results.'
    else: 
      print '*** Stripping'+line+' ('+stream+') has ***NOT*** stored FT results.'
  else:
    print '****** Stripping'+line+' ('+stream+') cannnot be found.\n       Try to remove this line from your list and check it in an isolated way.\n       If not found, then it is not stored in this DST.'
    break
print '\n'
