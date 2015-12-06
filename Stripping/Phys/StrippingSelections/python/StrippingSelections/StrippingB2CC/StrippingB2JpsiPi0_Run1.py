__author__ = ['Maximilien Chefdeville']
__date__ = '06/12/2015'
__version__ = '$Revision: 1.2$'

__all__ = ('B2JpsiPi0Conf_RI','default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from CommonParticles.Utils import updateDoD
from StandardParticles import StdLooseResolvedPi0
from StandardParticles import StdLooseMergedPi0
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import MeV

default_config = {
    'NAME'              : 'BetaSPi0_RI', # The real name of the lines is BetaS, this is a reference this for the Settings.
    'WGs'               : ['B2CC'],
    'BUILDERTYPE'       : 'B2JpsiPi0Conf_RI',
    'CONFIG'            : { 'BPVLTIME'                  :       0.2
                          , 'VCHI2PDOF'                 :       10
                          , 'JpsiMassWindow'            :       80
                          },
    'STREAMS'          : { 'Leptonic' : [ 'StrippingBetaSBd2JpsiPi0DetachedLine' ] }
                 }

### Lines stored in this file:
# StrippingBetaSBd2JpsiPi0DetachedLine

class B2JpsiPi0Conf_RI(LineBuilder) :
    __configuration_keys__ = ('BPVLTIME',
                              'VCHI2PDOF',
                              'JpsiMassWindow')

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.name = "BetaS" # To avoid confusion with line names.
        self.config = config

        ### Selections:

        self.SelJpsi = self.createSubSel( OutputList = self.name + 'NarrowJpsiForBetaSPi0',
                                          InputList = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu/Particles"),
                                          Cuts = "(PFUNA(ADAMASS('J/psi(1S)')) < 80 * MeV) & (BPVLTIME() > 0.2 *ps)")

        self.SelPi0R = self.createSubSel( OutputList = self.name + "ResolvedPi0ForBetaS",
                                          InputList = DataOnDemand(Location = "Phys/StdLooseResolvedPi0/Particles"),
                                          Cuts = "(PT > 1000.*MeV) & (MINTREE('gamma'==ABSID, CL) > 0.05 )")
                  
        self.SelBd2JpsiPi0R = self.createCombinationSel( OutputList = self.name + "Bd2JpsiPi0R",
                                                         DecayDescriptor = "B0 -> J/psi(1S) pi0",
                                                         DaughterLists  = [ self.SelJpsi, self.SelPi0R ],
                                                         PreVertexCuts = "in_range(4500,AM,6000)",
                                                         PostVertexCuts = "in_range(4700,M,5900) & (VFASPF(VCHI2PDOF) < %(VCHI2PDOF)s) & (BPVDIRA>0.99755) & (BPVIP()<0.2) & (BPVIPCHI2()<20)" % self.config )

        self.Bd2JpsiPi0Vars = {
            "log(pow(pi0_PT*1e-3,5)-1)"            : "log(pow( CHILD(PT,2) *1e-3,5)-1)",
            "log(J_psi_1S_PT*1e-3)"                : "log( CHILD(PT,1) *1e-3)",
            "-log(J_psi_1S_IP_OWNPV)"              : "-log(CHILD(MIPDV(PRIMARY),1))",
            "log(1/B0_IP_OWNPV-5)"                 : "log(1/(CHILD(MIPDV(PRIMARY),0)) - 5)",
            "log(1/(1-fabs(B0_DIRA_OWNPV))-400)"   : "log(1/(1-abs( CHILD(BPVDIRA,0)  ))-400)"            
            }

        self.MvaBd2JpsiPi0R = self.applyMVA(self.name + "MvaBd2JpsiPi0",
                                            SelB        = self.SelBd2JpsiPi0R,
                                            MVAVars     = self.Bd2JpsiPi0Vars,
                                            MVACutValue = "-0.4",
                                            MVAxmlFile  = "$TMVAWEIGHTSROOT/data/B2JpsiPi0_Fisher_v1r2.xml"
                                            )

        self.Bd2JpsiPi0DetachedLine  = StrippingLine( self.name + "Bd2JpsiPi0DetachedLine",
                                                      #algos = [ self.MvaBd2JpsiPi0R ],
                                                      algos = [ self.SelBd2JpsiPi0R ],
                                                      MDSTFlag = True,
                                                      EnableFlavourTagging = True )
        
        self.registerLine(self.Bd2JpsiPi0DetachedLine)

    ### Generic tools:

    def createSubSel( self, OutputList, InputList, Cuts ):
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
                          ReFitPVs = True ):
        '''create a selection using a ParticleCombiner with a single decay descriptor'''
        combiner = CombineParticles( DecayDescriptor = DecayDescriptor,
                                     DaughtersCuts = DaughterCuts,
                                     MotherCut = PostVertexCuts,
                                     CombinationCut = PreVertexCuts,
                                     ReFitPVs = ReFitPVs)
        return Selection ( OutputList,
                           Algorithm = combiner,
                           RequiredSelections = DaughterLists )


    def applyMVA( self, name,
                  SelB,
                  MVAVars,
                  MVAxmlFile,
                  MVACutValue
                  ):
        from MVADictHelpers import addTMVAclassifierValue
        from Configurables import FilterDesktop as MVAFilterDesktop
        
        _FilterB = MVAFilterDesktop( name + "Filter",
                                     Code = "VALUE('LoKi::Hybrid::DictValue/" + name + "')>" + MVACutValue  )
        
        addTMVAclassifierValue( Component = _FilterB,
                                XMLFile   = MVAxmlFile,
                                Variables = MVAVars,
                                ToolName  = name )
        
        return Selection( name,
                          Algorithm =  _FilterB,
                          RequiredSelections = [ SelB ] )
    
