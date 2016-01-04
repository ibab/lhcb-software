'''
Reconstruct XB -> D(*)- pi+ proton and control mode B0 -> D(*)- pi+
'''

__author__ = ['Liming Zhang']
__date__ = '1/12/2015'

__all__ = ('XB2DPiPConf','default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from CommonParticles.Utils import updateDoD
from StandardParticles import StdLoosePions
from StandardParticles import StdLooseKaons
from StandardParticles import StdAllLooseKaons
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import MeV
import copy

default_config = {
    'NAME'              : 'XB2DPiP',
    'WGs'               : ['BandQ'],
    'BUILDERTYPE'       : 'XB2DPiPConf',
    'STREAMS' : [ 'Bhadron' ],
    'CONFIG'    : {
                         'TRCHI2DOF'                 :       5
                 ,       'TRGHOSTPROB'               :       0.5
                 ,       'MIPCHI2'                   :       9
                 ,       'ProtonPIDp'                      :       5
                 ,       'ProtonPIDpK'                     :      -3
                 ,        'MomCuts'     : """
                          (VFASPF(VCHI2/VDOF) < 10.) 
                          & (BPVDIRA> 0.9999) 
                          & (BPVIPCHI2()<25) 
                          & (BPVVDCHI2>250)
                          & (BPVVDRHO>0.1*mm) 
                          & (BPVVDZ>2.0*mm)
                          & (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > 0.0 *mm )
                          """
                  ,'RelatedInfoTools': [{
                      'Type'              : 'RelInfoVertexIsolation',
                      'Location'          : 'RelInfoVertexIsolation'
                  }, {
                      'Type'              : 'RelInfoVertexIsolationBDT',
                      'Location'          : 'RelInfoVertexIsolationBDT'
                  }, {
                      'Type'              : 'RelInfoConeVariables',
                      'ConeAngle'         : 1.0,
                      "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                      'Location'          : 'RelInfoConeVariables_1.0'
                  }, {
                      'Type'              : 'RelInfoConeVariables',
                      'ConeAngle'         : 1.5,
                      "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                      'Location'          : 'RelInfoConeVariables_1.5'
                  }, {
                      'Type'              : 'RelInfoConeVariables',
                      'ConeAngle'         : 2.0,
                      "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                      'Location'          : 'RelInfoConeVariables_2.0'
                  }]
                  }    
    }

### Lines stored in this file:

class XB2DPiPConf(LineBuilder) :
    __configuration_keys__ = ('TRCHI2DOF',
                              'TRGHOSTPROB',
                              'MIPCHI2',
                              'ProtonPIDp',
                              'ProtonPIDpK',
                              'MomCuts',
                              'RelatedInfoTools' )

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.name = name
        self.config = config
	
        #### Input selections:
        #------------------------------------------------------------- D- + Ds-
        self.DplusList = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPi/Particles")
        self.DsList = DataOnDemand(Location = "Phys/StdLooseDplusKKPi/Particles")
        self.DstarList = DataOnDemand(Location = "Phys/StdLooseDstarWithD02KPi/Particles")
        
        from PhysSelPython.Wrappers import MergedSelection

        self.DpDsList = MergedSelection( self.name + "SelDpDs",
                                        RequiredSelections =  [ self.DplusList,
                                                                self.DsList
                                                                ])
        self.SelDpDsList = self.createSubSel( OutputList = "DpDsForXB2DPiP" + self.name,
                                              InputList = self.DpDsList,
                                              Cuts = "(MIPDV(PRIMARY) > 0.05*mm) & ((CHILD(MIPCHI2DV(), 1) + CHILD(MIPCHI2DV(), 2) + CHILD(MIPCHI2DV(), 3) ) > 100.) & ((MAXTREE(TRGHOSTPROB, ISBASIC) < %(TRGHOSTPROB)s))" % self.config)

        self.SelDstarList = self.createSubSel( OutputList = "DstarForXB2DPiP" + self.name,
                                              InputList = self.DstarList,
                                              Cuts = "((MAXTREE(TRGHOSTPROB, ISBASIC) < %(TRGHOSTPROB)s))" % self.config)

        #------------------------------------------------------------- Protons
        
        self.PionList = self.createSubSel( OutputList = "PionsForXB2DPiP" + self.name,
                                           InputList = DataOnDemand(Location = "Phys/StdLoosePions/Particles"),                                                
                                           Cuts = "(MIPCHI2DV(PRIMARY) > %(MIPCHI2)s ) & (TRCHI2DOF < %(TRCHI2DOF)s ) & (TRGHOSTPROB < %(TRGHOSTPROB)s)" % self.config )                                           

        self.ProtonList = self.createSubSel( OutputList = "ProtonsForXB2DPiP" + self.name,
                                           InputList = DataOnDemand(Location = "Phys/StdLooseProtons/Particles"),                                                
                                           Cuts = "(MIPCHI2DV(PRIMARY) > %(MIPCHI2)s )" \
                                           " & (TRCHI2DOF < %(TRCHI2DOF)s ) "\
                                           " & (PIDp> %(ProtonPIDp)s) & ((PIDp-PIDK)> %(ProtonPIDpK)s)" \
                                           "& (P>10.0*GeV) & (TRGHOSTPROB < %(TRGHOSTPROB)s)" % self.config )                                           

        self.makeB2DPi()
        self.makeB2DPiP()
    # -------------------------- B0->D(*)pi constrol channel:
    def makeB2DPi( self ):
        Bd2DPi = self.createCombinationSel( OutputList = "Bd2DPi" + self.name,
                                DecayDescriptor = "[B0 -> D- pi+]cc",
                                DaughterLists  = [ self.SelDpDsList, self.PionList ],
                                PreVertexCuts = "in_range(4200,AM,6750)",
                                PostVertexCuts = self.config['MomCuts'] )

        Bd2DPiLine = StrippingLine( self.name + "Bd2DPiLine", 
                                    algos = [ Bd2DPi ], 
                                    MDSTFlag = True, 
                                    EnableFlavourTagging = False, 
                                    RelatedInfoTools = self.config['RelatedInfoTools']
                                    )

        Bd2DstarPi = self.createCombinationSel( OutputList = "Bd2DstarPi" + self.name,
                                DecayDescriptor = "[B0 -> D*(2010)- pi+]cc",
                                DaughterLists  = [ self.SelDstarList , self.PionList ],
                                PreVertexCuts = "in_range(4200,AM,7250)",
                                PostVertexCuts = self.config['MomCuts'] )

        Bd2DstarPiLine = StrippingLine( self.name + "Bd2DstarPiLine", 
                                        algos = [ Bd2DstarPi ], 
                                        MDSTFlag = True, 
                                        EnableFlavourTagging = False,
                                        RelatedInfoTools = self.config['RelatedInfoTools']                                        
                                         )

        self.registerLine(Bd2DPiLine)
        self.registerLine(Bd2DstarPiLine)


    def makeB2DPiP( self ):
        Bd2DPiP = self.createCombinationsSel( OutputList = "Bd2DPiP" + self.name,
                                DecayDescriptors = ["[B+ -> D- pi+ p+]cc", "[B- -> D- pi+ p~-]cc"],
                                DaughterLists  = [ self.SelDpDsList, self.PionList, self.ProtonList ],
                                PreVertexCuts = "in_range(4200,AM,7250) & ( ACHI2DOCA(2,3)<20 )",
                                PostVertexCuts = self.config['MomCuts'] )

        Bd2DPiPLine = StrippingLine( self.name + "Bd2DPiPLine", 
                                     algos = [ Bd2DPiP ], 
                                     MDSTFlag = True, 
                                     EnableFlavourTagging = False,
                                     RelatedInfoTools = self.config['RelatedInfoTools']                                     
                                      )

        Bd2DstarPiP = self.createCombinationsSel( OutputList = "Bd2DstarPiP" + self.name,
                                DecayDescriptors = ["[B+ -> D*(2010)- pi+ p+]cc", "[B- -> D*(2010)- pi+ p~-]cc"],
                                DaughterLists  = [ self.SelDstarList , self.PionList, self.ProtonList ],
                                PreVertexCuts = "in_range(4200,AM,7250) & ( ACHI2DOCA(2,3)<20 )",
                                PostVertexCuts = self.config['MomCuts'] )

        Bd2DstarPiPLine = StrippingLine( self.name + "Bd2DstarPiPLine", 
                                         algos = [ Bd2DstarPiP ], 
                                         MDSTFlag = True, 
                                         EnableFlavourTagging = False,
                                         RelatedInfoTools = self.config['RelatedInfoTools']                                         
                                          )

        self.registerLine(Bd2DPiPLine)
        self.registerLine(Bd2DstarPiPLine)

    ### Common tools:

    def createSubSel( self, OutputList, InputList, Cuts ) :
        '''create a selection using a FilterDesktop'''
        filter = FilterDesktop(Code = Cuts)
        return Selection( OutputList,
                      Algorithm = filter,
                      RequiredSelections = [ InputList ] )

    def createCombinationSel( self, OutputList,
                          DecayDescriptor,
                          DaughterLists,
                          DaughterCuts = {} ,
                          PreVertexCuts = "ALL",
                          PostVertexCuts = "ALL",
                          ReFitPVs = True ) :
        '''create a selection using a ParticleCombiner with a single decay descriptor'''
        combiner = CombineParticles( DecayDescriptor = DecayDescriptor,
                                     DaughtersCuts = DaughterCuts,
                                     MotherCut = PostVertexCuts,
                                     CombinationCut = PreVertexCuts,
                                     ReFitPVs = ReFitPVs)
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists)
        
    def createCombinationsSel( self, OutputList,
	                  DecayDescriptors,
                          DaughterLists,
                          DaughterCuts = {} ,
                          PreVertexCuts = "ALL",
                          PostVertexCuts = "ALL",
                          ReFitPVs = True ) :
        '''For taking in multiple decay descriptors'''
        combiner = CombineParticles( DecayDescriptors = DecayDescriptors,
                                 DaughtersCuts = DaughterCuts,
                                 MotherCut = PostVertexCuts,
                                 CombinationCut = PreVertexCuts,
                                 ReFitPVs = ReFitPVs)
        return Selection ( OutputList,
                       Algorithm = combiner,
                       RequiredSelections = DaughterLists)


def _format_related_info(selection, config):
    """Format the RelatedInfoTools configuation dictionary.

    Replaces any dictionaries which have 'DaughterLocations' set to `None` with
    the correct locations and decay descriptors.
    HACK
    """
    decay_descriptor = selection.algorithm().DecayDescriptor.replace('cc', 'CC')
    daughter_names = ['Dp_h', 'Dp_l1', 'Dp_l2']

    config = copy.deepcopy(config)
    for tool_config in config:
        if 'DaughterLocations' not in tool_config or tool_config['DaughterLocations'] is not None:
            continue

        tool_config['DaughterLocations'] = {}
        dd_index = decay_descriptor.find('->')+2
        daughter_index = 0
        while ' ' in decay_descriptor[dd_index:]:
            d = '{} ^{}'.format(decay_descriptor[:dd_index], decay_descriptor[dd_index+1:])
            tool_config['DaughterLocations'][d] = '{}_{}'.format(tool_config['Location'],
                                                                 daughter_names[daughter_index])
            daughter_index += 1
            dd_index = decay_descriptor.find(' ', dd_index+1)
    print config
    return config
    