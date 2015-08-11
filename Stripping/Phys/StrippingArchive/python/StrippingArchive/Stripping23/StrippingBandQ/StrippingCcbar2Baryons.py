'''
Module for selecting Ccbar->LambdaLambda, XiXi

To include it:

from StrippingSelections.StrippingCcbar2Baryons import Ccbar2BaryonsConf
StrippingCcbar2BaryonsConf =  Ccbar2BaryonsConf( name = 'Ccbar2Baryons', config = Ccbar2BaryonsConf.config_default )
stream.appendLines( [
      StrippingCcbar2BaryonsConf.line
])
'''

__author__=['Jibo He']
__date__ = '03/10/2010'
__version__= '$Revision: 1.2 $'


__all__ = (
    'Ccbar2BaryonsConf',
    'makePP'
    )

config_default =  {
        'TRCHI2DOF'        :    5.   ,
        'CombMaxMass'      :  4100.  , # MeV, before Vtx fit
        'CombMinMass'      :  2750.  , # MeV, before Vtx fit
        'MaxMass'          :  4000.  , # MeV, after Vtx fit
        'MinMass'          :  2800.    # MeV, after Vtx fit
        }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdNoPIDsProtons, StdNoPIDsPions, StdNoPIDsDownPions

class Ccbar2BaryonsConf(LineBuilder):
    
    __configuration_keys__ = (
        'TRCHI2DOF',
        'CombMaxMass',
        'CombMinMass',
        'MaxMass',
        'MinMass'
        )

    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        self.name = name 
        self.config = config


        """
        Lambda 
        """
        self.LambdaLLForJpsi = self.createSubSel( OutputList = "LambdaLLFor" + self.name,
                                                  InputList =  DataOnDemand( Location = 'Phys/StdLooseLambdaLL/Particles' ), 
                                                  Cuts = "(VFASPF(VCHI2/VDOF)<16)"\
                                                  " & (ADMASS('Lambda0')<5*MeV)"\
                                                  " & (INTREE( ('pi+'==ABSID) & (TRCHI2DOF < %(TRCHI2DOF)s)))" \
                                                  " & (INTREE( ('p+'==ABSID)  & (TRCHI2DOF < %(TRCHI2DOF)s)))" \
                                                  " & (log(CHILD(MIPDV(PRIMARY),1)*CHILD(MIPDV(PRIMARY),2)/MIPDV(PRIMARY))>2.0)"\
                                                  " & (ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > 20*MeV )" % self.config )
        
        self.LambdaDDForJpsi = self.createSubSel( OutputList = "LambdaDDFor" + self.name,
                                                  InputList =  DataOnDemand( Location = 'Phys/StdLooseLambdaDD/Particles' ), 
                                                  Cuts = "(VFASPF(VCHI2/VDOF)<16)"\
                                                  " & (ADMASS('Lambda0')<5*MeV)"\
                                                  " & (INTREE( ('pi+'==ABSID) & (TRCHI2DOF < %(TRCHI2DOF)s)))" \
                                                  " & (INTREE( ('p+'==ABSID)  & (TRCHI2DOF < %(TRCHI2DOF)s) & (PIDp>5) ))" \
                                                  " & (log(CHILD(MIPDV(PRIMARY),1)*CHILD(MIPDV(PRIMARY),2)/MIPDV(PRIMARY))>2.5)"\
                                                  " & (ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > 20*MeV )" % self.config )
        
        self.LambdaForJpsiList = MergedSelection("MergedLambdaForJpsi" + self.name,
                                                 RequiredSelections =  [ self.LambdaLLForJpsi, 
                                                                         self.LambdaDDForJpsi, 
                                                                         ])
        
        """
        Pion 
        """
        self.PionLongForXi = self.createSubSel( OutputList = "PionLongForXiList" + self.name,
                                                InputList =  DataOnDemand( Location = 'Phys/StdNoPIDsPions/Particles' ), 
                                                Cuts = "(PT>0.1*GeV) & (P>2.*GeV) & (TRCHI2DOF < %(TRCHI2DOF)s )" % self.config
                                                )

        self.PionDownForXi = self.createSubSel( OutputList = "PionDownForXiList" + self.name,
                                                InputList =  DataOnDemand( Location = 'Phys/StdNoPIDsDownPions/Particles' ), 
                                                Cuts = "(PT>0.1*GeV) & (P>2.*GeV) & (TRCHI2DOF < %(TRCHI2DOF)s )" % self.config
                                                )
        
        self.PionForXiList = MergedSelection("MergedPionForXi" + self.name,
                                             RequiredSelections =  [ self.PionLongForXi, 
                                                                     self.PionDownForXi, 
                                                                     ])
        
        """
        Xi
        """
        self.XiList = self.createCombinationSel( OutputList = "XiFor" + self.name,
                                                 DaughterLists = [ self.LambdaForJpsiList, self.PionForXiList ],
                                                 DecayDescriptor = "[Xi- -> Lambda0 pi-]cc",
                                                 PreVertexCuts = "(ADAMASS('Xi-')<50*MeV) & (ADOCACHI2CUT(40, ''))",
                                                 PostVertexCuts = "(VFASPF(VCHI2/VDOF)<16)"\
                                                 " & (ADMASS('Xi-')<20*MeV)"
                                                 )

        
        self.makeJpsi2DiLambda()
        self.makeJpsi2DiXi()

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
    
    
    
    def makeJpsi2DiLambda(self):
        Jpsi2DiLambda = self.createCombinationSel( OutputList = "Jpsi2DiLambda" + self.name,
                                                   DecayDescriptor = " J/psi(1S) -> Lambda0 Lambda~0", 
                                                   DaughterLists = [ self.LambdaForJpsiList ], 
                                                   DaughterCuts  = { "Lambda0": "(PT>0.5*GeV)" },
                                                   PreVertexCuts = "(in_range( %(CombMinMass)s *MeV, AM, %(CombMaxMass)s *MeV))" % self.config,
                                                   PostVertexCuts = "(in_range( %(MinMass)s *MeV, MM, %(MaxMass)s *MeV)) & (VFASPF(VCHI2PDOF) < 16 )" %self.config )
        
        Jpsi2DiLambdaLine = StrippingLine( self.name + "Jpsi2DiLambdaLine",
                                           algos = [ Jpsi2DiLambda ] )
        
        self.registerLine(Jpsi2DiLambdaLine)


    def makeJpsi2DiXi(self):
        Jpsi2DiXi = self.createCombinationSel( OutputList = "Jpsi2DiXi" + self.name,
                                               DecayDescriptor = " J/psi(1S) -> Xi~+  Xi-", 
                                               DaughterLists = [ self.XiList ], 
                                               DaughterCuts  = { "Xi-": "(PT>0.5*GeV)" },
                                               PreVertexCuts = "(in_range( %(CombMinMass)s *MeV, AM, %(CombMaxMass)s *MeV))" % self.config,
                                               PostVertexCuts = "(in_range( %(MinMass)s *MeV, MM, %(MaxMass)s *MeV)) & (VFASPF(VCHI2PDOF) < 16 )" %self.config )
        
        Jpsi2DiXiLine = StrippingLine( self.name + "Jpsi2DiXiLine",
                                           algos = [ Jpsi2DiXi ] )
        
        self.registerLine(Jpsi2DiXiLine)
