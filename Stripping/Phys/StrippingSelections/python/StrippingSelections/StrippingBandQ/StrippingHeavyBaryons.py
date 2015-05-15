'''
Stripping line for HeavyBaryons studies : Xi_b-, Xi_b0 and Omega_b-
Author: Yasmine Amhis
'''
__author__ = ['Yasmine Amhis']
__date__ = '19/05/2011'
__version__ = '$Revision: 1.0 $'
__all__ = ('HeavyBaryonsConf',
           'default_config')

default_config = {
       'NAME' : 'HeavyBaryon',
       'BUILDERTYPE'  : 'HeavyBaryonsConf',
       'CONFIG'       : {    'TRCHI2DOF'             :       4.,
                             'PionPIDK'              :       5.,
                             'JpsiMassWindow'        :      80.,
                             'KaonPIDK'              :      -5.,
                             'DLSForLongLived'       :       5.,
                             'XiMassWindow'          :      30.,
                             'OmegaMassWindow'       :      30.,
                             "XibminusMassWindow"    :     300.,
                             "XibzeroMassWindow"     :     500.,
                             "OmegabminusMassWindow" :     500.
                             },
       'STREAMS' : [ 'Dimuon' ],
       'WGs'    : ['BandQ']
       }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StandardParticles import StdLoosePions, StdNoPIDsDownPions, StdLooseKaons, StdLooseDownKaons
from StandardParticles import  StdAllLoosePions, StdAllLooseKaons

from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import MeV


#-------------------------------------------------------------------------------------------------------------
class HeavyBaryonsConf(LineBuilder) :
       __configuration_keys__ = ( 
                                    "TRCHI2DOF"
                                  , "PionPIDK"
                                  , "JpsiMassWindow"
                                  , "KaonPIDK"
                                  , "DLSForLongLived"
                                  , "XiMassWindow"
                                  , "OmegaMassWindow"
                                  , "XibminusMassWindow" 
                                  , "XibzeroMassWindow"
                                  , "OmegabminusMassWindow"
                                   
                              )
       def __init__(self, name, config) :
           LineBuilder.__init__(self, name, config)
           self.name = name
           self.config = config
	
           # define input daughter lists for various selections
           self.WideJpsiList = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu/Particles")
           self.JpsiList = self.createSubSel( OutputList = 'MassConstrainedJpsiForHeavyBaryons' + self.name,
                                              InputList = self.WideJpsiList,
                                              Cuts = "(PFUNA(ADAMASS('J/psi(1S)')) < %(JpsiMassWindow)s)" % self.config)

           #make a merged list for pions 
           self.MergedPionsList = MergedSelection( "MergedPionsFor" + self.name,
                                                   #RequiredSelections =  [DataOnDemand(Location = "Phys/StdLoosePions/Particles"),
                                                   #                       DataOnDemand(Location = "Phys/StdNoPIDsDownPions/Particles")])
                                                   RequiredSelections =  [DataOnDemand(Location = "Phys/StdAllLoosePions/Particles"), # take all long tracks
                                                                          DataOnDemand(Location = "Phys/StdNoPIDsDownPions/Particles")]) # take all downstream tracks
           
        
           
           self.MergedKaonsList = MergedSelection( "MergedKaonsFor" + self.name,
                                                   #RequiredSelections =  [DataOnDemand(Location = "Phys/StdLooseKaons/Particles"),
                                                   #                       DataOnDemand(Location = "Phys/StdLooseDownKaons/Particles")])
                                                   RequiredSelections =  [DataOnDemand(Location = "Phys/StdAllLooseKaons/Particles"),# take all long tracks
                                                                          DataOnDemand(Location = "Phys/StdLooseDownKaons/Particles")])# take all downstream tracks
           #make a merged list for kaons
           self.PionsList = self.createSubSel( OutputList = "PionsFor" + self.name,
                                               InputList = self.MergedPionsList,
                                               Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s ) & (PIDK < %(PionPIDK)s)" % self.config )
           self.KaonsList = self.createSubSel( OutputList = "KaonsFor" + self.name,
                                               InputList = self.MergedKaonsList,
                                               Cuts = "(TRCHI2DOF < %(TRCHI2DOF)s ) & (PIDK > %(KaonPIDK)s)" % self.config )
           
           
           
           self.LambdaListLoose = MergedSelection("StdLooseLambdaMergedFor" + self.name,
                                                  RequiredSelections =  [DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles"),
                                                                         DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")])
           self.LambdaList =  self.createSubSel(OutputList = "LambdaFor" + self.name,
                                                InputList = self.LambdaListLoose ,
                                                Cuts = "(MAXTREE('p+'==ABSID, PT) > 500.*MeV) "\
                                                "& (MAXTREE('pi-'==ABSID, PT) > 100.*MeV) " \
                                                "& (ADMASS('Lambda0') < 15.*MeV) & (VFASPF(VCHI2) < 20 )  & (BPVDLS> %(DLSForLongLived)s ) " % self.config)
           
           
           
           self.XiminusList = self.makeXiminus()
           self.OmegaminusList = self.makeOmegaminus()
           self.makeXibminus2JpsiXi()
           self.makeXibzero2JpsiXistar    ()
           self.makeOmegabminus2JpsiOmega()
           
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
            
              ''' Make a Xi minus candidate '''
              Ximinus2LambdaPi = self.createCombinationSel(OutputList = "Ximinus2LambdaPi"+ self.name,
                                                           DecayDescriptor = "[Xi- -> Lambda0 pi-]cc",
                                                           DaughterLists   = [self.PionsList, self.LambdaList],
                                                           DaughterCuts    = {"pi-"      : "(PT>0.1*GeV) & (BPVIPCHI2()>9)"},
                                                           PreVertexCuts   = "(ADAMASS('Xi-') < %(XiMassWindow)s*MeV)"% self.config,
                                                           PostVertexCuts  = "(VFASPF(VCHI2/VDOF)<25) &(BPVDLS > %(DLSForLongLived)s) " %self.config)
              return Ximinus2LambdaPi


       def makeOmegaminus( self ): 
               ''' Make an Omega minus candidate '''
               Omegaminus2LambdaK = self.createCombinationSel(OutputList = "Omegaminus2LambdaK"+ self.name,
                                                              DecayDescriptor = "[Omega- -> Lambda0 K-]cc",
                                                              DaughterLists = [self.KaonsList, self.LambdaList],
                                                              DaughterCuts = {"K-"      : "(PT>0.1*GeV) & (BPVIPCHI2()>9)"},
                                                              PreVertexCuts = "(ADAMASS('Omega-') < %(OmegaMassWindow)s*MeV)"%self.config,
                                                              PostVertexCuts = "(VFASPF(VCHI2/VDOF)<25) & (BPVDLS> %(DLSForLongLived)s) " %self.config)
               
             
               return Omegaminus2LambdaK	       


       def makeXibminus2JpsiXi( self ):
             
               Xibminus2JpsiXi = self.createCombinationSel (OutputList = "Xibminus2JpsiXi" + self.name,
                                                            DecayDescriptor = "[Xi_b- -> Xi- J/psi(1S)]cc",
                                                            DaughterLists = [self.JpsiList , self.XiminusList ],
                                                            PreVertexCuts = "(ADAMASS('Xi_b-') <600*MeV )",
                                                            PostVertexCuts = "(VFASPF(VCHI2/VDOF)<25)  &  (ADMASS('Xi_b-') < %(XibminusMassWindow)s *MeV) "  % self.config)
               Xibminus2JpsiXiLine = StrippingLine (self.name + "Xib2JpsiXi",
                                                    algos = [Xibminus2JpsiXi])
               self.registerLine (Xibminus2JpsiXiLine)


       def makeXibzero2JpsiXistar( self ) :
              Xibzero2JpsiXistar = self.createCombinationSel(OutputList = "Xibzero2JpsiXistar" + self.name,
                                                             DecayDescriptor = "[Xi_b0 -> Xi- pi+ J/psi(1S)]cc",
                                                             DaughterLists = [self.JpsiList, self.XiminusList, self.PionsList],
                                                             DaughterCuts = {"pi+"      : "(PT>0.1*GeV) & (BPVIPCHI2()>9)"},
                                                             PreVertexCuts = "(ADAMASS('Xi_b0') <600*MeV )" %self.config,
                                                             PostVertexCuts = "(VFASPF(VCHI2/VDOF)<25)  &  (ADMASS('Xi_b0') < %(XibzeroMassWindow)s *MeV) "  % self.config)
              Xibzero2JpsiXistarLine = StrippingLine (self.name + "Xibzero2JpsiXistar",
                                                      algos = [Xibzero2JpsiXistar])
              self.registerLine (Xibzero2JpsiXistarLine)
              

       def makeOmegabminus2JpsiOmega( self ):
              
              Omegabminus2JpsiOmega = self.createCombinationSel (OutputList = "Omegabminus2JpsiOmega" + self.name,
                                                              DecayDescriptor = "[Omega_b- -> Omega- J/psi(1S)]cc",
                                                              DaughterLists = [self.JpsiList , self.OmegaminusList ],
                                                              PreVertexCuts = "(ADAMASS('Omega_b-') <600*MeV )",
                                                              PostVertexCuts = "(VFASPF(VCHI2/VDOF)<25)  &  (ADMASS('Omega_b-') <%(OmegabminusMassWindow)s *MeV)"  % self.config)
              Omegabminus2JpsiOmegaLine = StrippingLine (self.name + "Omegab2JpsiOmega",
                                                         algos = [Omegabminus2JpsiOmega])
              self.registerLine (Omegabminus2JpsiOmegaLine)
              
