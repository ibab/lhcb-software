"""

Stripping Lines for W ( ->e,mu ) + Jet Jet

Based on original works of X. Cid Vidal
WmuAKTJets + WeAKTJets

For each lepton type (e,mu) there are 2 lines made:
- Lepton-triggered line
- Jet-topo-triggered line
They are then cover a phasespace complementarily on each other.

"""

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from Configurables       import CombineParticles, LoKi__VertexFitter
from PhysSelPython.Wrappers import Selection, SimpleSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdAllLooseMuons, StdAllNoPIDsElectrons, StdJets
from CommonParticles.Utils import *
from GaudiKernel.SystemOfUnits import GeV, MeV


__author__  = "Xabier Cid Vidal"
__all__     = "default_config", "WJetsConf"

## if you want to prescale this line, please contact X. Cid Vidal before!

default_config = {

  ## Configs for Wmu+Jets lines
  'WmuJets': {
    'BUILDERTYPE' : 'WJetsConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'BhadronCompleteEvent' ],
    'CONFIG': {
      'WJets_Prescale'        : 1.0,
      'WJetsTagged_Prescale'  : 1.0,
      'RequiredRawEvents'     : ['Calo'],
      #
      'config_W': {
        'TOS'         : 'Hlt2EWSingleMuonVHighPtDecision', # 12.5 GeV
        'min_mu_pT'   : 10. * GeV,
        'max_mu_pT'   : 200000. * GeV,
      },
      'min_jet_pT'    : 15. * GeV,
      'dr_lepton_jet' : 0.5,
      'TOS_Jet'       : 'Hlt2Topo.*Decision',
    }
  },

  ## Configs for We+Jets lines
  'WeJets': {
    'BUILDERTYPE' : 'WJetsConf',
    'WGs'         : [ 'QEE' ],
    'STREAMS'     : [ 'BhadronCompleteEvent' ],
    'CONFIG': {
      'WJets_Prescale'        : 1.0,
      'WJetsTagged_Prescale'  : 1.0,
      'RequiredRawEvents'     : ['Calo'],
      #
      'config_W': {
        'TOS'       : 'Hlt2EWSingleElectronVHighPtDecision', # 15 GeV
        'min_e_pT'  : 10. * GeV,
        'max_e_pT'  : 200000. * GeV,
        'PrsCalMin' : 50. * MeV,
        'ECalMin'   : 0.10,
        'HCalMax'   : 0.05,      
      },
      'min_jet_pT'    : 15. * GeV,
      'dr_lepton_jet' : 0.5,
      'TOS_Jet'       : 'Hlt2Topo.*Decision',
    }

  },
}

preambulo = """
## For DR calculation.
from GaudiKernel.SystemOfUnits import degree

eta_1 = ACHILDFUN(1,ETA)
eta_2 = ACHILDFUN(2,ETA)
eta_3 = ACHILDFUN(3,ETA)

phi_1 = ACHILDFUN(1,PHI)
phi_2 = ACHILDFUN(2,PHI)
phi_3 = ACHILDFUN(3,PHI)

deta_13 = eta_1 - eta_3
deta_23 = eta_2 - eta_3

dphi_13 = phi_1 - phi_3 
dphi_23 = phi_2 - phi_3

dphi_13 = switch ( dphi_13 >  180 * degree ,  dphi_13 - 180 * degree , dphi_13 )
dphi_13 = switch ( dphi_13 < -180 * degree ,  dphi_13 + 180 * degree , dphi_13 )
dphi_23 = switch ( dphi_23 >  180 * degree ,  dphi_23 - 180 * degree , dphi_23 )
dphi_23 = switch ( dphi_23 < -180 * degree ,  dphi_23 + 180 * degree , dphi_23 )

dr_13 = sqrt(deta_13**2 + dphi_13**2)
dr_23 = sqrt(deta_23**2 + dphi_23**2)

""".split('\n')

#===============================================================================

def SelWmu( name, conf, TOS_HLT2=None ):
  """ 
  Define the W->mu selection 
  """
  ## Apply cuts, with TOS optionally
  code = '(PT>%(min_mu_pT)s) & (PT<%(max_mu_pT)s)'%conf
  if TOS_HLT2:
    code += "& (TOS('%s','Hlt2TriggerTisTos'))"%TOS_HLT2
  return SimpleSelection( name, FilterDesktop, [StdAllLooseMuons], Code=code)


def SelWe( name, conf, TOS_HLT2=None ):
  """ 
  Define the W->e selection 
  """
  ## Quality cut.
  code = '(PT>%(min_e_pT)s) & (PT<%(max_e_pT)s)'\
          '& (PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s) '\
          '& (PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s) '\
          '& (PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s)'\
          "& (HASTRACK & TRCUT(0<TrIDC('isTT')))"%conf
  ## Apply cuts, with TOS optionally
  if TOS_HLT2:
    code += "& (TOS('%s','Hlt2TriggerTisTos'))"%TOS_HLT2
  preambulo = ["from LoKiTracks.decorators import *"]
  return SimpleSelection( name, FilterDesktop, [StdAllNoPIDsElectrons], 
    Code=code, Preambulo=preambulo)


def SelWDiJets( name, conf, desc, sel_W, TOS_HLT2=None ):
  """
  Create the combination of W + DiJets
  """
  ## Create CombineParticle, with caveat on jet combining.
  DiJet = CombineParticles("Combine" + name)
  DiJet.ParticleCombiners = {"" : "LoKi::VertexFitter"}
  DiJet.addTool( LoKi__VertexFitter, name="LoKi::VertexFitter" )
  vfitter = getattr( DiJet , "LoKi::VertexFitter" )
  vfitter.Jets = ""

  ## Apply cuts, with TOS optionally
  #  Asking AT LEAST one of the jet to be TOSed by given trigger.
  ccut = "AALLSAMEBPV "\
         "& ( dr_13 > %(dr_lepton_jet)s )"\
         "& ( dr_23 > %(dr_lepton_jet)s )" %conf
  if TOS_HLT2:
    cut_tos     = "(TOS('%s','Hlt2TriggerTisTos'))"%TOS_HLT2
    cut_tosjet  = '(ACHILDCUT({0}, 0) | ACHILDCUT({0}, 1))'.format(cut_tos)
    ccut       += ('&' + cut_tosjet)

  DiJet.Preambulo       = preambulo
  DiJet.DecayDescriptor = desc
  DiJet.DaughtersCuts   = { "CELLjet": "(PT > %(min_jet_pT)s)"%conf }
  DiJet.CombinationCut  = ccut
  DiJet.MotherCut       = "ALL"

  return Selection( name, Algorithm=DiJet, RequiredSelections=[ sel_W, StdJets ])


#===============================================================================

class WJetsConf( LineBuilder ) :

  __configuration_keys__ = (
    'WJets_Prescale',
    'WJetsTagged_Prescale',
    'RequiredRawEvents',
    'config_W',
    'min_jet_pT',
    'dr_lepton_jet',
    'TOS_Jet',
  )

  def __init__( self, name, config ) :
    LineBuilder.__init__( self, name, config )

    ## Switch the W selection, depends on the name given.
    if name == 'WmuJets':
      SelW = SelWmu
      desc = "[H+ -> CELLjet CELLjet mu+]cc" 
    elif name == 'WeJets':
      SelW = SelWe 
      desc = "[H+ -> CELLjet CELLjet e+]cc"
    else:
      raise ValueError('Unknown W to make: %s'%name)

    ## Make W, 2 versions: TOS, no TOS
    conf    = config['config_W']
    W_tos   = SelW( name+'_W_tos'  , conf, conf['TOS'] )
    W_notos = SelW( name+'_W_notos', conf )

    ## Make w+DiJets, also 2 versions, opposite of the TOS used above
    WJets_toslepton = SelWDiJets( name+'_toslepton', config, desc, W_tos )
    WJets_tosjet    = SelWDiJets( name+'_tosjet'   , config, desc, W_notos, config['TOS_Jet'])

    ## Finally, register lines
    self.registerLine(StrippingLine( name + 'Line',
      prescale          = config['WJets_Prescale'],
      RequiredRawEvents = config['RequiredRawEvents'],
      checkPV           = False,
      selection         = WJets_toslepton,
    ))

    self.registerLine(StrippingLine( name + 'TaggedLine',
      prescale          = config['WJetsTagged_Prescale'],
      RequiredRawEvents = config['RequiredRawEvents'],
      checkPV           = False,
      selection         = WJets_tosjet,
    ))

