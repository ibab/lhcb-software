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

## 2.0
- Optimized for Run-II Stripping 23.
- Extends the code into 3 lines: EX, MX, HH. This covers all 10 possibilities of 
  ditau's decay modes.
> Generate SS lines automatically
> Generate cuts automatically + extra preambulo

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
from PhysSelPython.Wrappers import Selection, SimpleSelection, MergedSelection
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine
from StandardParticles import PFParticles
from Configurables import CombineParticles, FilterDesktop


__author__  = 'chitsanu.khurewathanakul@cern.ch'
__date__    = '2015-05-15'
__version__ = 2.0
__all__     = ( 'DitauConf', 'default_config' )


#-----#
# TAU #
#-----#

config_tau_e = {
  'min_PT'        : 2. * GeV,
  'min_TRPCHI2'   : 0.01,
  'min_ETA'       : 2,
  'max_ETA'       : 4.5,
  'min_BPVDLS'    : -100,
  'max_BPVDLS'    : 100,
  'max_BPVIP'     : 1.*mm,
  'min_PTFrac05C' : 0.5,
  # 'min_PTFrac05A' : -1,  # DON'T USE ME!
  'max_ECone05C'  : 400 * GeV,
  'max_HCALFrac'  : 0.5,
  # 'min_ECALFrac'  : 0.2,  # Not good for e-conversion
  'extracut'      : 'ALL'
}

config_tau_h1 = {
  'min_PT'          : 2. * GeV,
  'min_TRPCHI2'     : 0.01,
  'min_ETA'         : 2.0,
  'max_ETA'         : 4.5,
  'min_BPVDLS'      : -40,
  'max_BPVDLS'      : 40,
  'max_BPVIP'       : 1.*mm, 
  'max_ECone05C'    : 400 * GeV,
  'min_ECone02PN'   : 20 * GeV,
  'min_EFrac02PN05N': 0.4,
  'min_EFrac02PN05A': 0.6,
  'min_PTFrac05C'   : -1,   # no good
  'min_PTFrac05A'   : -1,   # no good
  'min_HCALFrac'    : -1,   # no good, many pion doesn't have HcalE
  'max_TRGHOSTPROB' : 0.1,
  'extracut'        : 'ALL'  
}

config_tau_mu = {
  'min_PT'        : 2. * GeV,
  'min_TRPCHI2'   : 0.01,
  'min_ETA'       : 2.0,
  'max_ETA'       : 4.5,
  'min_BPVDLS'    : -50,
  'max_BPVDLS'    : 50,
  'max_BPVIP'     : 0.3 * mm, 
  'min_PTFrac05C' : 0.6,
  'max_ECone02A'  : 100 * GeV,
  'max_PTCone05C' : 4 * GeV,
  # 'max_HCALFrac'  : 0.20,  # Not effective
  'extracut'      : 'ALL',  
}

config_tau_h3 = {
  'dcuts': {
    'min_PT'      : 500 * MeV,
    'min_TRPCHI2' : 0.01,
    # 'min_HCALFrac': 0.05,  # HCal not well defined
  },
  'ccuts': {
    # 'max_ABPVIPMAX' : 2,  # suboptimal
    'min_AM'        : 500.  * MeV,
    'max_AM'        : 1600. * MeV,
  },
  'mcuts': {
    'min_ETA'       : 2.0,
    'max_ETA'       : 4.5,
    'min_MT'        : 4 * GeV,
    'max_BPVCORRM'  : 6 * GeV,
    # 'min_ABSBPVDIRA': 0.9999,  # suboptimal
    # 'min_BPVDLS'    : 0,  # suboptimal
    'max_BPVIP'     : 0.5,
    'min_BPVVDZ'    : 0.1,
    #
    # 'max_ECone05C'  : 200 * GeV,  # suboptimal
    # 'max_ECone05A'  : 400 * GeV,  # suboptimal
    # 'max_PTCone05C' : 5 * GeV,  # suboptimal
    'min_PTFrac05C' : 0.7,
    'min_EFrac05C'  : 0.5,
    #
    'max_DRTRIOMAX' : 0.5,
    'max_DRTRIOMID' : 0.4,
    'max_DRTRIOMIN' : 0.3,
    'min_PTTRIOMIN' : 500 * MeV,
    'min_PTTRIOMID' : 1000 * MeV,
    'min_PTTRIOMAX' : 2000 * MeV,
    #
    'max_VCHI2'     : 50.,
  }
}

#-------#
# DITAU #
#-------#

config_ditau_e_e = {
  'ccuts': {
    'extracut': 'ATRUE',  
  },
  'mcuts': {
    'extracut': 'ALL',  
  }  
}

config_ditau_e_h1 = {
  'ccuts': {
    'min_AM'              : 2. * GeV,
    'max_ADOCAMAX'        : 1  * mm,
    'max_AECone05CMAX'    : 200 * GeV,
    'max_APTCone05CMAX'   : 10 * GeV,
    'min_AEFrac02PN05AMAX': 0.6,
    'min_AEFrac05AMIN'    : 0.1,
    'min_AEFrac05CMIN'    : 0.4,
    'min_AEFrac05CMIN'    : 0.4,
    'extracut'            : 'ATRUE',  
  },
  'mcuts': {
    'min_MT'  : 6 * GeV,
    'extracut': 'ALL',  
  }
}

config_ditau_e_h3 = {
  'ccuts': {
    'min_AM'            : 4. * GeV,
    'max_ADOCAMAX'      : 1  * mm,
    'min_APTMAX'        : 5 * GeV,
    'max_ABPVCORRMMAX'  : 4 * GeV,
    'max_AECone05CMIN'  : 40000,
    'max_AECone05CMAX'  : 100000,
    'max_APTCone05CMAX' : 5000,
    'min_AEFrac05AMAX'  : 0.5,
    'min_AEFrac05CMAX'  : 0.7,
    'extracut'          : 'ATRUE',  
  },
  'mcuts': {
    'min_MT'  : 8 * GeV,
    'extracut': 'ALL',  
  }
}

config_ditau_e_mu = {
  'ccuts': {
    'extracut'          : 'ATRUE',  
  },
  'mcuts': {
    'extracut': 'ALL',  
  }  
}

config_ditau_h1_h1 = {
  'ccuts': {
    'min_AM'              : 3 * GeV,
    'max_ADOCAMAX'        : 1 * mm,
    'min_APTMAX'          : 3 * GeV,
    #
    'max_AECone02CMIN'    : 60  * GeV, # i.e., don't expected any other charged particle
    'max_AECone02CMAX'    : 200 * GeV, 
    'max_AECone05CMIN'    : 100 * GeV,
    'max_AECone05CMAX'    : 300 * GeV,
    'max_APTCone05CMIN'   : 4   * GeV,
    'max_APTCone05CMAX'   : 20  * GeV,
    #
    'min_AEFrac05CMIN'   : 0.4,
    'min_AEFrac05CMAX'   : 0.7,
    'min_APTFrac05CMIN'  : 0.4,
    'min_APTFrac05CMAX'  : 0.7,
    #
    # 'min_AEFrac02PN05AMAX'  : 0.7,  # suboptimal
    'extracut'              : 'ATRUE',
  },
  'mcuts': {
    'min_MT'    : 6 * GeV,
    'extracut'  : 'ALL',  
  }
}

config_ditau_h1_h3 = {
  'ccuts': {
    'min_AM'            : 6. * GeV,
    'max_ADOCAMAX'      : 1  * mm,
    'max_ABPVCORRMMAX'  : 5 * GeV,
    #
    'max_AECone05CMAX'  : 300* GeV,
    'max_APTCone05CMAX' : 20 * GeV,
    #
    'min_AEFrac05AMAX'  : 0.4,
    'min_AEFrac05CMIN'  : 0.6,
    'min_AEFrac05CMAX'  : 0.8,
    'min_APTFrac05CMIN' : 0.6,
    'min_APTFrac05CMAX' : 0.8,
    'extracut'          : 'ATRUE',
  },
  'mcuts': {
    'min_MT'    : 10 * GeV,
    'extracut'  : 'ALL',  
  }
}

config_ditau_h1_mu = {
  'ccuts': {
    'min_AM'            : 3. * GeV,
    'max_ADOCAMAX'      : 1 * mm,
    #
    'max_AECone05CMAX'  : 400* GeV,
    'max_APTCone05CMAX' : 20 * GeV,
    #
    'min_AEFrac05CMIN'  : 0.4,
    'min_APTFrac05CMIN' : 0.6,
    'extracut'          : 'ATRUE',
  },
  'mcuts': {
    'min_MT'    : 4 * GeV,
    'extracut'  : 'ALL',  
  }
}

config_ditau_h3_h3 = {
  'ccuts': {
    'min_AM'            : 10. * GeV,
    'max_ADOCAMAX'      : 0.5  * mm,
    'max_ABPVCORRMMAX'  : 5 * GeV,
    'min_ABPVDIRAMAX'   : 0.9999,
    'min_APTFrac05CMAX' : 0.8,
    'min_AEFrac05AMIN'  : 0.3,
    'min_AEFrac05AMAX'  : 0.6,
    'min_AEFrac05CMIN'  : 0.6,
    'min_AEFrac05CMAX'  : 0.8,
    'extracut'          : 'ATRUE',
  },
  'mcuts': {
    'extracut': 'ALL',  
  }
}

config_ditau_h3_mu = {
  'ccuts': {
    'min_AM'            : 5. * GeV,
    'max_ADOCAMAX'      : 1  * mm,
    'max_ABPVCORRMMAX'  : 5 * GeV,
    'min_APTFrac05CMAX' : 0.8,
    'min_AEFrac05AMIN'  : 0.3,
    'min_AEFrac05AMAX'  : 0.5,
    'min_AEFrac05CMAX'  : 0.6,
    'extracut'          : 'ATRUE',  
  },
  'mcuts': {
    'min_MT'    : 8 * GeV,
    'extracut'  : 'ALL',  
  }
}

config_ditau_mu_mu = {
  'ccuts': {
    'min_AM'            : 4. * GeV,
    'max_ADOCAMAX'      : 1  * mm,    
    'min_APTFrac05CMAX' : 0.8,
    'min_AEFrac05AMAX'  : 0.5,
    'extracut'          : 'ATRUE',  
  },
  'mcuts': {
    'min_MT'    : 6 * GeV,
    'extracut'  : 'ALL',  
  }  
}

lines_decays = {
  'ee'   : '[ Z0 ->  e-    e+   ]cc',
  'eh1'  : '[ Z0 ->  e-    pi+  ]cc',
  'eh3'  : '[ Z0 ->  e-    tau+ ]cc',
  'emu'  : '[ Z0 ->  e-    mu+  ]cc',
  'h1h1' : '[ Z0 ->  pi-   pi+  ]cc',
  'h1h3' : '[ Z0 ->  pi-   tau+ ]cc',
  'h1mu' : '[ Z0 ->  pi-   mu+  ]cc',
  'h3h3' : '[ Z0 ->  tau-  tau+ ]cc',
  'h3mu' : '[ Z0 ->  tau-  mu+  ]cc',
  'mumu' : '[ Z0 ->  mu-   mu+  ]cc',
}

lines_merge = {
  'EX'  : ( 'ee'    , 'eh1'   , 'eh3'   , 'emu' ),
  'MX'  : ( 'emu'   , 'h1mu'  , 'h3mu'  , 'mumu'),
  'HH'  : ( 'h1h1'  , 'h1h3'  , 'h3h3'  ),
}

default_config = {
  'NAME'        : 'Ditau',
  'BUILDERTYPE' : 'DitauConf',
  'STREAMS'     : [ 'EW'  ],
  'WGs'         : [ 'QEE' ],
  'CONFIG'      : {
    # All subconfig have to be nested inside 'CONFIG'

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

    # ## Ditau ~ Lines
    'EX_prescale'  : 1.,
    'EX_postscale' : 1.,
    'MX_prescale'  : 1.,
    'MX_postscale' : 1.,
    'HH_prescale'  : 1.,
    'HH_postscale' : 1.,
    # 'preambulo': '',
  }
}



#==============================================================================

preambulo = """

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
PX2Cone05A  = SUMCONE(0.5**2, PX**2 , 'Phys/PFParticles')
PY2Cone05A  = SUMCONE(0.5**2, PY**2 , 'Phys/PFParticles')
PX2Cone05C  = SUMCONE(0.5**2, PX**2 , 'Phys/PFParticles', Q!=0)
PY2Cone05C  = SUMCONE(0.5**2, PY**2 , 'Phys/PFParticles', Q!=0)
PTCone05A   = ( PX2Cone05A + PY2Cone05A )**0.5
PTCone05C   = ( PX2Cone05C + PY2Cone05C )**0.5
PTFrac05C   = PT / ( PT + PTCone05C )  # core -- charged
PTFrac05A   = PT / ( PT + PTCone05A )  # core -- all

## Calo
HCALFrac = PPFUN(PP_CaloHcalE)/P
ECALFrac = PPFUN(PP_CaloEcalE)/P

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
_ADOCAMAX   = ADOCAMAX
ADOCAMAX    = _ADOCAMAX('')
_BPVIP      = BPVIP
BPVIP       = _BPVIP()
_BPVIPCHI2  = BPVIPCHI2
BPVIPCHI2   = _BPVIPCHI2()
ABSBPVDIRA  = abs(BPVDIRA)

""".split('\n')

def flip_samesign(s):
  """
  Given a decay descriptor in format X -> Y- Z+, flip sign to yield same-sign.
  Note: No regex check. I'll be nice to have for safety.
  """
  return s.replace('+', '-')

def gen_single_cut(key):
  ## Auto-generate the cut string from dict key. Lite for on-demand cut.
  ## 'max_PTCone05A' --> 'PTCone05A < {max_PTCone05A}'
  key = key.strip()
  if key.startswith('max_'):
    var = key.replace('max_', '')
    return '%s < {%s}' % (var,key)
  elif key.startswith('min_'):
    var = key.replace('min_', '')
    return '%s > {%s}' % (var,key)
  elif key == 'extracut':
    return '{extracut}'
  else:
    return None  # Fail to parse.

def parse_cuts_manual(config, *cuts):
  """Given the list of string & config, parse them into loki-ready string."""
  return ("("+") & (".join(cuts)+")").format(**config)

def parse_cuts_auto(config, *cuts):
  """
  Like above, but the loki cuts is generated on-the-fly for each key in 
  conf which supported my convention (see gen_single_cut).
  Generate only if the key doesn't already existed in the explicit cuts.
  """
  cuts = list(cuts) + [ gen_single_cut(key) for key in config if not any('{'+key+'}' in cut for cut in cuts) ]
  cuts = [ s for s in cuts if s ]  # Clean after
  return parse_cuts_manual(config, *cuts)

def get_tau_h3_dcut(config):
  dcut = parse_cuts_auto( config['dcuts'],
    '(ABSID==211) | (ABSID==321)'
  ) 
  return { 'pi+':dcut, 'pi-':dcut }


#------------------------------------------------------------------------------
# SELECTION
#------------------------------------------------------------------------------

tau_preselections = {
  'h1'  : lambda conf: parse_cuts_auto( conf, '(ABSID==211) | (ABSID==321)' ),
  'e'   : lambda conf: parse_cuts_auto( conf, 'ABSID==11' ),
  'mu'  : lambda conf: parse_cuts_auto( conf, 'ABSID==13' ),
}

def selection_tau_PF( config , ttype ):
  return SimpleSelection( 'FiltTauCand_'+ttype, FilterDesktop, [PFParticles], 
    'LooseTauCand_'+ttype,
    Preambulo = preambulo,
    Code      = tau_preselections[ttype](config),
  )

def selection_tau_h3( config ):
  return SimpleSelection( 'FiltTauCand_h3', CombineParticles, [PFParticles],
    'CombTauCand_h3',
    DecayDescriptor   = '[ tau- -> pi- pi- pi+ ]cc',
    Preambulo         = preambulo,
    DaughtersCuts     = get_tau_h3_dcut( config ),
    CombinationCut    = parse_cuts_auto(config['ccuts'], 'ATRUE'),
    MotherCut         = parse_cuts_auto(config['mcuts'], 'ALL'),
  )

def selection_ditau( config , dtype , decay , inputs ):
  return SimpleSelection( 'SelDitauCand_'+dtype, CombineParticles, inputs,
    'CombDitauCand_'+dtype,
    Preambulo         = preambulo,
    DecayDescriptor   = decay,
    CombinationCut    = parse_cuts_auto(config['ccuts'], 'ATRUE'),
    MotherCut         = parse_cuts_auto(config['mcuts'], 'ALL'),
    # ParticleCombiners = {'':'MomentumCombiner'},
    # CombinationCut  = 'AALLSAMEBPV',  # Too strong even for signal?
  )

#==============================================================================

class DitauConf(LineBuilder):

  __configuration_keys__ = default_config['CONFIG']  # Legacy field

  # Note: To be instantiated by LineBuilder( default_config['NAME'], default_config['CONFIG'] )
  def __init__(self, stripname, subconfigs):

    # Required the explicit name/dconfig, no default allow
    LineBuilder.__init__(self, stripname, subconfigs)

    ## Make 4 selections of tau candidates first
    self.tau_cands = MergedSelection('SelTauCand', [
      selection_tau_PF( subconfigs['tau_e']  , 'e' ),
      selection_tau_PF( subconfigs['tau_h1'] , 'h1' ),
      selection_tau_PF( subconfigs['tau_mu'] , 'mu' ),
      selection_tau_h3( subconfigs['tau_h3'] ),
    ])

    ## Make 10 selections of ditau cands
    self.ditau_cands = {}
    for dtype, decay in lines_decays.iteritems():
      config  = subconfigs['ditau_'+dtype]
      sel     = selection_ditau( config, dtype     , decay               , [self.tau_cands])
      sel_ss  = selection_ditau( config, dtype+'ss', flip_samesign(decay), [self.tau_cands])
      self.ditau_cands[dtype]      = sel
      self.ditau_cands[dtype+'ss'] = sel_ss

    ## Finally group the sels into single line
    for linename, dtypes in lines_merge.iteritems():
      for signmode in ('', 'ss'):
        whitelist   = [ dtype+signmode for dtype in dtypes ]
        sels_ditau  = [ sel for dtype,sel in self.ditau_cands.iteritems() if dtype in whitelist ]

        ## Make a line from merged sel, then register
        sname = stripname + linename + signmode  # e.g., DitauMXss
        self.registerLine(StrippingLine( sname + 'Line',
          prescale    = subconfigs[linename+'_prescale'],
          postscale   = subconfigs[linename+'_postscale'],
          selection   = MergedSelection( sname, sels_ditau ),
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