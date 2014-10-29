'''
D+ cross-section lines.

Adapted to current stripping framework by P. Spradlin.
'''

__author__ = ['Hamish Gordon', 'Patrick Spradlin']
__date__ = '03/09/2010'
__version__ = '$Revision: 1.4 $'

__all__ = ( 'StrippingD2HHHForXSecConf',
            'makeD2HHH',
            'default_config' )


from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from GaudiKernel.SystemOfUnits import MeV, mm, mrad
from LHCbKernel.Configuration import *
#from Configurables import FilterDesktop, CombineParticles
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingUtils.Utils import LineBuilder
import StandardParticles
if hasattr(StandardParticles, "StdAllNoPIDsPions"):
  from StandardParticles import StdAllNoPIDsPions
else:
  from StandardParticles import StdNoPIDsPions as StdAllNoPIDsPions
if hasattr(StandardParticles, "StdAllNoPIDsKaons"):
  from StandardParticles import StdAllNoPIDsKaons
else:
  from StandardParticles import StdNoPIDsKaons as StdAllNoPIDsKaons

class StrippingD2HHHForXSecConf(LineBuilder): # {

    __configuration_keys__ = (  'Daug_All_PT_MIN'
                              , 'Daug_2of3_PT_MIN'
                              , 'Daug_1of3_PT_MIN'
                              , 'Daug_All_BPVIPCHI2_MIN'
                              , 'Daug_2of3_BPVIPCHI2_MIN'
                              , 'Daug_1of3_BPVIPCHI2_MIN'
                              , 'K_PIDK_MIN'
                              , 'Pi_PIDK_MAX'
                              , 'Comb_AM_MIN'
                              , 'Comb_AM_MAX'
                              , 'Comb_ADOCAMAX_MAX'
                              , 'D_VCHI2VDOF_MAX'
                              , 'D_acosBPVDIRA_MAX'
                              , 'D_PVDispCut'
                              , 'HltFilter'
                              , 'PrescaleD2KPP'
                              , 'PrescaleD2KKP'
                              , 'PrescaleD2KKK'
                              , 'PrescaleD2PPP'
                              , 'PrescaleD2KPPDCS'
                              , 'PostscaleD2KPP'
                              , 'PostscaleD2KKP'
                              , 'PostscaleD2KKK'
                              , 'PostscaleD2PPP'
                              , 'PostscaleD2KPPDCS'
                             )


    ## Possible parameters and default values copied from the definition
    ##   of StrippingLine
    def _strippingLine ( self,
                          name             ,   # the base name for the Line
                          prescale  = 1  ,   # prescale factor
                          ODIN      = None ,   # ODIN predicate
                          L0DU      = None ,   # L0DU predicate
                          HLT       = None ,   # HltDecReports predicate
                          FILTER    = None ,   # 'VOID'-predicate, e.g. Global Event Cut
                          checkPV   = True ,   # Check PV before running algos
                          algos     = None ,   # the list of stripping members
                          selection = None ,
                          postscale = 1.0    ,   # postscale factor
                          MaxCandidates = "Override",   # Maxumum number
                          MaxCombinations = "Override", # Maxumum number
                          HDRLocation = None ) : # other configuration parameters
    # {

        if (prescale > 0) and (postscale > 0) : # {
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
        # }
        else : 
            return False

    # }


    def __init__(self, name, config) : # {

        ## Base class constructor
        LineBuilder.__init__(self, name, config)

        ## Line names
        d2KPP_name = name + 'D2KPP'
        d2KKP_name = name + 'D2KKP'
        d2KKK_name = name + 'D2KKK'
        d2PPP_name = name + 'D2PPP'
        d2PPK_name = name + 'D2KPPDCS'

        self.inNoPIDsPions = StdAllNoPIDsPions
        self.inNoPIDsKaons  = StdAllNoPIDsKaons


        ## The (K- pi+ pi+) final state
        ## No PID lines.  It looks like the Loose PID lines have the
        self.selD2KPP = makeD2HHH( name = d2KPP_name
               , inputSel = [ self.inNoPIDsPions, self.inNoPIDsKaons ]
               , Daug_All_PT_MIN         = config['Daug_All_PT_MIN']
               , Daug_2of3_PT_MIN        = config['Daug_2of3_PT_MIN']
               , Daug_1of3_PT_MIN        = config['Daug_1of3_PT_MIN']
               , Daug_All_BPVIPCHI2_MIN  = config['Daug_All_BPVIPCHI2_MIN']
               , Daug_2of3_BPVIPCHI2_MIN = config['Daug_2of3_BPVIPCHI2_MIN']
               , Daug_1of3_BPVIPCHI2_MIN = config['Daug_1of3_BPVIPCHI2_MIN']
               , K_PIDK_MIN              = config['K_PIDK_MIN']
               , Pi_PIDK_MAX             = config['Pi_PIDK_MAX']
               , Comb_AM_MIN             = config['Comb_AM_MIN']
               , Comb_AM_MAX             = config['Comb_AM_MAX']
               , Comb_ADOCAMAX_MAX       = config['Comb_ADOCAMAX_MAX']
               , D_VCHI2VDOF_MAX         = config['D_VCHI2VDOF_MAX']
               , D_acosBPVDIRA_MAX           = config['D_acosBPVDIRA_MAX']
               , D_PVDispCut             = config['D_PVDispCut']
               , decDescriptors = ['[D+ -> K- pi+ pi+]cc' ]
             )


        self.line_D2KPP = self._strippingLine( name = d2KPP_name + 'Line',
                                HLT = config['HltFilter'],
                                prescale = config['PrescaleD2KPP'],
                                postscale = config['PostscaleD2KPP'],
                                selection = self.selD2KPP)


        ## The (K- K+ pi+) final state
        ## No PID lines.
        self.selD2KKP = makeD2HHH( name = d2KKP_name 
               , inputSel = [ self.inNoPIDsPions, self.inNoPIDsKaons ]
               , Daug_All_PT_MIN         = config['Daug_All_PT_MIN']
               , Daug_2of3_PT_MIN        = config['Daug_2of3_PT_MIN']
               , Daug_1of3_PT_MIN        = config['Daug_1of3_PT_MIN']
               , Daug_All_BPVIPCHI2_MIN  = config['Daug_All_BPVIPCHI2_MIN']
               , Daug_2of3_BPVIPCHI2_MIN = config['Daug_2of3_BPVIPCHI2_MIN']
               , Daug_1of3_BPVIPCHI2_MIN = config['Daug_1of3_BPVIPCHI2_MIN']
               , K_PIDK_MIN              = config['K_PIDK_MIN']
               , Pi_PIDK_MAX             = config['Pi_PIDK_MAX']
               , Comb_AM_MIN             = config['Comb_AM_MIN']
               , Comb_AM_MAX             = config['Comb_AM_MAX']
               , Comb_ADOCAMAX_MAX       = config['Comb_ADOCAMAX_MAX']
               , D_VCHI2VDOF_MAX         = config['D_VCHI2VDOF_MAX']
               , D_acosBPVDIRA_MAX           = config['D_acosBPVDIRA_MAX']
               , D_PVDispCut             = config['D_PVDispCut']
               , decDescriptors = ['[D+ -> K- K+ pi+]cc' ]
             )


        self.line_D2KKP = self._strippingLine( name = d2KKP_name + 'Line',
                                HLT = config['HltFilter'],
                                prescale = config['PrescaleD2KKP'],
                                postscale = config['PostscaleD2KKP'],
                                selection = self.selD2KKP)



        ## The (K- K+ K+) final state
        self.selD2KKK = makeD2HHH( name = d2KKK_name 
               , inputSel = [ self.inNoPIDsKaons ]
               , Daug_All_PT_MIN         = config['Daug_All_PT_MIN']
               , Daug_2of3_PT_MIN        = config['Daug_2of3_PT_MIN']
               , Daug_1of3_PT_MIN        = config['Daug_1of3_PT_MIN']
               , Daug_All_BPVIPCHI2_MIN  = config['Daug_All_BPVIPCHI2_MIN']
               , Daug_2of3_BPVIPCHI2_MIN = config['Daug_2of3_BPVIPCHI2_MIN']
               , Daug_1of3_BPVIPCHI2_MIN = config['Daug_1of3_BPVIPCHI2_MIN']
               , K_PIDK_MIN              = config['K_PIDK_MIN']
               , Pi_PIDK_MAX             = config['Pi_PIDK_MAX']
               , Comb_AM_MIN             = config['Comb_AM_MIN']
               , Comb_AM_MAX             = config['Comb_AM_MAX']
               , Comb_ADOCAMAX_MAX       = config['Comb_ADOCAMAX_MAX']
               , D_VCHI2VDOF_MAX         = config['D_VCHI2VDOF_MAX']
               , D_acosBPVDIRA_MAX           = config['D_acosBPVDIRA_MAX']
               , D_PVDispCut             = config['D_PVDispCut']
               , decDescriptors = ['[D+ -> K- K+ K+]cc' ]
             )


        self.line_D2KKK = self._strippingLine( name = d2KKK_name + 'Line',
                                HLT = config['HltFilter'],
                                prescale = config['PrescaleD2KKK'],
                                postscale = config['PostscaleD2KKK'],
                                selection = self.selD2KKK)


        ## The (pi- pi+ pi+) final state
        ## No PID lines. 
        self.selD2PPP = makeD2HHH( name = d2PPP_name
               , inputSel = [ self.inNoPIDsPions, self.inNoPIDsKaons ]
               , Daug_All_PT_MIN         = config['Daug_All_PT_MIN']
               , Daug_2of3_PT_MIN        = config['Daug_2of3_PT_MIN']
               , Daug_1of3_PT_MIN        = config['Daug_1of3_PT_MIN']
               , Daug_All_BPVIPCHI2_MIN  = config['Daug_All_BPVIPCHI2_MIN']
               , Daug_2of3_BPVIPCHI2_MIN = config['Daug_2of3_BPVIPCHI2_MIN']
               , Daug_1of3_BPVIPCHI2_MIN = config['Daug_1of3_BPVIPCHI2_MIN']
               , K_PIDK_MIN              = config['K_PIDK_MIN']
               , Pi_PIDK_MAX             = config['Pi_PIDK_MAX']
               , Comb_AM_MIN             = config['Comb_AM_MIN']
               , Comb_AM_MAX             = config['Comb_AM_MAX']
               , Comb_ADOCAMAX_MAX       = config['Comb_ADOCAMAX_MAX']
               , D_VCHI2VDOF_MAX         = config['D_VCHI2VDOF_MAX']
               , D_acosBPVDIRA_MAX           = config['D_acosBPVDIRA_MAX']
               , D_PVDispCut             = config['D_PVDispCut']
               , decDescriptors = ['[D+ -> pi- pi+ pi+]cc' ]
             )


        self.line_D2PPP = self._strippingLine( name = d2PPP_name + 'Line',
                                HLT = config['HltFilter'],
                                prescale = config['PrescaleD2PPP'],
                                postscale = config['PostscaleD2PPP'],
                                selection = self.selD2PPP)


        ## The (K+ pi- pi+) final state
        ## No PID lines.  It looks like the Loose PID lines have the
        self.selD2PPK = makeD2HHH( name = d2PPK_name
               , inputSel = [ self.inNoPIDsPions, self.inNoPIDsKaons ]
               , Daug_All_PT_MIN         = config['Daug_All_PT_MIN']
               , Daug_2of3_PT_MIN        = config['Daug_2of3_PT_MIN']
               , Daug_1of3_PT_MIN        = config['Daug_1of3_PT_MIN']
               , Daug_All_BPVIPCHI2_MIN  = config['Daug_All_BPVIPCHI2_MIN']
               , Daug_2of3_BPVIPCHI2_MIN = config['Daug_2of3_BPVIPCHI2_MIN']
               , Daug_1of3_BPVIPCHI2_MIN = config['Daug_1of3_BPVIPCHI2_MIN']
               , K_PIDK_MIN              = config['K_PIDK_MIN']
               , Pi_PIDK_MAX             = config['Pi_PIDK_MAX']
               , Comb_AM_MIN             = config['Comb_AM_MIN']
               , Comb_AM_MAX             = config['Comb_AM_MAX']
               , Comb_ADOCAMAX_MAX       = config['Comb_ADOCAMAX_MAX']
               , D_VCHI2VDOF_MAX         = config['D_VCHI2VDOF_MAX']
               , D_acosBPVDIRA_MAX           = config['D_acosBPVDIRA_MAX']
               , D_PVDispCut             = config['D_PVDispCut']
               , decDescriptors = ['[D+ -> K+ pi- pi+]cc' ]
             )


        self.line_D2PPK = self._strippingLine( name = d2PPK_name + 'Line',
                                HLT = config['HltFilter'],
                                prescale = config['PrescaleD2KPPDCS'],
                                postscale = config['PostscaleD2KPPDCS'],
                                selection = self.selD2PPK )

    # }

# }




def makeD2HHH( name
               , inputSel
               , Daug_All_PT_MIN
               , Daug_2of3_PT_MIN
               , Daug_1of3_PT_MIN
               , Daug_All_BPVIPCHI2_MIN
               , Daug_2of3_BPVIPCHI2_MIN
               , Daug_1of3_BPVIPCHI2_MIN
               , K_PIDK_MIN
               , Pi_PIDK_MAX
               , Comb_AM_MIN
               , Comb_AM_MAX
               , Comb_ADOCAMAX_MAX
               , D_VCHI2VDOF_MAX
               , D_acosBPVDIRA_MAX
               , D_PVDispCut
               , decDescriptors
             ) : # {

    ## Construct a preambulo to simplify some calculations.
    lclPreambulo = [
      "from math import cos"
      , "bpvdirathresh = cos(%(D_acosBPVDIRA_MAX)s)" % locals()
      , "pidFiducialPMin = 3.0 * GeV"
      , "pidFiducialPMax = 100.0 * GeV"
    ]



    daugCuts = "(PT > %(Daug_All_PT_MIN)s)" \
               "& (BPVIPCHI2() > %(Daug_All_BPVIPCHI2_MIN)s)" % locals()

    pidFiducialCuts = "(HASRICH)" \
                      "& (in_range(pidFiducialPMin, P, pidFiducialPMax))" \
                      "& (in_range(2.0, ETA, 5.0))"

    kaonPIDCut = pidFiducialCuts + "& (PIDK-PIDpi > %(K_PIDK_MIN)s)" % locals()
    pionPIDCut = pidFiducialCuts + "& (PIDK-PIDpi < %(Pi_PIDK_MAX)s)" % locals()

    combCuts = "(in_range(%(Comb_AM_MIN)s, AM, %(Comb_AM_MAX)s))" \
               "& (AMAXCHILD(PT) > %(Daug_1of3_PT_MIN)s)" \
               "& (AMAXCHILD(BPVIPCHI2()) > %(Daug_1of3_BPVIPCHI2_MIN)s)"\
               "& (ANUM(PT > %(Daug_2of3_PT_MIN)s) >= 2)" \
               "& (ANUM(BPVIPCHI2() > %(Daug_2of3_BPVIPCHI2_MIN)s) >= 2)" % locals()

    dCuts = "(VFASPF(VCHI2/VDOF) < %(D_VCHI2VDOF_MAX)s)" \
            "& (%(D_PVDispCut)s)" \
            "& (BPVDIRA > bpvdirathresh)" % locals()


    _dplus = CombineParticles( DecayDescriptors = decDescriptors
                               , Preambulo = lclPreambulo
                               , DaughtersCuts = { "pi+" : daugCuts + '&' + pionPIDCut, 
                                                   "K+" : daugCuts + '&' + kaonPIDCut }
                               , CombinationCut = combCuts
                               , MotherCut = dCuts
                             )
    return Selection( name,
                      Algorithm = _dplus,
                      RequiredSelections = inputSel
                    )

# }




default_config = {
                    'Daug_All_PT_MIN'           :  200.0 * MeV
                  , 'Daug_2of3_PT_MIN'          :  400.0 * MeV
                  , 'Daug_1of3_PT_MIN'          :  400.0 * MeV
                  , 'Daug_All_BPVIPCHI2_MIN'    :    1.0
                  , 'Daug_2of3_BPVIPCHI2_MIN'   :    4.0
                  , 'Daug_1of3_BPVIPCHI2_MIN'   :    4.0
                  , 'K_PIDK_MIN'                :    0.0
                  , 'Pi_PIDK_MAX'               :    3.0
                  , 'Comb_AM_MIN'               : 1580.0 * MeV
                  , 'Comb_AM_MAX'               : 2260.0 * MeV
                  , 'Comb_ADOCAMAX_MAX'         :    0.5 * mm
                  , 'D_VCHI2VDOF_MAX'           :   25.0
                  , 'D_acosBPVDIRA_MAX'         :   35.0 * mrad
                  , 'D_PVDispCut'               : "((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150 * picosecond))"
                  , 'HltFilter'                 : "HLT_PASS_RE('Hlt1MB.*')"
                  #, 'HltFilter'                 : None
                  , 'PrescaleD2KPP'             :    0.01
                  , 'PrescaleD2KKP'             :    0.01
                  , 'PrescaleD2KKK'             :   -1.0
                  , 'PrescaleD2PPP'             :   -1.0
                  , 'PrescaleD2KPPDCS'          :   -1.0
                  , 'PostscaleD2KPP'            :    1.0
                  , 'PostscaleD2KKP'            :    1.0
                  , 'PostscaleD2KKK'            :   -1.0
                  , 'PostscaleD2PPP'            :   -1.0
                  , 'PostscaleD2KPPDCS'         :   -1.0
                 }

