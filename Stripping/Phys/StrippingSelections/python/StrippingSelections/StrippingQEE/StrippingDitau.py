#! /usr/bin/env python

"""

Stripping line for Ditau -> tau+ tau-
======================================

Possible signals
----------------

Higgs: H -> AA -> (b bbar) (tau+ tau-)
m_H ~ 125 GeV
m_A ~  30 GeV

Z0: Z0 -> tau+ tau-


PDG 2012 Data
-------------

Tau decays with following branching ratio

- 17.41 % : Muon              ( mu )
- 17.83 % : Electron          ( e  )
- 50.11 % : 1-Prong hadronic  ( h1 )
- 14.57 % : 3-Prongs hadronic ( h3 ) 
- < 0.1 % : Others: 5-prongs, etc.

Ditau decays in major channels are thus:

\%                mu                e                h1                h3
--- ---------------- ---------------- ----------------- -----------------
mu  $3.031\pm 0.010$ $6.208\pm 0.020$ $17.448\pm 0.051$ $14.602\pm 0.075$
e   ---              $3.179\pm 0.010$ $17.869\pm 0.051$ $5.196\pm 0.028$
h1  ---              ---              $25.110\pm 0.090$ $14.602\pm 0.075$
h3  ---              ---              ---               $2.123\pm 0.020$


Structure
---------

The selection is separated into 2 stages: Tau-candidates, and Ditau-candidates.

At the first level, each 4 types of tau ( e / h1 / h3 / mu ) will have their
own config dictionary. A selection cut will apply to this level before entering
the second stage. It's design to made this way since each of these modes required 
their own condition on PT, isolation cone, etc.

At the ditau level, each of the 10 possibilities of ditau will be made from the 
tau-candidates made at previous level. Each of the 10 modes will also has it own
config dictionary too, allowing maximum flexibility. Once these ditau are made,
they will be groupped into 3 lines which should shared several overlapping events
already, reducing the retention load.

EXLine: ee  , eh1 , eh3 , emu 
MXLine: emu , h1mu, h3mu, mumu 
HHLine: h1h1, h1h3, h3h3 

(As well as the same-sign lines, auto-gen on-the-fly: EXss, MXss, HHss)

The selection is based entirely on FilterDesktop + Combine particle. The config 
dictionary ( 14 of them in total ) is used to generate the cuts on-the-fly.
This allow further flexibility for cutting (not to mention the usefulness during
dev process) by omitting/adding key-val dynamically. 

For example, adding key:

    { 'min_XXX' : 12345 } 

will generate loki-ready cut:

    '(XXX > 12345)' 

dynamically to the `Code`

    The functor should be standard one, or otherwise put it in the `preambulo` section.
Alternatively, provide the loki-ready string to `extracut` which will also be used.


Version history
---------------

## 2.4 -- 2016-March ( For S26 )
- Tuned for 2016 data
- Change the isolation technique to use `RelInfoConeVariables`, instead of
  previously-used `SUMCONE` functor. The key to use this trick is:
      `RelInfoConeVariables + AddRelatedInfo + PassThroughSelection`
- Clean up unused portion of codes.
- New helper function to get minimal ntuple for debugging.

## 2.3 -- 2015-Dec ( For S21rXp1 )
- Tuned for Run-I incremental stripping. S21 had problem of Cone-radius-square.

## 2.2 -- 2015-Nov ( For S24, S21rXp1 )
- `default_config` tuned to S24 campaign.
- Use `from GaudiConfUtils.ConfigurableGenerators` where applicable
- Deleagated more tuning to default_config. Make better use of pythonic-dict unpacking.
  - At tool level, e.g., PVReFitting in tau_h3
  - At Line's constructor. Easing the development of RelatedInfoTools
- Restructure of control lines: 
  - Now there are 2 control Lines: 'XXnoiso', 'XXssnoiso'.
  - Both are made like the signal lines, but with isolation removed, in exchange 
    for prescale. This is planned to used for background such as QCD, V+jet
- New `RelatedInfo::RelInfoConeVariables` in place, which can be used immediately
  in the offline analysis.

## 2.1 -- 150729 (S23 Tuning period summer 2015)
- Properly expand SS lines (*explicit is better than implicit*)
- Fixed duplication in decay descriptor of same particle 
- Separate prescale between os/ss lines
- support for float/boolean value for autocut.
- Option for TES source of particle
- Option for extra preambulo
- Option for CombineParticles.ParticleCombiners
- Option for CombineParticles.Daughtercuts in ditau dict.

## 2.0
- Optimized for Run-II Stripping 23.
- Extends the code into 3 lines: EX, MX, HH. This covers all 10 possibilities of 
  ditau's decay modes.
- Generate SS lines automatically
- Generate cuts automatically + extra preambulo

## 1.2
- Start working on new Mu3Pi. A lot of non-backward structural changes

## 1.1
- Change to conform with new Stripping guideline (default_config['CONFIG'])

## 1.0
- First version.
- The lines aims to supersede `StrippingZ02TauTau.py` from S.Farry & P. Ilten
- The parameters has been optimized for 8 TeV MC of bbtautau & Ztautau
  At least, it shouldn't make it worse than Z02TauTauLines...


-----------------------------------------
Author: chitsanu.khurewathanakul@epfl.ch
Derived from works of S.Farry & P. Ilten
-----------------------------------------

"""

import os
from GaudiKernel.SystemOfUnits import MeV, GeV, micrometer
from PhysSelPython.Wrappers import Selection, SimpleSelection, PassThroughSelection, MergedSelection, AutomaticData
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import AddRelatedInfo
from GaudiConfUtils.ConfigurableGenerators import CombineParticles as GenCombineParticles
from Configurables import CombineParticles
from Configurables import LoKi__PVReFitter
from Configurables import RelInfoConeVariables # Tool
from Configurables import GaudiSequencer


__author__  = 'chitsanu.khurewathanakul@cern.ch'
__date__    = '2016-04-02'
__version__ = 2.4
__all__     = 'DitauConf', 'default_config', 'Tuples'


#-----#
# TAU #
#-----#

config_tau_e = {
  'ISMUONLOOSE'   : False,
  'min_PT'        : 4. * GeV,
  'min_ETA'       : 2.0,
  'max_ETA'       : 4.5,
  'min_CaloPrsE'  : 50 * MeV,
  'min_ECALFrac'  : 0.1,  # Not good for e-conversion ??
  'min_TRPCHI2'   : 0.01,
  'max_TRGHP'     : 0.2,
  'max_HCALFrac'  : 0.05,
  #
  'extracut'      : 'ALL'
}

config_tau_mu = {
  'ISMUON'        : True,
  'min_PT'        : 4. * GeV,
  'min_ETA'       : 2.0,
  'max_ETA'       : 4.5,
  'min_TRPCHI2'   : 0.01,
  'max_TRGHP'     : 0.2,
  'extracut'      : 'ALL',  
}

config_tau_h1 = {
  'ISMUONLOOSE'   : False,
  'min_PT'        : 4. * GeV,
  'min_ETA'       : 2.25,
  'max_ETA'       : 3.75,
  'min_HCALFrac'  : 0.05,
  'min_TRPCHI2'   : 0.01,
  'max_TRGHP'     : 0.2,
  'extracut'      : 'ALL'  
}

config_tau_h3 = {
  'dcuts': {
    'ISMUONLOOSE'   : False,
    'min_PT'        : 500 * MeV,
    'min_ETA'       : 2.0, # 2.0,
    'max_ETA'       : 4.5, # 4.5,
    'min_TRPCHI2'   : 0.01,
    'max_TRGHP'     : 0.2,
    #
    # 'min_HCALFrac'    : 0.01,
    'extracut'      : 'ALL',
  },
  'ccuts': {
    'min_AM'      : 700.  * MeV,
    'max_AM'      : 1500. * MeV,
    'min_APTMAX'  : 2000. * MeV, 
  },
  'mcuts': {
    'min_PT'        : 4 * GeV,
    'max_DRTRIOMAX' : 0.3,
    'max_VCHI2PDOF' : 20.,
  }
}

## Isolation fraction for each tau type
# Defined as pt(tau) / (p(tau)+p(cone))_T
# i.e., relative amount in PT of tau candidate, over the vectorial-sum of 
# momenta of candidate and cone. 
# Full isolated ~ 1
# Anti isolated ~ 0
config_tau_isolation = {
  'tau_e' : 0.7,
  'tau_mu': 0.8,
  'tau_h1': 0.8,
  'tau_h3': 0.8,
}


#-------#
# DITAU #
#-------#

## Blank default fallback. 
# Remember the Zen: *Explicit is better than Implicit*
pcuts0 = {'extracut': 'ALL'} 

config_ditau_e_e = {
  'dcuts': { 'e':pcuts0 },
  'ccuts': {
    'min_APTMAX':  9 * GeV,
    'min_AM'    : 12 * GeV,
  },
  'mcuts': pcuts0,
}

config_ditau_e_h1 = {
  'dcuts': { 'e' : pcuts0, 'pi': pcuts0 },
  'ccuts': {
    'min_APTMAX':   9 * GeV,
    'min_AM'    :  12 * GeV,
    'extracut'  : 'ATRUE',  
  },
  'mcuts': pcuts0,
}

config_ditau_e_h3 = {
  'dcuts': { 'e' : pcuts0, 'tau': pcuts0 },
  'ccuts': {
    'min_APTMAX':  9 * GeV,
    'min_AM'    : 12 * GeV,
  },
  'mcuts': pcuts0,
}

config_ditau_e_mu = {
  'dcuts': { 'e' : pcuts0, 'mu': pcuts0 },
  'ccuts': {
    'min_APTMAX':  9 * GeV,
    'min_AM'    : 12 * GeV,
  },
  'mcuts': pcuts0,
}

config_ditau_h1_h1 = {
  'dcuts': { 'pi' : pcuts0 },
  'ccuts': {
    'min_APTMAX' : 9 * GeV,
    'min_AM'     : 12 * GeV,
  },
  'mcuts': pcuts0,
}

config_ditau_h1_h3 = {
  'dcuts': { 'pi' : pcuts0, 'tau': pcuts0 },
  'ccuts': {
    'min_APTMAX': 12 * GeV,
    'min_AM'    : 16 * GeV,
  },
  'mcuts': pcuts0,
}

config_ditau_h1_mu = {
  'dcuts': { 'pi' : pcuts0, 'mu': pcuts0 },
  'ccuts': {
    'min_APTMAX':  9 * GeV,
    'min_AM'    : 12 * GeV,
  },
  'mcuts': pcuts0,
}

config_ditau_h3_h3 = {
  'dcuts': { 'tau' : pcuts0 },
  'ccuts': {
    'min_APTMAX': 12 * GeV,
    'min_AM'    : 16 * GeV,
  },
  'mcuts': pcuts0,
}

config_ditau_h3_mu = {
  'dcuts': { 'tau' : pcuts0, 'mu': pcuts0 },
  'ccuts': {
    'min_APTMAX' :  9 * GeV,
    'min_AM'     : 12 * GeV,
  },
  'mcuts': pcuts0,
}

config_ditau_mu_mu = {
  'dcuts': { 'mu': pcuts0 },
  'ccuts': {
    'min_APTMAX'  :  4 * GeV,
    'min_AM'      :  8 * GeV,
  },
  'mcuts': pcuts0,
}


## Specify how the decay is groupped into each line
# Groupped by Line's name.
# Note that each decay have selection name, this is quite special.
# - Each has to be unique!
# - The first half before '_' is used to pick the appropriate ditau-level config
# - 
lines_decays = {
  'EX': {
    'ee_os' : '  Z0 ->  e-    e+   ',
    'eh1_os': '[ Z0 ->  e-    pi+  ]cc',
    'eh3_os': '[ Z0 ->  e-    tau+ ]cc',
    'emu_os': '[ Z0 ->  e-    mu+  ]cc',
  },
  'EXnoiso': {
    'ee_os_noiso' : '  Z0 ->  e-    e+   ',
    'eh1_os_noiso': '[ Z0 ->  e-    pi+  ]cc',
    'eh3_os_noiso': '[ Z0 ->  e-    tau+ ]cc',
    'emu_os_noiso': '[ Z0 ->  e-    mu+  ]cc',
  },
  'EXssnoiso': {
    'ee_ss_noiso' : '[ Z0 ->  e-    e-   ]cc',
    'eh1_ss_noiso': '[ Z0 ->  e-    pi-  ]cc',
    'eh3_ss_noiso': '[ Z0 ->  e-    tau- ]cc',
    'emu_ss_noiso': '[ Z0 ->  e-    mu-  ]cc',  
  },
  'MX': {
    'emu_os'  : '[ Z0 ->  e-    mu+  ]cc',
    'h1mu_os' : '[ Z0 ->  pi-   mu+  ]cc',
    'h3mu_os' : '[ Z0 ->  tau-  mu+  ]cc',
    'mumu_os' : '  Z0 ->  mu-   mu+     ',
  },
  'MXnoiso': {
    'emu_os_noiso'  : '[ Z0 ->  e-    mu+  ]cc',
    'h1mu_os_noiso' : '[ Z0 ->  pi-   mu+  ]cc',
    'h3mu_os_noiso' : '[ Z0 ->  tau-  mu+  ]cc',
    'mumu_os_noiso' : '  Z0 ->  mu-   mu+     ',
  },
  'MXssnoiso': {
    'emu_ss_noiso'  : '[ Z0 ->  e-    mu-  ]cc',
    'h1mu_ss_noiso' : '[ Z0 ->  pi-   mu-  ]cc',
    'h3mu_ss_noiso' : '[ Z0 ->  tau-  mu-  ]cc',
    'mumu_ss_noiso' : '[ Z0 ->  mu-   mu-  ]cc',
  },
  'HH': {
    'h1h1_os' : '  Z0 ->  pi-   pi+     ',
    'h1h3_os' : '[ Z0 ->  pi-   tau+ ]cc',
    'h3h3_os' : '  Z0 ->  tau-  tau+    ',
  },
  'HHnoiso': {
    'h1h1_os_noiso' : '  Z0 ->  pi-   pi+     ',
    'h1h3_os_noiso' : '[ Z0 ->  pi-   tau+ ]cc',
    'h3h3_os_noiso' : '  Z0 ->  tau-  tau+    ',
  },
  'HHssnoiso': {
    'h1h1_ss_noiso' : '[ Z0 ->  pi-   pi-  ]cc',
    'h1h3_ss_noiso' : '[ Z0 ->  pi-   tau- ]cc',
    'h3h3_ss_noiso' : '[ Z0 ->  tau-  tau- ]cc',
  },
}

## Default config for the RelatedInfoTool framework.
config_rit_default = [
  {
    'Type'      : 'RelInfoConeVariables',
    'ConeAngle' : 0.5,
    'DaughterLocations': {
      'X0 -> ^X-  X+': 'IsoPlus',
      'X0 ->  X  ^X+': 'IsoMinus',
    },
  },
]

#==============================================================================

default_config = {
  'NAME'        : 'Ditau',
  'BUILDERTYPE' : 'DitauConf',
  'STREAMS'     : [ 'EW'  ],
  'WGs'         : [ 'QEE' ],
  'CONFIG'      : {

    ## Particle identification, full path to ./Particles please
    'TES_mu': 'Phys/StdAllLooseMuons/Particles', 
    'TES_e' : 'Phys/StdAllNoPIDsElectrons/Particles',
    'TES_pi': 'Phys/StdAllNoPIDsPions/Particles',

    ## Individual tau
    'tau_e'   : config_tau_e,
    'tau_h1'  : config_tau_h1,
    'tau_h3'  : config_tau_h3,
    'tau_mu'  : config_tau_mu,

    ## Isolation cuts, removed in same-sign control lines.
    'tau_isolation': config_tau_isolation,

    ## Individual ditau
    'DITAU': {
      'ee'   : config_ditau_e_e,
      'eh1'  : config_ditau_e_h1,
      'eh3'  : config_ditau_e_h3,
      'emu'  : config_ditau_e_mu,
      'h1h1' : config_ditau_h1_h1,
      'h1h3' : config_ditau_h1_h3,
      'h1mu' : config_ditau_h1_mu,
      'h3h3' : config_ditau_h3_h3,
      'h3mu' : config_ditau_h3_mu,
      'mumu' : config_ditau_mu_mu,
    },

    ## StrippingLine constructors. To be auto-populated with **kwargs
    'CONSTRUCTORS': {
      'EX': {
        'prescale'        : 1.,
        'checkPV'         : True,
        'RelatedInfoTools': config_rit_default, 
      },
      'EXnoiso': {
        'prescale'        : 0.01,
        'checkPV'         : True,
        'RelatedInfoTools': config_rit_default, 
      },
      'EXssnoiso': {
        'prescale'        : 0.01,
        'checkPV'         : True,
        # 'RelatedInfoTools': config_rit_default, 
      },
      'HH': {
        'prescale'        : 1.,
        'checkPV'         : True,
        'RelatedInfoTools': config_rit_default, 
      },
      'HHnoiso': {
        'prescale'        : 0.01,
        'checkPV'         : True,
        'RelatedInfoTools': config_rit_default, 
      },
      'HHssnoiso': {
        'prescale'        : 0.01,
        'checkPV'         : True,
        # 'RelatedInfoTools': config_rit_default, 
      },
      'MX': {
        'prescale'        : 1.,
        'checkPV'         : True,
        'RelatedInfoTools': config_rit_default, 
      },
      'MXnoiso': {
        'prescale'        : 0.1,
        'checkPV'         : True,
        'RelatedInfoTools': config_rit_default, 
      },
      'MXssnoiso': {
        'prescale'        : 0.1,
        'checkPV'         : True,
        # 'RelatedInfoTools': config_rit_default, 
      },
    },

    ## For advance usage
    ## Extra preambulo, give me barestring with proper linebreak.
    'preambulo'   : '\n', 
    ## Choice of ParticleCombiner for ditau
    'ditau_pcomb' : {'': 'MomentumCombiner:PUBLIC'},
    ## Fine-tuning the BPV threshold for PVRefit.
    # If this is null, then PV-Refitting in tau_h3 is not performed
    # In general, disable me unless there's cut based on BPV.
    'PVRefitter'  : None
    #   {
    #   'CheckTracksByLHCbIDs': True,
    #   'DeltaChi2'           : 0.01,
    #   'DeltaDistance'       : 5 * micrometer,
    # }
  }
}

#==============================================================================

## Default preambulo

preambulo0 = """

def _MakeTrio(f1, f2, f3):
  #Helper method to generate trio-functors for p with 3 children.
  from LoKiCore.math import min as lkmin
  from LoKiCore.math import max as lkmax
  from LoKiPhys.decorators import NDAUGHTERS
  from LoKiCore.functions import switch
  fmin = lkmin(f1, f2, f3)
  fmax = lkmax(f1, f2, f3)
  fmid = (f1+f2+f3) - fmin - fmax
  # also with guard.
  trioguard = lambda f: switch(NDAUGHTERS==3, f, -1)
  return trioguard(fmin), trioguard(fmid), trioguard(fmax)

def _MakeChildrenDR(i1, i2):                           
  from LoKiPhys.decorators import CHILD,ETA,PHI  
  from LoKiCore.math import cos,acos
  DETA = abs(CHILD(ETA,i1)-CHILD(ETA,i2))
  DPHI = abs(CHILD(PHI,i1)-CHILD(PHI,i2))
  DPHI = acos(cos(DPHI))  # mod pi       
  return (DPHI**2 + DETA**2)**0.5

DRTRIOMIN, DRTRIOMID, DRTRIOMAX = _MakeTrio( _MakeChildrenDR(1,2), _MakeChildrenDR(2,3), _MakeChildrenDR(3,1) )

## Combinations
ABPVCORRMMAX  = AMAXCHILD(BPVCORRM)
APTMIN        = AMINCHILD(PT)
APTMAX        = AMAXCHILD(PT)

## instantiated
_VCHI2PDOF  = VCHI2PDOF
VCHI2PDOF   = VFASPF(_VCHI2PDOF)

## PID
PIDPIONORKAON  = (ABSID==211) | (ABSID==321)
CaloPrsE      = PPFUN(PP_CaloPrsE)
CaloHcalE     = PPFUN(PP_CaloHcalE, -1)     # For lowerbound req
HCALFrac      = PPFUN(PP_CaloHcalE, -1)/P
ECALFrac      = PPFUN(PP_CaloEcalE)/P

""".split('\n')


def parse_single_cut( key, val ):
  """
  Auto-generate the cut string from config key-val. 
  Designed for on-demand cut.
  
  >>> parse_single_cut( 'min_PT', 500 )
  'PT > 500'
  >>> parse_single_cut( 'ISMUON', False )  # Boolean
  '~ISMUON'
  >>> parse_single_cut( 'ABSID', 13 )  # Float
  'ABSID == 13'
  >>> parse_single_cut( 'extracut', 'ISPINKELEPHANT' )
  'ISPINKELEPHANT'

  """
  key = key.strip()
  if key == 'extracut':
    return val
  if key.startswith('max_'):
    var = key.replace('max_', '')
    return '%s < %s' % (var,val)
  if key.startswith('min_'):
    var = key.replace('min_', '')
    return '%s > %s' % (var,val)
  if isinstance(val, bool):
    return '%s%s' % ( '' if val else '~', key )
  # finally... last fallback, bare equality
  return '%s == %s' % (key,val)

def join(cuts):
  """
  Join each predicate in cuts with '&' with bracket.
  - Ignore None
  - Not recursive
  """
  cuts = [ c for c in cuts if c is not None ]
  return "("+") & (".join(cuts)+")"

def parse_cuts_auto(config):
  """
  Given the custom-formatted config-dict, return LoKi-ready string cut generated 
  on-the-fly for each key-val in config (see `parse_single_cut`).

  Note: Since the content inside config may vary, the optional argument
  *cuts should therefore be provided with at least one AND-nullpotent cut,
  such as ALL (for single particle cut), or ATRUE (for combination cut).
  Just to be safe...
  """
  return join([ parse_single_cut(key,val) for key,val in config.iteritems() ])

def doubling_sign_dict(d):
  """
  Given a dict of unsigned-partice, return new dict with both sign.
  Useful for symmetric dict in CombineParticles.DaughtersCuts

  >>> doubling_sign_dict({'pi': 'PT>500'})
  {'pi-': 'PT>500', 'pi+': 'PT>500' }
  """
  return { key+sign:val for key,val in d.iteritems() for sign in ('+','-') }

# def cut_iso_child( index, infoloc ):
#   """
#   Make on-demand functor that cut on isolation var calculated by RelInfoConeVar.
#   This is the former `PTFrac05C` functor.
#   More bump since the PT-fraction needs to be calculated manually.
  
#   ** ACTS ON MOTHER, BUT GET INFO OF CHILD***

#   Remark that, due to the (weird) design of relatedInfo, even when I want to 
#   retrieve the isolation info of a child, I need to use its mother as a pointer,
#   and select the appropriate location where the child's info is stored.
#   As such, the isolation info is outside the CHILDCUT functor, but act on 
#   mother directly.  

#   """
#   PT = 'CHILD(PT,%i)'%index
#   PX = 'CHILD(PX,%i)'%index
#   PY = 'CHILD(PY,%i)'%index
#   CONEPX = "RELINFO('%s', 'CONEPX', 1E12)"%infoloc
#   CONEPY = "RELINFO('%s', 'CONEPY', 1E12)"%infoloc
#   return '({PT}/(({PX}+{CONEPX})**2+({PY}+{CONEPY})**2)**0.5)'.format(**locals())

# def cut_iso_tauchild( index, infoloc, tau_e, tau_mu, tau_h1, tau_h3, **kwargs ):
#   """
#   Extension of above, again, acts on ditau, in order to cut of tau's isolation.
#   This time with conf of isolation values.
#   """
#   func  = cut_iso_child( index, infoloc )
#   taue  = "(CHILDCUT(ABSID==11,{index})   & ({func} > {tau_e}))"
#   taumu = "(CHILDCUT(ABSID==13,{index})   & ({func} > {tau_mu}))"
#   tauh3 = "(CHILDCUT(ABSID==15,{index})   & ({func} > {tau_h3}))"
#   tauh1 = "(CHILDCUT(ABSID==211,{index})  & ({func} > {tau_h1}))"
#   cut   = '({taue}) | ({taumu}) | ({tauh1}) | ({tauh3})'
#   return cut.format(**locals()).format(**locals())

# def cut_iso_ditau( loc1, loc2, tau_mu, tau_e, tau_h1, tau_h3 ):
#   """
#   Even more complex for cutting on isolation from di-tau...
#   """
#   cut1 = cut_iso_tauchild( 1, loc1, **locals())
#   cut2 = cut_iso_tauchild( 2, loc2, **locals())
#   return join([cut1,cut2])

def cut_iso_tau( infoloc, tau_e, tau_mu, tau_h1, tau_h3, **kwargs ):
  """
  The version where tau itself is a pointer to RIF, not mom.
  """
  CONEPX = "RELINFO('%s', 'CONEPX')"%infoloc
  CONEPY = "RELINFO('%s', 'CONEPY')"%infoloc
  func   = '(PT/((PX+{CONEPX})**2+(PY+{CONEPY})**2)**0.5)'.format(**locals())
  #
  c1 = '(ABSID==11)  & ({func} > {tau_e})'
  c2 = '(ABSID==13)  & ({func} > {tau_mu})'
  c3 = '(ABSID==15)  & ({func} > {tau_h3})'
  c4 = '(ABSID==211) & ({func} > {tau_h1})'
  return  '({c1}) | ({c2}) | ({c3}) | ({c4})'.format(**locals()).format(**locals())


#------------------------------------------------------------------------------
# SELECTION
#------------------------------------------------------------------------------

def selection_filt( config, preambulo, sel, newname ):
  """
  Given a selection, apply FilterDesktop on top of it, 
  with cuts from config to be parsed.
  """
  return SimpleSelection( newname, FilterDesktop, sel,
    Preambulo = preambulo0 + preambulo.split('\n'),
    Code      = parse_cuts_auto(config),
  )

def selection_tau_h3( Config, preambulo, inputs ):
  config = Config['tau_h3']  ## Pickout tau_h3 config from big one.
  #
  dcut = parse_cuts_auto(config['dcuts'])
  algo = CombineParticles('CombTauNoiso_h3')
  algo.DecayDescriptor   = '[ tau- -> pi- pi- pi+ ]cc'
  algo.Preambulo         = preambulo0 + preambulo.split('\n')
  algo.DaughtersCuts     = {'pi-':dcut, 'pi+':dcut}
  algo.CombinationCut    = parse_cuts_auto( config['ccuts'] )
  algo.MotherCut         = parse_cuts_auto( config['mcuts'] )
  #
  config_refit = Config['PVRefitter']
  if config_refit:
    tool = LoKi__PVReFitter('PVRefitter_tauh3', **config_refit)
    algo.ReFitPVs = True
    algo.IgnoreP2PVFromInputLocations = True
    algo.addTool( tool )
    algo.PVReFitters.update({ '': 'LoKi::PVReFitter/PVRefitter_tauh3' })
  #
  return Selection( 'SelTauNoiso_h3', Algorithm=algo, RequiredSelections=inputs )


def selection_tauiso( tau_noiso, conf ):
  """
  Take single Selection containing heteregenous list of 'tau' (11,13,15,211),
  cut on isolation variable given for each tau type.

  The code is separated into 2 parts:
  1. Attach related info to given Selection: Phys/TauNoiso/Iso
  2. Perform actual cut

  Sadly, I couldn't find a way to add info without staging a new selection,
  so I use PassThroughSelection.

  """
  ## First part is attaching
  tool          = RelInfoConeVariables('TauIsoInfo', ConeAngle=0.5)
  algo          = AddRelatedInfo()
  algo.Inputs   = [ tau_noiso.outputLocation() ]
  algo.Location = 'Iso' # Use for chaining!
  algo.Tool     = tool.getFullName()
  tau_withinfo  = PassThroughSelection( 'TauInfo', Algorithm=algo, RequiredSelection=tau_noiso )

  ## Second part is filtering
  loc  = tau_noiso.outputLocation().replace('/Particles', '/Iso')
  code = cut_iso_tau( loc, **conf )
  return SimpleSelection( 'TauIso', FilterDesktop, [tau_withinfo], Code=code )


def selection_ditau( config, preambulo, pcomb, dcname, decay, inputs ):
  ## dcuts are quite complicate since it's a-priori nested dict index by particle-name.
  dcuts = { key:parse_cuts_auto(val) for key,val in doubling_sign_dict(config['dcuts']).iteritems() }
  return SimpleSelection( 'SelDitau_'+dcname, GenCombineParticles, inputs,
    Preambulo         = preambulo0 + preambulo.split('\n'),
    DecayDescriptor   = decay,
    DaughtersCuts     = dcuts,
    CombinationCut    = parse_cuts_auto(config['ccuts']),
    MotherCut         = parse_cuts_auto(config['mcuts']),
    ParticleCombiners = pcomb,
    # CombinationCut  = 'AALLSAMEBPV',  # BUGGY, don't use me!
  )

# def rif_isolation( fulllinename, conf ):
#   """
#   Make FilterDesktop to be used as RelatedInfoFilter
#   fulllinename ~ DitauEXssnoisoLine
#   """
#   code = cut_iso_ditau( 'Phys/TauNoiso/Iso', **conf )

#   # ## Prepare the cut, delegates hard work externally.
#   # locm = 'Phys/%s/IsoMinus'%fulllinename
#   # locp = 'Phys/%s/IsoPlus' %fulllinename
#   # code = cut_iso_ditau( loc1, loc2, **conf )

#   ## Prepare the algo, use vanilla version
#   from Configurables import FilterDesktop
#   filt = FilterDesktop(fulllinename.replace('Line','isoLine'))
#   filt.Code   = code
#   filt.Inputs = [ 'Phys/%s/Particles'%fulllinename ]
#   filt.CloneFilteredParticles = True

#   print filt

#   return filt

#==============================================================================

## Nomenclatures on name, just to not get myself confused...
# NAME    : Formal name of stripping line, by default, 'Ditau'
# dtype   : Decay type of ditau (no sign), e.g., eh1, mumu
# dcname  : Decay name of ditau (with sign), e.g., eh1_ss, mumu_os
# linename: Short line name, e.g., EX, MXss 
#           (note that opposite-sign os is dropped)


class DitauConf(LineBuilder):

  __configuration_keys__ = default_config['CONFIG'].keys()  # Legacy field

  # Note: To be instantiated by LineBuilder( default_config['NAME'], default_config['CONFIG'] )
  def __init__(self, NAME, CONFIG):

    ## Required the explicit name/dconfig, no default allow
    LineBuilder.__init__(self, NAME, CONFIG)

    ## Prepare raw particle for identification
    preambulo   = CONFIG['preambulo']
    ditau_pcomb = CONFIG['ditau_pcomb']
    input_e     = [ AutomaticData(Location=CONFIG['TES_e'])  ]
    input_mu    = [ AutomaticData(Location=CONFIG['TES_mu']) ]
    input_pi    = [ AutomaticData(Location=CONFIG['TES_pi']) ]

    ## Make pre-selection tau candidates, no isolation here.
    ## Merge these together (parallel, not sequential selection)
    tau_noiso = MergedSelection('TauNoiso', [
      selection_filt  ( CONFIG['tau_e']  , preambulo, input_e  , 'SelTauNoiso_e'  ),
      selection_filt  ( CONFIG['tau_mu'] , preambulo, input_mu , 'SelTauNoiso_mu' ),
      selection_filt  ( CONFIG['tau_h1'] , preambulo, input_pi , 'SelTauNoiso_h1' ),
      selection_tau_h3( CONFIG           , preambulo, input_pi ),
    ])

    # ## Cut on isolation: RelInfoConeVariables + AddRelatedInfo + FilterDesktop
    tau_iso = selection_tauiso( tau_noiso, CONFIG['tau_isolation'] )

    ## Prepare selection of ditau
    # Selections, indexed by dcname (for emu sharing). Think of it as caching.
    sels_ditau = {}  

    ## Loop over all lines, make groupping
    for linename, condict in CONFIG['CONSTRUCTORS'].iteritems():
      ## Make the fullname: e.g., 'Ditau' + 'EXss' + 'Line'
      fullname = NAME + linename + 'Line'
      ## Get list of decay, indexed by Line's name      
      dcdict = lines_decays[linename]  
      ## Make list of selections to be put in single line
      sels = []
      for dcname, decay in dcdict.iteritems():
        ## Make selection if it hasn't been done.
        if dcname not in sels_ditau:
          dtype   = dcname.split('_')[0]      # eh1_ss --> eh1
          config  = CONFIG['DITAU'][dtype]    # Same ditau-level config for OS/SS
          inputs  = [ tau_noiso if 'noiso' in linename else tau_iso ]
          # inputs  = [ tau_noiso ]
          sel     = selection_ditau( config, preambulo, ditau_pcomb, dcname, decay, inputs )
          sels_ditau[dcname] = sel
        sels.append(sels_ditau[dcname])
      ## Finally, merged it
      selection = MergedSelection( 'Sel'+linename, sels )

      # ## Adding isolation cut on the ditau level.
      # filt = None 
      # if 'noiso' not in linename:
      #   filt = rif_isolation( fullname, CONFIG['tau_isolation'] )

      ## Finally, compose a stripping line, register
      self.registerLine(StrippingLine( fullname,
        selection         = selection,
        # RelatedInfoFilter = filt,
        **CONFIG['CONSTRUCTORS'][linename]
      ))

#===============================================================================

def Tuples():
  """
  Simple class-like method to return DecayTreeTuple which can be used to 
  do quick-check on the output DST.

  Usage:
  >> from StrippingSelections.StrippingQEE import StrippingDitau
  >> DaVinci().appendToMainSequence( StrippingDitau.Tuples() )
  >> DaVinci().TupleFile = 'tuple.root'

  """
  import re
  from DecayTreeTuple.Configuration import DecayTreeTuple, EventTuple

  seq = GaudiSequencer('TupleSeq')
  seq.ModeOR       = True
  seq.ShortCircuit = False
  for lname, decays in lines_decays.iteritems():
    for process, desc in decays.iteritems():
      desc = re.sub( r'  ([a-z])', r' ^\1', desc) # regex insert ^
      desc = desc.replace('cc', 'CC') # different between CombineP & DTT
      #
      ## Extra complication for cand with RIF, such that the input location
      # after isolation filter is not at default line location, but on the 
      # last algo's location. Otherwise the tuple will pick up candidates 
      # before isolation filtering.
      root = 'Phys/Ditau%sLine'%lname
      # if 'noiso' not in root:
      #   root = root.replace('Line','isoLine')
      #
      tup   = DecayTreeTuple('Tup_'+process, NTupleDir='Ditau', TupleName=process)
      tup.Inputs    = [ root+'/Particles' ]
      tup.Decay     = desc
      tup.ToolList  = [
        'TupleToolKinematic',
      ]
      tool = tup.addTupleTool('TupleToolConeIsolation')
      tool.MinConeSize = 0.5
      tool.MaxConeSize = 0.5
      tool = tup.addTupleTool('LoKi::Hybrid::TupleTool')
      tool.Variables = {
        'CONEPX1': "RELINFO('%s/IsoPlus' , 'CONEPX')"%root,
        'CONEPX2': "RELINFO('%s/IsoMinus', 'CONEPX')"%root,
        'CONEPY1': "RELINFO('%s/IsoPlus' , 'CONEPY')"%root,
        'CONEPY2': "RELINFO('%s/IsoMinus', 'CONEPY')"%root,
        'CONEPT1': "RELINFO('%s/IsoPlus' , 'CONEPT')"%root,
        'CONEPT2': "RELINFO('%s/IsoMinus', 'CONEPT')"%root,
      }
      seq.Members.append(tup)
  return [seq]

