'''
Xic+ and Theta+ lines
Designed by Yury Shcheglov, Alexey Dzyuba, Nelya Sagidova to  study Xic+ and Theta+ particles
'''

__author__ = ['Yury Shcheglov']
__date__ = '2013/08/22'
__version__ = '$Revision: 2.0 $'

__all__ = ( 'StrippingXic2HHHConf',
            'makeXic2HHH',
            'default_config' )


from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from GaudiKernel.SystemOfUnits import MeV, mm, ns
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, AutomaticData
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions, StdTightPions,StdNoPIDsKaons, StdAllNoPIDsProtons

from Configurables import TisTosParticleTagger

class StrippingXic2HHHConf(LineBuilder): 

    __configuration_keys__ = (   'Daug_All_PT_MIN'
                               , 'Daug_1of3_PT_MIN'
                               , 'Daug_1of3_BPVIPCHI2_MIN'
                               , 'Daug_P_MIN'
                               , 'Daug_TRCHI2DOF_MAX'
                               , 'Daug_BPVIPCHI2_MIN'
                               , 'Proton_PIDp_MIN'
                               , 'K_IPCHI2_MIN'  
                               , 'Pi_PIDK_MAX'
                               , 'K_PIDK_MIN'
                               , 'Comb_MASS_MIN'
                               , 'Comb_MASS_MAX'
                               , 'Comb_ADOCAMAX_MAX'
                               , 'Xic_PT_MIN'
                               , 'Xic_VCHI2VDOF_MAX'
                               , 'Xic_BPVVDCHI2_MIN'
                               , 'Xic_BPVDIRA_MIN'
                               , 'Xic_BPVLTIME_MAX'
                               , 'Xic_BPVLTIME_MIN'
                               , 'HltFilter'
                               , 'PrescaleXic2PKPi'
                               , 'PostscaleXic2PKPi'
                               , 'PrescaleXic2PKK'
                               , 'PostscaleXic2PKK'
                               , 'PrescaleXic2PV0'
                               , 'PostscaleXic2PV0'
                             )

    __confdict__={}
    ## Possible parameters and default values copied from the definition
    ##   of StrippingLine
    def _strippingLine ( self,
                          name             ,   # the base name for the Line
                          prescale  = 1.0  ,   # prescale factor
                          ODIN      = None ,   # ODIN predicate
                          L0DU      = None ,   # L0DU predicate
                          HLT       = None ,   # HltDecReports predicate
                          FILTER    = None ,   # 'VOID'-predicate, e.g. Global Event Cut
                          checkPV   = True ,   # Check PV before running algos
                          algos     = None ,   # the list of stripping members
                          selection = None ,
                          postscale = 1.0    ,   # postscale factor
                          MaxCandidates = "Override",   # Maximum number
                          MaxCombinations = "Override", # Maximum number
                          HDRLocation = None ) : # other configuration parameters


        if (prescale > 0) and (postscale > 0) : 
            line = StrippingLine( name,
                                  prescale        = prescale,
                                  ODIN            = ODIN,
                                  L0DU            = L0DU,
                                  HLT             = HLT,
                                  FILTER          = FILTER,
                                  checkPV         = checkPV,
                                  algos           = algos,
                                  selection       = selection,
                                  postscale       = postscale,
                                  MaxCandidates   = MaxCandidates,
                                  MaxCombinations = MaxCombinations,
                                  HDRLocation     = HDRLocation )

            self.registerLine(line)
            return line

        else : 
            return False



    def __init__(self, name, config) : 

        LineBuilder.__init__(self, name, config)
        self.__confdict__= config

        xic_PKPi_name =  name + 'Xic2PKPi'
        xic_pKK_name  =  name + 'Xic2PKK'
        theta_pks_name  =  name + 'Theta2PKS0'
        xic_klam_name  = name + 'Xic2KLam'        


        self.KS0LL = DataOnDemand(Location = "Phys/StdLooseKsLL/Particles")
        self.KS0DD = DataOnDemand(Location = "Phys/StdLooseKsDD/Particles")
        self.LamLL = DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")
        self.LamDD = DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles")



 ############### PROTON SELECTIONS #########################################


        self.inProtons = Selection( "Protonsfor" + name,
                                  Algorithm = self._protonFilter(),
                                  RequiredSelections = [StdAllNoPIDsProtons])

 ############### PIONS, Lambda0, KS0 and KAONS SELECTIONS ##################
        
        self.inPions   = Selection( "Pionsfor" + name,
                                  Algorithm = self._pionFilter(),
                                  RequiredSelections = [StdNoPIDsPions])


        self.KS0 =  Selection( "KSfor" + name,
                                  Algorithm = self._ksFilter(),
                                  RequiredSelections = [self.KS0LL])

        self.Lam =  Selection( "Lambdafor" + name,
                                  Algorithm = self._lamFilter(),
                                  RequiredSelections = [self.LamLL,self.LamDD])
  
        self.inKaons  = Selection( "Kaonsfor" + name,
                                      Algorithm = self._kaonFilter(),
                                      RequiredSelections = [StdNoPIDsKaons])


 ###############################################################

        self.selXic2PKPi = makeXic2PKPi( name = xic_PKPi_name 
               , inputSel = [ self.inProtons, self.inKaons, self.inPions]
               , Daug_All_PT_MIN = config['Daug_All_PT_MIN']
               , Daug_1of3_PT_MIN  = config['Daug_1of3_PT_MIN']
               , Daug_1of3_BPVIPCHI2_MIN = config['Daug_1of3_BPVIPCHI2_MIN']
               , Daug_P_MIN = config['Daug_P_MIN']
               , Daug_TRCHI2DOF_MAX = config['Daug_TRCHI2DOF_MAX']
               , Daug_BPVIPCHI2_MIN = config['Daug_BPVIPCHI2_MIN']
               , Proton_PIDp_MIN = config['Proton_PIDp_MIN']
               , K_IPCHI2_MIN = 20.
               , Pi_PIDK_MAX = config['Pi_PIDK_MAX']
               , K_PIDK_MIN = config['K_PIDK_MIN']
               , Comb_MASS_MIN  = config['Comb_MASS_MIN']
               , Comb_MASS_MAX  = 2600. * MeV  #config['Comb_MASS_MAX']
               , Comb_ADOCAMAX_MAX = config['Comb_ADOCAMAX_MAX']
               , Xic_PT_MIN = config['Xic_PT_MIN']
               , Xic_VCHI2VDOF_MAX = config['Xic_VCHI2VDOF_MAX']
               , Xic_BPVVDCHI2_MIN = config['Xic_BPVVDCHI2_MIN']
               , Xic_BPVDIRA_MIN = config['Xic_BPVDIRA_MIN']
               , Xic_BPVLTIME_MAX = config['Xic_BPVLTIME_MAX']
               , Xic_BPVLTIME_MIN = config['Xic_BPVLTIME_MIN']
             )

        self.selXic2PKK = makeXic2PKPi( name = xic_pKK_name 
               , inputSel = [self.inKaons, self.inProtons]
               , Daug_All_PT_MIN = config['Daug_All_PT_MIN']
               , Daug_1of3_PT_MIN  = config['Daug_1of3_PT_MIN']
               , Daug_1of3_BPVIPCHI2_MIN = config['Daug_1of3_BPVIPCHI2_MIN']
               , Daug_P_MIN = config['Daug_P_MIN']
               , Daug_TRCHI2DOF_MAX = config['Daug_TRCHI2DOF_MAX']
               , Daug_BPVIPCHI2_MIN = config['Daug_BPVIPCHI2_MIN']
               , Proton_PIDp_MIN = config['Proton_PIDp_MIN']
               , K_IPCHI2_MIN = 0.
               , Pi_PIDK_MAX = config['Pi_PIDK_MAX']
               , K_PIDK_MIN = config['K_PIDK_MIN']
               , Comb_MASS_MIN  = config['Comb_MASS_MIN']
               , Comb_MASS_MAX  = 2800. * MeV  
               , Comb_ADOCAMAX_MAX = config['Comb_ADOCAMAX_MAX']
               , Xic_PT_MIN = config['Xic_PT_MIN']
               , Xic_VCHI2VDOF_MAX = config['Xic_VCHI2VDOF_MAX']
               , Xic_BPVVDCHI2_MIN = config['Xic_BPVVDCHI2_MIN']
               , Xic_BPVDIRA_MIN = 0.999
               , Xic_BPVLTIME_MAX = config['Xic_BPVLTIME_MAX']
               , Xic_BPVLTIME_MIN = config['Xic_BPVLTIME_MIN']
               , decDescriptors = [ "[Lambda_c+ -> p+ K- K+]cc" ]
             )

        self.selTheta2PKS0 = makeTheta2PKS0 ( name = theta_pks_name
               , inputSel = [ self.KS0,  self.inProtons ]
               , Daug_All_PT_MIN =  config['Daug_All_PT_MIN']
               , Daug_1of3_PT_MIN = config['Daug_1of3_PT_MIN']
               , Daug_P_MIN = config['Daug_P_MIN']
               , Daug_TRCHI2DOF_MAX = config['Daug_TRCHI2DOF_MAX']
               , Proton_PIDp_MIN = 30.                  
               , K_IPCHI2_MIN = 0. 
               , Comb_MASS_MIN  = 1440. * MeV            
               , Comb_MASS_MAX  = 1800. * MeV            
               , Comb_ADOCAMAX_MAX = 0.3 * mm
               , Xic_PT_MIN = config['Xic_PT_MIN']
               , Xic_BPVDIRA_MIN = -100.   
               , Xic_BPVLTIME_MAX = config['Xic_BPVLTIME_MAX']
               , Xic_BPVLTIME_MIN = config['Xic_BPVLTIME_MIN']
               , decDescriptors = [ "[Lambda_c+ -> p+ KS0]cc" ]
             )

        self.selXic2KLam = makeXic2KLam ( name = xic_klam_name
               , inputSel = [ self.Lam , self.inKaons]
               , Daug_All_PT_MIN  =  config['Daug_All_PT_MIN']            
               , Daug_1of3_PT_MIN =  config['Daug_1of3_PT_MIN']         
               , Daug_P_MIN = config['Daug_P_MIN']
               , Daug_TRCHI2DOF_MAX = config['Daug_TRCHI2DOF_MAX']
               , Proton_PIDp_MIN = 30.
               , K_IPCHI2_MIN = 4.
               , Comb_MASS_MIN  = 2100. * MeV           
               , Comb_MASS_MAX  = 2800. * MeV 
               , Comb_ADOCAMAX_MAX = 0.5 * mm
               , Xic_PT_MIN = config['Xic_PT_MIN']
               , Xic_BPVDIRA_MIN = -100.
               , Xic_BPVLTIME_MAX = config['Xic_BPVLTIME_MAX']                   
               , Xic_BPVLTIME_MIN = config['Xic_BPVLTIME_MIN']                   
               , decDescriptors = [ "[Lambda_c+ -> K+ Lambda0]cc" ]
             )



        self.line_Xic2PKPi = self._strippingLine( name = xic_PKPi_name + 'Line',
                                         HLT = config['HltFilter'],
                                         prescale  = config['PrescaleXic2PKPi'],
                                         postscale = config['PostscaleXic2PKPi'],
                                         selection = self.selXic2PKPi
                                       )

        self.line_Xic2PKK = self._strippingLine( name = xic_pKK_name + 'Line',
                                         HLT = config['HltFilter'],
                                         prescale  = config['PrescaleXic2PKK'],
                                         postscale = config['PostscaleXic2PKK'],
                                         selection = self.selXic2PKK
                                       )

        self.line_Theta2PKS0 = self._strippingLine( name = theta_pks_name + 'Line',
                                         prescale  = config['PrescaleXic2PV0'],
                                         postscale = config['PostscaleXic2PV0'],
                                         selection = self.selTheta2PKS0
                                       )

        self.line_Xic2KLam = self._strippingLine( name = xic_klam_name + 'Line',
                                         prescale  = config['PrescaleXic2PKK'],
                                         postscale = config['PostscaleXic2PKK'],
                                         selection = self.selXic2KLam
                                       )

    def _protonFilter( self ):
          _code = "(TRGHP < 0.4) & (PIDp-PIDpi > %(Proton_PIDp_MIN)s) & (P> 10000.0*MeV) & (TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s)" % self.__confdict__
          _proton = FilterDesktop( Code = _code )
          return _proton



    def _pionFilter( self ):
          _code = "(TRGHP < 0.4) & (PIDK-PIDpi < %(Pi_PIDK_MAX)s) & (P>1200*MeV)& (PT>400*MeV)&(TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s) " % self.__confdict__
          _pion = FilterDesktop( Code = _code )
          return _pion


    def _kaonFilter( self ):
          _code = "(TRGHP < 0.4) & (MIPCHI2DV(PRIMARY) > %(K_IPCHI2_MIN)s) & (PIDK-PIDpi > %(K_PIDK_MIN)s) & (P>1200*MeV) & (PT>400*MeV) &(TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s)" % self.__confdict__          
          _kaon = FilterDesktop( Code = _code )
          return _kaon

    def _ksFilter( self ):
          _code = "(MIPDV(PRIMARY)<0.5*mm)&(BPVDIRA>0)&(MM>487.*MeV)&(MM<507.*MeV)&(VFASPF(VCHI2PDOF)<10.)&(P>6000.*MeV)&(PT>2000.*MeV)&(BPVLTIME('PropertimeFitter/properTime:PUBLIC')>0.03*ns)&CHILDCUT((MIPCHI2DV(PRIMARY)>120.),1)&CHILDCUT((MIPCHI2DV(PRIMARY)>120.),2)&CHILDCUT((TRCHI2DOF<5.),1)&CHILDCUT((TRCHI2DOF<5.),2)" % self.__confdict__  
          _ks = FilterDesktop( Code = _code )
          return _ks


    def _lamFilter( self ):          
          _code = "(BPVDIRA>0.)&(P>6000.*MeV)&(PT>2000.*MeV)&(VFASPF(VCHI2PDOF)<10.)&(BPVLTIME('PropertimeFitter/properTime:PUBLIC')>0.03*ns)&(MM>1106.*MeV)&(MM<1126.*MeV)&CHILDCUT((MIPCHI2DV(PRIMARY)>120.),1)&CHILDCUT((MIPCHI2DV(PRIMARY)>120.),2)&CHILDCUT((TRCHI2DOF < 5. ),1)&CHILDCUT((TRCHI2DOF < 5. ),2)" % self.__confdict__
          _lam = FilterDesktop( Code = _code )
          return _lam


def makeXic2PKPi( name
               , inputSel
               , Daug_All_PT_MIN
               , Daug_1of3_PT_MIN
               , Daug_1of3_BPVIPCHI2_MIN
               , Daug_P_MIN
               , Daug_TRCHI2DOF_MAX
               , Daug_BPVIPCHI2_MIN
               , Proton_PIDp_MIN
               , K_IPCHI2_MIN
               , Pi_PIDK_MAX
               , K_PIDK_MIN
               , Comb_MASS_MIN 
               , Comb_MASS_MAX
               , Comb_ADOCAMAX_MAX
               , Xic_PT_MIN
               , Xic_VCHI2VDOF_MAX
               , Xic_BPVVDCHI2_MIN
               , Xic_BPVDIRA_MIN
               , Xic_BPVLTIME_MAX
               , Xic_BPVLTIME_MIN
               , decDescriptors = [ "[Lambda_c+ -> p+ K- pi+]cc" ]
             ) : 

    daugCuts = "(BPVIPCHI2() > %(Daug_BPVIPCHI2_MIN)s)" % locals()

    combCuts = "(AM > %(Comb_MASS_MIN)s)" \
               "& (AM < %(Comb_MASS_MAX)s)" \
               "& (AMAXCHILD(PT) > %(Daug_1of3_PT_MIN)s)" \
               "& (AMAXCHILD(BPVIPCHI2()) > %(Daug_1of3_BPVIPCHI2_MIN)s)" \
               "& (ADOCAMAX('') < %(Comb_ADOCAMAX_MAX)s)" % locals()


    xicCuts =  "(PT > %(Xic_PT_MIN)s)" \
               "& (VFASPF(VCHI2/VDOF) < %(Xic_VCHI2VDOF_MAX)s)" \
               "& (BPVVDCHI2 > %(Xic_BPVVDCHI2_MIN)s)" \
               "& (BPVDIRA > %(Xic_BPVDIRA_MIN)s)" \
               "& (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(Xic_BPVLTIME_MIN)s)" \
               "& (BPVLTIME('PropertimeFitter/properTime:PUBLIC') < %(Xic_BPVLTIME_MAX)s)" % locals()


    _Xic = CombineParticles(
        DecayDescriptors = decDescriptors
        , DaughtersCuts = { "pi+" : daugCuts, 
                            "K+"  : daugCuts, 
                            "p+"  : daugCuts}

        , CombinationCut = combCuts
        , MotherCut = xicCuts 
    )

    return Selection( name,
                      Algorithm = _Xic,
                      RequiredSelections = inputSel
                    )

def makeTheta2PKS0( name
               , inputSel
               , Daug_All_PT_MIN
               , Daug_1of3_PT_MIN
               , Daug_P_MIN
               , Daug_TRCHI2DOF_MAX
               , Proton_PIDp_MIN
               , K_IPCHI2_MIN 
               , Comb_MASS_MIN
               , Comb_MASS_MAX
               , Comb_ADOCAMAX_MAX
               , Xic_PT_MIN
               , Xic_BPVDIRA_MIN
               , Xic_BPVLTIME_MAX
               , Xic_BPVLTIME_MIN
               , decDescriptors = [ "[Lambda_c+ -> p+ KS0]cc" ]
             ) :

    combCuts = "(AM > %(Comb_MASS_MIN)s)" \
               "& (AM <  %(Comb_MASS_MAX)s)" \
               "& (AMAXCHILD(PT) > %(Daug_1of3_PT_MIN)s)" % locals()

    thetaCuts = "(PT > %(Xic_PT_MIN)s) & (MIPDV(PRIMARY)< 0.4 * mm)" % locals()

    _ThetaKS0 = CombineParticles(
        DecayDescriptors = decDescriptors
        , CombinationCut = combCuts
        , MotherCut = thetaCuts
    )

    return Selection( name,
                      Algorithm = _ThetaKS0,
                      RequiredSelections = inputSel
                    )

def makeXic2KLam( name
               , inputSel
               , Daug_All_PT_MIN
               , Daug_1of3_PT_MIN
               , Daug_P_MIN
               , Daug_TRCHI2DOF_MAX
               , Proton_PIDp_MIN
               , K_IPCHI2_MIN
               , Comb_MASS_MIN
               , Comb_MASS_MAX
               , Comb_ADOCAMAX_MAX
               , Xic_PT_MIN
               , Xic_BPVDIRA_MIN
               , Xic_BPVLTIME_MAX
               , Xic_BPVLTIME_MIN
               , decDescriptors = [ "[Lambda_c+ -> K+ Lambda0]cc" ]
             ) :


    combCuts = "(AM > %(Comb_MASS_MIN)s)" \
               "& (AM < %(Comb_MASS_MAX)s)" \
               "& (AMAXCHILD(PT) > %(Daug_1of3_PT_MIN)s)" % locals()
              

    xicCuts =  "(PT > %(Xic_PT_MIN)s)" % locals()

    _XicLam = CombineParticles(
        DecayDescriptors = decDescriptors
        , CombinationCut = combCuts
        , MotherCut = xicCuts
    )

    return Selection( name,
                      Algorithm = _XicLam,
                      RequiredSelections = inputSel
                    )
 


default_config = {  'Daug_All_PT_MIN'         : 400.0 * MeV
                  , 'Daug_1of3_PT_MIN'        : 700.0 * MeV
                  , 'Daug_P_MIN'              : 1200.0 * MeV
                  , 'Daug_TRCHI2DOF_MAX'      : 10.0
                  , 'Daug_BPVIPCHI2_MIN'      : 0.5
                  , 'Daug_1of3_BPVIPCHI2_MIN' : 5.0
                  , 'Proton_PIDp_MIN'         : 10.0
                  , 'K_IPCHI2_MIN'            : 20.0
                  , 'Pi_PIDK_MAX'             : 0.0
                  , 'K_PIDK_MIN'              : 5.0
                  , 'Comb_MASS_MIN'           : 1950.0 * MeV 
                  , 'Comb_MASS_MAX'           : 2800.0 * MeV 
                  , 'Comb_ADOCAMAX_MAX'       : 0.1 * mm
                  , 'Xic_PT_MIN'              : 1500.0 * MeV
                  , 'Xic_VCHI2VDOF_MAX'       : 10.0
                  , 'Xic_BPVVDCHI2_MIN'       : 5.0
                  , 'Xic_BPVDIRA_MIN'         : 0.9999
                  , 'Xic_BPVLTIME_MAX'        : 0.06 * ns
                  , 'Xic_BPVLTIME_MIN'        : 0.0 * ns
                  , 'HltFilter'               : "HLT_PASS('Hlt2CharmHadD2HHHDecision')"
                  , 'PrescaleXic2PKPi'        : 0.
                  , 'PostscaleXic2PKPi'       : 0.0
                  , 'PrescaleXic2PKK'         : 0.0
                  , 'PostscaleXic2PKK'        : 0.0
                  , 'PrescaleXic2PV0'         : 1.0
                  , 'PostscaleXic2PV0'        : 1.0
                 }


