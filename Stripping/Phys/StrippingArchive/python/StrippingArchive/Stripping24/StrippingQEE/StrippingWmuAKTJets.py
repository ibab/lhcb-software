# Stripping Lines for (W->muNu)+ AntiKT Jets
# X. Cid Vidal
#

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from Configurables       import CombineParticles, LoKi__VertexFitter
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdAllLooseMuons, StdJets
from CommonParticles.Utils import *

__author__=["Xabier Cid Vidal"]
__all__ = ["default_name","default_config","WmuAKTJetsConf"]

## if you want to prescale this line, please contact X. Cid Vidal before!
default_name = 'WmuAKTJets'

default_config = {
    'NAME': default_name,
    'BUILDERTYPE'  : 'WmuAKTJetsConf',
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'BhadronCompleteEvent' ],
    'CONFIG':{'WmuAKTJets_Prescale'    : 1.0,
              'WmuAKTJets_Postscale'   : 1.0,
              'min_mu_pT'              : 10., # GeV/c
              'max_mu_pT'              : 200000., # GeV/c
              'dr_lepton_jet'          : 0.5,
              'min_jet_pT'             : 15. # GeV/c
              }
    }


class WmuAKTJetsConf( LineBuilder ) :

    __configuration_keys__ = ( 'WmuAKTJets_Prescale',
                               'WmuAKTJets_Postscale',
                               'min_mu_pT',
                               'max_mu_pT',
                               'dr_lepton_jet',
                               'min_jet_pT'
                               )

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name
        self._config = config

        self.sel_Wmu    = self.makeW( self._myname + 'Wmu')
        self.sel_DiJet  = self.makeDiJet (self._myname + 'DiJet')

        self.line_WmuJets = StrippingLine( self._myname + 'Line',
                                       prescale  = config[ 'WmuAKTJets_Prescale' ],
                                       postscale = config[ 'WmuAKTJets_Postscale' ],
                                       RequiredRawEvents = ["Calo"],
                                       checkPV   = False,
                                       selection = self.sel_DiJet
                                       )

        self.registerLine( self.line_WmuJets )



    def makeW( self, _name):
        # Define the W->mu cuts
        _code     = '(PT>%(min_mu_pT)s*GeV) & (PT<%(max_mu_pT)s*GeV)' %self._config
        
        return Selection ( "sel"+_name,
                           Algorithm          = FilterDesktop( Code=_code ),
                           RequiredSelections = [StdAllLooseMuons]
                           )


    def makeDiJet(self,_name):

        DiJet = CombineParticles("Combine"+_name)
        DiJet.DecayDescriptor = "[H+ -> CELLjet CELLjet mu+]cc"
        DiJet.ParticleCombiners = {"" : "LoKi::VertexFitter"}
        DiJet.addTool( LoKi__VertexFitter, name="LoKi::VertexFitter" )
        vfitter = getattr ( DiJet , "LoKi::VertexFitter" )
        vfitter.Jets = ""

        DiJet.DaughtersCuts = { "CELLjet" :" (PT > %(min_jet_pT)s * GeV ) " %self._config }


        DiJet.Preambulo  = ["from GaudiKernel.SystemOfUnits import degree",

                            "eta_1 = ACHILDFUN(1,ETA)" ,
                            "eta_2 = ACHILDFUN(2,ETA)" ,
                            "eta_3 = ACHILDFUN(3,ETA)" ,

                            "phi_1 = ACHILDFUN(1,PHI)" ,
                            "phi_2 = ACHILDFUN(2,PHI)" ,
                            "phi_3 = ACHILDFUN(3,PHI)" ,

                            ##
                            "deta_13 = eta_1 - eta_3" ,
                            "deta_23 = eta_2 - eta_3" ,

                            ##
                            "dphi_13 = phi_1 - phi_3 " ,
                            "dphi_23 = phi_2 - phi_3" ,

                            ##
                            "dphi_13 = switch ( dphi_13 >  180 * degree ,  dphi_13 - 180 * degree , dphi_13 ) ",
                            "dphi_13 = switch ( dphi_13 < -180 * degree ,  dphi_13 + 180 * degree , dphi_13 ) ",
                            "dphi_23 = switch ( dphi_23 >  180 * degree ,  dphi_23 - 180 * degree , dphi_23 ) ",
                            "dphi_23 = switch ( dphi_23 < -180 * degree ,  dphi_23 + 180 * degree , dphi_23 ) ",

                            ##
                            "dr_13 = sqrt(deta_13**2 + dphi_13**2)" ,
                            "dr_23 = sqrt(deta_23**2 + dphi_23**2)" ,

                            ]

        DiJet.CombinationCut = "AALLSAMEBPV "\
                               "& ( dr_13 > %(dr_lepton_jet)s )"\
                               "& ( dr_23 > %(dr_lepton_jet)s )" %self._config

        DiJet.MotherCut = "ALL"

        requiredSelections = [self.sel_Wmu,StdJets]

        return Selection ("Sel"+_name,
                          Algorithm = DiJet,
                          RequiredSelections = requiredSelections)

