# -*- coding: utf-8 -*-
############################################
# Stripping Lines for                      #
#     Z->(Tau+ ->n  Prong nu_tau)          #
#        (Tau- ->n' Prong nu_tau)          #
# LFV Group                                #
#                                          #
# @author: Ch. Elsasser elsasser@cern.ch   #
############################################








from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter 
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions
from StandardParticles import StdLoosePions,StdTightPions



__author__=['Christian Elsasser']
__date__='10/08/2011'
__version__='$Revision: 1.0 $'

config_params = {'Z2TauTau_Prong_LinePrescale'  : 1.0,
                 'Z2TauTau_Prong_LinePostscale' : 1.0,
                 ###
                 'TAU_MASS_LOW'                 : '400.0',    # MeV/c²
                 'TAU_MASS_HIGH'                : '1777.0',   # MeV/c²
                 ###
                 'Z_MASS_LOW'                   : '10000.0',  # MeV/c²
                 #'Z_MASS_HIGH'                 : not set
                 'VCHI2_Z_MAX'                  : '100',       # dl
                 ###
                 'PT_HAD_MIN'                   : '2000',     # MeV/c
                 'TRACKCHI2_HAD_MAX'            : '5',        # dl
                 ###
                 'PT_TAU_MIN'                   : '2000',     # MeV/c
                 'VCHI2_TAU_MAX'                : '20'        # dl
                 #'FDCHI2_TAU_MIN'               : '0'         # dl
                 }
                 
   
default_name = "Z02TauTauProng"


"""
Z->(Tau->n Prong)(Tau->n Prong)
"""

__all__ = ('Z02TauTauProngConf')

class Z02TauTauProngConf(LineBuilder) :

    Z2TauTauLine      = None
    



    __configuration_keys__ = ('Z2TauTau_Prong_LinePrescale',
                              'Z2TauTau_Prong_LinePostscale',
                              ###
                              'TAU_MASS_LOW',
                              'TAU_MASS_HIGH',
                              ###
                              'Z_MASS_LOW',
                              #'Z_MASS_HIGH',
                              'VCHI2_Z_MAX',
                              ###
                              'PT_HAD_MIN',
                              'TRACKCHI2_HAD_MAX',
                              ###
                              'PT_TAU_MIN',
                              'VCHI2_TAU_MAX'
                              #'FDCHI2_TAU_MIN'
                               )

           
    
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)
        self.name = name

        self.__PVOutputLocation__                  = {name              :  "Rec/Vertex/"+name+"_ModPrimary",
                                                      name+"_SameSign"  :  "Rec/Vertex/"+name+"_SameSign_ModPrimary"}
        

       
        #self.__PVOutputLocation__                  = "Rec/Vertex/Primary" 

        ### DEFINE HADRONS
        selHad         = {name               : self._createHadProng(    name   = "Hadron_"+self.name,
                                                                        config = config),
                          name+"_SameSign"   : self._createHadProng(    name   = "Hadron_"+self.name+"_SameSign",
                                                                        config = config)
                         }
        
        ### DEFINE TAUS
        selTau         = {name               : self._createTauProng(    name   = "Tau_"+self.name,
                                                                        hadSel = selHad[self.name],
                                                                        config = config),
                          name+"_SameSign"   : self._createTauProng(    name   = "Tau_"+self.name+"_SameSign",
                                                                        hadSel = selHad[self.name+"_SameSign"],
                                                                        config = config)
                         }
       
        
       

        
        ##### DEFINE LINES

                                                     
        self.Z2TauTauLine           = StrippingLine(  name+"_Line",
                                                      prescale    = config['Z2TauTau_Prong_LinePrescale'],
                                                      postscale   = config['Z2TauTau_Prong_LinePostscale'],
                                                      selection   = self._createZ(name   = name,
                                                                                  tauSel = selTau[name],
                                                                                  config = config)
                                                      )
        self.Z2TauTauSameSignLine   = StrippingLine(  name+"_SameSign_Line",
                                                      prescale    = config['Z2TauTau_Prong_LinePrescale'],
                                                      postscale   = config['Z2TauTau_Prong_LinePostscale'],
                                                      selection   = self._createZ_SameSign(name   = name+"_SameSign",
                                                                                           tauSel = selTau[name+"_SameSign"],
                                                                                           config = config)
                                                      )
        

        self.registerLine( self.Z2TauTauLine )
        self.registerLine( self.Z2TauTauSameSignLine )
        
       
        #print "Created Line"

    
    ### CREATING Z CANDIDATE
    def _createZ(self, name,tauSel,config) :
        """
        Z0 selection
        """

        
        _combcut        = "(AM > "                        + config['Z_MASS_LOW']          + "*MeV)" # & "\
        _Zcut           = "(VFASPF(VCHI2PDOF) < "         + config['VCHI2_Z_MAX']         + ")"

        _CombineZ       = CombineParticles(  DecayDescriptors          = ["Z0 -> tau+ tau-"],
                                             CombinationCut            = _combcut,
                                             MotherCut                 = _Zcut
                                             #Preambulo                = "",#_preambulo
                                             )
        
        return Selection(   name+"_TauTau",
                            Algorithm            = _CombineZ,
                            RequiredSelections   = [ tauSel ]
                            )

    
    def _createZ_SameSign(self,name,tauSel,config) :
        """
        Z0 selection Same Sign
        """

        
        _combcut        = "(AM > "                        + config['Z_MASS_LOW']          + "*MeV)" # & "\
        _Zcut           = "(VFASPF(VCHI2PDOF) < "         + config['VCHI2_Z_MAX']         + ")"

        _CombineZ       = CombineParticles(  DecayDescriptors          = ['[Z0 -> tau+ tau+]cc'],
                                             CombinationCut            = _combcut,
                                             MotherCut                 = _Zcut,
                                             WriteP2PVRelations        = False
                                             #Preambulo                = "",#_preambulo
                                             )
        
        return Selection(   name+"_TauTau",
                            Algorithm            = _CombineZ,
                            RequiredSelections   = [ tauSel ]
                            )
    


    ### CREATING PRONG TAU
    def _createTauProng(self,name,hadSel,config) :
        """
        Tau Prong selection
        """

        selectionCuts   = self._hadIntermediateKinematicCutsProng(config)

        _combcut        = selectionCuts[0] + " & "\
                          "(AM > "                            + config['TAU_MASS_LOW']          + "*MeV)  & "\
                          "(AM < "                            + config['TAU_MASS_HIGH']         + "*MeV)"#  & "\
                          
        

        _taucut         = selectionCuts[1]
        

        _CombineTau     = CombineParticles(  DecayDescriptor           = "[tau+ -> pi+ pi+ pi-]cc ",
                                             CombinationCut            = _combcut,
                                             MotherCut                 = _taucut
                                             )

        return Selection(   name                 = name+"_TauProng",
                            Algorithm            = _CombineTau,
                            RequiredSelections   = [hadSel]
                            )

    ### CREATING PRONG 
    def _createHadProng(self, name, config) :
        """
        Hadron selection Prong
        """
        selectionCuts   = self._hadFinalStateKinematicCutsProng(config) + " & (((PIDp < 5) & (PIDe < 5) & (PIDmu < 5)) | ((PIDp-PIDK < 5) & (PIDe-PIDK < 5) & (PIDmu-PIDK < 5)))"

        print name

        _filter         = FilterDesktop(Code                      = selectionCuts
                                        )

        return Selection(   name+"_HadProng",
                            Algorithm            = _filter,
                            RequiredSelections   = [ StdLoosePions ])


                                               
    ### KINEMATIC CUTS FOR TAU
    def _hadIntermediateKinematicCutsProng(self,config):
        _combcode = "(APT > "                + config['PT_TAU_MIN'] +"*MeV)"
        _ptvxcode = "(VFASPF(VCHI2PDOF) < "  + config['VCHI2_TAU_MAX'] +")"
        return [_combcode,_ptvxcode]

    ### KINEMATIC CUTS FOR PRONG
    def _hadFinalStateKinematicCutsProng(self,config):
        _code     = "(PT > "                 + config['PT_HAD_MIN'] +"*MeV) & "\
                    "(TRCHI2DOF < "          + config['TRACKCHI2_HAD_MAX'] + ")"
        return _code 







        
