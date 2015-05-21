'''
Stripping line for Strange Baryon  studies 
Author: MS&IB&BP

The Xi, Omega strange baryons are selected from the decays
Xi -> Lambda pi,   Omega -> Lambda K

LLL DDL DDD
'''

__author__ = ['Mihai Straticiuc', 'Florin Maciuc', 'Nguyen Thi Dung']
__date__ = '26/06/2012'
__version__ = '$Revision: 0.3 $'

__all__ = ('StrippingStrangeBaryonsConf',
           'createSubSel',
           'createCombinationSel',                      
           'default_config')



from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StandardParticles import StdLoosePions, StdNoPIDsDownPions, StdLooseKaons, StdLooseDownKaons, StdLooseProtons, StdNoPIDsDownProtons
from CommonParticles import StdLooseLambda
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import MeV

default_config = {
      'NAME'        : 'StrangeBaryons',
      'BUILDERTYPE' : 'StrippingStrangeBaryonsConf',
      'STREAMS'     : [ 'EW'  ],
      'WGs'         : [ 'QEE' ],
      'CONFIG'      : { #PID cuts
                        'PreScale'   : 1,
                        'checkPV'   : True,
                        'HLT' : "HLT_PASS('Hlt1MBNoBiasDecision')|HLT_PASS('Hlt1MBMicroBiasTStationDecision')|HLT_PASS('Hlt1MBMicroBiasVeloDecision')|HLT_PASS('Hlt1MBMicroBiasTStationRateLimitedDecision')|HLT_PASS('Hlt1MBMicroBiasVeloRateLimitedDecision')",
                        'ProtonPIDppi'              :       -5.,  #(PIDp-PIDpi) > -5                   
                        'PionPIDpiK'              :       0.,   #(PIDp-PIDK) > 0
                   
                        # Lambda Decay
                        'TRCHI2DOF'              :       4., # < 4 for all 6 cases
                        'minCHI2IPPV_pPi_LL'    :      20., # > 20
                        'minCHI2IPPV_pPi'              :       4., # > 4 for the rest of 4 cases
                        'CHI2VTX_L'                   :      15., # < 15 for all 6 cases
                        'L_FDCHI2_OWNPV_LL'    :     150., # > 150
                        'L_FDCHI2_OWNPV'              :     100., # > 100 for the other 5 cases

                        'L_FDCHI2_OWNPV_LL_Omega'    :     70., # > 150
                        'L_FDCHI2_OWNPV_Omega'              :     70., # > 100 for the other 5 cases
       
                        'minCHI2IPPV_L_LL'     :       9., # > 9
                        'minCHI2IPPV_L'               :       2., # > 2 for the other 5 cases
                        'Lambda0MassWindow'            :       30., # < 6 for all six cases
                        'Lambda0MassWindowPost'            :    6., # < 6 for all six cases

                   
                        #Bachelor cuts

                        'minCHI2IPPV_Pi_Bachelor_LLL' :      10., # > 10
                        'minCHI2IPPV_Pi_Bachelor_DDD' :       4., # > 4
                        'minCHI2IPPV_Bachelor'     :       3., # > 3 for the other 3 cases: Tight DDL & DDD + Loose DDL
                        'minCHI2IPPV_K_Bachelor_D' :      3., # > 6
                        'minCHI2IPPV_K_Bachelor_L' :      3., # should have been 10 but use it for DDL case too , reduced to 3
                   
                        # Xi Decay
                        'CHI2VTX_Xi'                  :      25., # < 25 for all 6 cases
                        'Xi_FDCHI2_OWNPV_LLL'   :      30., # > 30
                        'Xi_FDCHI2_OWNPV_DDL'   :      15., # > 15
                        'Xi_FDCHI2_OWNPV'             :       5., # > 5 for the other 4 cases
                        'COS_L_Xi'                    :  0.9996, # > 0.9996 for all 6 cases
                        'XiMassWindow'              :      50., #?????????


                        # Omega Decay
                        'CHI2VTX_Omega'                  :      9., # < 25 for all 6 cases
                        'Omega_FDCHI2_OWNPV'             :      10., # > 5 for the other 4 cases
                        'COS_L_Omega'                    :      0.9996, # > 0.9996 for all 6 cases
                        'OmegaMassWindow'                :      50., #?????????
                 
                      },
}




#-------------------------------------------------------------------------------------------------------------
class StrippingStrangeBaryonsConf(LineBuilder) :
       __configuration_keys__ = default_config['CONFIG'].keys()
       
       def __init__(self, name, config) :
           LineBuilder.__init__(self, name, config)
           self.name = name
           self.config = config
       

           #### LLL case ####
           PionsForLambdaLList = createSubSel( OutputList = "PionsForLambda" + self.name,
                                               InputList = DataOnDemand("Phys/StdAllLoosePions/Particles"),
                                               Cuts = "(ISLONG) & (TRCHI2DOF < %(TRCHI2DOF)s ) & "\
                                               "(BPVIPCHI2() > %(minCHI2IPPV_pPi_LL)s)" % self.config )
           ProtonsForLambdaLList = createSubSel( OutputList = "ProtonsForLambdaLoose" + self.name,
                                                 InputList = DataOnDemand("Phys/StdAllLooseProtons/Particles"),
                                                 Cuts = "(ISLONG) & (TRCHI2DOF < %(TRCHI2DOF)s ) & "\
                                                 "(BPVIPCHI2() > %(minCHI2IPPV_L_LL)s)" % self.config )
           PionsForXiLList = createSubSel( OutputList = "PionsForXi" + self.name,
                                           InputList = DataOnDemand("Phys/StdAllLoosePions/Particles"),
                                           Cuts = "(ISLONG) & (TRCHI2DOF < %(TRCHI2DOF)s ) & "\
                                           "(BPVIPCHI2() > %(minCHI2IPPV_Pi_Bachelor_LLL)s)" % self.config )
           KaonsForOmegaLList = createSubSel( OutputList = "KaonsForOmega" + self.name,
                                              InputList = DataOnDemand("Phys/StdAllLooseKaons/Particles"),
                                              Cuts = "(ISLONG) & (TRCHI2DOF < %(TRCHI2DOF)s ) & "\
                                              "(BPVIPCHI2() > %(minCHI2IPPV_K_Bachelor_L)s)" % self.config )
           

           
           #### DDL ####
           PionsForLambdaDList = createSubSel( OutputList = "PionsForLambdaD" + self.name,
                                               InputList = DataOnDemand("Phys/StdNoPIDsDownPions/Particles"),
                                               Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s ) & "\
                                               "(BPVIPCHI2() > %(minCHI2IPPV_pPi)s)" % self.config )
           ProtonsForLambdaDList = createSubSel( OutputList = "ProtonsForLambdaD" + self.name,
                                                 InputList = DataOnDemand("Phys/StdNoPIDsDownProtons/Particles"),
                                                 Cuts = "HASRICH & ((PIDp-PIDpi) > %(ProtonPIDppi)s) & "\
                                                 "(TRCHI2DOF < %(TRCHI2DOF)s ) "\
                                                 "& (BPVIPCHI2() > %(minCHI2IPPV_pPi)s)" % self.config )
           PionsForXiDLList = createSubSel( OutputList = "PionsForXiDL" + self.name,
                                            InputList = DataOnDemand("Phys/StdAllLoosePions/Particles"),
                                            Cuts = "(ISLONG) & (TRCHI2DOF < %(TRCHI2DOF)s ) & "\
                                            "(BPVIPCHI2() > %(minCHI2IPPV_Bachelor)s)" % self.config )
           

           
           #### DDD ####de adaugat conditiile pentru protoni
           PionsForXiDDList = createSubSel( OutputList = "PionsForXiDD" + self.name,
                                            InputList = DataOnDemand("Phys/StdNoPIDsDownPions/Particles"),
                                            Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s ) & "\
                                            "(BPVIPCHI2() > %(minCHI2IPPV_Pi_Bachelor_DDD)s)" % self.config )
           KaonsForOmegaDList = createSubSel( OutputList = "KaonsForOmegaD" + self.name,
                                              InputList = DataOnDemand("Phys/StdLooseDownKaons/Particles"),
                                              Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s ) & "\
                                              "(BPVIPCHI2() > %(minCHI2IPPV_K_Bachelor_D)s)" % self.config )
           
           
           
           
           #print "Lambda0MassWindow: %(Lambda0MassWindow)s*MeV " % self.config
      	   #print "Lambda0MassWindowPost: %(Lambda0MassWindowPost)s*MeV" % self.config
           #print "this stripping Line is used, signed Florin"
           
           #Create Lambdas
           Lambda2pPiL = createCombinationSel(OutputList = "Lambda2pPiL"+ self.name,
                                              DecayDescriptor = "[Lambda0 -> p+ pi-]cc",
                                              DaughterLists   = [PionsForLambdaLList, ProtonsForLambdaLList],
                                              DaughterCuts    = {},
                                              PreVertexCuts   = "(ADAMASS('Lambda0') < %(Lambda0MassWindow)s*MeV)"% self.config,
  
                                              PostVertexCuts  = "(BPVIPCHI2() > %(minCHI2IPPV_L_LL)s) & (VFASPF(VCHI2) < %(CHI2VTX_L)s) &"\
                                              "(BPVVDCHI2 > %(L_FDCHI2_OWNPV_LL)s) & (ADMASS('Lambda0') < %(Lambda0MassWindowPost)s*MeV)" %self.config
                                              #PostVertexCuts  = "(BPVIPCHI2() > %(minCHI2IPPV_L_Loose)s) & (VFASPF(VCHI2) < %(CHI2VTX_L)s)" %self.config
                                              )



           Lambda2pPiLOmega = createCombinationSel(OutputList = "Lambda2pPiLOmega"+ self.name,
                                              DecayDescriptor = "[Lambda0 -> p+ pi-]cc",
                                              DaughterLists   = [PionsForLambdaLList, ProtonsForLambdaLList],
                                              DaughterCuts    = {},
                                              PreVertexCuts   = "(ADAMASS('Lambda0') < %(Lambda0MassWindow)s*MeV)"% self.config,
                                              PostVertexCuts  = "(BPVIPCHI2() > %(minCHI2IPPV_L_LL)s) & (VFASPF(VCHI2) < %(CHI2VTX_L)s) &"\
                                              "(BPVVDCHI2 > %(L_FDCHI2_OWNPV_LL_Omega)s) & (ADMASS('Lambda0') < %(Lambda0MassWindowPost)s*MeV)" %self.config
                                              #PostVertexCuts  = "(BPVIPCHI2() > %(minCHI2IPPV_L_Loose)s) & (VFASPF(VCHI2) < %(CHI2VTX_L)s)" %self.config
                                              )
           
           
           
           Lambda2pPiD = createCombinationSel(OutputList = "Lambda2pPiD"+ self.name,
                                              DecayDescriptor = "[Lambda0 -> p+ pi-]cc",
                                              DaughterLists   = [PionsForLambdaDList, ProtonsForLambdaDList],
                                              DaughterCuts    = {},
                                              PreVertexCuts   = "(ADAMASS('Lambda0') < %(Lambda0MassWindow)s*MeV)"% self.config,
                                              PostVertexCuts  = "(BPVIPCHI2() > %(minCHI2IPPV_L)s) & (VFASPF(VCHI2) < %(CHI2VTX_L)s) &"\
                                              "(BPVVDCHI2 > %(L_FDCHI2_OWNPV)s) & (ADMASS('Lambda0') < %(Lambda0MassWindowPost)s*MeV)" %self.config
                                              #PostVertexCuts  = "(BPVIPCHI2() > %(minCHI2IPPV_L)s) & (VFASPF(VCHI2) < %(CHI2VTX_L)s)" %self.config
                                              )


           Lambda2pPiDOmega = createCombinationSel(OutputList = "Lambda2pPiDOmega"+ self.name,
                                              DecayDescriptor = "[Lambda0 -> p+ pi-]cc",
                                              DaughterLists   = [PionsForLambdaDList, ProtonsForLambdaDList],
                                              DaughterCuts    = {},
                                              PreVertexCuts   = "(ADAMASS('Lambda0') < %(Lambda0MassWindow)s*MeV)"% self.config,
                                              PostVertexCuts  = "(BPVIPCHI2() > %(minCHI2IPPV_L)s) & (VFASPF(VCHI2) < %(CHI2VTX_L)s) &"\
                                              "(BPVVDCHI2 > %(L_FDCHI2_OWNPV_Omega)s) & (ADMASS('Lambda0') < %(Lambda0MassWindowPost)s*MeV)" %self.config
                                              #PostVertexCuts  = "(BPVIPCHI2() > %(minCHI2IPPV_L)s) & (VFASPF(VCHI2) < %(CHI2VTX_L)s)" %self.config
                                              )
           
             
           
           self.makeXiminus("XiminusLLL",  [Lambda2pPiL, PionsForXiLList], 'Xi_FDCHI2_OWNPV_LLL')
#           self.makeXiminus("XiminusDDL",  [Lambda2pPiD, PionsForXiDLList], 'Xi_FDCHI2_OWNPV_DDL')
           self.makeXiminus("XiminusDDD",  [Lambda2pPiD, PionsForXiDDList], 'Xi_FDCHI2_OWNPV')
           self.makeXiminus("XiminusDDL",  [Lambda2pPiD, PionsForXiDLList], 'Xi_FDCHI2_OWNPV_DDL')

           
           self.makeOmegaminus("OmegaminusLLL",  [Lambda2pPiLOmega, KaonsForOmegaLList], 'Omega_FDCHI2_OWNPV')
#           self.makeOmegaminus("OmegaminusDDL",  [Lambda2pPiD, KaonsForOmegaLList], 'Omega_FDCHI2_OWNPV')
           self.makeOmegaminus("OmegaminusDDD",  [Lambda2pPiDOmega, KaonsForOmegaDList], 'Omega_FDCHI2_OWNPV')
           self.makeOmegaminus("OmegaminusDDL",  [Lambda2pPiDOmega, KaonsForOmegaLList], 'Omega_FDCHI2_OWNPV')

           

#------------------------------------------------------------------------------------------

       def makeXiminus(self, OutputList, DaughterLists, FDCHI2 ):            
              ''' Make a Xi minus candidate '''
              myPostVertexCuts  = "(VFASPF(VCHI2)< %%(CHI2VTX_Xi)s) & (BPVVDCHI2 > %%(%s)s) & "\
                                                      "((CHILD(PX,1)*CHILD(PX,0)+CHILD(PY,1)*CHILD(PY,0)+CHILD(PZ,1)*CHILD(PZ,0))/(CHILD(P,1)*CHILD(P,0)) > %%(COS_L_Xi)s)" % (FDCHI2)
              #print "Make %s"%OutputList
              #PostVertexCuts = "(VFASPF(VCHI2)< %(CHI2VTX_Xi)s) & (LV01 > %(COS_L_Xi)s)" %self.config
              #PostVertexCuts = "(VFASPF(VCHI2)< %(CHI2VTX_Xi)s) " %self.config
              #PostVertexCuts += "(BPVVDCHI2 > %s)" %self.config[FDCHI2]
              myPostVertexCuts += " & (BPVIPCHI2()<1000)" # new cut
              
              Ximinus2LambdaPi = createCombinationSel(OutputList = OutputList,
                                                      DecayDescriptor = "[Xi- -> Lambda0 pi-]cc",
                                                      DaughterLists   = DaughterLists,
                                                      PreVertexCuts   = "(ADAMASS('Xi-') < %(XiMassWindow)s*MeV)"% self.config,
                                                      PostVertexCuts = myPostVertexCuts % self.config
                                                      #"(LV01 > %(COS_L_Xi)s) " %self.config
                                                      #PostVertexCuts  = "(VFASPF(VCHI2)< %(CHI2VTX_Xi)s) & (BPVVDCHI2 > %(L_FDCHI2_OWNPV)s)" %self.config
                                                      # PostVertexCuts  = "(VFASPF(VCHI2)< %(CHI2VTX_Xi)s)" %self.config
                                                      )
              
              #print "1st HLT = ", "%(HLT)s" %self.config
              Ximinus2LambdaPiLine = StrippingLine (OutputList+self.name, prescale = "%(PreScale)s" %self.config, HLT1 = "%(HLT)s" %self.config , algos = [Ximinus2LambdaPi],RequiredRawEvents = ["Muon","Calo","Rich"])
              self.registerLine (Ximinus2LambdaPiLine)



              
       def makeOmegaminus(self, OutputList, DaughterLists, FDCHI2 ): 
              ''' Make an Omega minus candidate '''
              #print "Make %s"%OutputList
              myPostVertexCuts = "(VFASPF(VCHI2)< %%(CHI2VTX_Omega)s) & (BPVVDCHI2 > %%(%s)s) & "\
                                                        "((CHILD(PX,1)*CHILD(PX,0)+CHILD(PY,1)*CHILD(PY,0)+CHILD(PZ,1)*CHILD(PZ,0))/(CHILD(P,1)*CHILD(P,0)) > %%(COS_L_Xi)s)" % (FDCHI2)
              myPostVertexCuts += " & (BPVIPCHI2()<1000)" # new cut 
              Omegaminus2LambdaK = createCombinationSel(OutputList = OutputList,
                                                        DecayDescriptor = "[Omega- -> Lambda0 K-]cc",
                                                        DaughterLists = DaughterLists,
#                                                        DaughterCuts = {"K-"      : "(PT>0.*GeV)"},
                                                        PreVertexCuts = "(ADAMASS('Omega-') < %(OmegaMassWindow)s*MeV)" % self.config,
                                                        PostVertexCuts = myPostVertexCuts % self.config
                                                        #PostVertexCuts = "(VFASPF(VCHI2/VDOF)<25) & (BPVDLS> %(DLSForLongLived)s) " %self.config
                                                        #PostVertexCuts = "(VFASPF(VCHI2)< %(CHI2VTX_Omega)s) & (BPVVDCHI2 > %(L_FDCHI2_OWNPV)s) & "\
                                                        #"((CHILD(PX,1)*CHILD(PX,0)+CHILD(PY,1)*CHILD(PY,0)+CHILD(PZ,1)*CHILD(PZ,0))/(CHILD(P,1)*CHILD(P,0)) > %(COS_L_Xi)s)" %self.config
                                                        #"(LV01 > %(COS_L_Xi)s) " %self.config
                                                        )
       	      # print "2nd HLT = ", "%(HLT)s" %self.config
              Omegaminus2LambdaKLine = StrippingLine(OutputList+self.name, prescale = "%(PreScale)s" %self.config, HLT1 = "%(HLT)s" %self.config, algos = [Omegaminus2LambdaK],RequiredRawEvents = ["Muon","Calo","Rich"])
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
#       print "here are combination parameters"
#       print DecayDescriptor
#       print DaughterLists
#       print DaughterCuts
#       print PreVertexCuts
#       print PostVertexCuts
       combiner = CombineParticles( DecayDescriptor = DecayDescriptor,
                                    DaughtersCuts = DaughterCuts,
                                    MotherCut = PostVertexCuts,
                                    CombinationCut = PreVertexCuts,
                                    ReFitPVs = False)
       return Selection ( OutputList,
                          Algorithm = combiner,
                          RequiredSelections = DaughterLists)
       
