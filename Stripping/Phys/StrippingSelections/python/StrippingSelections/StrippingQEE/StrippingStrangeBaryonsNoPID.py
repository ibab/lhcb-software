'''
Stripping line for Strange Baryon  studies 
Author: MF

The Xi, Omega strange baryons are selected from the decays
Xi -> Lambda pi,   Omega -> Lambda K

LLL
'''

__author__  = ['Florin Maciuc']
__date__    = '13/01/2014'
__version__ = '$Revision: 0.0 $'
__all__     = ( 'StrippingStrangeBaryonsNoPIDConf', 'default_config' )

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import MeV


default_config = {
    'NAME'        : 'StrangeBaryonsNoPID',
    'BUILDERTYPE' : 'StrippingStrangeBaryonsNoPIDConf',
    'STREAMS'     : [ 'EW'  ],
    'WGs'         : [ 'QEE' ],
    'CONFIG'      :  { #PID cuts
        'PreScale'    : 1,
        'checkPV'     : False,
        'HLT1'        : "HLT_PASS_RE('Hlt1.*NoBias.*Decision')|HLT_PASS_RE('Hlt1.*MB.*Bias.*Decision')|HLT_PASS_RE('Hlt1.*MicroBias.*Decision')",
        'RawEvents'   : [ "Muon", "Calo", "Rich" ],
        'ProtonPIDppi':       -5.,  #(PIDp-PIDpi) > -5                   
        'PionPIDpiK'  :       0.,   #(PIDp-PIDK) > 0
       
        # Lambda Decay
        'TRCHI2DOF'               :       4., # < 4 for all 6 cases
        'minCHI2IPPV_pPi_LL'      :      20., # > 20
        'minCHI2IPPV_pPi'         :       4., # > 4 for the rest of 4 cases
        'CHI2VTX_L'               :      15., # < 15 for all 6 cases
        'L_FDCHI2_OWNPV_LL'       :     150., # > 150
        'L_FDCHI2_OWNPV'          :     100., # > 100 for the other 5 cases
        'L_FDCHI2_OWNPV_LL_Omega' :     70., # > 150
        'L_FDCHI2_OWNPV_Omega'    :     70., # > 100 for the other 5 cases
        'minCHI2IPPV_L_LL'        :       9., # > 9
        'minCHI2IPPV_L'           :       2., # > 2 for the other 5 cases
        'Lambda0MassWindow'       :       30., # < 6 for all six cases
        'Lambda0MassWindowPost'   :   6., # < 6 for all six cases

        #Bachelor cuts
        'minCHI2IPPV_Pi_Bachelor_LLL' :      10., # > 10
        'minCHI2IPPV_Pi_Bachelor_DDD' :       4., # > 4
        'minCHI2IPPV_Bachelor'        :       3., # > 3 for the other 3 cases: Tight DDL & DDD + Loose DDL
        'minCHI2IPPV_K_Bachelor_D'    :      3., # > 6
        'minCHI2IPPV_K_Bachelor_L'    :      3., # should have been 10 but use it for DDL case too , reduced to 3
       
        # Xi Decay
        'CHI2VTX_Xi'          :      25., # < 25 for all 6 cases
        'Xi_FDCHI2_OWNPV_LLL' :      30., # > 30
        'Xi_FDCHI2_OWNPV_DDL' :      15., # > 15
        'Xi_FDCHI2_OWNPV'     :       5., # > 5 for the other 4 cases
        'COS_L_Xi'            :  0.9996, # > 0.9996 for all 6 cases
        'XiMassWindow'        :      50., #?????????

        # Omega Decay
        'CHI2VTX_Omega'       :      9., # < 25 for all 6 cases
        'Omega_FDCHI2_OWNPV'  :      10., # > 5 for the other 4 cases
        'COS_L_Omega'         :      0.9996, # > 0.9996 for all 6 cases
        'OmegaMassWindow'     :      50., #?????????
    },
}
    



#-------------------------------------------------------------------------------------------------------------
class StrippingStrangeBaryonsNoPIDConf(LineBuilder) :

       __configuration_keys__ = default_config['CONFIG'].keys()
       
       def __init__(self, name, config) :
           LineBuilder.__init__(self, name, config)
           self.name = name
           self.config = config
       

           #### LLL case ####
           PionsForLambdaLList = createSubSel( OutputList = "PionsForLambda" + self.name,
                                               InputList = DataOnDemand("Phys/StdAllNoPIDsPions/Particles"),
                                               Cuts = "(ISLONG) & (TRCHI2DOF < %(TRCHI2DOF)s ) & "\
                                               "(BPVIPCHI2() > %(minCHI2IPPV_pPi_LL)s)" % self.config )
           ProtonsForLambdaLList = createSubSel( OutputList = "ProtonsForLambdaNoPID" + self.name,
                                                 InputList = DataOnDemand("Phys/StdAllNoPIDsProtons/Particles"),
                                                 Cuts = "(ISLONG) & (TRCHI2DOF < %(TRCHI2DOF)s ) & "\
                                                 "(BPVIPCHI2() > %(minCHI2IPPV_L_LL)s)" % self.config )
           PionsForXiLList = createSubSel( OutputList = "PionsForXi" + self.name,
                                           InputList = DataOnDemand("Phys/StdAllNoPIDsPions/Particles"),
                                           Cuts = "(ISLONG) & (TRCHI2DOF < %(TRCHI2DOF)s ) & "\
                                           "(BPVIPCHI2() > %(minCHI2IPPV_Pi_Bachelor_LLL)s)" % self.config )
           KaonsForOmegaLList = createSubSel( OutputList = "KaonsForOmega" + self.name,
                                              InputList = DataOnDemand("Phys/StdAllNoPIDsKaons/Particles"),
                                              Cuts = "(ISLONG) & (TRCHI2DOF < %(TRCHI2DOF)s ) & "\
                                              "(BPVIPCHI2() > %(minCHI2IPPV_K_Bachelor_L)s)" % self.config )
           
           
           #Create Lambdas
           Lambda2pPiL = createCombinationSel(OutputList = "Lambda2pPiL"+ self.name,
                                              DecayDescriptor = "[Lambda0 -> p+ pi-]cc",
                                              DaughterLists   = [PionsForLambdaLList, ProtonsForLambdaLList],
                                              DaughterCuts    = {},
                                              PreVertexCuts   = "(ADAMASS('Lambda0') < %(Lambda0MassWindow)s*MeV)"% self.config,
  
                                              PostVertexCuts  = "(BPVIPCHI2() > %(minCHI2IPPV_L_LL)s) & (VFASPF(VCHI2) < %(CHI2VTX_L)s) &"\
                                              "(BPVVDCHI2 > %(L_FDCHI2_OWNPV_LL)s) & (ADMASS('Lambda0') < %(Lambda0MassWindowPost)s*MeV)" %self.config
                                             )


           Lambda2pPiLOmega = createCombinationSel(OutputList = "Lambda2pPiLOmega"+ self.name,
                                              DecayDescriptor = "[Lambda0 -> p+ pi-]cc",
                                              DaughterLists   = [PionsForLambdaLList, ProtonsForLambdaLList],
                                              DaughterCuts    = {},
                                              PreVertexCuts   = "(ADAMASS('Lambda0') < %(Lambda0MassWindow)s*MeV)"% self.config,
                                              PostVertexCuts  = "(BPVIPCHI2() > %(minCHI2IPPV_L_LL)s) & (VFASPF(VCHI2) < %(CHI2VTX_L)s) &"\
                                              "(BPVVDCHI2 > %(L_FDCHI2_OWNPV_LL_Omega)s) & (ADMASS('Lambda0') < %(Lambda0MassWindowPost)s*MeV)" %self.config
                                              )
           
           
           
           self.makeXiminus("XiminusLLLNoPID",  [Lambda2pPiL, PionsForXiLList], 'Xi_FDCHI2_OWNPV_LLL')

           self.makeOmegaminus("OmegaminusLLLNoPID",  [Lambda2pPiLOmega, KaonsForOmegaLList], 'Omega_FDCHI2_OWNPV')

           

#------------------------------------------------------------------------------------------

       def makeXiminus(self, OutputList, DaughterLists, FDCHI2 ):            
              ''' Make a Xi minus candidate '''
              myPostVertexCuts  = "(VFASPF(VCHI2)< %%(CHI2VTX_Xi)s) & (BPVVDCHI2 > %%(%s)s) & "\
                                                      "((CHILD(PX,1)*CHILD(PX,0)+CHILD(PY,1)*CHILD(PY,0)+CHILD(PZ,1)*CHILD(PZ,0))/(CHILD(P,1)*CHILD(P,0)) > %%(COS_L_Xi)s)" % (FDCHI2)
              myPostVertexCuts += " & (BPVIPCHI2()<1000)" # new cut
              
              Ximinus2LambdaPi = createCombinationSel(OutputList = OutputList,
                                                      DecayDescriptor = "[Xi- -> Lambda0 pi-]cc",
                                                      DaughterLists   = DaughterLists,
                                                      PreVertexCuts   = "(ADAMASS('Xi-') < %(XiMassWindow)s*MeV)"% self.config,
                                                      PostVertexCuts = myPostVertexCuts % self.config
                                                      )
              
              Ximinus2LambdaPiLine = StrippingLine(OutputList+self.name, 
                  prescale  = self.config['PreScale'], 
                  HLT1      = self.config['HLT1'], 
                  algos     = [Ximinus2LambdaPi],
                  RequiredRawEvents = self.config['RawEvents'],
              )
              self.registerLine (Ximinus2LambdaPiLine)



              
       def makeOmegaminus(self, OutputList, DaughterLists, FDCHI2 ): 
              ''' Make an Omega minus candidate '''
              myPostVertexCuts = "(VFASPF(VCHI2)< %%(CHI2VTX_Omega)s) & (BPVVDCHI2 > %%(%s)s) & "\
                                                        "((CHILD(PX,1)*CHILD(PX,0)+CHILD(PY,1)*CHILD(PY,0)+CHILD(PZ,1)*CHILD(PZ,0))/(CHILD(P,1)*CHILD(P,0)) > %%(COS_L_Xi)s)" % (FDCHI2)
              myPostVertexCuts += " & (BPVIPCHI2()<1000)" # new cut 
              Omegaminus2LambdaK = createCombinationSel(OutputList = OutputList,
                                                        DecayDescriptor = "[Omega- -> Lambda0 K-]cc",
                                                        DaughterLists = DaughterLists,
                                                        PreVertexCuts = "(ADAMASS('Omega-') < %(OmegaMassWindow)s*MeV)" % self.config,
                                                        PostVertexCuts = myPostVertexCuts % self.config
                                                        )
              Omegaminus2LambdaKLine = StrippingLine(OutputList+self.name, 
                  prescale  = self.config['PreScale'], 
                  HLT1      = self.config['HLT1'], 
                  algos     = [Omegaminus2LambdaK],
                  RequiredRawEvents = self.config['RawEvents'],
              )
              self.registerLine (Omegaminus2LambdaKLine)             


              


def createSubSel(OutputList, InputList, Cuts ) :
       '''create a selection using a FilterDesktop'''
       filter = FilterDesktop(Code = Cuts)
       return Selection( OutputList,
                         Algorithm = filter,
                         RequiredSelections = [ InputList ] )


def createCombinationSel( OutputList,
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
                                    ReFitPVs = False)
       return Selection ( OutputList,
                          Algorithm = combiner,
                          RequiredSelections = DaughterLists)
       
