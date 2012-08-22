# $Id: StrippingBs2PhiPhi.py, 2010-10-25 01:15 schleich Exp $

'''
Bs -> phi phi stripping selection.

Provides class StrippingBs2PhiPhiConf with methods to return stripping line objects.

Exports the following stripping lines as instance data members:
- Bs2PhiPhiLine      : main line with phi mass window of +/- 25 MeV/cc
- Bs2PhiPhiWideLine  : prescaled phi sideband line with MM(phi) < 1100 MeV/cc
- lines              : list with both lines

Both lines are build by the function _Bs2PhiPhi_X_Line, which accepts the name and the mass range flag (wide=True/False) as parameter.
'''

__author__  = [ 'Sean Benson' ]
__date__    = '2012/08/21'
__version__    = '2.0'


'''
Bs->PhiPhi stripping selection
'''

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

import StandardParticles
from StandardParticles import StdLooseKaons as MyLooseKaons
#if hasattr(StandardParticles, "StdAllLooseKaons"):
#  from StandardParticles import StdAllLooseKaons as MyLooseKaons
#else:
#  from StandardParticles import StdLooseKaons as MyLooseKaons

config_params = {
                  'KaonPT'              : 500      # MeV/c
                , 'KaonIPCHI2'          : 3.5      # adimensional
                , 'PhiPT'               : 0        # MeV/c
                , 'PhiPTsq'             : 2.0      # GeV*GeV/cc
                , 'PhiVertexCHI2pDOF'   : 12       # adimensional
                , 'PhiMassWindow'       : 25       # MeV/cc
                , 'PhiMassMax'          : 1090     # MeV/cc
                , 'BsVertexCHI2pDOF'    : 12       # adimensional
                , 'BsMassWindow'        : 300      # MeV/cc
                , 'WidePrescale'        : 0.05     # adimensional
}

class StrippingBs2PhiPhiConf(LineBuilder):

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
                , 'WidePrescale'              # adimensional
                )

    def __init__(self, name, config) :
      LineBuilder.__init__(self, name, config)

      self.Bs2PhiPhiLine     = self._Bs2PhiPhi_X_Line( name,        config, False )
      self.Bs2PhiPhiWideLine = self._Bs2PhiPhi_X_Line( name+"Wide", config, True )
      self.registerLine( self.Bs2PhiPhiLine )
      self.registerLine( self.Bs2PhiPhiWideLine )


    def _Bs2PhiPhi_X_Line( self, name, config, wide ) :

            Phi2KK_DC = "(PT>%(KaonPT)s*MeV)&(MIPCHI2DV(PRIMARY)>%(KaonIPCHI2)s)" % config
            Phi2KK_CC = "(AM<(%(PhiMassMax)s+30)*MeV)&(ADOCACHI2CUT(40, ''))" % config

            Phi2KK_MassCut = ""
            if wide == False :
              Phi2KK_MassCut = "&(ADMASS('phi(1020)')<%(PhiMassWindow)s*MeV)" % config
            else :
              Phi2KK_MassCut = "&(MM<%(PhiMassMax)s*MeV)" % config

            Phi2KK_MC = "(VFASPF(VCHI2/VDOF)<%(PhiVertexCHI2pDOF)s)" % config + Phi2KK_MassCut

    	    Bs2PhiPhi_DC = "(PT>%(PhiPT)s*MeV)" % config
    	    Bs2PhiPhi_CC = "(ADAMASS('B_s0')<((%(BsMassWindow)s+30)*MeV))&(ACHILD(PT,1)*ACHILD(PT,2)>%(PhiPTsq)s*GeV*GeV)" % config
    	    Bs2PhiPhi_MC = "(VFASPF(VCHI2/VDOF)<%(BsVertexCHI2pDOF)s)&(ADMASS('B_s0')<%(BsMassWindow)s*MeV)" % config
            ps = 1.0
            if wide == True :
              ps = config['WidePrescale']

    	    #print "Cuts for "+name+"Line"
            #print Phi2KK_DC
            #print Phi2KK_CC
            #print Phi2KK_MC

    	    #print Bs2PhiPhi_DC
    	    #print Bs2PhiPhi_CC
    	    #print Bs2PhiPhi_MC
            #print "Prescale: ", ps

	    #_stdLooseKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")

            _Bs2PhiPhiLooseDetachedPhi2KK = CombineParticles(
                            DecayDescriptor = "phi(1020) -> K+ K-"
                          , DaughtersCuts = {"K+": Phi2KK_DC}
                          , CombinationCut = Phi2KK_CC
                          , MotherCut = Phi2KK_MC
                       )


    	    _Bs2PhiPhi = CombineParticles(
             DecayDescriptor =  "B_s0 -> phi(1020) phi(1020)"
    	    , DaughtersCuts   = {"phi(1020)" : Bs2PhiPhi_DC}
            , CombinationCut  = Bs2PhiPhi_CC
            , MotherCut       = Bs2PhiPhi_MC
            )


	    Bs2PhiPhiLooseDetachedPhi2KK = Selection(
		name+ "_LoosePhi2KK",
		Algorithm = _Bs2PhiPhiLooseDetachedPhi2KK,
		RequiredSelections = [MyLooseKaons])
	    Bs2PhiPhi = Selection(
		 name,
		 Algorithm = _Bs2PhiPhi,
		 RequiredSelections = [Bs2PhiPhiLooseDetachedPhi2KK])

    	    return StrippingLine(name+"Line"
              , prescale = ps
              , postscale = 1
              , selection = Bs2PhiPhi
              )

