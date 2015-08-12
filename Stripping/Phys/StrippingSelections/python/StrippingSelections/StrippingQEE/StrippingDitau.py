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

- 46.70 % : 1-Prong hadronic  ( h1 )
- 17.83 % : Electron          ( e  )
- 17.41 % : Muon              ( mu )
- 11.69 % : 3-Prongs hadronic ( h3 ) 
-  6.37 % : Others: K, 5-prongs, etc.

Ditau decays in major channels are thus (in alphebetical order):

\%       e     mu     h1     h3    other
----- ------ ------ ------ ------ -------
e       3.2     -      -      -     -
mu      6.2    3.0     -      -     -
h1     16.7   16.3   21.8     -     -
h3      4.2    4.1   10.9    1.4    -
other   2.3    2.2    5.9    1.5   0.4


Structure
---------

The selection is separated into 2 stages: Tau-candidates, and ditau-candidates.

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
dev process) by omitting/adding key-val dynamically. ( For example, adding key
'min_XXX':12345 will generate cut (XXX > 12345) dynamically to the `Code` ).
The functor should be standard one, or otherwise put it in the preambulo section.


Version history
---------------

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

from GaudiKernel.SystemOfUnits import MeV, GeV, mm
from PhysSelPython.Wrappers import Selection, SimpleSelection, MergedSelection, AutomaticData
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine
from StandardParticles import PFParticles
from Configurables import CombineParticles, FilterDesktop


__author__  = 'chitsanu.khurewathanakul@cern.ch'
__date__    = '2015-07-29'
__version__ = 2.1
__all__     = [ 'DitauConf', 'default_config' ]


#-----#
# TAU #
#-----#

config_tau_e = {
  'ISMUONLOOSE'   : False,
  'ABSID'         : 11,
  'min_PT'        : 4. * GeV,
  'min_TRPCHI2'   : 0.01,
  'min_ETA'       : 2,
  'max_ETA'       : 4.5,
  # 'max_BPVIP'     : 1. * mm,
  'min_PTFrac05C' : 0.7,
  # 'min_PTFrac05A' : -1,  # DON'T USE ME!
  #
  # 'min_ECALFrac'  : 0.1,  # Not good for e-conversion
  'max_HCALFrac'  : 0.10,
  'min_CaloPrsE'  : 50 * MeV,
  #
  'extracut'      : 'ALL'
}


config_tau_mu = {
  'ISMUON'        : True,
  'ABSID'         : 13,
  'min_PT'        : 4. * GeV,
  'min_TRPCHI2'   : 0.01,
  'min_ETA'       : 2.0,
  'max_ETA'       : 4.5,
  # 'max_BPVIP'     : 1. * mm, 
  'min_PTFrac05C' : 0.8,
  # 'max_ECone02A'  : 100 * GeV,
  # 'max_PTCone05C' : 4 * GeV,
  # 'max_HCALFrac'  : 0.20,  # Not effective
  'extracut'      : 'ALL',  
}


config_tau_h1 = {
  'ISMUONLOOSE'     : False,
  'ISPIONORKAON'    : True,
  'min_PT'          : 4. * GeV,
  'min_TRPCHI2'     : 0.01,
  'min_ETA'         : 2.25,
  'max_ETA'         : 3.75,
  # 'min_BPVDLS'      : -40,
  # 'max_BPVDLS'      : 40,
  # 'max_BPVIP'       : 1. * mm, 
  # 'max_ECone05C'    : 400 * GeV,
  # 'min_ECone02PN'   : 20 * GeV,
  # 'min_EFrac02PN05N': 0.7,
  # 'min_EFrac02PN05A': 0.7,
  'min_PTFrac05C'   : 0.8,
  'min_HCALFrac'    : 0.05,   
  # 'max_TRGHOSTPROB' : 0.1,
  'extracut'        : 'ALL'  
}



config_tau_h3 = {
  'dcuts': {
    'ISMUONLOOSE'     : False,
    'ISPIONORKAON'    : True,
    'min_PT'          : 500 * MeV,
    'min_TRPCHI2'     : 0.01,
    'min_ETA'         : 2.25, # 2.0,
    'max_ETA'         : 3.75, # 4.5,
    'min_HCALFrac'    : 0.05,
    # 'max_TRGHOSTPROB' : 0.1,
    'extracut'        : 'ALL',
  },
  'ccuts': {
    # 'max_ABPVIPMAX' : 2,  # suboptimal
    'min_AM'        : 500.  * MeV,
    'max_AM'        : 1600. * MeV,
  },
  'mcuts': {
    # 'min_ETA'       : 2.25,
    # 'max_ETA'       : 3.75,
    # 'min_MT'        : 4 * GeV,
    # 'max_BPVCORRM'  : 6 * GeV,
    # 'min_ABSBPVDIRA': 0.9999,  # suboptimal
    # 'min_BPVDLS'    : 0,  # suboptimal
    # 'max_BPVIP'     : 1. * mm,
    # 'min_BPVVDZ'    : 0.1,
    #
    # 'max_ECone05C'  : 30 * GeV,
    'min_PTFrac05C' : 0.8,
    #
    'max_DRTRIOMAX' : 0.4,
    'max_DRTRIOMID' : 0.3,
    'max_DRTRIOMIN' : 0.2,
    'min_PTTRIOMIN' :  500 * MeV,
    'min_PTTRIOMID' : 1000 * MeV,
    'min_PTTRIOMAX' : 2000 * MeV,
    #
    'max_VCHI2PDOF' : 20.,
  }
}

#-------#
# DITAU #
#-------#

# Blank default fallback. Remember the Zen: *Explicit is better than Implicit*
pcuts0 = {'extracut': 'ALL'} 
ccuts0 = {'extracut': 'ATRUE'}


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
    'min_APTMAX'          :   9 * GeV,
    'min_AM'              :  12 * GeV,
    # 'max_ADOCAMAX'        :   1 * mm,
    # 'max_AECone05CMAX'    : 200 * GeV,
    # 'max_APTCone05CMAX'   :  10 * GeV,
    # 'min_AEFrac02PN05AMAX': 0.6,
    # 'min_AEFrac05AMIN'    : 0.1,
    # 'min_AEFrac05CMIN'    : 0.4,
    # 'min_AEFrac05CMIN'    : 0.4,
    'extracut'            : 'ATRUE',  
  },
  'mcuts': pcuts0,
}

config_ditau_e_h3 = {
  'dcuts': { 'e' : pcuts0, 'tau': pcuts0 },
  'ccuts': {
    'min_APTMAX':  9 * GeV,
    'min_AM'    : 12 * GeV,
    # 'min_AM'    : 8 * GeV,
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
    'min_APTMAX': 9 * GeV,
    'min_AM'    : 12 * GeV,
  },
  'mcuts': pcuts0,
}

config_ditau_h1_mu = {
  'dcuts': { 'pi' : pcuts0, 'mu': pcuts0 },
  'ccuts': {
    # 'min_APTMAX':  9 * GeV,
    'min_AM'    : 8 * GeV,
  },
  'mcuts': pcuts0,
}

config_ditau_h3_h3 = {
  'dcuts': { 'tau' : pcuts0 },
  'ccuts': {
    # 'min_APTMAX':  9 * GeV,
    # 'min_AM'    : 12 * GeV,
    'min_AM'    :  8 * GeV,
  },
  'mcuts': pcuts0,
}

config_ditau_h3_mu = {
  'dcuts': { 'tau' : pcuts0, 'mu': pcuts0 },
  'ccuts': {
    # 'min_APTMAX' :  9 * GeV,
    # 'min_AM'     : 12 * GeV,
    'min_AM'     :  8 * GeV,
  },
  'mcuts': pcuts0,
}

config_ditau_mu_mu = {
  'dcuts': { 'mu': pcuts0 },
  'ccuts': {
    # 'min_APTMAX'  :  9 * GeV,
    'min_AM'      :  8 * GeV,
  },
  'mcuts': pcuts0,
}

## Specify how the decay is groupped into each line
lines_decays = {
  'EX': {
    'ee_os' : '  Z0 ->  e-    e+   ',
    'eh1_os': '[ Z0 ->  e-    pi+  ]cc',
    'eh3_os': '[ Z0 ->  e-    tau+ ]cc',
    'emu_os': '[ Z0 ->  e-    mu+  ]cc',
  },
  'EXss': {
    'ee_ss' : '[ Z0 ->  e-    e-   ]cc',
    'eh1_ss': '[ Z0 ->  e-    pi-  ]cc',
    'eh3_ss': '[ Z0 ->  e-    tau- ]cc',
    'emu_ss': '[ Z0 ->  e-    mu-  ]cc',  
  },
  'MX': {
    'emu_os'  : '[ Z0 ->  e-    mu+  ]cc',
    'h1mu_os' : '[ Z0 ->  pi-   mu+  ]cc',
    'h3mu_os' : '[ Z0 ->  tau-  mu+  ]cc',
    'mumu_os' : '  Z0 ->  mu-   mu+     ',
  },
  'MXss': {
    'emu_ss'  : '[ Z0 ->  e-    mu-  ]cc',
    'h1mu_ss' : '[ Z0 ->  pi-   mu-  ]cc',
    'h3mu_ss' : '[ Z0 ->  tau-  mu-  ]cc',
    'mumu_ss' : '[ Z0 ->  mu-   mu-  ]cc',
  },
  'HH': {
    'h1h1_os' : '  Z0 ->  pi-   pi+     ',
    'h1h3_os' : '[ Z0 ->  pi-   tau+ ]cc',
    'h3h3_os' : '  Z0 ->  tau-  tau+    ',
  },
  'HHss': {
    'h1h1_ss' : '[ Z0 ->  pi-   pi-  ]cc',
    'h1h3_ss' : '[ Z0 ->  pi-   tau- ]cc',
    'h3h3_ss' : '[ Z0 ->  tau-  tau- ]cc',
  },
}

default_config = {
  'NAME'        : 'Ditau',
  'BUILDERTYPE' : 'DitauConf',
  'STREAMS'     : [ 'EW'  ],
  'WGs'         : [ 'QEE' ],
  'CONFIG'      : {

    ## Particle identification, full path to ./Particles please
    # 'TES_e' : 'Phys/PFParticles/Particles',
    # 'TES_e' : 'Phys/StdAllLooseElectrons/Particles',
    'TES_e' : 'Phys/StdAllNoPIDsElectrons/Particles',
    'TES_mu': 'Phys/StdAllLooseMuons/Particles', 
    'TES_pi': 'Phys/StdAllNoPIDsPions/Particles',

    ## Individual tau
    'tau_e'   : config_tau_e,
    'tau_h1'  : config_tau_h1,
    'tau_h3'  : config_tau_h3,
    'tau_mu'  : config_tau_mu,

    ## Individual ditau
    'ditau_ee'   : config_ditau_e_e,
    'ditau_eh1'  : config_ditau_e_h1,
    'ditau_eh3'  : config_ditau_e_h3,
    'ditau_emu'  : config_ditau_e_mu,
    'ditau_h1h1' : config_ditau_h1_h1,
    'ditau_h1h3' : config_ditau_h1_h3,
    'ditau_h1mu' : config_ditau_h1_mu,
    'ditau_h3h3' : config_ditau_h3_h3,
    'ditau_h3mu' : config_ditau_h3_mu,
    'ditau_mumu' : config_ditau_mu_mu,

    ## Prescale/Postscale
    'EX_prescale'     : 1.,
    'EX_postscale'    : 1.,
    'EXss_prescale'   : 1.,
    'EXss_postscale'  : 1.,
    'MX_prescale'     : 1.,
    'MX_postscale'    : 1.,
    'MXss_prescale'   : 1.,
    'MXss_postscale'  : 1.,
    'HH_prescale'     : 1.,
    'HH_postscale'    : 1.,
    'HHss_prescale'   : 1.,
    'HHss_postscale'  : 0.5,

    ## For advance usage
    # Extra preambulo, give me barestring with proper linebreak.
    'preambulo'   : '\n', 
    # Choice of ParticleCombiner for ditau
    'ditau_pcomb' : {'': 'MomentumCombiner'},
    # 'ditau_pcomb' : {},
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
PTTRIOMIN, PTTRIOMID, PTTRIOMAX = _MakeTrio(CHILD(PT,1), CHILD(PT,2), CHILD(PT,3))

MT = ((MM**2)+(PT**2))**0.5

## Energy cone
ECone02A      = SUMCONE(0.2**2, E  , 'Phys/PFParticles')
ECone02C      = SUMCONE(0.2**2, E  , 'Phys/PFParticles', Q!=0) # Charged
ECone02N      = SUMCONE(0.2**2, E  , 'Phys/PFParticles', Q==0) # Neutral
ECone05A      = SUMCONE(0.5**2, E  , 'Phys/PFParticles')
ECone05C      = SUMCONE(0.5**2, E  , 'Phys/PFParticles', Q!=0)
ECone05N      = SUMCONE(0.5**2, E  , 'Phys/PFParticles', Q==0)
ECone02PN     = E + ECone02N  # self+neutral

EFrac05C      = E / ( E+ECone05C )
EFrac05A      = E / ( E+ECone05A )
EFrac02PN05N  = ECone02PN / ( E + ECone05N )  # (self+neutral) / (self+neutral)
EFrac02PN05A  = ECone02PN / ( E + ECone05A )  # (self+neutral) / (self+neutral+charged)

## PT-vectorial cone
PXCone05A  = SUMCONE(0.5**2, PX, 'Phys/PFParticles')
PYCone05A  = SUMCONE(0.5**2, PY, 'Phys/PFParticles')
PXCone05C  = SUMCONE(0.5**2, PX, 'Phys/PFParticles', Q!=0)
PYCone05C  = SUMCONE(0.5**2, PY, 'Phys/PFParticles', Q!=0)
PTCone05A   = ( PXCone05A**2 + PYCone05A**2 )**0.5
PTCone05C   = ( PXCone05C**2 + PYCone05C**2 )**0.5
PTFrac05C   = PT / ( PT + PTCone05C )  # core -- charged
PTFrac05A   = PT / ( PT + PTCone05A )  # core -- all

## Calo
HCALFrac = PPFUN(PP_CaloHcalE)/P
ECALFrac = PPFUN(PP_CaloEcalE)/P
CaloPrsE = PPFUN(PP_CaloPrsE)

## Combinations
ABPVCORRMMAX  = AMAXCHILD(BPVCORRM)
ABPVDIRAMAX   = AMAXCHILD(BPVDIRA)
APTMIN        = AMINCHILD(PT)
APTMAX        = AMAXCHILD(PT)

AECone02CMIN  = AMINCHILD(ECone02C)
AECone02CMAX  = AMAXCHILD(ECone02C)
AECone05CMIN  = AMINCHILD(ECone05C)
AECone05CMAX  = AMAXCHILD(ECone05C)
APTCone05CMIN = AMINCHILD(PTCone05C)
APTCone05CMAX = AMAXCHILD(PTCone05C)

AEFrac05CMIN      = AMINCHILD(EFrac05C)
AEFrac05CMAX      = AMAXCHILD(EFrac05C)
AEFrac05AMIN      = AMINCHILD(EFrac05A)
AEFrac05AMAX      = AMAXCHILD(EFrac05A)
AEFrac02PN05AMAX  = AMAXCHILD(EFrac02PN05A)
APTFrac05CMIN     = AMINCHILD(PTFrac05C)
APTFrac05CMAX     = AMAXCHILD(PTFrac05C)
APTFrac05AMIN     = AMINCHILD(PTFrac05A)
APTFrac05AMAX     = AMAXCHILD(PTFrac05A)

## instantiated
_VCHI2      = VCHI2
VCHI2       = VFASPF(_VCHI2)
_VCHI2PDOF  = VCHI2PDOF
VCHI2PDOF   = VFASPF(_VCHI2PDOF)
_ADOCAMAX   = ADOCAMAX
ADOCAMAX    = _ADOCAMAX('')
_BPVIP      = BPVIP
BPVIP       = _BPVIP()
_BPVIPCHI2  = BPVIPCHI2
BPVIPCHI2   = _BPVIPCHI2()
ABSBPVDIRA  = abs(BPVDIRA)

ISPIONORKAON = (ABSID==211) | (ABSID==321)

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

#------------------------------------------------------------------------------
# SELECTION
#------------------------------------------------------------------------------

def selection_tau_single( config , preambulo, inputs, ttype ):
  return SimpleSelection( 'FiltTauCand_'+ttype, FilterDesktop, inputs, 
    'LooseTauCand_'+ttype,
    Preambulo = preambulo0 + preambulo.split('\n'),
    Code      = parse_cuts_auto(config),
  )

def selection_tau_h3( config, preambulo, inputs ):
  dcut = parse_cuts_auto(config['dcuts'])
  return SimpleSelection( 'FiltTauCand_h3', CombineParticles, inputs,
    'CombTauCand_h3',
    DecayDescriptor   = '[ tau- -> pi- pi- pi+ ]cc',
    Preambulo         = preambulo0 + preambulo.split('\n'),
    DaughtersCuts     = {'pi-':dcut, 'pi+':dcut},
    CombinationCut    = parse_cuts_auto( config['ccuts'] ),
    MotherCut         = parse_cuts_auto( config['mcuts'] ),
  )

def selection_ditau( config, preambulo, pcomb, dcname, decay , inputs ):
  # dcuts quite complicate since it's a-priori nested dict index by particle-name.
  dcuts = { key:parse_cuts_auto(val) for key,val in doubling_sign_dict(config['dcuts']).iteritems() }
  return SimpleSelection( 'SelDitauCand_'+dcname, CombineParticles, inputs,
    'CombDitauCand_'+dcname,
    Preambulo         = preambulo0 + preambulo.split('\n'),
    DecayDescriptor   = decay,
    DaughtersCuts     = dcuts,
    CombinationCut    = parse_cuts_auto(config['ccuts']),
    MotherCut         = parse_cuts_auto(config['mcuts']),
    ParticleCombiners = pcomb,
    # CombinationCut  = 'AALLSAMEBPV',  # BUGGY, don't use me!
  )

#==============================================================================

## Nomenclatures on name, just to not get myself confused...
# NAME    : Formal name of stripping line, by default, 'Ditau'
# dtype   : Decay type of ditau (no sign), e.g., eh1, mumu
# dcname  : Decay name of ditau (with sign), e.g., eh1_ss, mumu_os
# linename: Short line name, e.g., EX, MXss 
#           (note that opposite-sign os is dropped)


class DitauConf(LineBuilder):

  __configuration_keys__ = default_config['CONFIG']  # Legacy field

  # Note: To be instantiated by LineBuilder( default_config['NAME'], default_config['CONFIG'] )
  def __init__(self, NAME, CONFIG):

    # Required the explicit name/dconfig, no default allow
    LineBuilder.__init__(self, NAME, CONFIG)
    preambulo   = CONFIG['preambulo']
    ditau_pcomb = CONFIG['ditau_pcomb']
    input_e     = [ AutomaticData(Location=CONFIG['TES_e'])  ]
    input_mu    = [ AutomaticData(Location=CONFIG['TES_mu']) ]
    input_pi    = [ AutomaticData(Location=CONFIG['TES_pi']) ]

    ## Make 4 selections of tau candidates first
    tau_cands = MergedSelection('SelTauCand', [
    # tau_cands = [
      selection_tau_single( CONFIG['tau_e']  , preambulo, input_e , 'e'  ),
      selection_tau_single( CONFIG['tau_mu'] , preambulo, input_mu, 'mu' ),
      selection_tau_single( CONFIG['tau_h1'] , preambulo, input_pi, 'h1' ),
      selection_tau_h3    ( CONFIG['tau_h3'] , preambulo, input_pi       ),
    ])  # Need merge so that the selection is parallel, not sequential

    sels_ditau = {}  # Selections, indexed by dcname (for emu sharing)

    ## Loop over all lines, make groupping
    for linename, dcdict in lines_decays.iteritems():

      ## Make list of selections in single line
      sels = []
      for dcname, decay in dcdict.iteritems():
        if dcname in sels_ditau:  # for emu case
          sel = sels_ditau[dcname]
        else:
          dtype   = dcname.split('_')[0]   # eh1_ss --> eh1
          config  = CONFIG['ditau_'+dtype]
          sel     = selection_ditau( config, preambulo, ditau_pcomb, dcname, decay, [tau_cands] )
          sels_ditau[dcname] = sel
        sels.append(sel)

      ## Finally, compose a stripping line, register
      self.registerLine(StrippingLine( NAME + linename + 'Line', # e.g., 'Ditau' + 'EXss' + 'Line'
        prescale    = CONFIG[linename+'_prescale'],
        postscale   = CONFIG[linename+'_postscale'],
        selection   = MergedSelection( 'Sel'+linename, sels ),
        checkPV     = True,
      ))


#-------------------------------------------------------------------

# def line_builder_test():
#   """Invoke the test script from StrippingUtils."""
#   from StrippingUtils import LineBuilderTests
#   LineBuilderTests.test_line_builder( DitauConf, default_config['CONFIG'] )

# if __name__ == '__main__':
#   line_builder_test()

# # Workaround the LineBuilderTest
# # Exception: test_configuration_not_dictlike_raises
# if not isinstance(subconfigs, dict):
#   raise AttributeError
# # Exception: test_bad_configuration_raises
# if 'BAD_KEY' in subconfigs:
#   raise KeyError

# 