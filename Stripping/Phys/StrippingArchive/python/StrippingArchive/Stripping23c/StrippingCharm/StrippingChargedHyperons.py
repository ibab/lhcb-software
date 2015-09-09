'''
Stripping line for Omega --> Lambda,K and Xi --> Lambda pi derived from
  Stripping line for HeavyBaryons studies : Xi_b-, Xi_b0 and Omega_b-
  Author: Yasmine Amhis

Updated for Run 2 from Run1 version; the only changes are in format
'''
__author__ = ['Mike Sokoloff, Laurence Carson']
__date__ = '21/06/2015'
__version__ = '$Revision: 1.0 $'
__all__ = ('StrippingChargedHyperonsConf'
           ,'default_config')


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StandardParticles import StdLoosePions, StdNoPIDsDownPions, StdLooseKaons, StdLooseDownKaons
from StandardParticles import  StdAllLoosePions, StdAllLooseKaons

from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from PhysSelPython.Wrappers import MultiSelectionSequence
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import MeV, mm, picosecond


default_name='ChargedHyperons'
    #### This is the dictionary of all tunable cuts ########
default_config={
      'NAME'        :   'ChargedHyperons',
      'WGs'         :   ['Charm'],
      'BUILDERTYPE' : 'StrippingChargedHyperonsConf',
      'STREAMS'     : ['CharmCompleteEvent'],
      'CONFIG'      : {
                     'TRCHI2DOFMax'           : 3.0
                   , 'PionPIDK'               :  5.0
                   , 'KaonPIDK'               :  -5.0
                   , 'XiMassWindow'           :  25 * MeV
                   , 'OmegaMassWindow'        :  25 * MeV
                   , 'LambdaLLMinDecayTime'   :  5.0 * picosecond
                   , 'LambdaLLVtxChi2Max'     :   5.0
                   , 'LambdaDDVtxChi2Max'     :   5.0
                   , 'LambdaLLMassWin'        : 5.7 * MeV
                   , 'LambdaDDMassWin'        : 5.7 * MeV
                   , 'LambdaLLMinVZ'          : -100. * mm
                   , 'LambdaLLMaxVZ'          :  400. * mm
                   , 'LambdaDDMinVZ'          :  400. * mm
                   , 'LambdaDDMaxVZ'          : 2275. * mm
                   , 'TrGhostProbMax'         :  0.25
                   , 'ProbNNkMin'             :  0.20
                   , 'ProbNNpMinLL'           :  0.20
                   , 'ProbNNpMinDD'           :  0.05
                   , 'Bachelor_PT_MIN'        : 100 * MeV
                   , 'Bachelor_BPVIPCHI2_MIN' : 100.
                   , 'LambdaDeltaZ_MIN'       :  5.0 * mm
                   , 'Hyperon_BPVLTIME_MIN'   :  5.0 * picosecond
                   , 'PostVertexChi2_MIN'     :  5.0
                   , 'LambdaPr_PT_MIN'        : 500. * MeV
                   , 'LambdaPi_PT_MIN'        : 100. * MeV
      } ## end of 'CONFIG' 
}  ## end of default_config
#-------------------------------------------------------------------------------------------------------------
class StrippingChargedHyperonsConf(LineBuilder) :
       __configuration_keys__ = (
                                    "TRCHI2DOFMax"    ##  3.0
                                  , "PionPIDK"        ##  5.0
                                  , "KaonPIDK"        ##  -5.0
                                  , "XiMassWindow"    ##  20 * MeV
                                  , "OmegaMassWindow" ##  20 * MeV
                                  , "LambdaLLMinDecayTime"  ##  5.0  (ps)
                                  , "LambdaLLVtxChi2Max"    ##   5.0
                                  , "LambdaDDVtxChi2Max"    ##   5.0
                                  , "LambdaLLMassWin"       ##   5.7 * MeV
                                  , "LambdaDDMassWin"       ##   5.7 * MeV
                                  , "LambdaLLMinVZ"         ##  -100. (mm)
                                  , "LambdaLLMaxVZ"         ##   500. (mm)
                                  , "LambdaDDMinVZ"         ##   300. (mm)
                                  , "LambdaDDMaxVZ"         ##  2275. (mm)
                                  , "TrGhostProbMax"        ##   0.25
                                  , "ProbNNkMin"            ##   0.20
                                  , "ProbNNpMinLL"          ##   0.20
                                  , "ProbNNpMinDD"          ##   0.05
                                  , "Bachelor_PT_MIN"       ##   100 * MeV
                                  , "Bachelor_BPVIPCHI2_MIN" ## 100.
                                  , "LambdaDeltaZ_MIN"       ##  5.0 * mm
                                  , "Hyperon_BPVLTIME_MIN"   ##  5.0 * picosecond
                                  , "PostVertexChi2_MIN"     ##  5.0
                                  , "LambdaPr_PT_MIN"          ## 500. * MeV
                                  , "LambdaPi_PT_MIN"        ## 100. * MeV
)

       def __init__(self, name, config) :
           LineBuilder.__init__(self, name, config)
           self.name = name
           self.config = config
	

       
           self.LongPionsList = MergedSelection("LongPionsFor" + self.name,
                                                  RequiredSelections =  [DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")]) # take all long tracks
 
           
           self.DownstreamPionsList = MergedSelection("DownstreamPionsFor" + self.name,
                                                  RequiredSelections =  [DataOnDemand(Location = "Phys/StdNoPIDsDownPions/Particles")]) # take all long tracks
 
           self.LongKaonsList = MergedSelection("LongKaonsFor" + self.name,
                                                  RequiredSelections =  [DataOnDemand(Location = "Phys/StdAllLooseKaons/Particles")]) # take all long tracks
 
           
           self.DownstreamKaonsList = MergedSelection("DownstreamKaonsFor" + self.name,
                                                  RequiredSelections =  [DataOnDemand(Location = "Phys/StdLooseDownKaons/Particles")]) # take all long tracks
 
           

           self.GoodLongPionsList = self.createSubSel( OutputList = "GoodLongPionsFor" + self.name,
                                               InputList = self.LongPionsList,
                                               Cuts = "(TRCHI2DOF < %(TRCHI2DOFMax)s )"\
                                               " & (TRGHOSTPROB <%(TrGhostProbMax)s )" \
                                               " & (PIDK < %(PionPIDK)s )" % self.config )


           self.GoodDownstreamPionsList = self.createSubSel( OutputList = "GoodDownstreamPionsFor" + self.name,
                                               InputList = self.DownstreamPionsList,
                                               Cuts = "(TRCHI2DOF < %(TRCHI2DOFMax)s )"\
                                               " & (PIDK < %(PionPIDK)s )" % self.config )

           self.GoodLongKaonsList = self.createSubSel( OutputList = "GoodLongKaonsFor" + self.name,
                                               InputList = self.LongKaonsList,
                                               Cuts = "(TRCHI2DOF < %(TRCHI2DOFMax)s )"\
                                               " & (TRGHOSTPROB < %(TrGhostProbMax)s )" \
                                               " & (PROBNNk > %(ProbNNkMin)s )" \
                                               " & (PIDK > %(KaonPIDK)s )" % self.config )


           self.GoodDownstreamKaonsList = self.createSubSel( OutputList = "GoodDownstreamKaonsFor" + self.name,
                                               InputList = self.DownstreamKaonsList,
                                               Cuts = "(TRCHI2DOF < %(TRCHI2DOFMax)s )"\
                                               " & (PROBNNk > %(ProbNNkMin)s )" \
                                               " & (PIDK > %(KaonPIDK)s )" % self.config )


           
           
           
           self.LambdaListLooseDD = MergedSelection("StdLooseDDLambdaFor" + self.name,
                                                  RequiredSelections =  [DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles")])

           self.LambdaListLooseLL = MergedSelection("StdLooseLLLambdaFor" + self.name,
                                                  RequiredSelections =  [DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")])

           self.LambdaListLL =  self.createSubSel(OutputList = "LambdaLLFor" + self.name,
                                                InputList = self.LambdaListLooseLL ,
                                                Cuts = "(MAXTREE('p+'==ABSID, PT) > %(LambdaPr_PT_MIN)s ) "\
                                                "& (MAXTREE('pi-'==ABSID, PT) > %(LambdaPi_PT_MIN)s ) " \
                                                "& (MAXTREE('p+'==ABSID,PROBNNp) > %(ProbNNpMinLL)s ) "\
                                                "& (MINTREE('pi-'==ABSID, TRGHOSTPROB) < %(TrGhostProbMax)s )"\
                                                "& (MINTREE('p+'==ABSID, TRGHOSTPROB) < %(TrGhostProbMax)s )"\
                                                "& (ADMASS('Lambda0') < %(LambdaLLMassWin)s ) "
                                                "& (VFASPF(VCHI2/VDOF) < %(LambdaLLVtxChi2Max)s ) "\
                                                "& (VFASPF(VZ) > %(LambdaLLMinVZ)s ) " \
                                                "& (VFASPF(VZ) < %(LambdaLLMaxVZ)s ) " \
                                                "& (BPVLTIME() > %(LambdaLLMinDecayTime)s )" % self.config \
                                                )
           
           self.LambdaListDD =  self.createSubSel(OutputList = "LambdaDDFor" + self.name,
                                                InputList = self.LambdaListLooseDD ,
                                                Cuts = "(MAXTREE('p+'==ABSID, PT) > %(LambdaPr_PT_MIN)s ) "\
                                                "& (MAXTREE('pi-'==ABSID, PT) > %(LambdaPi_PT_MIN)s ) " \
                                                "& (MAXTREE('p+'==ABSID, PROBNNp) > %(ProbNNpMinDD)s ) "\
                                                "& (ADMASS('Lambda0') < %(LambdaDDMassWin)s ) " \
                                                "& (VFASPF(VCHI2/VDOF) <  %(LambdaDDVtxChi2Max)s ) "\
                                                "& (VFASPF(VZ) < %(LambdaDDMaxVZ)s ) " \
                                                "& (VFASPF(VZ) > %(LambdaDDMinVZ)s )" % self.config \
                                                )

           self.LambdaList = MergedSelection("LambdaFor" + self.name,
                                             RequiredSelections = [self.LambdaListLL, self.LambdaListDD]
                                             )
           
           self.XiminusList = self.makeXiminus()
           self.OmegaminusList = self.makeOmegaminus()
           
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
                                 PostVertexCuts = "ALL" ) :
           '''create a selection using a ParticleCombiner with a single decay descriptor'''
           combiner = CombineParticles( DecayDescriptor = DecayDescriptor,
                                        DaughtersCuts = DaughterCuts,
                                        MotherCut = PostVertexCuts,
                                        CombinationCut = PreVertexCuts,
                                        ReFitPVs = True)
           return Selection ( OutputList,
                              Algorithm = combiner,
                              RequiredSelections = DaughterLists)
       


#------------------------------------------------------------------------------------------

       def makeXiminus( self ):
            
              ''' Make a Xi minus candidate from long tracks '''
              Ximinus2LambdaPiLLL = self.createCombinationSel(OutputList = "Ximinus2LambdaPiLLL"+ self.name,
                                                           DecayDescriptor = "[Xi- -> Lambda0 pi-]cc",
                                                           DaughterLists   = [self.GoodLongPionsList, self.LambdaListLL],
                                                           DaughterCuts    = {"pi-"      : "(PT> %(Bachelor_PT_MIN)s ) &"\
                                                                             " (BPVIPCHI2() > %(Bachelor_BPVIPCHI2_MIN)s )"% self.config},
                                                           PreVertexCuts   = "(ADAMASS('Xi-') < %(XiMassWindow)s*MeV)"% self.config,
                                                           PostVertexCuts  = "(VFASPF(VCHI2/VDOF)<%(PostVertexChi2_MIN)s ) & "\
                                                                             " (BPVLTIME() > %(Hyperon_BPVLTIME_MIN)s)  & "\
                                                                             "(BPVVDZ>0) & "\
                                                                             "(CHILD(VFASPF(VZ),1)-VFASPF(VZ) > %(LambdaDeltaZ_MIN)s)"% self.config
                                                           )
              
            
              ''' Make a Xi minus candidate  from a LambdaDD and along pion'''
              Ximinus2LambdaPiDDL = self.createCombinationSel(OutputList = "Ximinus2LambdaPiDDL"+ self.name,
                                                           DecayDescriptor = "[Xi- -> Lambda0 pi-]cc",
                                                           DaughterLists   = [self.GoodLongPionsList, self.LambdaListDD],
                                                           DaughterCuts    = {"pi-"      : "(PT> %(Bachelor_PT_MIN)s ) &"\
                                                                             " (BPVIPCHI2() > %(Bachelor_BPVIPCHI2_MIN)s )"% self.config},
                                                           PreVertexCuts   = "(ADAMASS('Xi-') < %(XiMassWindow)s*MeV)"% self.config,
                                                           PostVertexCuts  = "(VFASPF(VCHI2/VDOF)< %(PostVertexChi2_MIN)s) &"\
                                                                             "(BPVLTIME() > %(Hyperon_BPVLTIME_MIN)s) & "\
                                                                             "(BPVVDZ>0) & "\
                                                                             "(CHILD(VFASPF(VZ),1)-VFASPF(VZ) > %(LambdaDeltaZ_MIN)s )"% self.config
                                                           ) 
            
              ''' Make a Xi minus candidate  from downstream tracks'''
              Ximinus2LambdaPiDDD = self.createCombinationSel(OutputList = "Ximinus2LambdaPiDDD"+ self.name,
                                                           DecayDescriptor = "[Xi- -> Lambda0 pi-]cc",
                                                           DaughterLists   = [self.GoodDownstreamPionsList, self.LambdaListDD],
                                                           DaughterCuts    = {"pi-"      : "(PT> %(Bachelor_PT_MIN)s ) &"\
                                                                             " (BPVIPCHI2() > %(Bachelor_BPVIPCHI2_MIN)s )"% self.config},
                                                           PreVertexCuts   = "(ADAMASS('Xi-') < %(XiMassWindow)s*MeV)"% self.config,
                                                           PostVertexCuts  = "(VFASPF(VCHI2/VDOF)< %(PostVertexChi2_MIN)s ) &"\
                                                                             "(BPVLTIME() >  %(Hyperon_BPVLTIME_MIN)s ) & "\
                                                                             "(BPVVDZ>0) & "\
                                                                             "(CHILD(VFASPF(VZ),1)-VFASPF(VZ) > %(LambdaDeltaZ_MIN)s )"% self.config

                                                           )
## Ximinus2LambdaPi is a "Selection" object; MergedSelection passes everything which gets to it
## even when the output list is empty
              Ximinus2LambdaPi = MergedSelection("Ximinus2LambdaPi"+self.name,
                                                 RequiredSelections = [Ximinus2LambdaPiLLL,Ximinus2LambdaPiDDL,Ximinus2LambdaPiDDD] )

## NullFilter is a "FilterDesktop" object which is a type of "Algorithm"
## This one will pass all candidates
              NullFilter= FilterDesktop(Code = "ALL")

## Ximinus2Lambda2PiSelection is *also* a Selection, but it is "more selective" 
## than  Ximinus2LambdaPi in the sense that it passes only events when something
## is in the output list
              Ximinus2LambdaPiSelection = Selection( "Ximinus2LambdaPiSelection"+self.name,
                                                    Algorithm = NullFilter,
                                                    RequiredSelections = [Ximinus2LambdaPi]) 
              Ximinus2LambdaPiLine = StrippingLine (self.name + "Ximinus2LambdaPi",
                                                         algos = [Ximinus2LambdaPiSelection])
              self.registerLine (Ximinus2LambdaPiLine)

##  --------------------  end of makeXiminus  ------------
#------------------------------------------------------------------------------------------

       def makeOmegaminus( self ):
            
              ''' Make an Omega minus candidate '''
              Omegaminus2LambdaKLLL = self.createCombinationSel(OutputList = "Omegaminus2LambdaKLLL"+ self.name,
                                                           DecayDescriptor = "[Omega- -> Lambda0 K-]cc",
                                                           DaughterLists   = [self.GoodLongKaonsList, self.LambdaListLL],
                                                           DaughterCuts    = {"K-"      : "(PT> %(Bachelor_PT_MIN)s ) &"\
                                                                             " (BPVIPCHI2() > %(Bachelor_BPVIPCHI2_MIN)s )"% self.config},
                                                           PreVertexCuts   = "(ADAMASS('Omega-') < %(OmegaMassWindow)s*MeV)"% self.config,
                                                           PostVertexCuts  = "(VFASPF(VCHI2/VDOF)< %(PostVertexChi2_MIN)s) &"\
                                                                             "(BPVLTIME() > %(Hyperon_BPVLTIME_MIN)s) & "\
                                                                             "(BPVVDZ>0) & "\
                                                                             "(CHILD(VFASPF(VZ),1)-VFASPF(VZ) > %(LambdaDeltaZ_MIN)s )"% self.config
                                                           )
             
            
              ''' Make an Omega minus candidate '''
              Omegaminus2LambdaKDDL = self.createCombinationSel(OutputList = "Omegaminus2LambdaKDDL"+ self.name,
                                                           DecayDescriptor = "[Omega- -> Lambda0 K-]cc",
                                                           DaughterLists   = [self.GoodLongKaonsList, self.LambdaListDD],
                                                           DaughterCuts    = {"K-"      : "(PT> %(Bachelor_PT_MIN)s ) &"\
                                                                             " (BPVIPCHI2() > %(Bachelor_BPVIPCHI2_MIN)s )"% self.config},
                                                           PreVertexCuts   = "(ADAMASS('Omega-') < %(OmegaMassWindow)s*MeV)"% self.config,
                                                           PostVertexCuts  = "(VFASPF(VCHI2/VDOF)< %(PostVertexChi2_MIN)s) &"\
                                                                             "(BPVLTIME() > %(Hyperon_BPVLTIME_MIN)s) & "\
                                                                             "(BPVVDZ>0) & "\
                                                                             "(CHILD(VFASPF(VZ),1)-VFASPF(VZ) > %(LambdaDeltaZ_MIN)s )"% self.config
                                                           )
             
              ''' Make an Omega minus candidate '''
              Omegaminus2LambdaKDDD = self.createCombinationSel(OutputList = "Omegaminus2LambdaKDDD"+ self.name,
                                                           DecayDescriptor = "[Omega- -> Lambda0 K-]cc",
                                                           DaughterLists   = [self.GoodDownstreamKaonsList, self.LambdaListDD],
                                                           DaughterCuts    = {"K-"      : "(PT> %(Bachelor_PT_MIN)s ) &"\
                                                                             " (BPVIPCHI2() > %(Bachelor_BPVIPCHI2_MIN)s )"% self.config},
                                                           PreVertexCuts   = "(ADAMASS('Omega-') < %(OmegaMassWindow)s*MeV)"% self.config,
                                                           PostVertexCuts  = "(VFASPF(VCHI2/VDOF)< %(PostVertexChi2_MIN)s) &"\
                                                                             "(BPVLTIME() > %(Hyperon_BPVLTIME_MIN)s) & "\
                                                                             "(BPVVDZ>0) & "\
                                                                             "(CHILD(VFASPF(VZ),1)-VFASPF(VZ) > %(LambdaDeltaZ_MIN)s )"% self.config
                                                           )


## Omegaminus2LambdaK is a "Selection" object; MergedSelection passes everything which gets to it
## even when the output list is empty


              Omegaminus2LambdaK = MergedSelection("Omegaminus2LambdaK"+self.name,
                                                 RequiredSelections = [Omegaminus2LambdaKLLL,Omegaminus2LambdaKDDL,Omegaminus2LambdaKDDD] )
## NullFilter is a "FilterDesktop" object which is a type of "Algorithm"
## This one will pass all candidates
              NullFilter= FilterDesktop(Code = "ALL")

## Omegaminus2Lambda2PiSelection is *also* a Selection, but it is "more selective" 
## than  Omegaminus2LambdaK in the sense that it passes only events when something
## is in the output list
              Omegaminus2LambdaKSelection = Selection( "Omegaminus2LambdaKSelection"+self.name,
                                                    Algorithm = NullFilter,
                                                    RequiredSelections = [Omegaminus2LambdaK])
              Omegaminus2LambdaKLine = StrippingLine (self.name + "Omegaminus2LambdaK",
                                                         algos = [Omegaminus2LambdaKSelection])
              self.registerLine (Omegaminus2LambdaKLine)

##  --------------------  end of makeOmegaminus  ------------

