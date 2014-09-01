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


Structure
---------

The selection is based on CombineParticles, requesting for LEPTON + TRACK combination.
There will be several isolation cuts on both species (often revolve around cone of R=0.5)
As well as cuts based on impact-parameter on best PV.

The lines aims to supersede `StrippingZ02TauTau.py` from S.Farry & P. Ilten


PDG 2012 Data
-------------

Tau decays with following branching ratio

- 49% : 1-Prong hadronic  ( h1 )
- 18% : Electron          ( e  )
- 17% : Muon              ( mu )
- 15% : 3-Prongs hadronic ( h3 ) 
- Others: K, 5-prongs, etc.

Ditau decays in major channels are thus (in alphebetical order):

-  3.2% : e_e      | EXLine
- 17.6% : e_h1     | EXLine
-  5.4% : e_h3     | Not yet implemented ( some can be found in EXLine )
-  6.1% : e_mu     | EXLine, MuXLine
- 24.0% : h1_h1    | Not yet implemented
- 14.7% : h1_h3    | Not yet implemented
- 16.7% : h1_mu    | MuXLine 
-  2.3% : h3_h3    | See C.Elsasser's `Z20TauTauProng.py`
-  5.1% : h3_mu    | Not yet implemented ( some can be found in MuXLine )
-  2.9% : mu_mu    | MuXLine 


Lines
-----

- Ditau_MuXLine   : tau+ -> mu+ pi-
- Ditau_MuXssLine : tau+ -> mu+ pi+
- Ditau_EXLine    : tau+ -> e+  pi-
- Ditau_EXssLine  : tau+ -> e+  pi+

The decays involving 3-prongs is to be implemented in the near future,
since it'll require noticably different strategy from the 1-prong ones.


Remark: The parameters has been optimized for 8 TeV MC of bbtautau & Ztautau
        At least, it shouldn't make it worse than Z02TauTauLines...


-----------------------------------------
Author: chitsanu.khurewathanakul@epfl.ch
Derived from works of S.Farry & P. Ilten
-----------------------------------------

"""

from GaudiKernel.SystemOfUnits import MeV, GeV, mm


__author__ = 'chitsanu.khurewathanakul@epfl.ch'
__date__   = '2014-08-15'
__all__    = [ 
               'lines_decays',
               'CUTS_GENERIC',
               'CUTS_MUON',
               'CUTS_ELECTRON',
               'CUTS_MUPI',
               'CUTS_EPI',
               'CUTS_COMBINATION',
               'CUTS_E_MOTHER',
               'CUTS_MU_MOTHER',
               'parse',
               'get_mu_cut',
               'get_electron_cut',
               'get_mupi_cut',
               'get_epi_cut',
               'get_combination_cut',
               'get_mu_mother_cut',
               'get_e_mother_cut',
               'get_ditau_selection',
               'DitauConf', 
               'default_config' ]


lines_decays   = {
  # Name  : ( Decay descriptor   , Config name ),
  'MuX'   : ( '[Z0 -> mu+ pi-]cc', 'CONFIG' ),
  'MuXss' : ( '[Z0 -> mu+ pi+]cc', 'CONFIG' ),
  'EX'    : ( '[Z0 -> e+  pi-]cc', 'CONFIG'  ),
  'EXss'  : ( '[Z0 -> e+  pi+]cc', 'CONFIG'  ),
}

default_config = {
  'NAME'        : 'Ditau',
  'WGs'         : [ 'QEE' ],
  'BUILDERTYPE' : 'DitauConf',
  'STREAMS'     : ['EW'],

  # Configuration for isolated muon + track (MuX)
  'CONFIG'  : {

    # Preambles
    'prescale'  : 1.0,
    'postscale' : 1.0,
    'HLTMuon'       : "HLT_PASS('Hlt2SingleMuonHighPTDecision')",  # PT > 10 GeV
    'HLTElectron'       : "HLT_PASS('Hlt2SingleTFElectronDecision') | HLT_PASS('Hlt2SingleTFVHighPtElectronDecision') ",  # PT10(tight) | PT15

    # Cuts on all single child ( mu / pi )
    'min_TRPCHI2'       : 0.01, # 0.01, 
    'max_BPVIP'         : 1 * mm, # 0.5 * mm,
    'max_Cone_N'        : 2, # 2,  # Number of charged track(s) in a 0.1-Cone

    # Muon only
    'mu_min_PT'         : 10 * GeV, # 5
    'mu_min_FracCone05E': 0.7,

    # Track only (StdAllNoPIDsPions)
    'mupi_min_PT'         : 3 * GeV, # 3
    'mupi_min_FracCone05E': 0.1,

    # Cut on ditau
    'mu_mother_min_MM'     : 11 * GeV, # M**2 >= 4*PT1*PT2
    
    # Electron only
    'e_min_PT'          : 10. * GeV,
    'e_min_FracCone05E' : 0.7,
    'e_max_TRCHI2DOF'   : 20,
    'e_min_CaloPrsE'    : 50. * MeV,
    'e_min_CaloEcalE'   : 0.1,   # Fraction of P
    'e_max_CaloHcalE'   : 0.05,  # Fraction of P
    
    # Track only (StdAllNoPIDsPions)
    'epi_min_PT'         : 2 * GeV,
    'epi_min_FracCone05E': 0.1,

    # Mother cuts
    'e_mother_min_MM'     : 8 * GeV, # M**2 >= 4*PT1*PT2
  } 

}


#==============================================================================


CUTS_GENERIC = [
  '{min_TRPCHI2}  <   TRPCHI2',
  '{max_BPVIP}    >   BPVIP()',
  '{max_Cone_N}   >=  SUMCONE( 0.1, ONE, "Phys/StdAllNoPIDsPions")',  # Allow maximum 3 more accompanied particles
]

CUTS_MUON = [
  '{mu_min_PT}          < PT',
  '{mu_min_FracCone05E} < E / ( E + SUMCONE( 0.5, E, "Phys/StdAllNoPIDsPions" ) ) ',
]

CUTS_ELECTRON = [
  '{e_min_PT}           < PT',
  '{e_min_FracCone05E}  < E / ( E + SUMCONE( 0.5, E, "Phys/StdAllNoPIDsPions" ) ) ',
  '{e_max_TRCHI2DOF}    > TRCHI2DOF',
  '{e_min_CaloPrsE}     < PPINFO(LHCb.ProtoParticle.CaloPrsE,0)',
  '{e_min_CaloEcalE}*P  < PPINFO(LHCb.ProtoParticle.CaloEcalE,0)',
  '{e_max_CaloHcalE}*P  > PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)',
]

CUTS_MUPI = [
  '{mupi_min_PT}          < PT',
  '{mupi_min_FracCone05E} < E / ( E + SUMCONE( 0.5, E, "Phys/StdAllNoPIDsPions" ) ) ',
]

CUTS_EPI = [
  '{epi_min_PT}          < PT',
  '{epi_min_FracCone05E} < E / ( E + SUMCONE( 0.5, E, "Phys/StdAllNoPIDsPions" ) ) ',
]

CUTS_COMBINATION = [
  'AALLSAMEBPV',
]

CUTS_MU_MOTHER = [
  '{mu_mother_min_MM} < MM',
]

CUTS_E_MOTHER = [
  '{e_mother_min_MM} < MM',
]


def parse(l, config):
  """Given the list of string+config, parse them into loki-ready string."""
  return ("("+") & (".join(l)+")").format(**config)

def get_mu_cut(config):
  return parse( CUTS_GENERIC + CUTS_MUON, config )

def get_electron_cut(config):
  return parse( CUTS_GENERIC + CUTS_ELECTRON, config )

def get_mupi_cut(config):
  return parse( CUTS_GENERIC + CUTS_MUPI, config )

def get_epi_cut(config):
  return parse( CUTS_GENERIC + CUTS_EPI, config )

def get_combination_cut(config):
  return parse( CUTS_COMBINATION, config )

def get_mu_mother_cut(config):
  return parse( CUTS_MU_MOTHER, config )

def get_e_mother_cut(config):
  return parse( CUTS_E_MOTHER, config )




#------------------------------------------------------------------------------
# SELECTION
#------------------------------------------------------------------------------

from Configurables import CombineParticles
from PhysSelPython.Wrappers import Selection
from StandardParticles import * # StdAllNoPIDsMuons, StdAllNoPIDsElectrons, StdAllNoPIDsPions

def get_ditau_selection(name, decay, config):

  mother_cuts      = dict()
  daughters_cuts   = dict()
  req_selections   = list()

  if ' mu' in decay:
    daughters_cuts['mu+'] = get_mu_cut( config )
    daughters_cuts['mu-'] = get_mu_cut( config )
    req_selections.append( StdAllLooseMuons ) # StdAllLooseMuons ) # StdAllNoPIDsMuons
    
  if ' e' in decay: 
    daughters_cuts['e+'] = get_electron_cut( config )
    daughters_cuts['e-'] = get_electron_cut( config )
    req_selections.append( StdAllNoPIDsElectrons ) # StdAllNoPIDsElectrons

  if ' pi' in decay:
   if ' mu' in decay:	   
     daughters_cuts['pi+'] = get_mupi_cut( config )
     daughters_cuts['pi-'] = get_mupi_cut( config )
     req_selections.append( StdAllNoPIDsPions ) # StdAllNoPIDsPions
   elif ' e' in decay:
     daughters_cuts['pi+'] = get_epi_cut( config ) 
     daughters_cuts['pi-'] = get_epi_cut( config )
     req_selections.append( StdAllNoPIDsPions ) # StdAllNoPIDsPions

  if ' mu' in decay:
   algo = CombineParticles(name+'_Combine',
                           DecayDescriptor     = decay,
                           DaughtersCuts       = daughters_cuts,
                           # DaughtersCuts       = {},
                           # MotherCut           = 'ALL',
                           MotherCut           = get_mu_mother_cut(config),
                           CombinationCut      = get_combination_cut(config),
                           ParticleCombiners   = {'':'MomentumCombiner'},
                           # WriteP2PVRelations  = False,
                          )
  elif ' mu' not in decay:
   algo = CombineParticles(name+'_Combine',
                           DecayDescriptor     = decay,
                           DaughtersCuts       = daughters_cuts,
                           # DaughtersCuts       = {},   
                           # MotherCut           = 'ALL',
                           MotherCut           = get_e_mother_cut(config),
                           CombinationCut      = get_combination_cut(config),
                           ParticleCombiners   = {'':'MomentumCombiner'},
                           # WriteP2PVRelations  = False,
                          )


  return Selection(name+'_Selection', Algorithm=algo, RequiredSelections=req_selections)



#==============================================================================

from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine

class DitauConf(LineBuilder):

  __configuration_keys__ = default_config.keys()

  # Note: To be instantiated by LineBuilder( default_config['NAME'], default_config )
  def __init__(self, name_strip, dconfig):

    # Required the explicit name/dconfig, no default allow
    LineBuilder.__init__(self, name_strip, dconfig)

    for name_line, ( decay, name_config) in lines_decays.iteritems():

      config    = dconfig[name_config]

      fullname  = name_strip + '_' + name_line  # e.g., Ditau_MuX

      selection = get_ditau_selection(fullname, decay, config)

      if 'mu' in decay:
       trigger=config['HLTMuon']
      elif 'e' in decay:
       trigger=config['HLTElectron']

      line = StrippingLine( fullname + 'Line',
        HLT               = trigger,
        prescale          = config['prescale'],
        postscale         = config['postscale'],
        checkPV           = True,
        selection         = selection,
        RequiredRawEvents = [ 'Calo' ],
        MDSTFlag          = False ,
      )

      self.registerLine( line )

#-------------------------------------------------------------------

#def line_builder_test():
#  """Invoke the test script from StrippingUtils."""
#  from StrippingUtils import LineBuilderTests
#  LineBuilderTests.test_line_builder( DitauConf, default_config )

#if __name__ == '__main__':
#  line_builder_test()

