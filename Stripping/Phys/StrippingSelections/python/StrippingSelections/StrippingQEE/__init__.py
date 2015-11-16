"""
Module importing stripping selection line builder modules
for QEE WG.
"""


_selections = (
  ## These line has explicit request from users for Run-II measurement.
  'StrippingDitau',
  'StrippingDisplVertices',    
  'StrippingH24Mu',
  'StrippingInclbJets',
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

for _sel in _selections :  
  try :
    __import__( '%s.%s'  % ( __name__, _sel ) )
  except Exception, x:
    print '[WARNING] Submodule %s.%s raises the exception "%s" and will be skipped !' % ( __name__,_sel,x )

_strippingModules = [ val for key,val in dict(locals()).iteritems() if key.startswith('Stripping') ]
