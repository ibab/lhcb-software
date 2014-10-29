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
__date__   = '2014-09-01'
__all__    = [ 'DitauConf', 'default_config' ]


config_MuX = {

  # Preambles
  'prescale'  : 1.0,
  'postscale' : 1.0,
  'HLT'       : "HLT_PASS('Hlt2SingleMuonHighPTDecision')",  # PT > 10 GeV
  
  # Cuts on all single child ( mu / pi )
  'min_TRPCHI2'       : 0.01, # 0.01, 
  'max_BPVIP'         : 1 * mm, # 0.5 * mm,
  'max_Cone_N'        : 2, # 2,  # Number of charged track(s) in a 0.1-Cone

  # Muon only
  'mu_min_PT'         : 10 * GeV, # 5
  'mu_min_FracCone05E': 0.7,

  # Track only (StdAllNoPIDsPions)
  'pi_min_PT'         : 3 * GeV, # 3
  'pi_min_FracCone05E': 0.1,

  # Cut on ditau
  'mother_min_MM'     : 11 * GeV, # M**2 >= 4*PT1*PT2
}


config_EX = {
      
  # Preambles
  'prescale'  : 1.0,
  'postscale' : 1.0,
  'HLT'       : "HLT_PASS('Hlt2SingleTFElectronDecision') | HLT_PASS('Hlt2SingleTFVHighPtElectronDecision') ",  # PT10(tight) | PT15

  # Cuts on all single child ( e / pi )
  'min_TRPCHI2'     : 0.01,
  'max_BPVIP'       : 1 * mm,
  'max_Cone_N'      : 2,
  
  # Electron only
  'e_min_PT'          : 10. * GeV,
  'e_min_FracCone05E' : 0.7,
  'e_max_TRCHI2DOF'   : 20,
  'e_min_CaloPrsE'    : 50. * MeV,
  'e_min_CaloEcalE'   : 0.1,   # Fraction of P
  'e_max_CaloHcalE'   : 0.05,  # Fraction of P
  
  # Track only (StdAllNoPIDsPions)
  'pi_min_PT'         : 2 * GeV,
  'pi_min_FracCone05E': 0.1,

  # Mother cuts
  'mother_min_MM'     : 8 * GeV, # M**2 >= 4*PT1*PT2
}


lines_decays   = {
  # Name  : ( Decay descriptor   , Config name ),
  'MuX'   : ( '[Z0 -> mu+ pi-]cc', 'CONFIG_MuX' ),
  'MuXss' : ( '[Z0 -> mu+ pi+]cc', 'CONFIG_MuX' ),
  'EX'    : ( '[Z0 -> e+  pi-]cc', 'CONFIG_EX'  ),
  'EXss'  : ( '[Z0 -> e+  pi+]cc', 'CONFIG_EX'  ),
}

default_config = {
  'NAME'        : 'Ditau',
  'WGs'         : [ 'QEE' ],
  'BUILDERTYPE' : 'DitauConf',
  'STREAMS'     : { 'EW' : [ 'StrippingDitau_{}Line'.format(s) for s in lines_decays.keys() ] },
  'CONFIG'      : {
    # All subconfig have to be nested inside 'CONFIG'

    # Configuration for isolated muon + track (MuX)
    'CONFIG_MuX': config_MuX,

    # Configuration for isolated electron + track (EX)
    'CONFIG_EX' : config_EX,

    # Configuration for isolated muon + 3-prongs (Mu3P)
    #'CONFIG_Mu3P' : None,

    # Configuration for 3-prongs + 3-prongs (3P3P)
    #'CONFIG_3P3P' : None,
    
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

CUTS_PI = [
  '{pi_min_PT}          < PT',
  '{pi_min_FracCone05E} < E / ( E + SUMCONE( 0.5, E, "Phys/StdAllNoPIDsPions" ) ) ',
]

CUTS_COMBINATION = [
  'AALLSAMEBPV',
]

CUTS_MOTHER = [
  '{mother_min_MM} < MM',
]


def parse(l, config):
  """Given the list of string+config, parse them into loki-ready string."""
  return ("("+") & (".join(l)+")").format(**config)

def get_mu_cut(config):
  return parse( CUTS_GENERIC + CUTS_MUON, config )

def get_electron_cut(config):
  return parse( CUTS_GENERIC + CUTS_ELECTRON, config )

def get_pi_cut(config):
  return parse( CUTS_GENERIC + CUTS_PI, config )

def get_combination_cut(config):
  return parse( CUTS_COMBINATION, config )

def get_mother_cut(config):
  return parse( CUTS_MOTHER, config )



#------------------------------------------------------------------------------
# SELECTION
#------------------------------------------------------------------------------

from Configurables import CombineParticles
from PhysSelPython.Wrappers import Selection
from StandardParticles import * # StdAllNoPIDsMuons, StdAllNoPIDsElectrons, StdAllNoPIDsPions

def get_ditau_selection(name, decay, config):

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
    daughters_cuts['pi+'] = get_pi_cut( config )
    daughters_cuts['pi-'] = get_pi_cut( config )
    req_selections.append( StdAllNoPIDsPions ) # StdAllNoPIDsPions


  algo = CombineParticles(name+'_Combine',
    DecayDescriptor     = decay,
    DaughtersCuts       = daughters_cuts,
    # DaughtersCuts       = {},
    # MotherCut           = 'ALL',
    MotherCut           = get_mother_cut(config),
    CombinationCut      = get_combination_cut(config),
    ParticleCombiners   = {'':'MomentumCombiner'},
    # WriteP2PVRelations  = False,
  )

  return Selection(name+'_Selection', Algorithm=algo, RequiredSelections=req_selections)



#==============================================================================

def merged_config_dict():
  """Debug: Get through the new LineBuiler syntax."""
  result = {}
  for config in default_config['CONFIG'].values():
    result.update( config )
  return result




from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine

class DitauConf(LineBuilder):

  __merged_config = merged_config_dict() # To be used in a traditional way

  # __configuration_keys__ = default_config.keys()  # Seems strange, but work with (perhaps outdated) LineBuilderTests
  # __configuration_keys__ = list(set([ key for elem in default_config.values() if isinstance(elem,dict) for key in elem.keys() ]))
  __configuration_keys__ = __merged_config.keys()

  # Note: To be instantiated by LineBuilder( default_config['NAME'], default_config['CONFIG'] )
  def __init__(self, name_strip, list_subconfig):

    # Required the explicit name/dconfig, no default allow
    LineBuilder.__init__(self, name_strip, self.__merged_config)

    for name_line, ( decay, name_config ) in lines_decays.iteritems():

      # Exception: test_configuration_not_dictlike_raises
      if not isinstance(list_subconfig, dict):
        raise AttributeError
      # Exception: test_bad_configuration_raises
      if 'BAD_KEY' in list_subconfig:
        raise KeyError

      config    = list_subconfig[name_config] 

      fullname  = name_strip + '_' + name_line  # e.g., Ditau_MuX

      selection = get_ditau_selection(fullname, decay, config)

      line = StrippingLine( fullname + 'Line',
        HLT               = config['HLT'],
        prescale          = config['prescale'],
        postscale         = config['postscale'],
        checkPV           = True,
        selection         = selection,
        # RequiredRawEvents = [ 'Calo', 'Rich', 'Muon' ],
        # MDSTFlag          = False ,
      )

      self.registerLine( line )

#-------------------------------------------------------------------

def line_builder_test():
  """Invoke the test script from StrippingUtils."""
  from StrippingUtils import LineBuilderTests
  LineBuilderTests.test_line_builder( DitauConf, default_config['CONFIG'] )

if __name__ == '__main__':
  line_builder_test()
  # print 123
  # print DitauConf.__configuration_keys__


