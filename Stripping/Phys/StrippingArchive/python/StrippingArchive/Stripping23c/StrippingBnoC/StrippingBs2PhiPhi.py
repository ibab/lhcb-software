# $Id: StrippingBs2PhiPhi.py, 2010-10-25 01:15 schleich Exp $

'''
Bs -> phi phi stripping selection.

Provides class StrippingBs2PhiPhiConf with methods to return stripping line objects.

Exports the following stripping lines as instance data members:
- Bs2PhiPhiLine      : main line with phi mass window of +/- 25 MeV/cc
- Bs2PhiPhiWideLine  : prescaled phi sideband line with MM(phi) < 1100 MeV/cc
- Bs2PhiPhiUpLine  : same as nominal line but enforces at least one upstream kaon
- lines              : list with lines
'''

__author__  = [ 'Sean Benson' ]
__date__    = '2013/08/15'
__version__    = '3.0'


'''
Bs->PhiPhi stripping selection
'''

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

import StandardParticles
#from StandardParticles import StdLooseANNKaons as MyLooseKaons
from StandardParticles import StdLooseANNUpKaons as MyLooseUpKaons
#if hasattr(StandardParticles, "StdAllLooseKaons"):
#  from StandardParticles import StdAllLooseKaons as MyLooseKaons
#else:
#  from StandardParticles import StdLooseKaons as MyLooseKaons
from StandardParticles import StdAllLooseANNKaons as MyLooseKaons

default_config = {
    'NAME'        : 'BetaSBs2PhiPhi',
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'StrippingBs2PhiPhiConf',
    'CONFIG'      : {'KaonPT'              : 400      # MeV/c
                     , 'KaonIPCHI2'          : 0.0      # adimensional
                     , 'PhiPT'               : 0        # MeV/c
                     , 'PhiPTsq'             : 1.2      # GeV*GeV/cc
                     , 'PhiVertexCHI2pDOF'   : 15       # adimensional
                     , 'PhiMassWindow'       : 25       # MeV/cc
                     , 'PhiMassMax'          : 1090     # MeV/cc
                     , 'BsVertexCHI2pDOF'    : 15       # adimensional
                     , 'BsMassWindow'        : 300      # MeV/cc
                     , 'WidePrescale'        : 0.15     # adimensional
                     },
    'STREAMS'     : { 'BhadronCompleteEvent' : ['StrippingBetaSBs2PhiPhiLine',
                                                'StrippingBetaSBs2PhiPhiUpLine'],
                      'Bhadron'              : ['StrippingBetaSBs2PhiPhiWideLine']
                    }
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
      self.Bs2PhiPhiUpLine   = self._Bs2PhiPhiUp_X_Line( name+"Up", config )
      self.registerLine( self.Bs2PhiPhiLine )
      self.registerLine( self.Bs2PhiPhiWideLine )
      self.registerLine( self.Bs2PhiPhiUpLine )


    def _Bs2PhiPhi_X_Line( self, name, config, wide ) :

            Phi2KK_DC = "(TRGHOSTPROB < 0.5) & (PT>%(KaonPT)s*MeV)&(MIPCHI2DV(PRIMARY)>%(KaonIPCHI2)s)" % config
            Phi2KK_CC = "(AM<(%(PhiMassMax)s+30)*MeV)" % config
            #Phi2KK_CC = "(AM<(%(PhiMassMax)s+30)*MeV)&(ADOCACHI2CUT(40, ''))" % config

            Phi2KK_MassCut = ""
            if wide == False :
              Phi2KK_MassCut = "&(ADMASS('phi(1020)')<%(PhiMassWindow)s*MeV)" % config
            else :
              Phi2KK_MassCut = "&(MM<%(PhiMassMax)s*MeV)" % config

            Phi2KK_MC = "(VFASPF(VCHI2/VDOF)<%(PhiVertexCHI2pDOF)s)" % config + Phi2KK_MassCut

            Bs2PhiPhi_DC = "(PT>%(PhiPT)s*MeV)" % config
            Bs2PhiPhi_CC = "(ADAMASS('B_s0')<((%(BsMassWindow)s+30)*MeV))&(ACHILD(PT,1)*ACHILD(PT,2)>%(PhiPTsq)s*GeV*GeV)" % config
            Bs2PhiPhi_MC = "(BPVDIRA > 0.999) & (VFASPF(VCHI2/VDOF)<%(BsVertexCHI2pDOF)s)&(ADMASS('B_s0')<%(BsMassWindow)s*MeV)" % config
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

            if wide == True:
      	      return StrippingLine(name+"Line"
                , prescale = ps
                , postscale = 1
                , selection = Bs2PhiPhi
                , EnableFlavourTagging = True,
                RelatedInfoTools = [ 
                    { "Type"         : "RelInfoConeVariables",
                    "ConeAngle"    : 0.8,
                    "Variables"    : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                    'RecursionLevel' : 2,
                    "Locations"     : {'Phys/StdAllLooseANNKaons' : ['P2ConeVar08_1','P2ConeVar08_2','P2ConeVar08_3','P2ConeVar08_4'] } },
                    { "Type"         : "RelInfoConeVariables",
                    "ConeAngle"    : 1.0,
                    "Variables"    : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                    'RecursionLevel' : 2,
                    "Locations"     : {'Phys/StdAllLooseANNKaons' : ['P2ConeVar10_1','P2ConeVar10_2','P2ConeVar10_3','P2ConeVar10_4'] } },
                    { "Type"         : "RelInfoConeVariables",
                    "ConeAngle"    : 1.3,
                    "Variables"    : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                    'RecursionLevel' : 2,
                    "Locations"     : {'Phys/StdAllLooseANNKaons' : ['P2ConeVar13_1','P2ConeVar13_2','P2ConeVar13_3','P2ConeVar13_4'] } },
                    { "Type"         : "RelInfoConeVariables",
                    "ConeAngle"    : 1.7,
                    "Variables"    : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                    'RecursionLevel' : 2,
                    "Locations"     : {'Phys/StdAllLooseANNKaons' : ['P2ConeVar17_1','P2ConeVar17_2','P2ConeVar17_3','P2ConeVar17_4'] } },
                    { "Type"         : "RelInfoVertexIsolation", "Location"     : "VertexIsoInfo" } ] 
              )
            else:
              return StrippingLine(name+"Line"
                , prescale = ps
                , postscale = 1
                , selection = Bs2PhiPhi
                , EnableFlavourTagging = True
                , RequiredRawEvents = ['Trigger', 'Muon', 'Calo', 'Rich', 'Velo', 'Tracker'])


    def _Bs2PhiPhiUp_X_Line( self, name, config ) :

            Phi2KK_DC = "(TRGHOSTPROB < 0.5) & (PT>%(KaonPT)s*MeV)&(MIPCHI2DV(PRIMARY)>%(KaonIPCHI2)s)" % config
            Phi2KK_CC = "(AM<(%(PhiMassMax)s+30)*MeV)" % config
            #Phi2KK_CC = "(AM<(%(PhiMassMax)s+30)*MeV)&(ADOCACHI2CUT(40, ''))" % config

            UpAddition = "(INTREE( HASTRACK & ISUP ) & INTREE( HASTRACK & ISLONG )) & "

            Phi2KK_MassCut = "&(ADMASS('phi(1020)')<%(PhiMassWindow)s*MeV)" % config

            Phi2KK_MC = "(VFASPF(VCHI2/VDOF)<%(PhiVertexCHI2pDOF)s)" % config + Phi2KK_MassCut

            Bs2PhiPhi_DC = "(PT>%(PhiPT)s*MeV)" % config
            Bs2PhiPhi_CC = "(ADAMASS('B_s0')<((%(BsMassWindow)s+30)*MeV))&(ACHILD(PT,1)*ACHILD(PT,2)>%(PhiPTsq)s*GeV*GeV)" % config
            Bs2PhiPhi_MC = UpAddition+"(BPVDIRA > 0.999) & (VFASPF(VCHI2/VDOF)<%(BsVertexCHI2pDOF)s)&(ADMASS('B_s0')<%(BsMassWindow)s*MeV)" % config

            _Bs2PhiPhiLooseAllDetachedPhi2KK = CombineParticles(
                            DecayDescriptor = "phi(1020) -> K+ K-"
                          , DaughtersCuts = {"K+": Phi2KK_DC}
                          , CombinationCut = Phi2KK_CC
                          , MotherCut = Phi2KK_MC
                       )

            _Bs2PhiPhiAll = CombineParticles(
             DecayDescriptor =  "B_s0 -> phi(1020) phi(1020)"
    	    , DaughtersCuts   = {"phi(1020)" : Bs2PhiPhi_DC}
            , CombinationCut  = Bs2PhiPhi_CC
            , MotherCut       = Bs2PhiPhi_MC
            )


            Bs2PhiPhiLooseAllDetachedPhi2KK = Selection(
		name+ "_LooseAllPhi2KK",
		Algorithm = _Bs2PhiPhiLooseAllDetachedPhi2KK,
		RequiredSelections = [MyLooseKaons,MyLooseUpKaons])
            Bs2PhiPhiAll = Selection(
		 name,
		 Algorithm = _Bs2PhiPhiAll,
		 RequiredSelections = [Bs2PhiPhiLooseAllDetachedPhi2KK])

            return StrippingLine(name+"Line"
              , prescale = 1
              , postscale = 1
              , selection = Bs2PhiPhiAll
              , EnableFlavourTagging = True
              , RequiredRawEvents = ['Trigger', 'Muon', 'Calo', 'Rich', 'Velo', 'Tracker'])
