"""
Module importing stripping selection line builder modules
for QEE WG.
"""

## For autumn-2015 Run-II End-of-2015 restripping (S24)
## https://twiki.cern.ch/twiki/bin/view/LHCb/LHCbStripping24
list_S24 = (
  ## These line has explicit request from users for Run-II measurement.
  'StrippingDitau',
  'StrippingDisplVertices',
  'StrippingH24Mu',
  'StrippingHighPtTau',
  'StrippingInclbJets',
  'StrippingLb2dp',
  'StrippingLowMultINC',
  'StrippingMuMuSS',           
  'StrippingSingleTrackTIS',
  'StrippingTaggedJets',          # Need Phys/JetTagging post-v1r9
  'StrippingWMu',
  'StrippingWmuAKTJets',
  'StrippingWeAKTJets',
  'StrippingZ02MuMu',

  ## These lines are recovered from S21 `just-in-case`, 
  ## but there's no explicit request in S23+ yet.
  'StrippingDijets',
  'StrippingDY2ee',
  'StrippingDY2MuMu',
  'StrippingLLP2MuX',
  'StrippingSbarSCorrelations',
  'StrippingStrangeBaryons',
  'StrippingStrangeBaryonsNoPID',
  'StrippingWe',
  'StrippingZ02ee',
  
  ## Depreciated
  # 'StrippingHighPtTopoJets',
  # 'StrippingJets',
)

## For winter-2015 Run-I incremental stripping (S21r1p1, S21r0p1)
## https://twiki.cern.ch/twiki/bin/view/LHCb/LHCbStripping21%28r0%2Cr1%29p1
list_S21rXp1 = (
  'StrippingDitau',
  'StrippingFullDiJets',
  'StrippingLb2dp',
  'StrippingMicroDiJets',
  'StrippingTaggedJets',          # Need Phys/JetTagging post-v1r9
)

## Choose the list to use here
_selections = list_S21rXp1


## Boilerplate codes
for _sel in _selections :  
  try :
    __import__( '%s.%s'  % ( __name__, _sel ) )
  except Exception, x:
    print '[WARNING] Submodule %s.%s raises the exception "%s" and will be skipped !' % ( __name__,_sel,x )

## Extract the successfully-imported modules
_strippingModules = [ val for key,val in dict(locals()).iteritems() if key.startswith('Stripping') ]
