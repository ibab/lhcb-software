
from Gaudi.Configuration import *
from Configurables       import FilterDesktop, CombineParticles, LoKi__VertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdJets
from CommonParticles.Utils import *
from Configurables import FilterJet
from GaudiKernel.SystemOfUnits import GeV

__author__=["Xabier Cid Vidal","Cedric Potterat"]
__all__ = ["default_name","default_config","TaggedJetsConf"]

## if you want to prescale this line, please contact the authors before!
default_name = 'TaggedJets'

default_config = {
    'NAME': default_name,
    'BUILDERTYPE'  : 'TaggedJetsConf',
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'BhadronCompleteEvent' ],
    'CONFIG':{'TaggedJetsPair_Prescale'   : 1.0,
              'TaggedJetsPair_Postscale'  : 1.0,
              'TaggedJetsPairExclusiveDiJet_Prescale':1.0,
              'TaggedJetsPairExclusiveDiJet_Postscale':1.0,
              'DiTaggedJetsPair_Prescale' : 1.0,
              'DiTaggedJetsPair_Postscale': 1.0,
              'TaggedJetsFour_Prescale'   : 1.0,
              'TaggedJetsFour_Postscale'  : 1.0,
              'TaggedJetsSix_Prescale'    : 1.0,
              'TaggedJetsSix_Postscale'   : 1.0,
              'TaggedJetsEight_Prescale'  : 1.0,
              'TaggedJetsEight_Postscale' : 1.0,
              'min_jet_pT'                : 25. * GeV, # GeV/c
              'min_jet_pT_ExclusiveDiJet' : 20. * GeV, # GeV/c
              }
    }


class TaggedJetsConf( LineBuilder ) :

    __configuration_keys__ = ( 'TaggedJetsPair_Prescale',
                               'TaggedJetsPair_Postscale',
                               'TaggedJetsPairExclusiveDiJet_Prescale',
                               'TaggedJetsPairExclusiveDiJet_Postscale',
                               'DiTaggedJetsPair_Prescale',
                               'DiTaggedJetsPair_Postscale',
                               'TaggedJetsFour_Prescale',
                               'TaggedJetsFour_Postscale',
                               'TaggedJetsSix_Prescale',
                               'TaggedJetsSix_Postscale',
                               'TaggedJetsEight_Prescale',
                               'TaggedJetsEight_Postscale',
                               'min_jet_pT',
                               'min_jet_pT_ExclusiveDiJet'
                               )

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name
        self._config = config

        ## B TAGGING!!
        filterJet = FilterJet('BDTTagJets')
        filterJet.Inputs = ["Phys/StdJets/Particles"]
        filterJet.tagToolName = 'LoKi::BDTTag'
        filterJet.Cut = 0
        taggedJetsLocation = updateDoD ( filterJet )
        self.__taggedJets = DataOnDemand(Location = taggedJetsLocation.keys()[0])
        
        self.sel_DiJet  = self.makeJetGroup (self._myname + 'DiJet',2,ptcut=1)
        self.sel_DiJet_ExclusiveDiJet  = self.makeJetGroup (self._myname + 'DiJetExclusiveDiJet',2,ptcut=2)
        self.sel_DiJet_noPT  = self.makeJetGroup (self._myname + 'DiJetNoPT',2)  
        self.sel_DiJetPair  = self.makeDiJetPair (self._myname + 'DiJetPair')
        self.sel_FourJet  = self.makeJetGroup (self._myname + 'FourJet',4)
        self.sel_SixJet  = self.makeJetGroup (self._myname + 'SixJet',6)
        self.sel_EightJet  = self.makeJetGroup (self._myname + 'EightJet',8)
        
        
        # filter events with more than two jets
        self.filterJets = " ( CONTAINS('Phys/StdJets/Particles') == 2 ) "

        
        self.line_TaggedJetsPair = StrippingLine( self._myname + 'JetPairLine',
                                                  prescale  = config[ 'TaggedJetsPair_Prescale' ],
                                                  postscale = config[ 'TaggedJetsPair_Postscale' ],
                                                  RequiredRawEvents = ["Calo"],
                                                  checkPV   = False,
                                                  selection = self.sel_DiJet
                                                  )

        self.line_TaggedJetsPairExclusiveDiJet = StrippingLine( self._myname + 'JetPairLineExclusiveDiJet',
                                                     prescale  = config[ 'TaggedJetsPairExclusiveDiJet_Prescale' ],
                                                     postscale = config[ 'TaggedJetsPairExclusiveDiJet_Postscale' ],
                                                     FILTER = self.filterJets,
                                                     RequiredRawEvents = ["Calo"],
                                                     checkPV   = False,
                                                     selection = self.sel_DiJet_ExclusiveDiJet
                                                     )

        
        self.line_DiTaggedJetsPair = StrippingLine( self._myname + 'DiJetPairLine',
                                                    prescale  = config[ 'DiTaggedJetsPair_Prescale' ],
                                                    postscale = config[ 'DiTaggedJetsPair_Postscale' ],
                                                    RequiredRawEvents = ["Calo"],
                                                    checkPV   = False,
                                                    selection = self.sel_DiJetPair
                                                    )
        

        self.line_TaggedJetsFour = StrippingLine( self._myname + 'FourJetLine',
                                                  prescale  = config[ 'TaggedJetsFour_Prescale' ],
                                                  postscale = config[ 'TaggedJetsFour_Postscale' ],
                                                  RequiredRawEvents = ["Calo"],
                                                  checkPV   = False,
                                                  selection = self.sel_FourJet
                                                  )

        
        self.line_TaggedJetsSix = StrippingLine( self._myname + 'SixJetLine',
                                                 prescale  = config[ 'TaggedJetsSix_Prescale' ],
                                                 postscale = config[ 'TaggedJetsSix_Postscale' ],
                                                 RequiredRawEvents = ["Calo"],
                                                 checkPV   = False,
                                                 selection = self.sel_SixJet
                                                 )

        self.line_TaggedJetsEight = StrippingLine( self._myname + 'EightJetLine',
                                                   prescale  = config[ 'TaggedJetsEight_Prescale' ],
                                                   postscale = config[ 'TaggedJetsEight_Postscale' ],
                                                   RequiredRawEvents = ["Calo"],
                                                   checkPV   = False,
                                                   selection = self.sel_EightJet
                                                   )
        
        self.registerLine( self.line_TaggedJetsPair )
        self.registerLine( self.line_TaggedJetsPairExclusiveDiJet )
        self.registerLine( self.line_DiTaggedJetsPair )
        self.registerLine( self.line_TaggedJetsFour )
        self.registerLine( self.line_TaggedJetsSix )
        self.registerLine( self.line_TaggedJetsEight )



    def makeJetGroup(self,_name,njets,ptcut=0):

        JetGroup = CombineParticles("Combine"+_name)
        if njets==2: JetGroup.DecayDescriptor = "H_10 -> CELLjet CELLjet"
        elif njets==4: JetGroup.DecayDescriptor = "H_10 -> CELLjet CELLjet CELLjet CELLjet"
        elif njets==6: JetGroup.DecayDescriptor = "H_10 -> CELLjet CELLjet CELLjet CELLjet CELLjet CELLjet"
        elif njets==8: JetGroup.DecayDescriptor = "H_10 -> CELLjet CELLjet CELLjet CELLjet CELLjet CELLjet CELLjet CELLjet"
        else: raise ValueError
        
        JetGroup.ParticleCombiners = {"" : "LoKi::VertexFitter"}
        JetGroup.addTool( LoKi__VertexFitter, name="LoKi::VertexFitter" )
        vfitter = getattr ( JetGroup , "LoKi::VertexFitter" )
        vfitter.Jets = ""
        
        if ptcut==1: JetGroup.DaughtersCuts = { "CELLjet" :" (PT > %(min_jet_pT)s ) " %self._config }
        elif ptcut==2: JetGroup.DaughtersCuts = { "CELLjet" :" (PT > %(min_jet_pT_ExclusiveDiJet)s ) " %self._config }    
        
        JetGroup.MotherCut = "ALL"
        
        requiredSelections = [self.__taggedJets]
        
        return Selection ("Sel"+_name,
                          Algorithm = JetGroup,
                          RequiredSelections = requiredSelections)



    def makeDiJetPair(self,_name):

        DiJetPair = CombineParticles("Combine"+_name)
        DiJetPair.DecayDescriptor = "H_20 -> H_10 H_10"
        DiJetPair.ParticleCombiners = {"" : "LoKi::VertexFitter"}
        DiJetPair.addTool( LoKi__VertexFitter, name="LoKi::VertexFitter" )
        
        DiJetPair.MotherCut = "ALL"
        
        requiredSelections = [self.sel_DiJet_noPT]

        return Selection ("Sel"+_name,
                          Algorithm = DiJetPair,
                          RequiredSelections = requiredSelections)
