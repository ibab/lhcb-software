# Stripping Lines for (W->eNu)+ AntiKT Jets
# X. Cid Vidal (cuts on electrons copied from We line)
#

from Gaudi.Configuration import *
from Configurables       import FilterDesktop, CombineParticles, LoKi__VertexFitter
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdJets, StdAllNoPIDsElectrons
from CommonParticles.Utils import *


## if you want to prescale this line, please contact X. Cid Vidal before!
__author__=["Xabier Cid Vidal"]
__all__ = ["default_name","default_config","WeAKTJetsConf"]


default_name = 'WeAKTJets'
default_config = {
    'NAME':default_name,
    'BUILDERTYPE'  : 'WeAKTJetsConf',
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'BhadronCompleteEvent' ],
    'CONFIG':{'WeAKTJets_Prescale'    : 1.0,
              'WeAKTJets_Postscale'   : 1.0,
              'min_e_pT'              : 10., #GeV/c
              'max_e_pT'              : 200000., #GeV/c
              'dr_lepton_jet'         : 0.5,
              'PrsCalMin'             : 50.,
              'ECalMin'               : 0.10,
              'HCalMax'               : 0.05,
              'min_jet_pT'            : 15. #GeV/c
              }
    }



class WeAKTJetsConf( LineBuilder ) :

    __configuration_keys__ = ( 'WeAKTJets_Prescale',
                               'WeAKTJets_Postscale',
                               'min_e_pT',
                               'max_e_pT',
                               'dr_lepton_jet',
                               'PrsCalMin',
                               'ECalMin',
                               'HCalMax',
                               'min_jet_pT'
                               )

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name
        self._config = config

        self.sel_We    = self.makeW( self._myname + 'We')
        self.sel_DiJet  = self.makeDiJet (self._myname + 'DiJet')

        self.line_WeJets = StrippingLine( self._myname + 'Line',
                                          prescale  = config[ 'WeAKTJets_Prescale' ],
                                          postscale = config[ 'WeAKTJets_Postscale' ],
                                          RequiredRawEvents = ["Calo"],
                                          checkPV   = False,
                                          selection = self.sel_DiJet
                                          )

        self.registerLine( self.line_WeJets )



    def makeW( self, _name):
        # Define the W->e cuts
        _code    = '(PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s) '\
                   '& (PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s) '\
                   '& (PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s)'\
                   '& (PT>%(min_e_pT)s*GeV) & (PT<%(max_e_pT)s*GeV) '\
                   '& (HASTRACK & TRCUT(0<TrIDC(\'isTT\')))' %self._config


        _filter = FilterDesktop( _name,
                                 Preambulo = ["from LoKiTracks.decorators import *"],
                                 Code      = _code
                                 )

        return Selection ( "sel"+_name,
                           Algorithm          = _filter,
                           RequiredSelections = [StdAllNoPIDsElectrons]
                           )


    def makeDiJet(self,_name):

        DiJet = CombineParticles("Combine"+_name)
        DiJet.DecayDescriptor = "[H+ -> CELLjet CELLjet e+]cc"
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

        requiredSelections = [self.sel_We,StdJets]

        return Selection ("Sel"+_name,
                          Algorithm = DiJet,
                          RequiredSelections = requiredSelections)


