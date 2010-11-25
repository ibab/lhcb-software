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

__author__  = [ 'Sebastian Schleich' ]
__date__    = '2010/10/25'


'''
Bs->PhiPhi stripping selection
'''
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingSelections.Utils import checkConfig
from Configurables import FilterDesktop, CombineParticles
import GaudiKernel.SystemOfUnits as Units


class StrippingBs2PhiPhiConf(object):

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

    config_default = {
                  'KaonPT'              : 500      # MeV/c
                , 'KaonIPCHI2'          : 3.5      # adimensional
                , 'PhiPT'               : 900      # MeV/c
                , 'PhiPTsq'             : 2.0      # GeV*GeV/cc
                , 'PhiVertexCHI2pDOF'   : 25       # adimensional
                , 'PhiMassWindow'       : 25       # MeV/cc
                , 'PhiMassMax'          : 1090     # MeV/cc
                , 'BsVertexCHI2pDOF'    : 25       # adimensional
                , 'BsMassWindow'        : 300      # MeV/cc
                , 'WidePrescale'        : 0.05     # adimensional
                }

    config_Sept2010 = {
                  'KaonPT'              : 500      # MeV/c
                , 'KaonIPCHI2'          : 3.5      # adimensional
                , 'PhiPT'               : 900      # MeV/c
                , 'PhiPTsq'             : 4.0      # GeV*GeV/cc
                , 'PhiVertexCHI2pDOF'   : 25       # adimensional
                , 'PhiMassWindow'       : 25       # MeV/cc
                , 'PhiMassMax'          : 1090     # MeV/cc
                , 'BsVertexCHI2pDOF'    : 25       # adimensional
                , 'BsMassWindow'        : 300      # MeV/cc
                , 'WidePrescale'        : 0.05     # adimensional
                }

    def __init__(self, name = "Bs2PhiPhi", config=None) :
      checkConfig(StrippingBs2PhiPhiConf.__configuration_keys__, config)
      self.Bs2PhiPhiLine     = self._Bs2PhiPhi_X_Line( name, config, False )
      self.Bs2PhiPhiWideLine = self._Bs2PhiPhi_X_Line( name+"Wide", config, True )
      self.lines = [self.Bs2PhiPhiLine, self.Bs2PhiPhiWideLine]



    def _Bs2PhiPhi_X_Line( self, name, config, wide ) :
            Phi2KK_DC = "(PT>%(KaonPT)s*MeV)&(MIPCHI2DV(PRIMARY)>%(KaonIPCHI2)s)" % config
            Phi2KK_CC = "(AM<(%(PhiMassMax)s+30)*MeV)" % config
            Phi2KK_MC = "(VFASPF(VCHI2/VDOF)<%(PhiVertexCHI2pDOF)s)&(MM<%(PhiMassMax)s*MeV)" % config

            PhiFilter = ""
            if wide == False :
              PhiFilter = PhiFilter + "(ADMASS('phi(1020)')<%(PhiMassWindow)s*MeV)" % config
            else :
              PhiFilter = PhiFilter + "(MM<%(PhiMassMax)s*MeV)" % config

    	    Bs2PhiPhi_DC = "(PT>%(PhiPT)s*MeV)" % config
    	    Bs2PhiPhi_CC = "(ADAMASS('B_s0')<((%(BsMassWindow)s+30)*MeV))&(ACHILD(PT,1)*ACHILD(PT,2)>%(PhiPTsq)s*GeV*GeV)" % config
    	    Bs2PhiPhi_MC = "(VFASPF(VCHI2/VDOF)<%(BsVertexCHI2pDOF)s)&(ADMASS('B_s0')<%(BsMassWindow)s*MeV)" % config
            ps = 1
            if wide == True :
              ps = config['WidePrescale']

    	    #print "Cuts for "+name+"Line"
            #print Phi2KK_DC
            #print Phi2KK_CC
            #print Phi2KK_MC

    	    #print PhiFilter
    	    #print Bs2PhiPhi_DC
    	    #print Bs2PhiPhi_CC
    	    #print Bs2PhiPhi_MC
            #print "Prescale: ", ps

            Bs2PhiPhiLooseDetachedPhi2KK = CombineParticles(
                            "Bs2PhiPhi_LoosePhi2KK"
                          , InputLocations = ["StdLooseKaons"]
                          , DecayDescriptor = "phi(1020) -> K+ K-"
                          , DaughtersCuts = {"K+": Phi2KK_DC}
                          , CombinationCut = Phi2KK_CC
                          , MotherCut = Phi2KK_MC
                       )    

    	    Bs2PhiPhiPhi2KK = FilterDesktop(
                  name + "Phi2KK"
                , InputLocations = [ "Bs2PhiPhi_LoosePhi2KK" ]
                , Code = PhiFilter
                )

    	    Bs2PhiPhi = CombineParticles(
              name
            , DecayDescriptor =  "B_s0 -> phi(1020) phi(1020)"
            , InputLocations  = [ name+"Phi2KK" ]
    	    , DaughtersCuts   = {"phi(1020)" : Bs2PhiPhi_DC}
            , CombinationCut  = Bs2PhiPhi_CC
            , MotherCut       = Bs2PhiPhi_MC 
            )

    	    return StrippingLine(name+"Line"
              , prescale = ps
              , postscale = 1
              , algos = [Bs2PhiPhiLooseDetachedPhi2KK, Bs2PhiPhiPhi2KK, Bs2PhiPhi]
              )

