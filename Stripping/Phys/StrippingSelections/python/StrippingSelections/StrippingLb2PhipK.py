# $Id: StrippingLb2PhipK.py, 2010-10-25 01:15 schleich Exp $

'''
Lb -> phi K p stripping selection.

Provides class StrippingLb2PhipKConf with a method to return a stripping line object.
'''

__author__  = [ 'Laurence Carson, Adam Morris' ]
__date__    = '2013/12/08'
__version__ = '1.0'


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

import StandardParticles
from StandardParticles import StdLooseKaons as MyLooseKaons
from StandardParticles import StdLooseProtons as MyLooseProtons

config_params = {
                  'KaonPT'              : 400      # MeV/c
                , 'KaonIPCHI2'          : 4.0      # adimensional
                , 'PhiPT'               : 0        # MeV/c
                , 'PhiPTsq'             : 2.0      # GeV*GeV/cc
                , 'PhiVertexCHI2pDOF'   : 15       # adimensional
                , 'PhiMassWindow'       : 25       # MeV/cc
                , 'PhiMassMax'          : 1090     # MeV/cc
                , 'BsVertexCHI2pDOF'    : 15       # adimensional
                , 'BsMassWindow'        : 300      # MeV/cc
}

class StrippingLb2PhipKConf(LineBuilder):

    __configuration_keys__ = (
                  'KaonPT'                    # MeV/c
                , 'KaonIPCHI2'                # adimensional
                , 'PhiPT'                     # MeV/c
                , 'PhiPTsq'                   # GeV*GeV/cc
                , 'PhiVertexCHI2pDOF'         # adimensional
                , 'PhiMassWindow'             # MeV/cc
                , 'PhiMassMax'                # MeV/cc
                , 'BsVertexCHI2pDOF'          # adimensional
                , 'BsMassWindow'              # MeV/cc
                )

    def __init__(self, name, config) :
      LineBuilder.__init__(self, name, config)

      self.Lb2PhipKLine     = self._Lb2PhipK_X_Line( name, config )
      self.registerLine( self.Lb2PhipKLine )


    def _Lb2PhipK_X_Line( self, name, config) :

            # Cuts on the phi and KK pair
            Phi2KK_DC = "(TRGHOSTPROB < 0.5) & (PT>%(KaonPT)s*MeV)&(MIPCHI2DV(PRIMARY)>%(KaonIPCHI2)s)" % config
            Phi2KK_CC = "(AM<(%(PhiMassMax)s+30)*MeV)&(ADOCACHI2CUT(40, ''))" % config
            Phi2KK_MassCut = "&(ADMASS('phi(1020)')<%(PhiMassWindow)s*MeV)" % config
            Phi2KK_MC = "(VFASPF(VCHI2/VDOF)<%(PhiVertexCHI2pDOF)s)" % config + Phi2KK_MassCut
            
            # Same cuts are applied to both the proton and kaon in the pK pair
            pK_DC = "(TRGHOSTPROB < 0.5) & (PT>%(KaonPT)s*MeV)&(MIPCHI2DV(PRIMARY)>%(KaonIPCHI2)s)" % config
            pK_CC = "(AM<(4600)*MeV)&(ADOCACHI2CUT(40, ''))" % config
            pK_MC = "(VFASPF(VCHI2/VDOF)<%(PhiVertexCHI2pDOF)s)" % config

            # Cuts on the Lambda_b
      	    Lb2PhipK_DC = "(PT>%(PhiPT)s*MeV)" % config
      	    Lb2PhipK_CC = "(ADAMASS('Lambda_b0')<((%(BsMassWindow)s+30)*MeV))&(ACHILD(PT,1)*ACHILD(PT,2)>%(PhiPTsq)s*GeV*GeV)" % config
      	    Lb2PhipK_MC = "(BPVDIRA > 0.999) & (VFASPF(VCHI2/VDOF)<%(BsVertexCHI2pDOF)s)&(ADMASS('Lambda_b0')<%(BsMassWindow)s*MeV)" % config
      	    
            ps = 1.0


            _Lb2PhipKLooseDetachedPhi2KK = CombineParticles(
                            DecayDescriptor = "phi(1020) -> K+ K-"
                          , DaughtersCuts = {"K+": Phi2KK_DC}
                          , CombinationCut = Phi2KK_CC
                          , MotherCut = Phi2KK_MC
                       )
                       
            # Lambda(1520)0 used as nominal intermediary particle for the pK pair
            _Lb2PhipKLooseDetachedpK = CombineParticles(
                            DecayDescriptor = "Lambda(1520)0 -> p+ K-"
                          , DaughtersCuts = {"K-": pK_DC, "p+": pK_DC}
                          , CombinationCut = pK_CC
                          , MotherCut = pK_MC
                       )


    	    _Lb2PhipK = CombineParticles(
             DecayDescriptor =  "Lambda_b0 -> Lambda(1520)0 phi(1020)"
    	    , DaughtersCuts   = {"phi(1020)" : Lb2PhipK_DC, "Lambda(1520)0" : Lb2PhipK_DC}
            , CombinationCut  = Lb2PhipK_CC
            , MotherCut       = Lb2PhipK_MC
            )


	    Lb2PhipKLooseDetachedPhi2KK = Selection(
		name+ "_LoosePhi2KK",
		Algorithm = _Lb2PhipKLooseDetachedPhi2KK,
		RequiredSelections = [MyLooseKaons])
		
	    Lb2PhipKLooseDetachedpK = Selection(
		name+ "_LoosepK",
		Algorithm = _Lb2PhipKLooseDetachedpK,
		RequiredSelections = [MyLooseKaons,MyLooseProtons])
		
	    Lb2PhipK = Selection(
		 name,
		 Algorithm = _Lb2PhipK,
		 RequiredSelections = [Lb2PhipKLooseDetachedPhi2KK,Lb2PhipKLooseDetachedpK])

    	    return StrippingLine(name+"Line"
              , prescale = ps
              , postscale = 1
              , selection = Lb2PhipK
              , EnableFlavourTagging = False)

