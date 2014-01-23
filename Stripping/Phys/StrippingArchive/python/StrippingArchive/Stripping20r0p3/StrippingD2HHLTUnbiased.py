'''
Prompt untagged lifetime unbiased D0->KK/Kpi stripping selection
for running on MB/NB events. For calibration of IP cuts for 2011.
'''

__author__ = ['Harry Cliff','Vladimir Gligorov']
__date__ = '18/10/2010'
__version__ = '$Revision: 1.0 $'

__all__ = ( 'StrippingD2HHLTUnbiasedConf',
            'makeD02HH',
            'makeDstar2D0Pi',
            'default_config' )


from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from GaudiKernel.SystemOfUnits import MeV, mm, ns
from LHCbKernel.Configuration import *
#from Configurables import FilterDesktop, CombineParticles
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsPions, StdAllNoPIDsKaons

class StrippingD2HHLTUnbiasedConf(LineBuilder): # {

    __configuration_keys__ = (   'Daug_PT_MIN'
                               , 'Daug_TRCHI2DOF_MAX'
                               , 'K_PIDK_MIN'
                               , 'Pi_PIDK_MAX'
                               , 'D0_APT_MIN'
                               , 'D0_AP_MIN'
                               , 'D0_ADOCA_MAX'
                               , 'D0_VCHI2VDOF_MAX'
                               , 'D0_ADAMASS_WIN'
                               , 'D0_ADMASS_WIN'
                               , 'D0_BPVLTIME_MIN'
                               , 'Dstar_AMDiff_MAX'
                               , 'Dstar_MDiff_MAX'
                               , 'Dstar_VCHI2VDOF_MAX'
                               , 'HltFilter'
                               , 'PrescaleD02HH'
                               , 'PrescaleDstar2D0Pi_D02HH'
                               , 'PostscaleD02HH'
                               , 'PostscaleDstar2D0Pi_D02HH'
                             )


    def __init__(self, name, config) : # {

        LineBuilder.__init__(self, name, config)

        d02HH_name = name + 'D02HH'
        dstar_name  = name + 'Dstar2D0Pi_D02HH'

        self.inPions = StdAllNoPIDsPions
        self.inKaons = StdAllNoPIDsKaons

        self.selD02HH = makeD02HH( d02HH_name
                                   , inputSel = [ self.inPions, self.inKaons ]
                                   , Daug_PT_MIN        = config['Daug_PT_MIN']
                                   , Daug_TRCHI2DOF_MAX = config['Daug_TRCHI2DOF_MAX']
                                   , K_PIDK_MIN         = config['K_PIDK_MIN']
                                   , Pi_PIDK_MAX        = config['Pi_PIDK_MAX']
                                   , D0_APT_MIN         = config['D0_APT_MIN']
                                   , D0_AP_MIN          = config['D0_AP_MIN']
                                   , D0_ADOCA_MAX       = config['D0_ADOCA_MAX']
                                   , D0_VCHI2VDOF_MAX   = config['D0_VCHI2VDOF_MAX']
                                   , D0_BPVLTIME_MIN    = config['D0_BPVLTIME_MIN']
                                   , D0_ADAMASS_WIN     = config['D0_ADAMASS_WIN']
                                   , D0_ADMASS_WIN      = config['D0_ADMASS_WIN']
                                 )


        self.line_D02HH = StrippingLine( d02HH_name + 'Line',
                                         HLT = config['HltFilter'],
                                         prescale  = config['PrescaleD02HH'],
                                         postscale = config['PostscaleD02HH'],
                                         algos = [ self.selD02HH ]
                                       )
        self.registerLine(self.line_D02HH)


        self.selDstar2D0Pi_D02HH = makeDstar2D0Pi( dstar_name
                    , inputSel = [ self.inPions, self.selD02HH ]
                    , Daug_TRCHI2DOF_MAX  = config['Daug_TRCHI2DOF_MAX']
                    , Dstar_AMDiff_MAX    = config['Dstar_AMDiff_MAX']
                    , Dstar_VCHI2VDOF_MAX = config['Dstar_VCHI2VDOF_MAX']
                    , Dstar_MDiff_MAX     = config['Dstar_MDiff_MAX']
        )

        self.line_Dstar2D0Pi_D02HH = StrippingLine( dstar_name + 'Line',
                                         HLT = config['HltFilter'],
                                         prescale   = config['PrescaleDstar2D0Pi_D02HH'],
                                         postscale = config['PostscaleDstar2D0Pi_D02HH'],
                                         algos = [ self.selDstar2D0Pi_D02HH ]
                                        )
        self.registerLine(self.line_Dstar2D0Pi_D02HH)

    # }

# }


def makeD02HH( name
               , inputSel
               , Daug_PT_MIN
               , Daug_TRCHI2DOF_MAX
               , K_PIDK_MIN
               , Pi_PIDK_MAX
               , D0_APT_MIN
               , D0_AP_MIN
               , D0_ADOCA_MAX
               , D0_VCHI2VDOF_MAX
               , D0_BPVLTIME_MIN
               , D0_ADAMASS_WIN
               , D0_ADMASS_WIN
               , decDescriptors = [ "D0 -> pi+ pi-",
                                    "D0 -> K- pi+",
                                    "D0 -> K+ pi-",
                                    "D0 -> K+ K-" ]
             ) : # {

    daugCuts = "(PT > %(Daug_PT_MIN)s)" \
               "& (TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s)" % locals()

    kaonPIDCut = "(PIDK-PIDpi > %(K_PIDK_MIN)s)" % locals()
    pionPIDCut = "(PIDK-PIDpi < %(Pi_PIDK_MAX)s)" % locals()

    combCuts = "(ADAMASS('D0') < %(D0_ADAMASS_WIN)s)" \
               "& (AP > %(D0_AP_MIN)s)" \
               "& (APT > %(D0_APT_MIN)s)" \
               "& (ADOCA(1,2) < %(D0_ADOCA_MAX)s)" % locals()

    d0Cuts = "(ADMASS('D0') < %(D0_ADMASS_WIN)s)" \
             "& (VFASPF(VCHI2/VDOF) < %(D0_VCHI2VDOF_MAX)s)" \
             "& (BPVLTIME() > %(D0_BPVLTIME_MIN)s)" % locals()


    _D0 = CombineParticles(
        DecayDescriptors = decDescriptors
        , DaughtersCuts = { "pi+" : daugCuts + '&' + pionPIDCut,
                            "K+"  : daugCuts + '&' + kaonPIDCut }
        , CombinationCut = combCuts
        , MotherCut = d0Cuts
    )



    return Selection( name,
                      Algorithm = _D0,
                      RequiredSelections = inputSel
                    )

# }



def makeDstar2D0Pi( name
                    , inputSel
                    , Daug_TRCHI2DOF_MAX
                    , Dstar_AMDiff_MAX
                    , Dstar_VCHI2VDOF_MAX
                    , Dstar_MDiff_MAX
                    , decDescriptors= [ "D*(2010)+ -> D0 pi+",
                                        "D*(2010)- -> D0 pi-" ]
                  ) : # {

    daugCuts = "(TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s)" % locals()

    combCuts = "((AM - AM1) < %(Dstar_AMDiff_MAX)s)" % locals()

    dstarCuts = "(VFASPF(VCHI2/VDOF) < %(Dstar_VCHI2VDOF_MAX)s)" \
                "& ((M - M1) < %(Dstar_MDiff_MAX)s)" % locals()

    _Dstar = CombineParticles(
        DecayDescriptors = decDescriptors
        , DaughtersCuts = { "pi+" : daugCuts }
        , CombinationCut = combCuts
        , MotherCut = dstarCuts
    )

    return Selection( name,
                      Algorithm = _Dstar,
                      RequiredSelections = inputSel
                    )

# }




default_config = {
                    'Daug_PT_MIN'               : 750.0*MeV
                  , 'Daug_TRCHI2DOF_MAX'        : 3.0
                  , 'K_PIDK_MIN'                : 5.0
                  , 'Pi_PIDK_MAX'               : 5.0
                  #
                  , 'D0_APT_MIN'                : 2500.0*MeV
                  , 'D0_AP_MIN'                 : 5000.0*MeV
                  , 'D0_ADOCA_MAX'              : 0.1*mm
                  , 'D0_VCHI2VDOF_MAX'          : 10.0
                  , 'D0_BPVLTIME_MIN'           : 0.00015*ns
                  , 'D0_ADAMASS_WIN'            : 50.*MeV
                  , 'D0_ADMASS_WIN'             : 50.*MeV
                  #
                  , 'Dstar_AMDiff_MAX'          : 160.0*MeV
                  , 'Dstar_MDiff_MAX'           : 155.0*MeV
                  , 'Dstar_VCHI2VDOF_MAX'       : 100.0
                  , 'HltFilter'          : "HLT_PASS_RE('Hlt1(MB|CharmCalibrationNoBias).*')"
                  #
                  , 'PrescaleD02HH'             :   1.0
                  , 'PrescaleDstar2D0Pi_D02HH'  :   1.0
                  , 'PostscaleD02HH'            :   1.0
                  , 'PostscaleDstar2D0Pi_D02HH' :   1.0
                 }


